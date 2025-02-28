#ifndef _ABIBITS_SYSCONF_H
#define _ABIBITS_SYSCONF_H

/* Kernel-supported ABI SC flags */
#define _SC_PAGESIZE         1
#define _SC_OPEN_MAX         2
#define _SC_HOST_NAME_MAX    3
#define _SC_AVPHYS_PAGES     4
#define _SC_PHYS_PAGES       5
#define _SC_NPROCESSORS_ONLN 6
#define _SC_TOTAL_PAGES      7
#define _SC_LIST_PROCS       8
#define _SC_LIST_MOUNTS      9
#define _SC_UNAME            10
#define _SC_CHILD_MAX        11
#define _SC_LIST_THREADS     12
#define _SC_LIST_CLUSTERS    13
#define _SC_LIST_NETINTER    14
#define _SC_DUMPLOGS         15
#define _SC_NGROUPS_MAX      16
#define _SC_SYMLOOP_MAX      17

/* SC Aliases */
#define _SC_PAGE_SIZE _SC_PAGESIZE

/* The rest of libc-emulated SC macros */
/* MISSING: remaining _SC_macros */
#define _SC_ARG_MAX                      18
#define _SC_GETPW_R_SIZE_MAX             19
#define _SC_GETGR_R_SIZE_MAX             20
#define _SC_CLK_TCK                      21
#define _SC_VERSION                      22
#define _SC_SAVED_IDS                    23
#define _SC_JOB_CONTROL                  24
#define _SC_LINE_MAX                     25
#define _SC_XOPEN_CRYPT                  26
#define _SC_NPROCESSORS_CONF             27
#define _SC_TTY_NAME_MAX                 28
#define _SC_RE_DUP_MAX                   29
#define _SC_ATEXIT_MAX                   30
#define _SC_LOGIN_NAME_MAX               31
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 32
#define _SC_THREAD_KEYS_MAX              33
#define _SC_THREAD_STACK_MIN             34
#define _SC_THREAD_THREADS_MAX           35
#define _SC_TZNAME_MAX                   36
#define _SC_ASYNCHRONOUS_IO              37
#define _SC_FSYNC                        38
#define _SC_MAPPED_FILES                 39
#define _SC_MEMLOCK                      40
#define _SC_MEMLOCK_RANGE                41
#define _SC_MEMORY_PROTECTION            42
#define _SC_MESSAGE_PASSING              43
#define _SC_PRIORITY_SCHEDULING          44
#define _SC_REALTIME_SIGNALS             45
#define _SC_SEMAPHORES                   46
#define _SC_SHARED_MEMORY_OBJECTS        47
#define _SC_SYNCHRONIZED_IO              48
#define _SC_THREADS                      49
#define _SC_THREAD_ATTR_STACKADDR        50
#define _SC_THREAD_ATTR_STACKSIZE        51
#define _SC_THREAD_PRIORITY_SCHEDULING   52
#define _SC_THREAD_PRIO_INHERIT          53
#define _SC_THREAD_PRIO_PROTECT          54
#define _SC_THREAD_PROCESS_SHARED        55
#define _SC_THREAD_SAFE_FUNCTIONS        56
#define _SC_TIMERS                       57
#define _SC_TIMER_MAX                    58
#define _SC_2_CHAR_TERM                  59
#define _SC_2_C_BIND                     60
#define _SC_2_C_DEV                      61
#define _SC_2_FORT_DEV                   62
#define _SC_2_FORT_RUN                   63
#define _SC_2_LOCALEDEF                  64
#define _SC_2_SW_DEV                     65
#define _SC_2_UPE                        66
#define _SC_2_VERSION                    67
#define _SC_CLOCK_SELECTION              68
#define _SC_CPUTIME                      69
#define _SC_THREAD_CPUTIME               70
#define _SC_MONOTONIC_CLOCK              71
#define _SC_READER_WRITER_LOCKS          72
#define _SC_SPIN_LOCKS                   73
#define _SC_REGEXP                       74
#define _SC_SHELL                        75
#define _SC_SPAWN                        76
#define _SC_2_PBS                        78
#define _SC_2_PBS_ACCOUNTING             79
#define _SC_2_PBS_LOCATE                 80
#define _SC_2_PBS_TRACK                  81
#define _SC_2_PBS_MESSAGE                82
#define _SC_STREAM_MAX                   83
#define _SC_AIO_LISTIO_MAX               84
#define _SC_AIO_MAX                      85
#define _SC_DELAYTIMER_MAX               86
#define _SC_MQ_OPEN_MAX                  88
#define _SC_MQ_PRIO_MAX                  89
#define _SC_RTSIG_MAX                    90
#define _SC_SIGQUEUE_MAX                 91
#define _SC_IOV_MAX                      92

#endif
