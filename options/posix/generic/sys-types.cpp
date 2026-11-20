#include <abi-bits/blkcnt_t.h>
#include <abi-bits/blksize_t.h>
#include <abi-bits/dev_t.h>
#include <abi-bits/fsblkcnt_t.h>
#include <abi-bits/fsfilcnt_t.h>
#include <abi-bits/gid_t.h>
#include <abi-bits/ino_t.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/nlink_t.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/uid_t.h>
#include <bits/ansi/clock_t.h>
#include <bits/ansi/time_t.h>
#include <bits/off_t.h>
#include <bits/posix/id_t.h>
#include <bits/reclen_t.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <concepts>
#include <limits>

template <typename T>
concept PosixInteger = std::integral<T> && !std::same_as<std::remove_cv_t<T>, bool>;

template <typename T>
concept PosixSignedInteger = PosixInteger<T> && std::signed_integral<T>;

template <typename T>
concept PosixUnsignedInteger = PosixInteger<T> && std::unsigned_integral<T>;

// mode_t shall be an integer type.
static_assert(PosixInteger<mode_t>);

// dev_t shall be an integer type.
static_assert(PosixInteger<dev_t>);

// nlink_t, uid_t, gid_t, and id_t shall be integer types.
static_assert(PosixInteger<nlink_t>);
static_assert(PosixInteger<uid_t>);
static_assert(PosixInteger<gid_t>);
static_assert(PosixInteger<id_t>);

// blkcnt_t and off_t shall be signed integer types.
static_assert(PosixSignedInteger<blkcnt_t> && std::is_signed_v<blkcnt_t>);
static_assert(PosixSignedInteger<off_t> && std::is_signed_v<off_t>);

// fsblkcnt_t, fsfilcnt_t, reclen_t, and ino_t shall be defined as unsigned integer types.
static_assert(PosixUnsignedInteger<fsblkcnt_t>);
static_assert(PosixUnsignedInteger<fsfilcnt_t>);
static_assert(PosixUnsignedInteger<reclen_t>);
static_assert(PosixUnsignedInteger<ino_t>);

// size_t shall be an unsigned integer type.
static_assert(PosixUnsignedInteger<size_t>);

// blksize_t, pid_t, and ssize_t shall be signed integer types.
static_assert(PosixSignedInteger<blksize_t>);
static_assert(PosixSignedInteger<pid_t>);
static_assert(PosixSignedInteger<ssize_t>);

// clock_t shall be an integer or real-floating type.
static_assert(PosixInteger<clock_t> || std::is_floating_point_v<clock_t>);

// time_t shall be an integer type with a width (see <stdint.h>) of at least 64 bits.
static_assert(
    PosixInteger<time_t> && (std::numeric_limits<time_t>::digits + std::is_signed_v<time_t> >= 64)
);
