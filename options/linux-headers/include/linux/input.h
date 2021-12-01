
#ifndef _LINUX_INPUT_H
#define _LINUX_INPUT_H

#include <stdint.h>
#include <linux/input-event-codes.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <bits/posix/timeval.h>

struct input_id {
	uint16_t bustype;
	uint16_t vendor;
	uint16_t product;
	uint16_t version;
};

struct input_event {
	struct timeval time;
#define input_event_sec time.tv_sec
#define input_event_usec time.tv_usec
	uint16_t type;
	uint16_t code;
	int32_t value;
};

struct input_absinfo {
	int32_t value;
	int32_t minimum;
	int32_t maximum;
	int32_t fuzz;
	int32_t flat;
	int32_t resolution;
};

#define BUS_USB 0x03
#define BUS_BLUETOOTH 0x05
#define BUS_I8042 0x11
#define BUS_I2C 0x18
#define BUS_SPI 0x1C
#define BUS_RMI 0x1D

#define MT_TOOL_FINGER 0x00
#define MT_TOOL_PEN 0x01
#define MT_TOOL_PALM 0x02
#define MT_TOOL_DIAL 0x0a
#define MT_TOOL_MAX 0x0f

#endif // _LINUX_INPUT_H

