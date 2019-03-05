
#include <abi-bits/abi.h>
#include <bits/ensure.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <frg/vector.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

namespace {
	constexpr bool debugPathResolution = false;
}

long random(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// ----------------------------------------------------------------------------
// Path handling.
// ----------------------------------------------------------------------------

int mkstemp(char *pattern) {
	auto n = strlen(pattern);
	__ensure(n >= 6);
	if(n < 6) {
		errno = EINVAL;
		return -1;
	}
	for(size_t i = 0; i < 6; i++) {
		if(pattern[n - 6 + i] == 'X')
			continue;
		errno = EINVAL;
		return -1;
	}

	// TODO: Do an exponential search.
	for(size_t i = 0; i < 999999; i++) {
		__ensure(sprintf(pattern + (n - 6), "%06zu", i) == 6);
//		mlibc::infoLogger() << "mlibc: mkstemp candidate is "
//				<< (const char *)pattern << frg::endlog;

		// TODO: Add a mode argument to sys_open().
		int fd;
		if(int e = mlibc::sys_open(pattern, O_RDWR | O_CREAT | O_EXCL, /*S_IRUSR | S_IWUSR,*/ &fd); !e) {
			return fd;
		}else if(e != EEXIST) {
			errno = e;
			return -1;
		}
	}

	errno = EEXIST;
	return -1;
}

char *mkdtemp(char *path) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *realpath(const char *path, char *out) {
	if(debugPathResolution)
		mlibc::infoLogger() << "mlibc realpath(): Called on '" << path << "'" << frg::endlog;
	frg::string_view path_view{path};

	// TODO: Replace by a small_vector to avoid allocations.
	// In case of the root, the string only contains the null-terminator.
	frg::vector<char, MemoryAllocator> resolv{getAllocator()};
	size_t ps;

	// If the path is relative, we have to preprend the working directory.
	if(path[0] == '/') {
		resolv.push_back(0);
		ps = 1;
	}else{
		if(!mlibc::sys_getcwd) {
			MLIBC_MISSING_SYSDEP();
			errno = ENOSYS;
			return nullptr;
		}

		// Try to getcwd() until the buffer is large enough.
		resolv.resize(128);
		while(true) {
			int e = mlibc::sys_getcwd(resolv.data(), resolv.size());
			if(e == ERANGE) {
				resolv.resize(2 * resolv.size());
			}else if(!e) {
				break;
			}else{
				errno = e;
				return nullptr;
			}
		}
		frg::string_view cwd_view{resolv.data()};
		if(cwd_view == "/") {
			// Restore our invariant that we only store the null-terminator for the root.
			resolv.resize(1);
			resolv[0] = 0;
		}else{
			resolv.resize(cwd_view.size() + 1);
		}
		ps = 0;
	}

	// TODO: Replace by a small_vector to avoid allocations.
	// Contains unresolved links as a relative path compared to resolv.
	frg::vector<char, MemoryAllocator> lnk{getAllocator()};
	size_t ls = 0;

	auto process_segment = [&] (frg::string_view s_view) -> int {
		if(debugPathResolution)
			mlibc::infoLogger() << "mlibc realpath(): resolv is '" << resolv.data() << "'"
					<< ", segment is " << s_view.data()
					<< ", size: " << s_view.size() << frg::endlog;

		if(!s_view.size() || s_view == ".") {
			// Keep resolv invariant.
			return 0;
		}else if(s_view == "..") {
			// Remove a single segment from resolv.
			if(resolv.size() > 1) {
				auto slash = strchr(resolv.data(), '/');
				__ensure(slash); // We never remove the leading sla.
				resolv.resize((slash - resolv.data()) + 1);
				*slash = 0; // Replace the slash by a null-terminator.
			}
			return 0;
		}

		// Append the segment to resolv.
		auto rsz = resolv.size();
		resolv[rsz - 1] = '/'; // Replace null-terminator by a slash.
		resolv.resize(rsz + s_view.size() + 1);
		memcpy(resolv.data() + rsz, s_view.data(), s_view.size());
		resolv[rsz + s_view.size()] = 0;

		// stat() the path to (1) see if it exists and (2) see if it is a link.
		if(!mlibc::sys_stat) {
			MLIBC_MISSING_SYSDEP();
			errno = ENOSYS;
			return -1;
		}
		if(debugPathResolution)
			mlibc::infoLogger() << "mlibc realpath(): stat()ing '"
					<< resolv.data() << "'" << frg::endlog;
		struct stat st;
		if(int e = mlibc::sys_stat(mlibc::fsfd_target::path,
				-1, resolv.data(), AT_SYMLINK_NOFOLLOW, &st); e)
			return e;

		__ensure(!S_ISLNK(st.st_mode) && "TODO: Use readlink in realpath()");
		// TODO: If it is a link, prepend it to lnk, adjust ls and revert the change to resolv.
		return 0;
	};

	// Each iteration of this outer loop consumes segment of the input path.
	// This design avoids copying the input path into lnk;
	// the latter could often involve additional allocations.
	while(ps < path_view.size()) {
		frg::string_view ps_view;
		if(auto slash = strchr(path + ps, '/'); slash) {
			ps_view = frg::string_view{path + ps, slash - (path + ps)};
		}else{
			ps_view = frg::string_view{path + ps, strlen(path) - ps};
		}
		ps += ps_view.size() + 1;

		// Handle one segment from the input path.
		if(int e = process_segment(ps_view); e) {
			errno = e;
			return nullptr;
		}

		// This inner loop consumes segments of lnk.
		while(ls < lnk.size()) {
			frg::string_view ls_view;
			if(auto slash = strchr(lnk.data() + ls, '/'); slash) {
				ls_view = frg::string_view{lnk.data() + ls, slash - (lnk.data() + ls)};
			}else{
				ls_view = frg::string_view{lnk.data() + ls, strlen(lnk.data()) - ls};
			}
			ls += ls_view.size() + 1;

			// Handle one segment from the link
			if(int e = process_segment(ls_view); e) {
				errno = e;
				return nullptr;
			}
		}
	}

	if(debugPathResolution)
		mlibc::infoLogger() << "mlibc realpath(): Returns '" << resolv.data() << "'" << frg::endlog;

	if(resolv.size() > PATH_MAX) {
		errno = ENAMETOOLONG;
		return nullptr;
	}

	if(!out)
		out = reinterpret_cast<char *>(getAllocator().allocate(resolv.size()));
	strcpy(out, resolv.data());
	return out;
}

