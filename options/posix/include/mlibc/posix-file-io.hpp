#ifndef MLIBC_POSIX_FILE_IO_HPP
#define MLIBC_POSIX_FILE_IO_HPP

#include <frg/span.hpp>
#include <frg/vector.hpp>
#include <mlibc/file-io.hpp>
#include <mlibc/allocator.hpp>

namespace mlibc {

struct mem_file : abstract_file {
	mem_file(int flags, void (*do_dispose)(abstract_file *) = nullptr) : abstract_file{do_dispose}, _flags{flags} { };

	int reopen(const char *path, const char *mode) override;
protected:
	int determine_type(stream_type *type) override;
	int determine_bufmode(buffer_mode *mode) override;

	virtual frg::span<char> _buffer() = 0;
	virtual size_t _buffer_size() const = 0;

	off_t _pos = 0;
	int _flags = 0;
	// maintains the size of buffer contents as required by POSIX
	off_t _max_size = 0;
};

struct memstream_mem_file final : public mem_file {
	memstream_mem_file(char **ptr, size_t *sizeloc, int flags, void (*do_dispose)(abstract_file *) = nullptr);

	int close() override;

	int io_read(char *buffer, size_t max_size, size_t *actual_size) override;
	int io_write(const char *buffer, size_t max_size, size_t *actual_size) override;
	int io_seek(off_t offset, int whence, off_t *new_offset) override;

	frg::span<char> _buffer() override {
		return {_buf.data(), _buffer_size()};
	}

	size_t _buffer_size() const override {
		return _buf.size();
	}

private:
	void _update_ptrs();

	// Where to write back buffer and size on flush and close.
	char **_bufloc;
	size_t *_sizeloc;

	frg::vector<char, MemoryAllocator> _buf = {getAllocator()};
};

struct fmemopen_mem_file final : public mem_file {
	fmemopen_mem_file(void *in_buf, size_t size, int flags, void (*do_dispose)(abstract_file *) = nullptr);

	int close() override;

	int io_read(char *buffer, size_t max_size, size_t *actual_size) override;
	int io_write(const char *buffer, size_t max_size, size_t *actual_size) override;
	int io_seek(off_t offset, int whence, off_t *new_offset) override;

	frg::span<char> _buffer() override {
		return {reinterpret_cast<char *>(_inBuffer), _buffer_size()};
	}

	size_t _buffer_size() const override {
		return _inBufferSize;
	}

private:
	void *_inBuffer;
	size_t _inBufferSize;

	bool _needsDeallocation = false;
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

	[[maybe_unused]] int _flags;
	cookie_io_functions_t _funcs;
};

} // namespace mlibc

#endif // MLIBC_POSIX_FILE_IO_HPP
