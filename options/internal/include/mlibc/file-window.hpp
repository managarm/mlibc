#ifndef MLIBC_FILE_WINDOW
#define MLIBC_FILE_WINDOW

#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>
#include <internal-config.h>

struct file_window {
	file_window(const char *path) {
		int fd;
		if(mlibc::sys_open("/etc/localtime", O_RDONLY, &fd))
			mlibc::panicLogger() << "mlibc: Error opening file_window to "
					<< path << frg::endlog;

		struct stat info;
		if(mlibc::sys_fstat(fd, &info))
			mlibc::panicLogger() << "mlibc: Error getting TZinfo stats" << frg::endlog;

#ifdef MLIBC_MAP_FILE_WINDOWS
		if(mlibc::sys_vm_map(nullptr, (size_t)info.st_size, PROT_READ, MAP_PRIVATE,
				fd, 0, &_ptr))
			mlibc::panicLogger() << "mlibc: Error mapping TZinfo" << frg::endlog;
#else
		_ptr = getAllocator().allocate(info.st_size);
		__ensure(_ptr);

		size_t progress = 0;
		while(progress < info.st_size) {
			ssize_t chunk;
			if(int e = mlibc::sys_read(fd, reinterpret_cast<char *>(_ptr) + progress,
					info.st_size - progress, &chunk); e)
				mlibc::panicLogger() << "mlibc: Read from file_window failed" << frg::endlog;
			if(!chunk)
				break;
			progress += chunk;
		}
		if(progress != info.st_size)
			mlibc::panicLogger() << "stat reports " << info.st_size << " but we only read "
					<< progress << " bytes" << frg::endlog;
#endif

		if(mlibc::sys_close(fd))
			mlibc::panicLogger() << "mlibc: Error closing TZinfo" << frg::endlog;
	}

	// TODO: Write destructor to deallocate/unmap memory.

	void *get() {
		return _ptr;
	}

private:
	void *_ptr;
};

#endif // MLIBC_FILE_WINDOW

