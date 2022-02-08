
#ifndef _LINUX_FB_H
#define _LINUX_FB_H

#include <linux/types.h>
#include <stdint.h>
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
    uint32_t smem_len;
    uint32_t type;
    uint32_t type_aux;
    uint32_t visual;
    uint16_t xpanstep;
    uint16_t ypanstep;
    uint16_t ywrapstep;
    uint32_t line_length;
    unsigned long mmio_start;
    uint32_t mmio_len;
    uint32_t accel;
    uint16_t capabilities;
    uint16_t reserved[2];
};

struct fb_bitfield {
    uint32_t offset;
    uint32_t length;
    uint32_t msb_right;
};

struct fb_var_screeninfo {
    uint32_t xres;
    uint32_t yres;
    uint32_t xres_virtual;
    uint32_t yres_virtual;
    uint32_t xoffset;
    uint32_t yoffset;
    uint32_t bits_per_pixel;
    uint32_t grayscale;
    struct fb_bitfield red;
    struct fb_bitfield green;
    struct fb_bitfield blue;
    struct fb_bitfield transp;
    uint32_t nonstd;
    uint32_t activate;
    uint32_t height;
    uint32_t width;
    uint32_t accel_flags;
    uint32_t pixclock;
    uint32_t left_margin;
    uint32_t right_margin;
    uint32_t upper_margin;
    uint32_t lower_margin;
    uint32_t hsync_len;
    uint32_t vsync_len;
    uint32_t sync;
    uint32_t vmode;
    uint32_t rotate;
    uint32_t colorspace;
    uint32_t reserved[4];
};

struct fb_cmap {
    uint32_t start;
    uint32_t len;
    uint16_t *red;
    uint16_t *green;
    uint16_t *blue;
    uint16_t *transp;
};

struct fb_con2fbmap {
    uint32_t console;
    uint32_t framebuffer;
};

struct fb_vblank {
    uint32_t flags;
    uint32_t count;
    uint32_t vcount;
    uint32_t hcount;
    uint32_t reserved[4];
};

struct fb_copyarea {
    uint32_t dx;
    uint32_t dy;
    uint32_t width;
    uint32_t height;
    uint32_t sx;
    uint32_t sy;
};

struct fb_fillrect {
    uint32_t dx;
    uint32_t dy;
    uint32_t width;
    uint32_t height;
    uint32_t color;
    uint32_t rop;
};

struct fb_image {
    uint32_t dx;
    uint32_t dy;
    uint32_t width;
    uint32_t height;
    uint32_t fg_color;
    uint32_t bg_color;
    uint8_t depth;
    const char *data;
    struct fb_cmap cmap;
};

struct fbcurpos {
    uint16_t x, y;
};

struct fb_cursor {
    uint16_t set;
    uint16_t enable;
    uint16_t rop;
    const char *mask;
    struct fbcurpos hot;
    struct fb_image image;
};

#ifdef __cplusplus
}
#endif

#endif // _LINUX_FB_H

