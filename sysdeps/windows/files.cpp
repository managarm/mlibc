#include <abi-bits/errno.h>
#include <abi-bits/fcntl.h>
#include <mlibc/all-sysdeps.hpp>

namespace {

using dword = unsigned long;

struct large_integer {
	long long quad_part;
};

// values from the win32 file api. keeping them here avoids depending on the mingw headers
constexpr dword standard_handle_ids[] = {
	static_cast<dword>(-10),
	static_cast<dword>(-11),
	static_cast<dword>(-12),
};
constexpr dword max_io_size = 0xFFFFFFFFul;

constexpr dword generic_read = 0x80000000ul;
constexpr dword generic_write = 0x40000000ul;
constexpr dword file_append_data = 0x00000004ul;

constexpr dword file_share_read = 0x00000001ul;
constexpr dword file_share_write = 0x00000002ul;
constexpr dword file_share_delete = 0x00000004ul;

constexpr dword create_new = 1;
constexpr dword create_always = 2;
constexpr dword open_existing = 3;
constexpr dword open_always = 4;
constexpr dword truncate_existing = 5;

constexpr dword file_attribute_normal = 0x00000080ul;

constexpr dword file_begin = 0;
constexpr dword file_current = 1;
constexpr dword file_end = 2;

constexpr dword error_invalid_function = 1;
constexpr dword error_file_not_found = 2;
constexpr dword error_path_not_found = 3;
constexpr dword error_too_many_open_files = 4;
constexpr dword error_access_denied = 5;
constexpr dword error_invalid_handle = 6;
constexpr dword error_not_enough_memory = 8;
constexpr dword error_outofmemory = 14;
constexpr dword error_invalid_drive = 15;
constexpr dword error_sharing_violation = 32;
constexpr dword error_lock_violation = 33;
constexpr dword error_handle_eof = 38;
constexpr dword error_file_exists = 80;
constexpr dword error_invalid_parameter = 87;
constexpr dword error_broken_pipe = 109;
constexpr dword error_disk_full = 112;
constexpr dword error_already_exists = 183;

constexpr int max_fds = 256;

extern "C" void *GetStdHandle(dword);
extern "C" int GetConsoleMode(void *, dword *);
extern "C" void *CreateFileA(const char *, dword, dword, void *, dword, dword, void *);
extern "C" int ReadFile(void *, void *, dword, dword *, void *);
extern "C" int WriteFile(void *, const void *, dword, dword *, void *);
extern "C" int CloseHandle(void *);
extern "C" dword GetLastError();
extern "C" int SetFilePointerEx(void *, large_integer, large_integer *, dword);

void *file_table[max_fds];
bool file_table_initialized;

bool is_valid_handle(void *handle) {
	return handle && handle != reinterpret_cast<void *>(-1);
}

dword clamp_io_size(size_t size) {
	return size > max_io_size ? max_io_size : static_cast<dword>(size);
}

void initialize_file_table() {
	if(file_table_initialized)
		return;

	file_table_initialized = true;
	for(size_t fd = 0;
			fd < sizeof(standard_handle_ids) / sizeof(*standard_handle_ids); fd++) {
		auto handle = GetStdHandle(standard_handle_ids[fd]);
		if(is_valid_handle(handle))
			file_table[fd] = handle;
	}
}

bool is_valid_fd(int fd) {
	return fd >= 0 && fd < max_fds;
}

void *get_handle(int fd) {
	initialize_file_table();
	if(!is_valid_fd(fd))
		return nullptr;
	return file_table[fd];
}

int allocate_fd() {
	initialize_file_table();
	for(int fd = 0; fd < max_fds; fd++) {
		if(!file_table[fd])
			return fd;
	}
	return -1;
}

int win32_error_to_errno(dword error) {
	switch(error) {
		case error_file_not_found:
		case error_path_not_found:
		case error_invalid_drive:
			return ENOENT;
		case error_access_denied:
		case error_sharing_violation:
		case error_lock_violation:
			return EACCES;
		case error_too_many_open_files:
			return EMFILE;
		case error_invalid_handle:
			return EBADF;
		case error_not_enough_memory:
		case error_outofmemory:
			return ENOMEM;
		case error_invalid_function:
			return ESPIPE;
		case error_file_exists:
		case error_already_exists:
			return EEXIST;
		case error_invalid_parameter:
			return EINVAL;
		case error_broken_pipe:
			return EPIPE;
		case error_disk_full:
			return ENOSPC;
		default:
			return EIO;
	}
}

constexpr int open_access_mode_mask = O_WRONLY | O_RDWR;

int validate_open_flags(int flags) {
	constexpr int supported_flags = O_CREAT | O_EXCL | O_TRUNC | O_APPEND
			| O_CLOEXEC | O_NOCTTY;
	if(flags & ~(supported_flags | open_access_mode_mask))
		return EINVAL;

	auto access_mode = flags & open_access_mode_mask;
	if(access_mode == open_access_mode_mask)
		return EINVAL;

	if((flags & O_EXCL) && !(flags & O_CREAT))
		return EINVAL;

	if((flags & O_TRUNC) && access_mode == O_RDONLY)
		return EINVAL;

	return 0;
}

dword desired_access_for_open(int flags) {
	auto access_mode = flags & open_access_mode_mask;
	dword desired_access = 0;
	if(access_mode != O_WRONLY)
		desired_access |= generic_read;

	if(access_mode != O_RDONLY)
		desired_access |= flags & O_APPEND ? file_append_data : generic_write;

	return desired_access;
}

dword creation_disposition_for_open(int flags) {
	if(!(flags & O_CREAT))
		return flags & O_TRUNC ? truncate_existing : open_existing;

	if(flags & O_EXCL)
		return create_new;

	if(flags & O_TRUNC)
		return create_always;

	return open_always;
}

bool seek_method_for_whence(int whence, dword *method) {
	switch(whence) {
		case SEEK_SET:
			*method = file_begin;
			return true;
		case SEEK_CUR:
			*method = file_current;
			return true;
		case SEEK_END:
			*method = file_end;
			return true;
		default:
			return false;
	}
}

void write_stderr(const char *message) {
	auto handle = get_handle(2);
	if(!handle)
		return;

	size_t remaining = 0;
	while(message[remaining])
		remaining++;

	for(auto cursor = message; remaining;) {
		auto chunk = clamp_io_size(remaining);
		dword written = 0;
		if(!WriteFile(handle, cursor, chunk, &written, nullptr) || !written)
			return;

		cursor += written;
		remaining -= written;
	}
}

} // namespace

void __mlibc_windows_init_file_table() {
	initialize_file_table();
}

namespace mlibc {

void Sysdeps<LibcLog>::operator()(const char *message) {
	write_stderr(message);
}

int Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t, int *fd) {
	*fd = -1;

	if(auto error = validate_open_flags(flags); error)
		return error;

	auto new_fd = allocate_fd();
	if(new_fd < 0)
		return EMFILE;

	auto handle = CreateFileA(pathname, desired_access_for_open(flags),
			file_share_read | file_share_write | file_share_delete, nullptr,
			creation_disposition_for_open(flags), file_attribute_normal, nullptr);
	if(!is_valid_handle(handle))
		return win32_error_to_errno(GetLastError());

	file_table[new_fd] = handle;
	*fd = new_fd;
	return 0;
}

int Sysdeps<Read>::operator()(int fd, void *buffer, size_t size, ssize_t *bytes_read) {
	*bytes_read = 0;
	auto handle = get_handle(fd);
	if(!handle)
		return EBADF;

	if(!size)
		return 0;

	auto chunk = clamp_io_size(size);
	dword actual_size = 0;
	if(!ReadFile(handle, buffer, chunk, &actual_size, nullptr)) {
		auto error = GetLastError();
		if(error == error_handle_eof || error == error_broken_pipe)
			return 0;

		return win32_error_to_errno(error);
	}

	*bytes_read = actual_size;
	return 0;
}

int Sysdeps<Write>::operator()(int fd, const void *buffer, size_t size, ssize_t *bytes_written) {
	*bytes_written = 0;
	auto handle = get_handle(fd);
	if(!handle)
		return EBADF;

	if(!size)
		return 0;

	auto chunk = clamp_io_size(size);
	dword written = 0;
	if(!WriteFile(handle, buffer, chunk, &written, nullptr))
		return win32_error_to_errno(GetLastError());

	*bytes_written = written;
	return 0;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	*new_offset = 0;
	auto handle = get_handle(fd);
	if(!handle)
		return EBADF;

	dword move_method;
	if(!seek_method_for_whence(whence, &move_method))
		return EINVAL;

	large_integer result;
	if(!SetFilePointerEx(handle, {offset}, &result, move_method))
		return win32_error_to_errno(GetLastError());

	*new_offset = result.quad_part;
	return 0;
}

int Sysdeps<Close>::operator()(int fd) {
	auto handle = get_handle(fd);
	if(!handle)
		return EBADF;

	if(!CloseHandle(handle))
		return win32_error_to_errno(GetLastError());

	file_table[fd] = nullptr;
	return 0;
}

int Sysdeps<Isatty>::operator()(int fd) {
	auto handle = get_handle(fd);
	if(!handle)
		return EBADF;

	dword mode;
	return GetConsoleMode(handle, &mode) ? 0 : ENOTTY;
}

} // namespace mlibc
