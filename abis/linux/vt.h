#ifndef _ABIBITS_VT_H
#define _ABIBITS_VT_H

#include <mlibc-config.h>

#if !__MLIBC_LINUX_OPTION
#  error "<linux/vt.h> is inherently Linux specific. Enable the Linux option or do not use this header."
#endif /* !__MLIBC_LINUX_OPTION */

#define MIN_NR_CONSOLES 1
#define MAX_NR_CONSOLES 63

#define VT_OPENQRY 0x5600
#define VT_GETMODE 0x5601
#define VT_SETMODE 0x5602
#define VT_GETSTATE 0x5603
#define VT_SENDSIG 0x5604
#define VT_RELDISP 0x5605
#define VT_ACTIVATE 0x5606
#define VT_WAITACTIVE 0x5607
#define VT_DISALLOCATE 0x5608
#define VT_RESIZE 0x5609
#define VT_RESIZEX 0x560A
#define VT_LOCKSWITCH 0x560B
#define VT_UNLOCKSWITCH 0x560C
#define VT_GETHIFONTMASK 0x560D
#define VT_WAITEVENT 0x560E
#define VT_SETACTIVATE 0x560F

struct vt_mode {
  char mode;
  char waitv;
  short relsig;
  short acqsig;
  short frsig;
};

#define VT_AUTO 0x00
#define VT_PROCESS 0x01
#define VT_ACKACQ 0x02

struct vt_stat {
  unsigned short v_active;
  unsigned short v_signal;
  unsigned short v_state;
};

struct vt_sizes {
  unsigned short v_rows;
  unsigned short v_cols;
  unsigned short v_scrollsize;
};

struct vt_consize {
  unsigned short v_rows;
  unsigned short v_cols;
  unsigned short v_vlin;
  unsigned short v_clin;
  unsigned short v_vcol;
  unsigned short v_ccol;
};

#define VT_EVENT_SWITCH 0x0001
#define VT_EVENT_BLANK 0x0002
#define VT_EVENT_UNBLANK 0x0004
#define VT_EVENT_RESIZE 0x0008
#define VT_MAX_EVENT 0x000F

struct vt_event {
  unsigned int event;

  unsigned int oldev;
  unsigned int newev;
  unsigned int pad[4];
};

struct vt_setactivate {
  unsigned int console;
  struct vt_mode mode;
};

#endif /* _ABIBITS_VT_H */
