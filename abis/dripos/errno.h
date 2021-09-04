#ifndef _ABIBITS_ERRNO_H
#define _ABIBITS_ERRNO_H

#define	EPERM		1		/* Operation not permitted */
#define	ENOENT		2		/* No such file or directory */
#define	ESRCH		3		/* No such process */
#define	EINTR		4		/* Interrupted system call */
#define	EIO		    5		/* Input/output error */
#define	ENXIO		6		/* Device not configured */
#define	E2BIG		7		/* Argument list too long */
#define	ENOEXEC		8		/* Exec format error */
#define	EBADF		9		/* Bad file descriptor */
#define	ECHILD		10		/* No child processes */
#define	EDEADLK		11		/* Resource deadlock avoided */
					/* 11 was EAGAIN */

#define	ENOMEM		12		/* Cannot allocate memory */
#define	EACCES		13		/* Permission denied */
#define	EFAULT		14		/* Bad address */

#define	ENOTBLK		15		/* Block device required */
#define	EBUSY		16		/* Device busy */

#define	EEXIST		17		/* File exists */
#define	EXDEV		18		/* Cross-device link */
#define	ENODEV		19		/* Operation not supported by device */
#define	ENOTDIR		20		/* Not a directory */
#define	EISDIR		21		/* Is a directory */
#define	EINVAL		22		/* Invalid argument */
#define	ENFILE		23		/* Too many open files in system */
#define	EMFILE		24		/* Too many open files */
#define	ENOTTY		25		/* Inappropriate ioctl for device */

#define	ETXTBSY		26		/* Text file busy */

#define	EFBIG		27		/* File too large */
#define	ENOSPC		28		/* No space left on device */
#define	ESPIPE		29		/* Illegal seek */
#define	EROFS		30		/* Read-only file system */
#define	EMLINK		31		/* Too many links */
#define	EPIPE		32		/* Broken pipe */

/* math software */
#define	EDOM		33		/* Numerical argument out of domain */
#define	ERANGE		34		/* Result too large */

/* non-blocking and interrupt i/o */
#define	EAGAIN		35		/* Resource temporarily unavailable */

#define	EWOULDBLOCK	EAGAIN		/* Operation would block */
#define	EINPROGRESS	36		/* Operation now in progress */
#define	EALREADY	37		/* Operation already in progress */

/* ipc/network software -- argument errors */
#define	ENOTSOCK	38		/* Socket operation on non-socket */
#define	EDESTADDRREQ	39		/* Destination address required */
#define	EMSGSIZE	40		/* Message too long */
#define	EPROTOTYPE	41		/* Protocol wrong type for socket */
#define	ENOPROTOOPT	42		/* Protocol not available */
#define	EPROTONOSUPPORT	43		/* Protocol not supported */
#define	ESOCKTNOSUPPORT	44		/* Socket type not supported */
#define	EOPNOTSUPP	45		/* Operation not supported on socket */
#define	EPFNOSUPPORT	46		/* Protocol family not supported */
#define	EAFNOSUPPORT	47		/* Address family not supported by protocol family */
#define	EADDRINUSE	48		/* Address already in use */
#define	EADDRNOTAVAIL	49		/* Can't assign requested address */

/* ipc/network software -- operational errors */
#define	ENETDOWN	50		/* Network is down */
#define	ENETUNREACH	51		/* Network is unreachable */
#define	ENETRESET	52		/* Network dropped connection on reset */
#define	ECONNABORTED	53		/* Software caused connection abort */
#define	ECONNRESET	54		/* Connection reset by peer */
#define	ENOBUFS		55		/* No buffer space available */
#define	EISCONN		56		/* Socket is already connected */
#define	ENOTCONN	57		/* Socket is not connected */
#define	ESHUTDOWN	58		/* Can't send after socket shutdown */
#define	ETOOMANYREFS	59		/* Too many references: can't splice */
#define	ETIMEDOUT	60		/* Connection timed out */
#define	ECONNREFUSED	61		/* Connection refused */

#define	ELOOP		62		/* Too many levels of symbolic links */

#define	ENAMETOOLONG	63		/* File name too long */

/* should be rearranged */
#define	EHOSTDOWN	64		/* Host is down */
#define	EHOSTUNREACH	65		/* No route to host */
#define	ENOTEMPTY	66		/* Directory not empty */

/* quotas & mush */
#define	EPROCLIM	67		/* Too many processes */
#define	EUSERS		68		/* Too many users */
#define	EDQUOT		69		/* Disc quota exceeded */

/* Network File System */
#define	ESTALE		70		/* Stale NFS file handle */
#define	EREMOTE		71		/* Too many levels of remote in path */
#define	EBADRPC		72		/* RPC struct is bad */
#define	ERPCMISMATCH	73		/* RPC version wrong */
#define	EPROGUNAVAIL	74		/* RPC prog. not avail */
#define	EPROGMISMATCH	75		/* Program version wrong */
#define	EPROCUNAVAIL	76		/* Bad procedure for program */


#define	ENOLCK		77		/* No locks available */
#define	ENOSYS		78		/* Function not implemented */

#define	EFTYPE		79		/* Inappropriate file type or format */
#define EOVERFLOW   80
#define EILSEQ      81

#define EBADMSG			82
#define ECANCELED		83
#define EIDRM			84
#define EMULTIHOP		85
#define ENOLINK			86
#define ENOMSG			87
#define ENOTRECOVERABLE	88
#define ENOTSUP			89
#define EOWNERDEAD		90
#define EPROTO			91
#define ENODATA			92
#define ETIME			93
#define ENOKEY			94
#define EBADFD			95
#define ENOMEDIUM		96
#define ENONET			97
#define ESTRPIPE		98
#define EREMOTEIO		99
#define ERFKILL			100
#define EBADR			101
#define EUNATCH			102
#define EMEDIUMTYPE		103
#define EKEYREJECTED	104
#define EUCLEAN			105
#define EBADSLT			106
#define ENOANO			107
#define ENOCSI			108
#define ENOSTR			109
#define ENOPKG			110
#define EKEYREVOKED		111
#define EXFULL			112
#define ELNRNG			113
#define ENOTUNIQ		114
#define ERESTART		115
#define ENOTSUP			116

#endif
