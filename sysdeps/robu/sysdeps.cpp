#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/fsfd_target.hpp>
#include <bits/ensure.h>

#include "robu-abi.hpp"

extern "C" uint64_t __robu_heap_base;

namespace {

enum FdKind {
	FD_NONE = 0,
	FD_RAMFS_DIR,
	FD_PIPE_READ,
	FD_PIPE_WRITE,
	FD_VFS,
};

constexpr int MAX_FDS = 64;

struct FdEntry {
	FdKind kind = FD_NONE;
	uint64_t handle = 0;
	uint64_t size = 0;
	uint32_t server_tid = 0;
};

FdEntry g_fds[MAX_FDS];

int alloc_fd() {
	for (int i = 0; i < MAX_FDS; i++) {
		if (g_fds[i].kind == FD_NONE) {
			return i;
		}
	}
	return -1;
}

bool fd_valid(int fd) {
	return fd >= 0 && fd < MAX_FDS && g_fds[fd].kind != FD_NONE;
}

int64_t console_handle_cached = -1;
int64_t console_handle() {
	if (console_handle_cached < 0) {
		console_handle_cached = robu::vfs_open(robu::devfs_tid(), "console", 0);
	}
	return console_handle_cached;
}

void console_write_all(const char *buf, size_t len) {
	int64_t h = console_handle();
	if (h < 0) {
		return;
	}
	size_t off = 0;
	while (off < len) {
		size_t chunk = len - off;
		if (chunk > (size_t)robu::VFS_WRITE_MAX) {
			chunk = robu::VFS_WRITE_MAX;
		}
		robu::vfs_write(robu::devfs_tid(), (uint64_t)h, buf + off, chunk);
		off += chunk;
	}
}

void ensure_stdio_defaults() {
	for (int fd = 0; fd <= 2; fd++) {
		if (g_fds[fd].kind == FD_NONE) {
			g_fds[fd] = { FD_VFS, (uint64_t)console_handle(), 0, robu::devfs_tid() };
		}
	}
}

bool g_fd_inherit_done;

} // namespace

extern "C" void __robu_fd_inherit(uint64_t spawn_info) {
	g_fd_inherit_done = true;
	if (!spawn_info) {
		return;
	}
	const robu::robu_spawn_info *info = reinterpret_cast<const robu::robu_spawn_info *>(spawn_info);
	if (info->magic != robu::SPAWN_INFO_MAGIC) {
		return;
	}
	const robu::robu_spawn_fd *fds = reinterpret_cast<const robu::robu_spawn_fd *>(info + 1);
	uint32_t nfds = info->nfds;
	if (nfds > (uint32_t)robu::SPAWN_FD_INFO_MAX) {
		nfds = robu::SPAWN_FD_INFO_MAX;
	}
	for (uint32_t i = 0; i < nfds; i++) {
		int fd = (int)fds[i].fd;
		if (fd < 0 || fd > 2) continue;
		if (fds[i].kind == robu::SPAWN_FD_KIND_PIPE_READ) {
			g_fds[fd].kind = FD_PIPE_READ;
		} else if (fds[i].kind == robu::SPAWN_FD_KIND_PIPE_WRITE) {
			g_fds[fd].kind = FD_PIPE_WRITE;
		} else {
			g_fds[fd].kind = (FdKind)fds[i].kind;
		}
		g_fds[fd].handle = fds[i].handle;
		g_fds[fd].server_tid = fds[i].server_tid;
	}
}

namespace {

constexpr int CWD_MAX = 256;
char g_cwd[CWD_MAX] = "/";

void resolve_path(const char *path, char *out, size_t out_size) {
	char raw[CWD_MAX];
	if (path[0] == '/') {
		strncpy(raw, path, sizeof(raw) - 1);
		raw[sizeof(raw) - 1] = '\0';
	} else {
		size_t cwdlen = strlen(g_cwd);
		if (cwdlen >= sizeof(raw)) cwdlen = sizeof(raw) - 1;
		memcpy(raw, g_cwd, cwdlen);
		raw[cwdlen] = '\0';
		if (cwdlen > 0 && raw[cwdlen - 1] != '/') {
			strncat(raw, "/", sizeof(raw) - strlen(raw) - 1);
		}
		strncat(raw, path, sizeof(raw) - strlen(raw) - 1);
	}
	char *segs[16];
	int nseg = 0;
	char *save = nullptr;
	for (char *tok = strtok_r(raw, "/", &save); tok; tok = strtok_r(nullptr, "/", &save)) {
		if (strcmp(tok, ".") == 0) continue;
		if (strcmp(tok, "..") == 0) {
			if (nseg > 0) nseg--;
			continue;
		}
		if (nseg < (int)(sizeof(segs) / sizeof(segs[0]))) {
			segs[nseg++] = tok;
		}
	}
	size_t pos = 0;
	if (out_size > 1) out[pos++] = '/';
	out[pos] = '\0';
	for (int i = 0; i < nseg; i++) {
		size_t len = strlen(segs[i]);
		if (pos > 1) {
			if (pos + 1 >= out_size) break;
			out[pos++] = '/';
		}
		if (pos + len >= out_size) break;
		memcpy(out + pos, segs[i], len);
		pos += len;
		out[pos] = '\0';
	}
}

int ramfs_name_from_path(const char *resolved, char *name, size_t name_size) {
	const char *p = resolved;
	if (*p == '/') p++;
	if (*p == '\0') {
		return EISDIR;
	}
	if (strlen(p) >= (size_t)robu::RAMFS_PATH_MAX) {
		return ENAMETOOLONG;
	}
	strncpy(name, p, name_size - 1);
	name[name_size - 1] = '\0';
	return 0;
}

uint32_t resolve_mount_for_dir(const char *resolved, const char **rel_out) {
	size_t rlen = strlen(resolved);
	char match_buf[CWD_MAX];
	if (rlen + 2 > sizeof(match_buf)) {
		*rel_out = resolved + rlen;
		return 0;
	}
	memcpy(match_buf, resolved, rlen);
	match_buf[rlen] = '/';
	match_buf[rlen + 1] = '\0';
	int matched_len = 0;
	uint32_t tid = robu::resolve_mount(match_buf, &matched_len);
	*rel_out = ((size_t)matched_len <= rlen) ? resolved + matched_len : resolved + rlen;
	return tid;
}

void fill_stat(struct stat *buf, uint64_t size, int is_dir, dev_t dev, ino_t ino) {
	memset(buf, 0, sizeof(*buf));
	buf->st_mode = is_dir ? (S_IFDIR | 0755) : (S_IFREG | 0644);
	buf->st_size = (off_t)size;
	buf->st_nlink = 1;
	buf->st_blksize = 512;
	buf->st_blocks = (blkcnt_t)((size + 511) / 512);
	buf->st_dev = dev;
	buf->st_ino = ino;
}

constexpr dev_t ROBU_STDEV_DEVFS = 1;
constexpr dev_t ROBU_STDEV_RAMFS = 2;
constexpr dev_t ROBU_STDEV_VFS = 5;

int stat_path(const char *resolved, struct stat *buf) {
	if (strcmp(resolved, "/") == 0) {
		fill_stat(buf, 0, 1, ROBU_STDEV_RAMFS, 1);
		return 0;
	}
	const char *rel;
	uint32_t mount_tid = resolve_mount_for_dir(resolved, &rel);
	if (mount_tid != 0) {
		uint64_t size = 0;
		int is_dir = 0;
		uint64_t ino = 0;
		if (robu::vfs_stat(mount_tid, rel, &size, &is_dir, &ino) != 0) {
			return ENOENT;
		}
		if (mount_tid == robu::devfs_tid()) {
			fill_stat(buf, size, is_dir, ROBU_STDEV_DEVFS, (ino_t)ino);
			buf->st_mode = S_IFCHR | 0666;
			return 0;
		}
		if (mount_tid == robu::ramfs_tid()) {
			fill_stat(buf, size, is_dir, ROBU_STDEV_RAMFS, (ino_t)ino);
			return 0;
		}
		fill_stat(buf, size, is_dir, ROBU_STDEV_VFS, (ino_t)ino);
		return 0;
	}
	return ENOENT;
}

uint8_t *g_anon_cursor;

}

namespace mlibc {

[[noreturn]] void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("!!! mlibc panic !!!");
	sysdep<Exit>(-1);
	__builtin_trap();
}

void Sysdeps<LibcLog>::operator()(const char *msg) {
	console_write_all(msg, strlen(msg));
	console_write_all("\n", 1);
}

int Sysdeps<Isatty>::operator()(int fd) {
	if (fd_valid(fd) && g_fds[fd].kind == FD_VFS && g_fds[fd].server_tid == robu::devfs_tid()) {
		return 0;
	}
	return ENOTTY;
}

int Sysdeps<Open>::operator()(const char *path, int flags, mode_t, int *fd_out) {
	ensure_stdio_defaults();
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));
	if (strcmp(resolved, "/") == 0) {
		if ((flags & O_ACCMODE) != O_RDONLY) return EISDIR;
		int fd = alloc_fd();
		if (fd < 0) return EMFILE;
		g_fds[fd] = { FD_RAMFS_DIR, robu::RAMFS_ROOT_INO, 0, robu::ramfs_tid() };
		*fd_out = fd;
		return 0;
	}

	int matched_len = 0;
	uint32_t mount_tid = robu::resolve_mount(resolved, &matched_len);
	if (mount_tid != 0) {
		const char *rel = resolved + matched_len;
		if (mount_tid == robu::ramfs_tid()) {
			uint64_t size;
			int is_dir;
			uint64_t ino;
			if (robu::vfs_stat(mount_tid, rel, &size, &is_dir, &ino) == 0 && is_dir) {
				if ((flags & O_ACCMODE) != O_RDONLY) return EISDIR;
				int fd = alloc_fd();
				if (fd < 0) return EMFILE;
				g_fds[fd] = { FD_RAMFS_DIR, ino, 0, mount_tid };
				*fd_out = fd;
				return 0;
			}
		}
		uint64_t vflags = 0;
		if (flags & O_CREAT) vflags |= robu::VFS_O_CREAT;
		if (flags & O_TRUNC) vflags |= robu::VFS_O_TRUNC;
		if (flags & O_APPEND) vflags |= robu::VFS_O_APPEND;
		int64_t h = robu::vfs_open(mount_tid, rel, vflags);
		if (h < 0) return h == robu::VFS_ERR_NOT_FOUND ? ENOENT : EIO;
		int fd = alloc_fd();
		if (fd < 0) { robu::vfs_close(mount_tid, (uint64_t)h); return EMFILE; }
		g_fds[fd] = { FD_VFS, (uint64_t)h, 0, mount_tid };
		*fd_out = fd;
		return 0;
	}
	return ENOENT;
}

int Sysdeps<Dup>::operator()(int fd, int, int *newfd_out) {
	ensure_stdio_defaults();
	if (!fd_valid(fd)) {
		return EBADF;
	}
	int newfd = alloc_fd();
	if (newfd < 0) {
		return EMFILE;
	}
	g_fds[newfd] = g_fds[fd];
	*newfd_out = newfd;
	return 0;
}

int Sysdeps<Dup2>::operator()(int fd, int, int newfd) {
	ensure_stdio_defaults();
	if (!fd_valid(fd)) {
		return EBADF;
	}
	if (newfd < 0 || newfd >= MAX_FDS) {
		return EBADF;
	}
	if (newfd == fd) {
		return 0;
	}
	if (fd_valid(newfd)) {
		Sysdeps<Close>{}(newfd);
	}
	g_fds[newfd] = g_fds[fd];
	return 0;
}

int Sysdeps<Close>::operator()(int fd) {
	ensure_stdio_defaults();
	if (!fd_valid(fd)) {
		return EBADF;
	}
	bool shared = false;
	for (int i = 0; i < MAX_FDS; i++) {
		if (i != fd && g_fds[i].kind == g_fds[fd].kind && g_fds[i].handle == g_fds[fd].handle &&
		    g_fds[i].server_tid == g_fds[fd].server_tid) {
			shared = true;
			break;
		}
	}
	if (!shared) {
		switch (g_fds[fd].kind) {
		case FD_PIPE_READ: robu::pipe_close_raw(g_fds[fd].handle, 0); break;
		case FD_PIPE_WRITE: robu::pipe_close_raw(g_fds[fd].handle, 1); break;
		case FD_VFS: robu::vfs_close(g_fds[fd].server_tid, g_fds[fd].handle); break;
		default: break;
		}
	}
	g_fds[fd] = FdEntry{};
	return 0;
}

int Sysdeps<Write>::operator()(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	ensure_stdio_defaults();
	if (!fd_valid(fd)) {
		return EBADF;
	}
	const uint8_t *p = (const uint8_t *)buf;
	size_t total = 0;
	if (g_fds[fd].kind == FD_PIPE_WRITE) {
		while (total < count) {
			uint64_t n = 0;
			int64_t rc = robu::pipe_write_raw(g_fds[fd].handle, p + total, count - total, &n);
			if (rc == robu::IPC_ERR_NOT_FOUND) {
				return total > 0 ? 0 : EPIPE;
			}
			if (rc != robu::IPC_ERR_NONE) {
				return total > 0 ? 0 : EIO;
			}
			if (n == 0) {
				robu::ipc_raw(0, 1, robu::IPC_FLAG_NONE, nullptr, nullptr);
				continue;
			}
			total += (size_t)n;
		}
		*bytes_written = (ssize_t)total;
		return 0;
	}
	while (total < count) {
		size_t chunk = count - total;
		int64_t n;
		switch (g_fds[fd].kind) {
		case FD_VFS: n = robu::vfs_write(g_fds[fd].server_tid, g_fds[fd].handle, p + total, chunk); break;
		default: return EBADF;
		}
		if (n <= 0) {
			if (total > 0) break;
			return EIO;
		}
		total += (size_t)n;
	}
	*bytes_written = (ssize_t)total;
	return 0;
}

int Sysdeps<Read>::operator()(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	ensure_stdio_defaults();
	if (!fd_valid(fd)) {
		return EBADF;
	}
	uint8_t *p = (uint8_t *)buf;
	size_t total = 0;
	if (g_fds[fd].kind == FD_PIPE_READ) {
		uint64_t n = 0;
		int64_t rc = robu::pipe_read_raw(g_fds[fd].handle, p, count, &n);
		while (rc == robu::IPC_ERR_NONE && n == 0) {
			robu::ipc_raw(0, 1, robu::IPC_FLAG_NONE, nullptr, nullptr);
			rc = robu::pipe_read_raw(g_fds[fd].handle, p, count, &n);
		}
		if (rc == robu::IPC_ERR_NOT_FOUND) {
			*bytes_read = 0;
			return 0;
		}
		if (rc != robu::IPC_ERR_NONE) {
			return EIO;
		}
		*bytes_read = (ssize_t)n;
		return 0;
	}
	bool is_console = g_fds[fd].kind == FD_VFS && g_fds[fd].server_tid == robu::devfs_tid() &&
	                  g_fds[fd].handle == robu::DEV_CONSOLE;
	while (total < count) {
		size_t chunk = count - total;
		int64_t n;
		switch (g_fds[fd].kind) {
		case FD_VFS: n = robu::vfs_read(g_fds[fd].server_tid, g_fds[fd].handle, p + total, chunk); break;
		default: return EBADF;
		}
		if (n < 0) return EIO;
		if (n == 0) {
			if (is_console && total == 0) {
				robu::ipc_raw(0, 1, robu::IPC_FLAG_NONE, nullptr, nullptr);
				continue;
			}
			break;
		}
		total += (size_t)n;
		break;
	}
	*bytes_read = (ssize_t)total;
	return 0;
}

int Sysdeps<Seek>::operator()(int, off_t, int, off_t *) {
	return ESPIPE;
}

int Sysdeps<Stat>::operator()(fsfd_target fsfdt, int fd, const char *path, int, struct stat *statbuf) {
	ensure_stdio_defaults();
	if (fsfdt == fsfd_target::fd) {
		if (!fd_valid(fd)) return EBADF;
		switch (g_fds[fd].kind) {
		case FD_RAMFS_DIR:
			fill_stat(statbuf, 0, 1, ROBU_STDEV_RAMFS, (ino_t)g_fds[fd].handle);
			return 0;
		case FD_VFS: {
			uint64_t size, ino;
			int is_dir;
			if (robu::vfs_fstat(g_fds[fd].server_tid, g_fds[fd].handle, &size, &is_dir, &ino) != 0) return EBADF;
			if (g_fds[fd].server_tid == robu::devfs_tid()) {
				fill_stat(statbuf, size, is_dir, ROBU_STDEV_DEVFS, (ino_t)ino);
				statbuf->st_mode = S_IFCHR | 0666;
				return 0;
			}
			if (g_fds[fd].server_tid == robu::ramfs_tid()) {
				fill_stat(statbuf, size, is_dir, ROBU_STDEV_RAMFS, (ino_t)ino);
				return 0;
			}
			fill_stat(statbuf, size, is_dir, ROBU_STDEV_VFS, (ino_t)ino);
			return 0;
		}
		default:
			return EBADF;
		}
	}
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));
	return stat_path(resolved, statbuf);
}

int Sysdeps<OpenDir>::operator()(const char *path, int *handle) {
	ensure_stdio_defaults();
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));
	uint64_t dir_ino;
	uint32_t server_tid;
	if (strcmp(resolved, "/") == 0) {
		dir_ino = robu::RAMFS_ROOT_INO;
		server_tid = robu::ramfs_tid();
	} else {
		const char *rel;
		server_tid = resolve_mount_for_dir(resolved, &rel);
		if (server_tid == 0) return ENOENT;
		uint64_t size, ino;
		int is_dir;
		if (robu::vfs_stat(server_tid, rel, &size, &is_dir, &ino) != 0 || !is_dir) {
			return ENOTDIR;
		}
		dir_ino = ino;
	}
	int fd = alloc_fd();
	if (fd < 0) return EMFILE;
	g_fds[fd] = { FD_RAMFS_DIR, dir_ino, 0, server_tid };
	*handle = fd;
	return 0;
}

int Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	if (!fd_valid(handle) || g_fds[handle].kind != FD_RAMFS_DIR) {
		return EBADF;
	}
	static uint64_t cursors[MAX_FDS];
	char name[32];
	int is_dir;
	int64_t rc = robu::vfs_readdir(g_fds[handle].server_tid, g_fds[handle].handle, cursors[handle], name, &is_dir);
	if (rc != 0) {
		*bytes_read = 0;
		return 0;
	}
	cursors[handle]++;
	struct dirent *de = (struct dirent *)buffer;
	if (max_size < sizeof(*de)) {
		return EINVAL;
	}
	memset(de, 0, sizeof(*de));
	de->d_ino = 1;
	de->d_off = 0;
	de->d_reclen = sizeof(*de);
	de->d_type = is_dir ? DT_DIR : DT_REG;
	strncpy(de->d_name, name, sizeof(de->d_name) - 1);
	*bytes_read = sizeof(*de);
	return 0;
}

int Sysdeps<GetCwd>::operator()(char *buffer, size_t size) {
	size_t len = strlen(g_cwd);
	if (len + 1 > size) {
		return ERANGE;
	}
	memcpy(buffer, g_cwd, len + 1);
	return 0;
}

int Sysdeps<Chdir>::operator()(const char *path) {
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));
	bool valid = strcmp(resolved, "/") == 0 || strcmp(resolved, "/dev") == 0;
	if (!valid) {
		char name[robu::RAMFS_PATH_MAX];
		if (ramfs_name_from_path(resolved, name, sizeof(name)) == 0) {
			uint64_t size, ino;
			int is_dir;
			valid = robu::ramfs_stat(name, &size, &is_dir, &ino) == 0 && is_dir;
		}
	}
	if (!valid) {
		return ENOENT;
	}
	strncpy(g_cwd, resolved, sizeof(g_cwd) - 1);
	g_cwd[sizeof(g_cwd) - 1] = '\0';
	return 0;
}

int Sysdeps<Utimensat>::operator()(int, const char *, const struct timespec *, int) {
	return 0;
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	robu::msg_regs m{};
	m.word[0] = (uint64_t)pointer;
	int64_t rc = robu::ipc_raw(0, 0, robu::IPC_FLAG_SET_FSBASE, &m, nullptr);
	return rc == robu::IPC_ERR_NONE ? 0 : EINVAL;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	if (!g_anon_cursor) {
		g_anon_cursor = (uint8_t *)__robu_heap_base;
	}
	size = (size + 0xFFF) & ~(size_t)0xFFF;
	*pointer = g_anon_cursor;
	g_anon_cursor += size;
	return 0;
}

int Sysdeps<AnonFree>::operator()(void *, size_t) {
	return 0;
}

int Sysdeps<VmMap>::operator()(void *, size_t size, int, int, int fd, off_t, void **window) {
	if (fd != -1) {
		return ENOSYS;
	}
	void *p;
	Sysdeps<AnonAllocate>::operator()(size, &p);
	*window = p;
	return 0;
}

int Sysdeps<VmUnmap>::operator()(void *, size_t) {
	return 0;
}

int Sysdeps<FutexWait>::operator()(int *, int, const struct timespec *) {
	return ENOSYS;
}

int Sysdeps<FutexWake>::operator()(int *, bool) {
	return ENOSYS;
}

int Sysdeps<ClockGet>::operator()(int, time_t *secs, long *nanos) {
	uint64_t ticks = robu::kinfo_ticks();
	uint64_t hz = robu::kinfo()->clock_hz ? robu::kinfo()->clock_hz : 1;
	*secs = (time_t)(ticks / hz);
	*nanos = (long)((ticks % hz) * (1000000000ull / hz));
	return 0;
}

[[noreturn]] void Sysdeps<Exit>::operator()(int status) {
	robu::exit_raw(status);
}

int Sysdeps<Fork>::operator()(pid_t *child) {
	uint64_t child_or_zero = 0;
	int64_t rc = robu::fork_raw(&child_or_zero);
	if (rc != robu::IPC_ERR_NONE) {
		return rc == robu::IPC_ERR_NO_MEM ? ENOMEM : EAGAIN;
	}
	*child = (pid_t)child_or_zero;
	return 0;
}

pid_t Sysdeps<FutexTid>::operator()() {
	return (pid_t)robu::self_tid();
}

pid_t Sysdeps<GetPid>::operator()() {
	return (pid_t)robu::self_tid();
}

int Sysdeps<Pipe>::operator()(int *fds, int) {
	uint64_t handle = 0;
	int64_t rc = robu::pipe_create(&handle);
	if (rc != robu::IPC_ERR_NONE) {
		return rc == robu::IPC_ERR_NO_MEM ? ENOMEM : EMFILE;
	}
	int rfd = alloc_fd();
	if (rfd < 0) {
		robu::pipe_close_raw(handle, 0);
		robu::pipe_close_raw(handle, 1);
		return EMFILE;
	}
	g_fds[rfd] = { FD_PIPE_READ, handle, 0 };
	int wfd = alloc_fd();
	if (wfd < 0) {
		g_fds[rfd] = FdEntry{};
		robu::pipe_close_raw(handle, 0);
		robu::pipe_close_raw(handle, 1);
		return EMFILE;
	}
	g_fds[wfd] = { FD_PIPE_WRITE, handle, 0 };
	fds[0] = rfd;
	fds[1] = wfd;
	return 0;
}

int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
	int64_t rc = robu::exec_raw(path, argv, envp);
	if (rc == robu::IPC_ERR_NOT_FOUND) {
		return ENOENT;
	}
	if (rc == robu::IPC_ERR_NO_MEM) {
		return ENOMEM;
	}
	if (rc != robu::IPC_ERR_NONE) {
		return EINVAL;
	}
	return 0;
}

int Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *, pid_t *ret_pid) {
	bool nohang = (flags & WNOHANG) != 0;
	int64_t rc = robu::robu_waitpid(pid, status, nohang);
	if (rc == 0 && nohang) {
		*ret_pid = 0;
		return 0;
	}
	if (rc < 0) {
		return ECHILD;
	}
	*ret_pid = (pid_t)rc;
	return 0;
}

}


extern "C" int __libc_spawn(const char *name, char *const argv[], char *const envp[]) {
	auto fd_export = [](int fd, uint32_t *kind, uint64_t *handle, uint32_t *server_tid) -> bool {
		ensure_stdio_defaults();
		if (!fd_valid(fd)) return false;
		if (g_fds[fd].kind == FD_PIPE_READ) {
			*kind = robu::SPAWN_FD_KIND_PIPE_READ;
		} else if (g_fds[fd].kind == FD_PIPE_WRITE) {
			*kind = robu::SPAWN_FD_KIND_PIPE_WRITE;
		} else {
			*kind = (uint32_t)g_fds[fd].kind;
		}
		*handle = g_fds[fd].handle;
		*server_tid = g_fds[fd].server_tid;
		return true;
	};
	int64_t rc = robu::robu_spawn(name, argv, envp, fd_export);
	if (rc < 0) {
		errno = ENOENT;
		return -1;
	}
	return (int)rc;
}
