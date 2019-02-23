#ifndef MLIBC_FILE_IO_HPP
#define MLIBC_FILE_IO_HPP

#include <stdio.h>

#include <frg/list.hpp>

namespace mlibc {

enum class stream_type {
	unknown,
	file_like,
	pipe_like
};

enum class buffer_mode {
	unknown,
	no_buffer,
	line_buffer,
	full_buffer
};

struct abstract_file : __mlibc_file_base {
public:
	abstract_file();

	abstract_file(const abstract_file &) = delete;

	abstract_file &operator= (const abstract_file &) = delete;

	virtual ~abstract_file();

	virtual int close() = 0;

	int read(char *buffer, size_t max_size, size_t *actual_size);
	int write(const char *buffer, size_t max_size, size_t *actual_size);

	int update_bufmode(buffer_mode mode);

	void purge();
	int flush();

	int tell(off_t *current_offset);
	int seek(off_t offset, int whence);

protected:
	virtual int determine_type(stream_type *type) = 0;
	virtual int determine_bufmode(buffer_mode *mode) = 0;
	virtual int io_read(char *buffer, size_t max_size, size_t *actual_size) = 0;
	virtual int io_write(const char *buffer, size_t max_size, size_t *actual_size) = 0;
	virtual int io_seek(off_t offset, int whence, off_t *new_offset) = 0;

private:
	int _init_type();
	int _init_bufmode();

	int _write_back();

	int _reset();
	void _ensure_allocation();

	stream_type _type;
	buffer_mode _bufmode;

public:
	// All files are stored in a global linked list, so that they can be flushed at exit().
	frg::default_list_hook<abstract_file> _list_hook;
};

struct fd_file : abstract_file {
	fd_file(int fd);

	int fd();

	int close() override;

protected:
	int determine_type(stream_type *type) override;
	int determine_bufmode(buffer_mode *mode) override;

	int io_read(char *buffer, size_t max_size, size_t *actual_size) override;
	int io_write(const char *buffer, size_t max_size, size_t *actual_size) override;
	int io_seek(off_t offset, int whence, off_t *new_offset) override;

private:
	// Underlying file descriptor.
	int _fd;
};

} // namespace mlibc

#endif // MLIBC_FILE_IO_HPP
