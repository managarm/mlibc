
#ifndef _LINUX_KD_H
#define _LINUX_KD_H

#define KDSETMODE 0x4B3A
#define KDSKBMODE 0x4B45
#define KDGETMODE 0x4B3B
#define KDGKBMODE 0x4B45

#define KD_TEXT 0x00
#define KD_GRAPHICS 0x01

#define K_RAW 0x00
#define K_UNICODE 0x03
#define K_OFF 0x04

#endif // _LINUX_KD_H

