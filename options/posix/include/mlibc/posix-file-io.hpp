#ifndef MLIBC_POSIX_FILE_IO_HPP
#define MLIBC_POSIX_FILE_IO_HPP

#include <frg/vector.hpp>
#include <mlibc/file-io.hpp>
#include <mlibc/allocator.hpp>

namespace mlibc {

struct mem_file : abstract_file {
	mem_file(char **ptr, size_t *sizeloc, void (*do_dispose)(abstract_file *) = nullptr);

	int close() override;
	int reopen(const char *path, const char *mode) override;
protected:
	int determine_type(stream_type *type) override;
	int determine_bufmode(buffer_mode *mode) override;

	int io_read(char *buffer, size_t max_size, size_t *actual_size) override;
	int io_write(const char *buffer, size_t max_size, size_t *actual_size) override;
	int io_seek(off_t offset, int whence, off_t *new_offset) override;
private:
	void _update_ptrs();
	// Where to write back buffer and size on flush and close.
	char **_bufloc;
	size_t *_sizeloc;

	// Actual buffer.
	frg::vector<char, MemoryAllocator> _buf;
	size_t _pos;
};

struct cookie_file : abstract_file {
	cookie_file(void *cookie, int flags, cookie_io_functions_t funcs, void (*do_dispose)(abstract_file *) = nullptr)
		: abstract_file{do_dispose}, _cookie{cookie}, _flags{flags}, _funcs{funcs} { }

	int close() override;
	int reopen(const char *path, const char *mode) override;
protected:
	int determine_type(stream_type *type) override;
	int determine_bufmode(buffer_mode *mode) override;

	int io_read(char *buffer, size_t max_size, size_t *actual_size) override;
	int io_write(const char *buffer, size_t max_size, size_t *actual_size) override;
	int io_seek(off_t offset, int whence, off_t *new_offset) override;

private:
	void *_cookie;

	int _flags;
	cookie_io_functions_t _funcs;
};

} // namespace mlibc

#endif // MLIBC_POSIX_FILE_IO_HPP
