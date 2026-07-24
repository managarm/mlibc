#ifndef _ABIBITS_PATHS_H
#define _ABIBITS_PATHS_H

#define	_PATH_BSHELL	"/bin/sh"
#define	_PATH_CONSOLE	"/device/console"
#define	_PATH_DEVNULL	"/device/null"
#define _PATH_GSHADOW	"/etc/gshadow"
#define	_PATH_KLOG	"/device/kconsole"
#define	_PATH_LASTLOG	"/var/log/lastlog"
#define	_PATH_MAILDIR	"/var/mail"
#define	_PATH_MAN	"/usr/share/man"
#define	_PATH_MNTTAB	"/etc/fstab"
#define	_PATH_MOUNTED	"/etc/mtab"
#define	_PATH_NOLOGIN	"/etc/nologin"
#define _PATH_PRESERVE	"/var/lib"
#define	_PATH_SENDMAIL	"/usr/sbin/sendmail"
#define	_PATH_SHADOW	"/etc/shadow"
#define	_PATH_SHELLS	"/etc/shells"
#define	_PATH_TTY	"/device/tty"
#define _PATH_UTMP	"/tmp"
#define	_PATH_VI	"/usr/bin/vi"
#define _PATH_WTMP	"/tmp"

#define	_PATH_DEV	"/device/"
#define	_PATH_TMP	"/tmp/"
#define	_PATH_VARDB	"/tmp/"
#define	_PATH_VARRUN	"/tmp/"
#define	_PATH_VARTMP	"/tmp/"

#ifdef _GNU_SOURCE
#define _PATH_UTMPX _PATH_UTMP
#define _PATH_WTMPX _PATH_WTMP

#endif

#endif