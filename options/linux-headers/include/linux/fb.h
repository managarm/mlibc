
#ifndef _LINUX_FB_H
#define _LINUX_FB_H

#include <linux/types.h>
#include <sys/ioctl.h>

#define FB_MAX 32

#define FBIOGET_VSCREENINFO 0x4600
#define FBIOPUT_VSCREENINFO 0x4601
#define FBIOGET_FSCREENINFO 0x4602
#define FBIOGETCMAP 0x4604
#define FBIOPUTCMAP 0x4605
#define FBIOPAN_DISPLAY 0x4606
#define FBIO_CURSOR _IOWR('F', 0x08, struct fb_cursor)

#define FBIOGET_CON2FBMAP 0x460F
#define FBIOPUT_CON2FBMAP 0x4610
#define FBIOBLANK 0x4611
#define FBIOGET_VBLANK _IOWR('F', 0x12, struct fb_vblank)
#define FBIO_ALLOC 0x4613
#define FBIO_FREE 0x4614
#define FBIOGET_GLYPH 0x4615
#define FBIOGET_HWCINFO 0x4616
#define FBIOPUT_MODEINFO 0x4617
#define FBIOGET_DISPINFO 0x4618
#define FBIO_WAITFORVSYNC _IOW('F', 0x20, unsigned int)

#define FB_TYPE_PACKED_PIXELS 0
#define FB_TYPE_PLANES 1
#define FB_TYPE_INTERLEAVED_PLANES 2
#define FB_TYPE_TEXT 3
#define FB_TYPE_VGA_PLANES 4
#define FB_TYPE_FOURCC 5

#define FB_VISUAL_MONO01 0
#define FB_VISUAL_MONO10 1
#define FB_VISUAL_TRUECOLOR 2
#define FB_VISUAL_PSEUDOCOLOR 3
#define FB_VISUAL_DIRECTCOLOR 4
#define FB_VISUAL_STATIC_PSEUDOCOLOR 5
#define FB_VISUAL_FOURCC 6

#define FB_ACCEL_NONE 0

#define FB_NONSTD_HAM 1
#define FB_NONSTD_REV_PIX_IN_B 2

#define FB_ACTIVATE_NOW 0
#define FB_ACTIVATE_NXTOPEN 1
#define FB_ACTIVATE_TEST 2
#define FB_ACTIVATE_MASK 15

#define FB_ACTIVATE_VBL 16
#define FB_CHANGE_CMAP 32
#define FB_ACTIVATE_ALL 64
#define FB_ACTIVATE_FORCE 128
#define FB_ACTIVATE_INV_MODE 256
#define FB_ACTIVATE_INV_ALL 512

#define FB_ACCELF_TEXT 0x1

#define FB_SYNC_HOR_HIGH_ACT 0x1
#define FB_SYNC_VERT_HIGH_ACT 0x2
#define FB_SYNC_EXT 0x4
#define FB_SYNC_COMP_HIGH_ACT 0x8
#define FB_SYNC_BROADCAST 0x10
#define FB_SYNC_ON_GREEN 0x20

#define FB_VMODE_NONINTERLACED 0
#define FB_VMODE_INTERLACED 1
#define FB_VMODE_DOUBLE 2
#define FB_VMODE_ODD_FLD_FIRST 4
#define FB_VMODE_MASK 255

#define FB_VMODE_YWRAP 256
#define FB_VMODE_SMOOTH_XPAN 512
#define FB_VMODE_CONUPDATE 512

#define FB_ROTATE_UR 0
#define FB_ROTATE_CW 1
#define FB_ROTATE_UD 2
#define FB_ROTATE_CCW 3

#define PICOS2KHZ(a) (1000000000UL/(a))
#define KHZ2PICOS(a) (1000000000UL/(a))

#define VESA_NO_BLANKING 0
#define VESA_VSYNC_SUSPEND 1
#define VESA_HSYNC_SUSPEND 2
#define VESA_POWERDOWN 3

enum {
    FB_BLANK_UNBLANK = VESA_NO_BLANKING,
    FB_BLANK_NORMAL = VESA_NO_BLANKING + 1,
    FB_BLANK_VSYNC_SUSPEND = VESA_VSYNC_SUSPEND + 1,
    FB_BLANK_HSYNC_SUSPEND = VESA_HSYNC_SUSPEND + 1,
    FB_BLANK_POWERDOWN = VESA_POWERDOWN + 1,
};

#define FB_VBLANK_VBLANKING 0x001
#define FB_VBLANK_HBLANKING 0x002
#define FB_VBLANK_HAVE_VBLANK 0x004
#define FB_VBLANK_HAVE_HBLANK 0x008
#define FB_VBLANK_HAVE_COUNT 0x010
#define FB_VBLANK_HAVE_VCOUNT 0x020
#define FB_VBLANK_HAVE_HCOUNT 0x040
#define FB_VBLANK_VSYNCING 0x080
#define FB_VBLANK_HAVE_VSYNC 0x100

#define ROP_COPY 0
#define ROP_XOR 1

#define FB_CUR_SETIMAGE 0x01
#define FB_CUR_SETPOS 0x02
#define FB_CUR_SETHOT 0x04
#define FB_CUR_SETCMAP 0x08
#define FB_CUR_SETSHAPE 0x10
#define FB_CUR_SETSIZE 0x20
#define FB_CUR_SETALL 0xFF

#ifdef __cplusplus
extern "C" {
#endif

struct fb_fix_screeninfo {
    char id[16];
    unsigned long smem_start;
    unsigned int smem_len;
    unsigned int type;
    unsigned int type_aux;
    unsigned int visual;
    unsigned short xpanstep;
    unsigned short ypanstep;
    unsigned short ywrapstep;
    unsigned int line_length;
    unsigned long mmio_start;
    unsigned int mmio_len;
    unsigned int accel;
    unsigned short capabilities;
    unsigned short reserved[2];
};

struct fb_bitfield {
    unsigned int offset;
    unsigned int length;
    unsigned int msb_right;
};

struct fb_var_screeninfo {
    unsigned int xres;
    unsigned int yres;
    unsigned int xres_virtual;
    unsigned int yres_virtual;
    unsigned int xoffset;
    unsigned int yoffset;
    unsigned int bits_per_pixel;
    unsigned int grayscale;
    struct fb_bitfield red;
    struct fb_bitfield green;
    struct fb_bitfield blue;
    struct fb_bitfield transp;
    unsigned int nonstd;
    unsigned int activate;
    unsigned int height;
    unsigned int width;
    unsigned int accel_flags;
    unsigned int pixclock;
    unsigned int left_margin;
    unsigned int right_margin;
    unsigned int upper_margin;
    unsigned int lower_margin;
    unsigned int hsync_len;
    unsigned int vsync_len;
    unsigned int sync;
    unsigned int vmode;
    unsigned int rotate;
    unsigned int colorspace;
    unsigned int reserved[4];
};

struct fb_cmap {
    unsigned int start;
    unsigned int len;
    unsigned short *red;
    unsigned short *green;
    unsigned short *blue;
    unsigned short *transp;
};

struct fb_con2fbmap {
    unsigned int console;
    unsigned int framebuffer;
};

struct fb_vblank {
    unsigned int flags;
    unsigned int count;
    unsigned int vcount;
    unsigned int hcount;
    unsigned int reserved[4];
};

struct fb_copyarea {
    unsigned int dx;
    unsigned int dy;
    unsigned int width;
    unsigned int height;
    unsigned int sx;
    unsigned int sy;
};

struct fb_fillrect {
    unsigned int dx;
    unsigned int dy;
    unsigned int width;
    unsigned int height;
    unsigned int color;
    unsigned int rop;
};

struct fb_image {
    unsigned int dx;
    unsigned int dy;
    unsigned int width;
    unsigned int height;
    unsigned int fg_color;
    unsigned int bg_color;
    unsigned char depth;
    const char *data;
    struct fb_cmap cmap;
};

struct fbcurpos {
    unsigned short x, y;
};

struct fb_cursor {
    unsigned short set;
    unsigned short enable;
    unsigned short rop;
    const char *mask;
    struct fbcurpos hot;
    struct fb_image image;
};

#ifdef __cplusplus
}
#endif

#endif // _LINUX_FB_H

