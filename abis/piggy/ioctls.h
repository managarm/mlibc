#ifndef _ABIBITS_IOCTLS_H
#define _ABIBITS_IOCTLS_H

#define HWCLKGTM        0x1001
#define HWCLKSTM        0x1002

#define TCGETS          0x5401
#define TCSETS          0x5402
#define TCSETSW         0x5403
#define TCSETSF         0x5404
#define TIOCGPGRP       0x540F
#define TIOCSPGRP       0x5410
#define TIOCGWINSZ      0x5413
#define TIOCSWINSZ      0x5414

#define FIONREAD        0x541B

#define SIOCGIFNAME     0x8910
#define SIOCGIFCONF     0x8912
#define SIOCGIFFLAGS    0x8913
#define SIOCSIFFLAGS    0x8914
#define SIOCGIFMTU      0x8921
#define SIOCSIFMTU      0x8922
#define SIOCGIFADDR     0x8923
#define SIOCSIFADDR     0x8924
#define SIOCGIFNETMASK  0x8925
#define SIOCSIFNETMASK  0x8926
#define SIOCGIFBRDADDR  0x8927
#define SIOCSIFBRDADDR  0x8928
#define SIOCGIFHWADDR   0x8929
#define SIOCGIFINDEX    0x8933
#define SIOCADDRT       0x8958
#define SIOCDELRT       0x8959
#define SIOCIFBIND      0x895A

#endif /* _ABIBITS_IOCTLS_H */
