//
// Created by tender on 18-9-24.
//

#ifndef VIRTUAL_TOUCHPAD_H

#define VIRTUAL_TOUCHPAD_H

#define PACKET_UNKNOWN			0x01

#define PACKET_V4_HEAD			0x05
#define PACKET_V4_MOTION		0x06
#define PACKET_V4_STATUS		0x07

#define PMAX 200
#define PMIN 0
#define WMAX 255
#define WMIN 0

#define X_RES 10
#define Y_RES 10

#define DRIVER_NAME "virtual_touchpad"
/*
 * weight value for v4 hardware
 */
#define ETP_WEIGHT_VALUE	5
#define VTP_MAX_FINGER 5

#include <linux/cdev.h>
/*
 * The base position for one finger, v4 hardware
 */
struct finger_pos {
	unsigned int x;
	unsigned int y;
};

struct elantech_data {
	struct input_dev *tp_dev;	/* Relative device for trackpoint */
	char tp_phys[32];
	unsigned char debug;
	unsigned char capabilities[3];
	unsigned char samples[3];
	unsigned char hw_version;
	unsigned int fw_version;
	unsigned int single_finger_reports;
	unsigned int y_max;
	unsigned int width;
	struct finger_pos mt[VTP_MAX_FINGER];
	unsigned char parity[256];
	//TODO 下面两行不需要了。
	int (*send_cmd)(struct psmouse *psmouse, unsigned char c, unsigned char *param);
	void (*original_set_rate)(struct psmouse *psmouse, unsigned int rate);
};

struct vtp_dev{
	struct cdev mcdev;
	struct elantech_data * etd;
	unsigned char* packet;
};


#endif //VIRTUAL_TOUCHPAD_H
