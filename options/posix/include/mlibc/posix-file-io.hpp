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

struct buf_file : abstract_file {
	buf_file(char *ptr, size_t size, void (*do_dispose)(abstract_file *) = nullptr);

	int close() override;
protected:
	int determine_type(stream_type *type) override;
	int determine_bufmode(buffer_mode *mode) override;

	int io_read(char *buffer, size_t max_size, size_t *actual_size) override;
	int io_write(const char *buffer, size_t max_size, size_t *actual_size) override;
	int io_seek(off_t offset, int whence, off_t *new_offset) override;

private:
	char *_buf;
	size_t _size;
	size_t _pos;
	bool _was_allocated;

	void (*_do_dispose)(abstract_file *);
};

} // namespace mlibc

#endif // MLIBC_POSIX_FILE_IO_HPP
