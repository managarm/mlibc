#ifndef _ASM_IOCTLS_H
#define _ASM_IOCTLS_H

/* List of all the IOCTLs supported, for further explanation on the meanings */
/* please refer to documentation. If you did not get any, good luck! */
/* Some IOCTL codes may be the same, in which case the device they are used in */
/* gives them meaning. Cross-device IOCTLs have more distinct values. */

#define PS2MOUSE_2_1_SCALING     1
#define PS2MOUSE_1_1_SCALING     2
#define PS2MOUSE_SET_RES         3
#define PS2MOUSE_SET_SAMPLE_RATE 4
#define RTC_RD_TIME              1
#define RTC_SET_TIME             2
#define FIOQSIZE                 0x5460
#define TCGETS                   0x5401
#define TCSETS                   0x5402
#define TCSETSW                  0x5403
#define TCSETSF                  0x5404
#define TCGETA                   0x5405
#define TCSETA                   0x5406
#define TCSETAW                  0x5407
#define TCSETAF                  0x5408
#define TCSBRK                   0x5409
#define TCXONC                   0x540A
#define TCFLSH                   0x540B
#define TIOCEXCL                 0x540C
#define TIOCNXCL                 0x540D
#define TIOCSCTTY                0x540E
#define TIOCGPGRP                0x540F
#define TIOCSPGRP                0x5410
#define TIOCOUTQ                 0x5411
#define TIOCSTI                  0x5412
#define TIOCGWINSZ               0x5413
#define TIOCSWINSZ               0x5414
#define TIOCMGET                 0x5415
#define TIOCMBIS                 0x5416
#define TIOCMBIC                 0x5417
#define TIOCMSET                 0x5418
#define TIOCGSOFTCAR             0x5419
#define TIOCSSOFTCAR             0x541A
#define FIONREAD                 0x541B
#define TIOCINQ                  FIONREAD
#define TIOCLINUX                0x541C
#define TIOCCONS                 0x541D
#define TIOCGSERIAL              0x541E
#define TIOCSSERIAL              0x541F
#define TIOCPKT                  0x5420
#define FIONBIO                  0x5421
#define TIOCNOTTY                0x5422
#define TIOCSETD                 0x5423
#define TIOCGETD                 0x5424
#define TCSBRKP                  0x5425
#define TIOCSBRK                 0x5427
#define TIOCCBRK                 0x5428
#define TIOCGSID                 0x5429
#define TCGETS2                  3
#define TCSETS2                  3
#define TCSETSW2                 3
#define TCSETSF2                 3
#define TIOCGRS485               0x542E
#define TIOCSRS485               0x542F
#define TIOCGPTN                 3
#define TIOCSPTLCK               3
#define TIOCGDEV                 3
#define TCGETX                   0x5432
#define TCSETX                   0x5433
#define TCSETXF                  0x5434
#define TCSETXW                  0x5435
#define TIOCSIG                  0x36
#define TIOCVHANGUP              0x5437
#define TIOCGPKT                 3
#define TIOCGPTLCK               3
#define TIOCGEXCL                3
#define TIOCGPTPEER              3
#define TIOCGISO7816             3
#define TIOCSISO7816             3
#define FIONCLEX                 0x5450
#define FIOCLEX                  0x5451
#define FIOASYNC                 0x5452
#define TIOCSERCONFIG            0x5453
#define TIOCSERGWILD             0x5454
#define TIOCSERSWILD             0x5455
#define TIOCGLCKTRMIOS           0x5456
#define TIOCSLCKTRMIOS           0x5457
#define TIOCSERGSTRUCT           0x5458
#define TIOCSERGETLSR            0x5459
#define TIOCSERGETMULTI          0x545A
#define TIOCSERSETMULTI          0x545B
#define TIOCMIWAIT               0x545C
#define TIOCGICOUNT              0x545D
#define TIOCPKT_DATA             0
#define TIOCPKT_FLUSHREAD        1
#define TIOCPKT_FLUSHWRITE       2
#define TIOCPKT_STOP             4
#define TIOCPKT_START            8
#define TIOCPKT_NOSTOP           16
#define TIOCPKT_DOSTOP           32
#define TIOCPKT_IOCTL            64
#define TIOCSER_TEMT	            0x01

#endif /* _ASM_IOCTLS_H */
