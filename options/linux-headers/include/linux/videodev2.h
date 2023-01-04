
#ifndef _LINUX_VIDEODEV2_H
#define _LINUX_VIDEODEV2_H

#include <stdint.h>
#include <linux/types.h>
#include <linux/v4l2-controls.h>

#define VIDEO_MAX_FRAME 32
#define VIDEO_MAX_PLANES 8

#define V4L2_CAP_VIDEO_CAPTURE 0x00000001
#define V4L2_CAP_VIDEO_OUTPUT 0x00000002
#define V4L2_CAP_VIDEO_OVERLAY 0x00000004

#define V4L2_CAP_TUNER 0x00010000
#define V4L2_CAP_AUDIO 0x00020000
#define V4L2_CAP_RADIO 0x00040000
#define V4L2_CAP_READWRITE 0x01000000

#define V4L2_CAP_VIDEO_CAPTURE_MPLANE 0x00001000
#define V4L2_CAP_VIDEO_OUTPUT_MPLANE 0x00002000

#define V4L2_CAP_DEVICE_CAPS 0x80000000

#ifdef __cplusplus
extern "C" {
#endif

#define v4l2_fourcc(a, b, c, d)\
	((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))
#define v4l2_fourcc_be(a, b, c, d)	(v4l2_fourcc(a, b, c, d) | (1U << 31))

enum v4l2_buf_type {
	V4L2_BUF_TYPE_VIDEO_CAPTURE        = 1,
	V4L2_BUF_TYPE_VIDEO_OUTPUT         = 2,
	V4L2_BUF_TYPE_VIDEO_OVERLAY        = 3,
	V4L2_BUF_TYPE_VBI_CAPTURE          = 4,
	V4L2_BUF_TYPE_VBI_OUTPUT           = 5,
	V4L2_BUF_TYPE_SLICED_VBI_CAPTURE   = 6,
	V4L2_BUF_TYPE_SLICED_VBI_OUTPUT    = 7,
	V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY = 8,
	V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE = 9,
	V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE  = 10,
	V4L2_BUF_TYPE_SDR_CAPTURE          = 11,
	V4L2_BUF_TYPE_SDR_OUTPUT           = 12,
	V4L2_BUF_TYPE_META_CAPTURE         = 13,
	V4L2_BUF_TYPE_META_OUTPUT	   = 14,
	/* Deprecated, do not use */
	V4L2_BUF_TYPE_PRIVATE              = 0x80,
};

enum v4l2_frmsizetypes {
	V4L2_FRMSIZE_TYPE_DISCRETE	= 1,
	V4L2_FRMSIZE_TYPE_CONTINUOUS	= 2,
	V4L2_FRMSIZE_TYPE_STEPWISE	= 3,
};

struct v4l2_frmsize_discrete {
	uint32_t width;
	uint32_t height;
};

struct v4l2_frmsize_stepwise {
	uint32_t min_width;
	uint32_t max_width;
	uint32_t step_width;
	uint32_t min_height;
	uint32_t max_height;
	uint32_t step_height;
};

struct v4l2_frmsizeenum {
	uint32_t index;
	uint32_t pixel_format;
	uint32_t type;

	union {
		struct v4l2_frmsize_discrete discrete;
		struct v4l2_frmsize_stepwise stepwise;
	};

	uint32_t reserved[2];
};

struct v4l2_rect {
	int32_t left;
	int32_t top;
	uint32_t width;
	uint32_t height;
};

struct v4l2_fract {
	uint32_t numerator;
	uint32_t denominator;
};

struct v4l2_clip {
	struct v4l2_rect c;
	struct v4l2_clip *next;
};

enum v4l2_frmivaltypes {
	V4L2_FRMIVAL_TYPE_DISCRETE	= 1,
	V4L2_FRMIVAL_TYPE_CONTINUOUS	= 2,
	V4L2_FRMIVAL_TYPE_STEPWISE	= 3,
};

struct v4l2_frmival_stepwise {
	struct v4l2_fract min;
	struct v4l2_fract max;
	struct v4l2_fract step;
};

struct v4l2_frmivalenum {
	uint32_t index;
	uint32_t pixel_format;
	uint32_t width;
	uint32_t height;
	uint32_t type;

	union {
		struct v4l2_fract discrete;
		struct v4l2_frmival_stepwise stepwise;
	};

	uint32_t reserved[2];
};

struct v4l2_capability {
	uint8_t driver[16];
	uint8_t card[32];
	uint8_t bus_info[32];
	uint32_t version;
	uint32_t capabilities;
	uint32_t device_caps;
	uint32_t reserved[31];
};

struct v4l2_queryctrl {
	uint32_t id;
	uint32_t type;
	uint8_t name[32];
	int32_t minimum;
	int32_t maximum;
	int32_t step;
	int32_t default_value;
	uint32_t flags;
	uint32_t reserved[2];
};

struct v4l2_control {
	uint32_t id;
	int32_t value;
};

struct v4l2_pix_format {
	uint32_t width;
	uint32_t height;
	uint32_t pixelformat;
	uint32_t field;
	uint32_t bytesperline;
	uint32_t sizeimage;
	uint32_t colorspace;
	uint32_t priv;
	uint32_t flags;
	union {
		uint32_t ycbcr_enc;
		uint32_t hsv_enc;
	};
	uint32_t quantization;
	uint32_t xfer_func;
};

#define V4L2_PIX_FMT_BGR24   v4l2_fourcc('B', 'G', 'R', '3')

struct v4l2_plane_pix_format {
	uint32_t sizeimage;
	uint32_t bytesperline;
	uint16_t reserved[6];
} __attribute__ ((packed));

struct v4l2_pix_format_mplane {
	uint32_t width;
	uint32_t height;
	uint32_t pixelformat;
	uint32_t field;
	uint32_t colorspace;

	struct v4l2_plane_pix_format plane_fmt[VIDEO_MAX_PLANES];
	uint8_t num_planes;
	uint8_t flags;
	 union {
		uint8_t ycbcr_enc;
		uint8_t hsv_enc;
	};
	uint8_t quantization;
	uint8_t xfer_func;
	uint8_t reserved[7];
} __attribute__ ((packed));

struct v4l2_window {
	struct v4l2_rect w;
	uint32_t field;
	uint32_t chromakey;
	struct v4l2_clip *clips;
	uint32_t clipcount;
	void *bitmap;
	uint8_t global_alpha;
};

struct v4l2_vbi_format {
	uint32_t sampling_rate;
	uint32_t offset;
	uint32_t samples_per_line;
	uint32_t sample_format;
	int32_t start[2];
	uint32_t count[2];
	uint32_t flags;
	uint32_t reserved[2];
};

struct v4l2_sliced_vbi_format {
	uint16_t service_set;
	uint16_t service_lines[2][24];
	uint32_t io_size;
	uint32_t reserved[2];
};

struct v4l2_sdr_format {
	uint32_t pixelformat;
	uint32_t buffersize;
	uint8_t reserved[24];
} __attribute__ ((packed));

struct v4l2_meta_format {
	uint32_t dataformat;
	uint32_t buffersize;
} __attribute__ ((packed));

struct v4l2_format {
	uint32_t type;
	union {
		struct v4l2_pix_format pix;
		struct v4l2_pix_format_mplane pix_mp;
		struct v4l2_window win;
		struct v4l2_vbi_format vbi;
		struct v4l2_sliced_vbi_format sliced;
		struct v4l2_sdr_format sdr;
		struct v4l2_meta_format meta;
		uint8_t raw_data[200];
	} fmt;
};

#ifdef __cplusplus
}
#endif

#define VIDIOC_QUERYCAP _IOR('V', 0, struct v4l2_capability)
#define VIDIOC_G_FMT _IOWR('V',  4, struct v4l2_format)
#define VIDIOC_S_FMT _IOWR('V',  5, struct v4l2_format)
#define VIDIOC_G_CTRL _IOWR('V', 27, struct v4l2_control)
#define VIDIOC_S_CTRL _IOWR('V', 28, struct v4l2_control)
#define VIDIOC_QUERYCTRL _IOWR('V', 36, struct v4l2_queryctrl)
#define VIDIOC_TRY_FMT _IOWR('V', 64, struct v4l2_format)
#define VIDIOC_ENUM_FRAMESIZES _IOWR('V', 74, struct v4l2_frmsizeenum)
#define VIDIOC_ENUM_FRAMEINTERVALS _IOWR('V', 75, struct v4l2_frmivalenum)

#endif // _LINUX_VIDEODEV2_H
