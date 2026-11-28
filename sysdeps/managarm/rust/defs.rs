use crate::prelude::*;
use crate::*;

pub type __u8 = c_uchar;
pub type __u16 = c_ushort;
pub type __s16 = c_short;
pub type __u32 = c_uint;
pub type __s32 = c_int;
pub type __u64 = c_ulonglong;
pub type __s64 = c_longlong;

pub const PTHREAD_COND_INITIALIZER: pthread_cond_t = pthread_cond_t {
    opaque: [0; 8],
};

pub const PTHREAD_MUTEX_INITIALIZER: pthread_mutex_t = pthread_mutex_t {
    opaque: [0; 8],
};

s! {
    pub struct siginfo_t {
        pub si_signo: crate::c_int,
        pub si_errno: crate::c_int,
        pub si_code: crate::c_int,
        pub _pad: [c_int; 29],
        _align: [u64; 0],
    }

	#[allow(unpredictable_function_pointer_comparisons)]
    pub struct sigaction {
        pub sa_sigaction: crate::sighandler_t,
        pub sa_flags: c_int,
        pub sa_restorer: Option<extern "C" fn()>,
        pub sa_mask: crate::sigset_t,
    }
}

pub type blkcnt64_t = i64;
pub type rlimit64 = crate::rlimit;
pub type rlim64_t = crate::rlim_t;
pub type dirent64 = crate::dirent;
pub type stat64 = crate::stat;
pub type statfs64 = crate::statfs;
pub type statvfs64 = crate::statvfs;
pub type idtype_t = c_uint;
pub type Ioctl = c_ulong;
pub type pthread_t = *mut c_void;
