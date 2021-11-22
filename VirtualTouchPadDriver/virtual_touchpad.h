//
// Created by tender on 18-9-24.
//

#ifndef AVMS_VIRTUAL_MOUSEPAD_H
#define AVMS_VIRTUAL_MOUSEPAD_H

#define PACKET_UNKNOWN            0x01

#define PACKET_V4_HEAD            0x05
#define PACKET_V4_MOTION        0x06
#define PACKET_V4_STATUS        0x07
#define PACKET_CONFIG           0x08

#define VTP_DEVICE_NAME "virtual_touchpad"

#define PMAX 15
#define PMIN 0
#define WMAX 255
#define WMIN 0

#define X_RES 10
#define Y_RES 10

/*
 * weight value for v4 hardware
 */
#define ETP_WEIGHT_VALUE        5
#define VTP_MAX_FINGER 5

#include <linux/cdev.h>

/*
 * The base position for one finger, v4 hardware
 */
struct finger_pos {
    unsigned int x;
    unsigned int y;
};

struct device_info {
    int max_x_mm;
    int max_y_mm;
    int res_x;
    int res_y;
    bool connect;
    bool isTouchPad;
};

struct elantech_data {
    struct input_dev *tp_dev;
    struct input_dev *ts_dev;
    char tp_phys[32];
    //TODO this should be clear ??
    struct finger_pos mt[VTP_MAX_FINGER];
};

struct vtp_dev {
    struct cdev mcdev;
    struct elantech_data *etd;
    unsigned char *packet;
};


#endif //AVMS_VIRTUAL_MOUSEPAD_H
