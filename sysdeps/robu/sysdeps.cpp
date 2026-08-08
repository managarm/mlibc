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

// --- fd table ----------------------------------------------------------
// Mirrors apps/libc/src/fdtable.c's model: an fd is just (kind, handle).
// ramfs/devfs/procfs/sysfs all track read/write position server-side per
// handle, so there is no client-side seek offset to maintain.

enum FdKind {
	FD_NONE = 0,
	FD_DEVFS,
	FD_RAMFS,
	FD_RAMFS_DIR,
	FD_PROCFS,
	FD_SYSFS,
};

constexpr int MAX_FDS = 64;

struct FdEntry {
	FdKind kind = FD_NONE;
	uint64_t handle = 0;
	uint64_t size = 0; // procfs/sysfs: size reported at open()
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
		console_handle_cached = robu::devfs_open("/dev/console");
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
		if (chunk > (size_t)robu::DEVFS_WRITE_MAX) {
			chunk = robu::DEVFS_WRITE_MAX;
		}
		robu::devfs_write((uint64_t)h, buf + off, chunk);
		off += chunk;
	}
}

void ensure_stdio_defaults() {
	for (int fd = 0; fd <= 2; fd++) {
		if (g_fds[fd].kind == FD_NONE) {
			g_fds[fd].kind = FD_DEVFS;
			g_fds[fd].handle = (uint64_t)console_handle();
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
		g_fds[fd].kind = (FdKind)fds[i].kind;
		g_fds[fd].handle = fds[i].handle;
	}
}

namespace {

// --- path resolution -----------------------------------------------------
// Mirrors fdtable.c's resolve_path()/ramfs_name_from_path(): normalize
// "." / ".." against a tracked cwd, then dispatch on a fixed set of
// mount-point prefixes, falling through to a flat ramfs path otherwise.

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
constexpr dev_t ROBU_STDEV_PROCFS = 3;
constexpr dev_t ROBU_STDEV_SYSFS = 4;

int stat_path(const char *resolved, struct stat *buf) {
	if (strncmp(resolved, "/dev/", 5) == 0) {
		int64_t h = robu::devfs_open(resolved);
		if (h < 0) return ENOENT;
		robu::devfs_close((uint64_t)h);
		fill_stat(buf, 0, 0, ROBU_STDEV_DEVFS, (ino_t)h + 1);
		buf->st_mode = S_IFCHR | 0666;
		return 0;
	}
	if (strncmp(resolved, "/proc/", 6) == 0) {
		uint64_t size = 0;
		int64_t h = robu::procfs_open(resolved + 6, &size);
		if (h < 0) return ENOENT;
		robu::procfs_close((uint64_t)h);
		fill_stat(buf, size, 0, ROBU_STDEV_PROCFS, (ino_t)h + 1);
		return 0;
	}
	if (strncmp(resolved, "/var/sys/", 9) == 0) {
		uint64_t size = 0;
		int64_t h = robu::sysfs_open(resolved + 9, &size);
		if (h < 0) return ENOENT;
		robu::sysfs_close((uint64_t)h);
		fill_stat(buf, size, 0, ROBU_STDEV_SYSFS, (ino_t)h + 1);
		return 0;
	}
	if (strcmp(resolved, "/") == 0) {
		fill_stat(buf, 0, 1, ROBU_STDEV_RAMFS, 1);
		return 0;
	}
	char name[robu::RAMFS_PATH_MAX];
	int rc = ramfs_name_from_path(resolved, name, sizeof(name));
	if (rc != 0) return rc;
	uint64_t size, ino;
	int is_dir;
	if (robu::ramfs_stat(name, &size, &is_dir, &ino) != 0) {
		return ENOENT;
	}
	fill_stat(buf, size, is_dir, ROBU_STDEV_RAMFS, (ino_t)ino);
	return 0;
}

uint8_t *g_anon_cursor;

} // namespace

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
	if (fd_valid(fd) && g_fds[fd].kind == FD_DEVFS) {
		return 0;
	}
	return ENOTTY;
}

int Sysdeps<Open>::operator()(const char *path, int flags, mode_t, int *fd_out) {
	ensure_stdio_defaults();
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));

	if (strncmp(resolved, "/dev/", 5) == 0) {
		int64_t h = robu::devfs_open(resolved);
		if (h < 0) return ENOENT;
		int fd = alloc_fd();
		if (fd < 0) { robu::devfs_close((uint64_t)h); return EMFILE; }
		g_fds[fd] = { FD_DEVFS, (uint64_t)h, 0 };
		*fd_out = fd;
		return 0;
	}
	if (strncmp(resolved, "/proc/", 6) == 0) {
		uint64_t size;
		int64_t h = robu::procfs_open(resolved + 6, &size);
		if (h < 0) return ENOENT;
		int fd = alloc_fd();
		if (fd < 0) { robu::procfs_close((uint64_t)h); return EMFILE; }
		g_fds[fd] = { FD_PROCFS, (uint64_t)h, size };
		*fd_out = fd;
		return 0;
	}
	if (strncmp(resolved, "/var/sys/", 9) == 0) {
		uint64_t size;
		int64_t h = robu::sysfs_open(resolved + 9, &size);
		if (h < 0) return ENOENT;
		int fd = alloc_fd();
		if (fd < 0) { robu::sysfs_close((uint64_t)h); return EMFILE; }
		g_fds[fd] = { FD_SYSFS, (uint64_t)h, size };
		*fd_out = fd;
		return 0;
	}
	if (strcmp(resolved, "/") == 0) {
		if ((flags & O_ACCMODE) != O_RDONLY) return EISDIR;
		int fd = alloc_fd();
		if (fd < 0) return EMFILE;
		g_fds[fd] = { FD_RAMFS_DIR, robu::RAMFS_ROOT_INO, 0 };
		*fd_out = fd;
		return 0;
	}

	char name[robu::RAMFS_PATH_MAX];
	int rc = ramfs_name_from_path(resolved, name, sizeof(name));
	if (rc != 0) return rc;

	uint64_t size;
	int is_dir;
	if (robu::ramfs_stat(name, &size, &is_dir, nullptr) == 0 && is_dir) {
		if ((flags & O_ACCMODE) != O_RDONLY) return EISDIR;
		int fd = alloc_fd();
		if (fd < 0) return EMFILE;
		uint64_t ino;
		robu::ramfs_stat(name, &size, &is_dir, &ino);
		g_fds[fd] = { FD_RAMFS_DIR, ino, 0 };
		*fd_out = fd;
		return 0;
	}

	uint64_t rflags = 0;
	if (flags & O_CREAT) rflags |= robu::RAMFS_O_CREAT;
	if (flags & O_TRUNC) rflags |= robu::RAMFS_O_TRUNC;
	if (flags & O_APPEND) rflags |= robu::RAMFS_O_APPEND;
	int64_t h = robu::ramfs_open(name, rflags);
	if (h < 0) {
		return ENOENT;
	}
	int fd = alloc_fd();
	if (fd < 0) { robu::ramfs_close((uint64_t)h); return EMFILE; }
	g_fds[fd] = { FD_RAMFS, (uint64_t)h, 0 };
	*fd_out = fd;
	return 0;
}

int Sysdeps<Close>::operator()(int fd) {
	ensure_stdio_defaults();
	if (!fd_valid(fd)) {
		return EBADF;
	}
	bool shared = false;
	for (int i = 0; i < MAX_FDS; i++) {
		if (i != fd && g_fds[i].kind == g_fds[fd].kind && g_fds[i].handle == g_fds[fd].handle) {
			shared = true;
			break;
		}
	}
	if (!shared) {
		switch (g_fds[fd].kind) {
		case FD_DEVFS: robu::devfs_close(g_fds[fd].handle); break;
		case FD_RAMFS: robu::ramfs_close(g_fds[fd].handle); break;
		case FD_PROCFS: robu::procfs_close(g_fds[fd].handle); break;
		case FD_SYSFS: robu::sysfs_close(g_fds[fd].handle); break;
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
	while (total < count) {
		size_t chunk = count - total;
		int64_t n;
		switch (g_fds[fd].kind) {
		case FD_DEVFS: n = robu::devfs_write(g_fds[fd].handle, p + total, chunk); break;
		case FD_RAMFS: n = robu::ramfs_write(g_fds[fd].handle, p + total, chunk); break;
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
	while (total < count) {
		size_t chunk = count - total;
		int64_t n;
		switch (g_fds[fd].kind) {
		case FD_DEVFS: n = robu::devfs_read(g_fds[fd].handle, p + total, chunk); break;
		case FD_RAMFS: n = robu::ramfs_read(g_fds[fd].handle, p + total, chunk); break;
		case FD_PROCFS: n = robu::procfs_read(g_fds[fd].handle, p + total, chunk); break;
		case FD_SYSFS: n = robu::sysfs_read(g_fds[fd].handle, p + total, chunk); break;
		default: return EBADF;
		}
		if (n < 0) return EIO;
		if (n == 0) break;
		total += (size_t)n;
		// Console/ramfs reads may legitimately return short of `count`;
		// don't loop trying to fill the whole buffer in one Read() call.
		break;
	}
	*bytes_read = (ssize_t)total;
	return 0;
}

int Sysdeps<Seek>::operator()(int, off_t, int, off_t *) {
	// ramfs/devfs/procfs/sysfs track position server-side per handle with
	// no seek verb -- matches apps/libc's own (lack of) seek support.
	return ESPIPE;
}

int Sysdeps<Stat>::operator()(fsfd_target fsfdt, int fd, const char *path, int, struct stat *statbuf) {
	ensure_stdio_defaults();
	if (fsfdt == fsfd_target::fd) {
		if (!fd_valid(fd)) return EBADF;
		switch (g_fds[fd].kind) {
		case FD_DEVFS:
			fill_stat(statbuf, 0, 0, ROBU_STDEV_DEVFS, (ino_t)g_fds[fd].handle + 1);
			statbuf->st_mode = S_IFCHR | 0666;
			return 0;
		case FD_RAMFS_DIR:
			fill_stat(statbuf, 0, 1, ROBU_STDEV_RAMFS, (ino_t)g_fds[fd].handle);
			return 0;
		case FD_PROCFS:
			fill_stat(statbuf, g_fds[fd].size, 0, ROBU_STDEV_PROCFS, (ino_t)g_fds[fd].handle + 1);
			return 0;
		case FD_SYSFS:
			fill_stat(statbuf, g_fds[fd].size, 0, ROBU_STDEV_SYSFS, (ino_t)g_fds[fd].handle + 1);
			return 0;
		case FD_RAMFS: {
			uint64_t size, ino;
			int is_dir;
			if (robu::ramfs_fstat(g_fds[fd].handle, &size, &is_dir, &ino) != 0) return EBADF;
			fill_stat(statbuf, size, is_dir, ROBU_STDEV_RAMFS, (ino_t)ino);
			return 0;
		}
		default:
			return EBADF;
		}
	}
	// path, fd_path (dirfd is ignored beyond AT_FDCWD -- no real subdirectory
	// fd concept here, matching fdtable.c's own openat()/fstatat() shape).
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));
	return stat_path(resolved, statbuf);
}

int Sysdeps<OpenDir>::operator()(const char *path, int *handle) {
	ensure_stdio_defaults();
	char resolved[CWD_MAX];
	resolve_path(path, resolved, sizeof(resolved));
	uint64_t dir_ino;
	if (strcmp(resolved, "/") == 0) {
		dir_ino = robu::RAMFS_ROOT_INO;
	} else {
		char name[robu::RAMFS_PATH_MAX];
		int rc = ramfs_name_from_path(resolved, name, sizeof(name));
		if (rc != 0) return rc;
		uint64_t size, ino;
		int is_dir;
		if (robu::ramfs_stat(name, &size, &is_dir, &ino) != 0 || !is_dir) {
			return ENOTDIR;
		}
		dir_ino = ino;
	}
	int fd = alloc_fd();
	if (fd < 0) return EMFILE;
	g_fds[fd] = { FD_RAMFS_DIR, dir_ino, 0 };
	*handle = fd;
	return 0;
}

int Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	if (!fd_valid(handle) || g_fds[handle].kind != FD_RAMFS_DIR) {
		return EBADF;
	}
	// The cursor lives in the high bits of `size` (unused for directories);
	// simplest to keep a small side table instead.
	static uint64_t cursors[MAX_FDS];
	char name[32];
	int is_dir;
	int64_t rc = robu::ramfs_readdir(g_fds[handle].handle, cursors[handle], name, &is_dir);
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

// This kernel has no fork()+execve() image-replace primitive, so Fork and
// Execve are deliberately left unimplemented (RobuSysdepTags doesn't list
// them; mlibc's posix layer falls back to ENOSYS for callers that need
// them). Waitpid, however, maps directly onto this kernel's real
// IPC_FLAG_WAIT verb, so mlibc's standard waitpid()/wait() work as-is.
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

// This kernel's spawn primitive is not POSIX fork()+exec() (see above), so
// it's exposed as the same non-POSIX __libc_spawn() extension apps/libc
// already provided -- minibox-shell.c and hello_initsys call this directly.
extern "C" int __libc_spawn(const char *name, char *const argv[], char *const envp[]) {
	auto fd_export = [](int fd, uint32_t *kind, uint64_t *handle) -> bool {
		ensure_stdio_defaults();
		if (!fd_valid(fd)) return false;
		*kind = (uint32_t)g_fds[fd].kind;
		*handle = g_fds[fd].handle;
		return true;
	};
	int64_t rc = robu::robu_spawn(name, argv, envp, fd_export);
	if (rc < 0) {
		errno = ENOENT;
		return -1;
	}
	return (int)rc;
}
