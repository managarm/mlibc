#ifndef MLIBC_FILE_IO_HPP
#define MLIBC_FILE_IO_HPP

#include <stdio.h>

#include <mlibc/lock.hpp>
#include <mlibc/allocator.hpp>
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
	abstract_file(void (*do_dispose)(abstract_file *) = nullptr);

	abstract_file(const abstract_file &) = delete;

	abstract_file &operator= (const abstract_file &) = delete;

	virtual ~abstract_file();

	void dispose();

	virtual int close() = 0;
	virtual int reopen(const char *path, const char *mode) = 0;

	int read(char *buffer, size_t max_size, size_t *actual_size);
	int write(const char *buffer, size_t max_size, size_t *actual_size);
	int unget(char c);

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

	int _reset();
private:
	int _init_type();
	int _init_bufmode();

	int _write_back();
	int _save_pos();

	void _ensure_allocation();

	stream_type _type;
	buffer_mode _bufmode;
	void (*_do_dispose)(abstract_file *);

public:
	// lock for file operations
	RecursiveFutexLock _lock;
	// All files are stored in a global linked list, so that they can be flushed at exit().
	frg::default_list_hook<abstract_file> _list_hook;
};

struct fd_file : abstract_file {
	fd_file(int fd, void (*do_dispose)(abstract_file *) = nullptr, bool force_unbuffered = false);

	int fd();

	int close() override;
	int reopen(const char *path, const char *mode) override;

	static int parse_modestring(const char *mode);

protected:
	int determine_type(stream_type *type) override;
	int determine_bufmode(buffer_mode *mode) override;

	int io_read(char *buffer, size_t max_size, size_t *actual_size) override;
	int io_write(const char *buffer, size_t max_size, size_t *actual_size) override;
	int io_seek(off_t offset, int whence, off_t *new_offset) override;

private:
	// Underlying file descriptor.
	int _fd;
	bool _force_unbuffered;
};

template <typename T>
void file_dispose_cb(abstract_file *base) {
	frg::destruct(getAllocator(), static_cast<T *>(base));
}

} // namespace mlibc

#endif // MLIBC_FILE_IO_HPP
