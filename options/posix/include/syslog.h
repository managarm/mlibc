
#ifndef _SYSLOG_H
#define _SYSLOG_H

#include <stdarg.h>
#include <stddef.h>
#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_PID 0x01
#define LOG_CONS 0x02
#define LOG_NDELAY 0x08
#define LOG_ODELAY 0x04
#define LOG_NOWAIT 0x10
#define LOG_PERROR 0x20

#define LOG_KERN (0<<3)
#define LOG_USER (1<<3)
#define LOG_MAIL (2<<3)
#define LOG_DAEMON (3<<3)
#define LOG_AUTH (4<<3)
#define LOG_SYSLOG (5<<3)
#define LOG_LPR (6<<3)
#define LOG_NEWS (7<<3)
#define LOG_UUCP (8<<3)
#define LOG_CRON (9<<3)
#define LOG_AUTHPRIV (10<<3)
#define LOG_FTP (11<<3)

#define LOG_LOCAL0 (16<<3)
#define LOG_LOCAL1 (17<<3)
#define LOG_LOCAL2 (18<<3)
#define LOG_LOCAL3 (19<<3)
#define LOG_LOCAL4 (20<<3)
#define LOG_LOCAL5 (21<<3)
#define LOG_LOCAL6 (22<<3)
#define LOG_LOCAL7 (23<<3)

#define LOG_PRIMASK 7
#define LOG_PRI(p) ((p)&LOG_PRIMASK)
#define LOG_MAKEPRI(f, p) (((f)<<3) | (p))
#define LOG_MASK(p) (1<<(p))
#define LOG_UPTO(p) ((1<<((p)+1))-1)
#define LOG_NFACILITIES 24
#define LOG_FACMASK (0x7F<<3)
#define LOG_FAC(p) (((p)&LOG_FACMASK)>>3)

#define LOG_EMERG 0
#define LOG_ALERT 1
#define LOG_CRIT 2
#define LOG_ERR 3
#define LOG_WARNING 4
#define LOG_NOTICE 5
#define LOG_INFO 6
#define LOG_DEBUG 7

#if __MLIBC_BSD_OPTION
#if defined(SYSLOG_NAMES)
#define INTERNAL_NOPRI 0x10
#define INTERNAL_MARK LOG_MAKEPRI(LOG_NFACILITIES << 3, 0)

typedef struct _code {
	char *c_name;
	int c_val;
} CODE;
#endif /* SYSLOG_NAMES */
#endif /* __MLIBC_BSD_OPTION */

#ifndef __MLIBC_ABI_ONLY

#if __MLIBC_BSD_OPTION
#if defined(SYSLOG_NAMES)
CODE prioritynames[] = {
	{ "alert", LOG_ALERT },
	{ "crit", LOG_CRIT },
	{ "debug", LOG_DEBUG },
	{ "emerg", LOG_EMERG },
	{ "err", LOG_ERR },
	{ "error", LOG_ERR },
	{ "info", LOG_INFO },
	{ "none", INTERNAL_NOPRI },
	{ "notice", LOG_NOTICE },
	{ "panic", LOG_EMERG },
	{ "warn", LOG_WARNING },
	{ "warning", LOG_WARNING },
	{ NULL, -1 }
};

CODE facilitynames[] = {
	{ "auth", LOG_AUTH },
	{ "authpriv", LOG_AUTHPRIV },
	{ "cron", LOG_CRON },
	{ "daemon", LOG_DAEMON },
	{ "ftp", LOG_FTP },
	{ "kern", LOG_KERN },
	{ "lpr", LOG_LPR },
	{ "mail", LOG_MAIL },
	{ "mark", INTERNAL_MARK },
	{ "news", LOG_NEWS },
	{ "security", LOG_AUTH },
	{ "syslog", LOG_SYSLOG },
	{ "user", LOG_USER },
	{ "uucp", LOG_UUCP },
	{ "local0", LOG_LOCAL0 },
	{ "local1", LOG_LOCAL1 },
	{ "local2", LOG_LOCAL2 },
	{ "local3", LOG_LOCAL3 },
	{ "local4", LOG_LOCAL4 },
	{ "local5", LOG_LOCAL5 },
	{ "local6", LOG_LOCAL6 },
	{ "local7", LOG_LOCAL7 },
	{ NULL, -1 }
};
#endif /* SYSLOG_NAMES */
#endif /* __MLIBC_BSD_OPTION */

void closelog(void);
void openlog(const char *__ident, int __option, int __facility);
int setlogmask(int __mask);
__attribute__((__format__(__printf__, 2, 3)))
void syslog(int __priority, const char *__format, ...);

/* This is a linux extension */
__attribute__((__format__(__printf__, 2, 0)))
void vsyslog(int __priority, const char *__format, va_list __args);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYSLOG_H */

