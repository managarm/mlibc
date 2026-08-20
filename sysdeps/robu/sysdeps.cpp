#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <dirent.h>
#include <bits/winsize.h>
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
uint64_t g_dir_cursors[MAX_FDS];

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

bool is_tty_handle(uint64_t handle) {
	return handle == robu::DEV_CONSOLE || (handle >= robu::DEV_RSTTY1 && handle <= robu::DEV_RSTTY6);
}

int fd_to_vt(int fd) {
	if (!fd_valid(fd) || g_fds[fd].kind != FD_VFS || g_fds[fd].server_tid != robu::devfs_tid()) {
		return 0;
	}
	uint64_t h = g_fds[fd].handle;
	if (h >= robu::DEV_RSTTY1 && h <= robu::DEV_RSTTY6) {
		return (int)(h - robu::DEV_RSTTY1);
	}
	return 0;
}

int64_t console_handle_cached = -1;
int64_t console_handle() {
	if (console_handle_cached < 0) {
		const char *tty_dev = getenv("ROBU_TTY_DEV");
		console_handle_cached = robu::vfs_open(robu::devfs_tid(), tty_dev && tty_dev[0] ? tty_dev : "console", 0);
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

}

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

int vfs_err_to_errno(int64_t status) {
	switch (status) {
	case 0: return 0;
	case robu::VFS_ERR_NOT_FOUND: return ENOENT;
	case robu::VFS_ERR_EXISTS: return EEXIST;
	case robu::VFS_ERR_IS_DIR: return EISDIR;
	case robu::VFS_ERR_NOT_DIR: return ENOTDIR;
	case robu::VFS_ERR_NOT_EMPTY: return ENOTEMPTY;
	case robu::VFS_ERR_NOT_SUPPORTED: return ENOTSUP;
	case robu::VFS_ERR_NO_SPACE: return ENOSPC;
	default: return EIO;
	}
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
	ensure_stdio_defaults();
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
		g_dir_cursors[fd] = 0;
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
				g_dir_cursors[fd] = 0;
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
	                  is_tty_handle(g_fds[fd].handle);
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
	g_dir_cursors[fd] = 0;
	*handle = fd;
	return 0;
}

int Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	if (!fd_valid(handle) || g_fds[handle].kind != FD_RAMFS_DIR) {
		return EBADF;
	}
	char name[32];
	int is_dir;
	int64_t rc = robu::vfs_readdir(g_fds[handle].server_tid, g_fds[handle].handle, g_dir_cursors[handle], name, &is_dir);
	if (rc != 0) {
		*bytes_read = 0;
		return 0;
	}
	g_dir_cursors[handle]++;
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

int Sysdeps<Mkdir>::operator()(const char *path, mode_t) {
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));
	int matched_len = 0;
	uint32_t mount_tid = robu::resolve_mount(resolved, &matched_len);
	if (mount_tid == 0) return ENOENT;
	return vfs_err_to_errno(robu::vfs_mkdir(mount_tid, resolved + matched_len));
}

int Sysdeps<Rmdir>::operator()(const char *path) {
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));
	int matched_len = 0;
	uint32_t mount_tid = robu::resolve_mount(resolved, &matched_len);
	if (mount_tid == 0) return ENOENT;
	return vfs_err_to_errno(robu::vfs_rmdir(mount_tid, resolved + matched_len));
}

int Sysdeps<Unlinkat>::operator()(int, const char *path, int) {
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));
	int matched_len = 0;
	uint32_t mount_tid = robu::resolve_mount(resolved, &matched_len);
	if (mount_tid == 0) return ENOENT;
	return vfs_err_to_errno(robu::vfs_unlink(mount_tid, resolved + matched_len));
}

int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
	char old_resolved[CWD_MAX], new_resolved[CWD_MAX];
	resolve_path(old_path, old_resolved, sizeof(old_resolved));
	resolve_path(new_path, new_resolved, sizeof(new_resolved));
	int old_matched = 0, new_matched = 0;
	uint32_t old_tid = robu::resolve_mount(old_resolved, &old_matched);
	uint32_t new_tid = robu::resolve_mount(new_resolved, &new_matched);
	if (old_tid == 0 || new_tid == 0) return ENOENT;
	if (old_tid != new_tid) return EXDEV;
	return vfs_err_to_errno(robu::vfs_link(old_tid, old_resolved + old_matched, new_resolved + new_matched));
}

int Sysdeps<Mknodat>::operator()(int, const char *path, int mode, int dev) {
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));
	int matched_len = 0;
	uint32_t mount_tid = robu::resolve_mount(resolved, &matched_len);
	if (mount_tid == 0) return ENOENT;
	return vfs_err_to_errno(robu::vfs_mknod(mount_tid, resolved + matched_len, (uint64_t)mode, (uint64_t)dev));
}

void Sysdeps<Sync>::operator()() {
	robu::vfs_quiesce_disk();
}

int Sysdeps<Sleep>::operator()(time_t *secs, long *nanos) {
	uint64_t hz = robu::kinfo()->clock_hz;
	if (hz == 0) hz = 100;
	uint64_t ticks = (uint64_t)(*secs) * hz + ((uint64_t)(*nanos) * hz) / 1000000000ull;
	if (ticks == 0 && (*secs != 0 || *nanos != 0)) ticks = 1;
	robu::sleep_raw(ticks);
	*secs = 0;
	*nanos = 0;
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

pid_t Sysdeps<GetPpid>::operator()() {
	robu::msg_regs q{};
	q.word[0] = robu::self_tid();
	int64_t rc = robu::ipc_raw(0, 0, robu::IPC_FLAG_THREAD_INFO, &q, nullptr);
	if (rc != 0) {
		return 1;
	}
	return (pid_t)q.word[3];
}

int Sysdeps<Ttyname>::operator()(int fd, char *buf, size_t size) {
	if (!fd_valid(fd) || g_fds[fd].kind != FD_VFS || g_fds[fd].server_tid != robu::devfs_tid()) {
		return ENOTTY;
	}
	const char *name = "/dev/console";
	size_t len = strlen(name);
	if (len + 1 > size) {
		return ERANGE;
	}
	memcpy(buf, name, len + 1);
	return 0;
}

int Sysdeps<GetResuid>::operator()(uid_t *ruid, uid_t *euid, uid_t *suid) {
	*ruid = 0;
	*euid = 0;
	*suid = 0;
	return 0;
}

int Sysdeps<GetResgid>::operator()(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	*rgid = 0;
	*egid = 0;
	*sgid = 0;
	return 0;
}

int Sysdeps<GetHostname>::operator()(char *buffer, size_t bufsize) {
	const char *name = "robu";
	size_t len = strlen(name);
	if (len + 1 > bufsize) {
		return ENAMETOOLONG;
	}
	memcpy(buffer, name, len + 1);
	return 0;
}

int Sysdeps<GetGroups>::operator()(size_t size, gid_t *list, int *ret) {
	(void)size;
	(void)list;
	*ret = 0;
	return 0;
}

int Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int *result) {
	if (!fd_valid(fd)) {
		return EBADF;
	}
	switch (request) {
	case F_GETFD:
		*result = 0;
		return 0;
	case F_SETFD:
		(void)va_arg(args, int);
		*result = 0;
		return 0;
	case F_GETFL:
		*result = 0;
		return 0;
	case F_SETFL:
		(void)va_arg(args, int);
		*result = 0;
		return 0;
	case F_DUPFD:
	case F_DUPFD_CLOEXEC: {
		int min_fd = va_arg(args, int);
		if (min_fd < 0) {
			return EINVAL;
		}
		int newfd = -1;
		for (int i = min_fd; i < MAX_FDS; i++) {
			if (g_fds[i].kind == FD_NONE) {
				newfd = i;
				break;
			}
		}
		if (newfd < 0) {
			return EMFILE;
		}
		g_fds[newfd] = g_fds[fd];
		*result = newfd;
		return 0;
	}
	default:
		return EINVAL;
	}
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

int Sysdeps<Sigaction>::operator()(int signum, const struct sigaction *act, struct sigaction *oldact) {
	uint64_t new_handler = 0, new_flags = 0, new_mask = 0;
	int do_set = act ? 1 : 0;
	if (act) {
		new_handler = (act->sa_flags & SA_SIGINFO)
		              ? reinterpret_cast<uint64_t>(act->sa_sigaction)
		              : reinterpret_cast<uint64_t>(act->sa_handler);
		new_flags = (uint64_t)act->sa_flags;
		new_mask = *reinterpret_cast<const uint64_t *>(&act->sa_mask);
	}
	uint64_t old_handler = 0, old_flags = 0, old_mask = 0;
	int64_t rc = robu::sig_action_raw(signum, new_handler, new_flags, new_mask, do_set,
	                                   &old_handler, &old_flags, &old_mask);
	if (rc != robu::IPC_ERR_NONE) {
		return EINVAL;
	}
	if (oldact) {
		memset(oldact, 0, sizeof(*oldact));
		if (old_flags & SA_SIGINFO) {
			oldact->sa_sigaction = reinterpret_cast<void (*)(int, siginfo_t *, void *)>(old_handler);
		} else {
			oldact->sa_handler = reinterpret_cast<void (*)(int)>(old_handler);
		}
		oldact->sa_flags = (int)old_flags;
		*reinterpret_cast<uint64_t *>(&oldact->sa_mask) = old_mask;
	}
	return 0;
}

int Sysdeps<Kill>::operator()(pid_t pid, int sig) {
	int64_t rc = robu::sig_kill_raw((uint64_t)pid, sig);
	return rc == robu::IPC_ERR_NONE ? 0 : ESRCH;
}

int Sysdeps<Tgkill>::operator()(int pid, int tid, int sig) {
	(void)pid;
	int64_t rc = robu::sig_kill_raw((uint64_t)tid, sig);
	return rc == robu::IPC_ERR_NONE ? 0 : ESRCH;
}

int Sysdeps<Sigprocmask>::operator()(int how, const sigset_t *set, sigset_t *retrieve) {
	uint64_t new_mask = set ? *reinterpret_cast<const uint64_t *>(set) : 0;
	uint64_t old_mask = 0;
	int64_t rc = robu::sig_procmask_raw((uint64_t)how, new_mask, set ? 1 : 0, &old_mask);
	if (rc != robu::IPC_ERR_NONE) {
		return EINVAL;
	}
	if (retrieve) {
		memset(retrieve, 0, sizeof(*retrieve));
		*reinterpret_cast<uint64_t *>(retrieve) = old_mask;
	}
	return 0;
}

int Sysdeps<Sigpending>::operator()(sigset_t *set) {
	uint64_t pending = robu::sig_pending_raw();
	if (set) {
		memset(set, 0, sizeof(*set));
		*reinterpret_cast<uint64_t *>(set) = pending;
	}
	return 0;
}

int Sysdeps<Sigsuspend>::operator()(const sigset_t *set) {
	uint64_t new_mask = set ? *reinterpret_cast<const uint64_t *>(set) : 0;
	uint64_t old_mask = 0;
	robu::sig_procmask_raw(SIG_SETMASK, new_mask, 1, &old_mask);
	robu::sleep_raw(0xFFFFFFFFULL);
	robu::sig_procmask_raw(SIG_SETMASK, old_mask, 1, nullptr);
	return EINTR;
}

int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr) {
	ensure_stdio_defaults();
	int vt = fd_to_vt(fd);
	memset(attr, 0, sizeof(*attr));
	attr->c_cflag = CREAD | CS8;
	attr->c_lflag = ISIG | ECHOE | ECHOK | ECHO;
	if (!robu::console_mode_query_raw(vt)) {
		attr->c_lflag |= ICANON;
	}
	attr->c_cc[VMIN] = 1;
	attr->c_cc[VTIME] = 0;
	return 0;
}

int Sysdeps<Tcsetattr>::operator()(int fd, int when, const struct termios *attr) {
	(void)when;
	ensure_stdio_defaults();
	int vt = fd_to_vt(fd);
	int raw = !(attr->c_lflag & ICANON);
	robu::console_mode_set_raw(vt, raw);
	return 0;
}

int Sysdeps<Tcgetwinsize>::operator()(int fd, struct winsize *winsz) {
	(void)fd;
	winsz->ws_row = 25;
	winsz->ws_col = 80;
	winsz->ws_xpixel = 0;
	winsz->ws_ypixel = 0;
	return 0;
}

int Sysdeps<Tcsetwinsize>::operator()(int fd, const struct winsize *winsz) {
	(void)fd;
	(void)winsz;
	return 0;
}

int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result) {
	(void)fd;
	if (result) {
		*result = 0;
	}
	if (request == robu::ROBU_TIOCGWINSZ) {
		struct winsize *ws = (struct winsize *)arg;
		ws->ws_row = 25;
		ws->ws_col = 80;
		ws->ws_xpixel = 0;
		ws->ws_ypixel = 0;
		return 0;
	}
	if (request == robu::ROBU_TIOCGPGRP) {
		*(int *)arg = (int)robu::tcgetpgrp_raw();
		return 0;
	}
	if (request == robu::ROBU_TIOCSPGRP) {
		robu::tcsetpgrp_raw((uint64_t)(*(int *)arg));
		return 0;
	}
	if (request == robu::ROBU_TIOCSCTTY) {
		return 0;
	}
	return ENOTTY;
}

int Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
	int64_t rc = robu::setpgid_raw((uint64_t)pid, (uint64_t)pgid);
	return rc == robu::IPC_ERR_NONE ? 0 : ESRCH;
}

int Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
	uint64_t g = 0;
	int64_t rc = robu::getpgid_raw((uint64_t)pid, &g);
	if (rc != robu::IPC_ERR_NONE) {
		return ESRCH;
	}
	*pgid = (pid_t)g;
	return 0;
}

int Sysdeps<SetSid>::operator()(pid_t *sid) {
	uint64_t s = 0;
	robu::setsid_raw(&s);
	if (sid) {
		*sid = (pid_t)s;
	}
	return 0;
}

int Sysdeps<Pselect>::operator()(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set,
		const struct timespec *timeout, const sigset_t *, int *num_events) {
	if (write_set) {
		FD_ZERO(write_set);
	}
	if (except_set) {
		FD_ZERO(except_set);
	}
	if (!read_set) {
		*num_events = 0;
		return 0;
	}

	int fd = -1;
	for (int i = 0; i < num_fds; i++) {
		if (FD_ISSET(i, read_set)) {
			fd = i;
			break;
		}
	}
	FD_ZERO(read_set);
	if (fd < 0) {
		*num_events = 0;
		return 0;
	}

	ensure_stdio_defaults();
	if (!fd_valid(fd)) {
		*num_events = 0;
		return 0;
	}

	bool is_console = g_fds[fd].kind == FD_VFS && g_fds[fd].server_tid == robu::devfs_tid() &&
	                   is_tty_handle(g_fds[fd].handle);

	bool has_timeout = timeout != nullptr;
	uint64_t ticks_left = has_timeout
		? (uint64_t)timeout->tv_sec * 100 + (uint64_t)timeout->tv_nsec / 10000000
		: 0;

	for (;;) {
		bool ready = is_console ? robu::vfs_peek(robu::devfs_tid(), g_fds[fd].handle) > 0 : true;
		if (ready) {
			FD_SET(fd, read_set);
			*num_events = 1;
			return 0;
		}
		if (has_timeout) {
			if (ticks_left == 0) {
				*num_events = 0;
				return 0;
			}
			ticks_left--;
		}
		robu::sleep_raw(1);
	}
}

uid_t Sysdeps<GetUid>::operator()() {
	return 0;
}

uid_t Sysdeps<GetEuid>::operator()() {
	return 0;
}

gid_t Sysdeps<GetGid>::operator()() {
	return 0;
}

gid_t Sysdeps<GetEgid>::operator()() {
	return 0;
}

int Sysdeps<SetUid>::operator()(uid_t) {
	return 0;
}

int Sysdeps<SetGid>::operator()(gid_t) {
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
