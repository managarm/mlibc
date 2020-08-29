#ifndef MLIBC_FSFD_TARGET
#define MLIBC_FSFD_TARGET

namespace mlibc {

enum class fsfd_target {
	none,
	path,
	fd,
	fd_path
};

} // namespace mlibc

#endif // MLIBC_FSFD_TARGET
