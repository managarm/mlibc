
#ifndef _LINUX_VIDEODEV2_H
#define _LINUX_VIDEODEV2_H

#include <linux/types.h>

#define V4L2_CAP_VIDEO_CAPTURE 0x00000001
#define V4L2_CAP_VIDEO_OUTPUT 0x00000002
#define V4L2_CAP_VIDEO_OVERLAY 0x00000004

#define V4L2_CAP_TUNER 0x00010000
#define V4L2_CAP_AUDIO 0x00020000
#define V4L2_CAP_RADIO 0x00040000

#ifdef __cplusplus
extern "C" {
#endif

struct v4l2_capability {
	__u8 driver[16];
	__u8 card[32];
	__u8 bus_info[32];
	__u32 version;
	__u32 capabilities;
	__u32 device_caps;
	__u32 reserved[31];
};

#ifdef __cplusplus
}
#endif

#define VIDIOC_QUERYCAP _IOR('V', 0, struct v4l2_capability)

#endif // _LINUX_VIDEODEV2_H

