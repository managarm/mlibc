#ifndef _IRONCLAD_H
#define _IRONCLAD_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FB_DIMENSIONS 1

struct ironclad_fb_dimensions {
	uint16_t width;
	uint16_t height;
	uint16_t pitch;
	uint16_t bpp;
	uint8_t  red_mask_size;
	uint8_t  red_mask_shift;
	uint8_t  green_mask_size;
	uint8_t  green_mask_shift;
	uint8_t  blue_mask_size;
	uint8_t  blue_mask_shift;
};

#define PS2MOUSE_2_1_SCALING     1
#define PS2MOUSE_1_1_SCALING     2
#define PS2MOUSE_SET_RES         3
#define PS2MOUSE_SET_SAMPLE_RATE 4

struct ironclad_mouse_data {
	int  x_variation;
	int  y_variation;
	bool is_left;
	bool is_right;
};
#ifdef __cplusplus
}
#endif

#endif // _IRONCLAD_H
