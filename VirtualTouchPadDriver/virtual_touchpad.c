#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/pci.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/input/mt.h>

#include <linux/spinlock.h>

#include "virtual_touchpad.h"
#include <linux/err.h>
//vtp=virtual touch pad
#ifndef vtp_MAJOR
#define vtp_MAJOR 60
#endif


MODULE_LICENSE("GPL");

// Uncomment to print debugging messages
//#define VTP_DEBUG

dev_t vtp_dev_num;


struct vtp_dev *vtp_read_data_dev;
static int inited = false;

static int vtp_major = vtp_MAJOR;
static int vtp_minor;
static struct class *vtp_class = NULL;

static ssize_t vtp_write(struct file *filp, const char __user

*buf,
                         size_t count, loff_t
                         *f_pos);

static ssize_t vtp_read(struct file *filp, char __user

*buf,
                        size_t count, loff_t
                        *f_pos);

static void setup_dev(struct input_dev *target_input_dev, struct device_info *info);

static int vtp_open(struct inode *inode, struct file *filp);

static void vtp_exit(void);

static int vtp_init(void);

struct file_operations vtp_fops = {
        .write = vtp_write,
        .open = vtp_open,
        .read=vtp_read,
};
int result;


static int vtp_open(struct inode *inode, struct file *filp) {
    struct vtp_dev *vtpdev;
    vtpdev = container_of(inode->i_cdev, struct vtp_dev, mcdev);
    filp->private_data = vtpdev;

    return 0;

}

static void input_sync_v4(struct input_dev *dev_tps) {
    input_mt_sync_frame(dev_tps);
    input_sync(dev_tps);
}

int fingerCount = 0;
DEFINE_SPINLOCK(countLock);

static void clear_state(struct input_dev *dev) {
    int i;
    for (i = 0; i < VTP_MAX_FINGER; ++i) {
        input_mt_slot(dev, i);
        input_mt_report_slot_state(dev, MT_TOOL_FINGER, false);
    }
    //input_sync_v4(dev);
}

bool isTouchScreen = false;
int last_finger_count = 0;
/** if user release one or more finger ,the following touch will be ommit **/
bool ommit_this = false;

static void process_packet_status_v4(struct vtp_dev *vtp_dev1) {
    struct input_dev *dev_tp = vtp_dev1->etd->tp_dev;
    struct input_dev *dev_ts = vtp_dev1->etd->ts_dev;
    unsigned char *packet = vtp_dev1->packet;
    unsigned fingers;
    int i;

    spin_lock(&countLock);
    fingerCount = 0;

//    clear_state(dev_tp);
//    clear_state(dev_ts);
    /* notify finger state change */
    fingers = packet[1] & 0x1f;
//    printk("status for ids[0x%x] \r\n", fingers);
    for (i = 0; i < VTP_MAX_FINGER; i++) {
        if ((fingers & (1 << i)) == 0) {
            input_mt_slot(dev_tp, i);
            input_mt_report_slot_state(dev_tp, MT_TOOL_FINGER, false);
            input_mt_slot(dev_ts, i);
            input_mt_report_slot_state(dev_ts, MT_TOOL_FINGER, false);
        } else {
            ++fingerCount;
        }
    }
    spin_unlock(&countLock);

    if (fingerCount < last_finger_count) {
        //release one finger ,ommit motion
        if (fingerCount != 0) {
        } else {
            last_finger_count = 0;
        }
        ommit_this = true;
    } else {
        ommit_this = false;
        last_finger_count = fingerCount;
    }
//	if (fingerCount <= 1) {
//		clear_state(dev_ts);
//	} else {
//    clear_state(dev_tp);
//	}
    input_sync_v4(dev_tp);
//	input_sync_v4(dev_ts);
}

bool firstPointerInMultiTouch = true;

static void process_packet_head_v4(struct vtp_dev *vtp_dev1) {
    if (ommit_this) {
        return;
    }
    struct input_dev *dev;
    struct elantech_data *etd = vtp_dev1->etd;
    unsigned char *packet = vtp_dev1->packet;
    int id = ((packet[3] & 0xe0) >> 5);// - 1;
//    printk("head for id[%d] \r\n", id);
    int pres, traces;

    if (id < 0)
        return;

    etd->mt[id].x = ((packet[1] & 0x0f) << 8) | packet[2];
    etd->mt[id].y = (((packet[4] & 0x0f) << 8) | packet[5]);
    //
    // if (fingerCount <= 1) {
    //     if (isTouchScreen) {
    //         //first single touch after multi-touch ,we should let mouse pos continue
    //         etd->mt[id].x = etd->last_pointer_pos2.x;
    //         etd->mt[id].y = etd->last_pointer_pos2.y;
    //     } else {
    //         // normal single touch,but maybe first touch in multi-touch

    //         etd->last_pointer_pos2.x = etd->last_pointer_pos.x;
    //         etd->last_pointer_pos2.y = etd->last_pointer_pos.y;
    //         etd->last_pointer_pos.x = etd->mt[id].x;
    //         etd->last_pointer_pos.y = etd->mt[id].y;
    //     }
    // } else {
    //     if (isTouchScreen) {
    //         etd->mt[id].x = etd->mt[id].x - etd->last_pointer_pos_delta2.x;
    //         etd->mt[id].y = etd->mt[id].y - etd->last_pointer_pos_delta2.y;
    //         //later finger in multi touch state

    //     } else {
    //         //first touch in multi touch state
    //                  etd->last_pointer_pos_delta2.x = etd->mt[id].x - etd->last_pointer_pos2.x;
    //                  etd->last_pointer_pos_delta2.y = etd->mt[id].y - etd->last_pointer_pos2.y;

    //                  etd->mt[id].x = etd->last_pointer_pos2.x;
    //                  etd->mt[id].y = etd->last_pointer_pos2.y;

    //     }

    // }
    pres = (packet[1] & 0xf0) | ((packet[4] & 0xf0) >> 4);
    traces = (packet[0] & 0xf0) >> 4;

    //spin_lock(&countLock);

//	if (fingerCount <= 1||fingerCount>=3) {
    dev = etd->tp_dev;
//    clear_state(vtp_dev1->etd->ts_dev);
    input_sync_v4(vtp_dev1->etd->ts_dev);
    isTouchScreen = false;
//	} else {
//		dev = etd->ts_dev;
//		isTouchScreen = true;
//		clear_state(vtp_dev1->etd->tp_dev);
//		input_sync_v4(vtp_dev1->etd->tp_dev);
//	}
    //spin_unlock(&countLock);
    //if (fingerCount <= 1) {//maybe single touch or multi touch's first touch
    //		etd->last_pointer_pos.x=etd->mt[id].x;
    //		etd->last_pointer_pos.y=etd->mt[id].y;
    firstPointerInMultiTouch = true;
    // } else {//multi touch
    //     if (firstPointerInMultiTouch) {
    //         etd->last_pointer_pos_delta2.x = etd->mt[id].x - etd->last_pointer_pos2.x;
    //         etd->last_pointer_pos_delta2.y = etd->mt[id].y - etd->last_pointer_pos2.y;

    //         etd->mt[id].x = etd->last_pointer_pos2.x;
    //         etd->mt[id].y = etd->last_pointer_pos2.y;

    //         firstPointerInMultiTouch = false;
    //     } else {
    //         etd->mt[id].x = etd->mt[id].x - etd->last_pointer_pos_delta.x;
    //         etd->mt[id].y = etd->mt[id].y - etd->last_pointer_pos_delta.y;
    //     }
    // }

    input_mt_slot(dev, id);
    input_mt_report_slot_state(dev, MT_TOOL_FINGER, true);

    input_report_abs(dev, ABS_MT_POSITION_X, etd->mt[id].x);
    input_report_abs(dev, ABS_MT_POSITION_Y, etd->mt[id].y);
    input_report_abs(dev, ABS_MT_PRESSURE, pres);
    //	input_report_abs(dev, ABS_MT_TOUCH_MAJOR, traces * etd->width);
    //	input_report_abs(dev, ABS_MT_WIDTH_MAJOR, traces * etd->width+1);
    input_report_abs(dev, ABS_MT_TOUCH_MAJOR, 1);
    input_report_abs(dev, ABS_MT_WIDTH_MAJOR, 2);
    /* report this for backwards compatibility */
    input_report_abs(dev, ABS_TOOL_WIDTH, traces);

    input_sync_v4(dev);
}

static void process_packet_motion_v4(struct vtp_dev *vtp_dev1) {
    if (ommit_this) {
        return;
    }
    struct input_dev *dev;
    struct elantech_data *etd = vtp_dev1->etd;
    unsigned char *packet = vtp_dev1->packet;
    int weight, delta_x1 = 0, delta_y1 = 0, delta_x2 = 0, delta_y2 = 0;
    int id, sid;

    id = ((packet[0] & 0xe0) >> 5);// - 1;
    if (id < 0)
        return;
    if (!isTouchScreen) {
        dev = vtp_dev1->etd->tp_dev;
        clear_state(vtp_dev1->etd->ts_dev);
        input_sync_v4(vtp_dev1->etd->ts_dev);
    } else {
        dev = vtp_dev1->etd->ts_dev;
        clear_state(vtp_dev1->etd->tp_dev);
        input_sync_v4(vtp_dev1->etd->tp_dev);
    }
    sid = ((packet[3] & 0xe0) >> 5);// - 1;
    weight = (packet[0] & 0x10) ? ETP_WEIGHT_VALUE : 1;
    /*
     * Motion packets give us the delta of x, y values of specific fingers,
     * but in two's complement. Let the compiler do the conversion for us.
     * Also _enlarge_ the numbers to int, in case of overflow.
     */
    delta_x1 = (signed char) packet[1];
    delta_y1 = (signed char) packet[2];
    delta_x2 = (signed char) packet[4];
    delta_y2 = (signed char) packet[5];

    if ((delta_x1 == 0) && (delta_y1 == 0)) {
        goto PROCESS_ID_2;
    }

    etd->mt[id].x += delta_x1 * weight;
    etd->mt[id].y += delta_y1 * weight;
    printk("motion for id:%d, x[%d],y[%d]\r\n", id, etd->mt[id].x, etd->mt[id].y);
    input_mt_slot(dev, id);
    input_report_abs(dev, ABS_MT_POSITION_X, etd->mt[id].x);
    input_report_abs(dev, ABS_MT_POSITION_Y, etd->mt[id].y);

    PROCESS_ID_2:

    if (sid >= 0) {
        if ((delta_x2 == 0) && (delta_y2 == 0)) {
            goto MOTION_OUT;
        }
        etd->mt[sid].x += delta_x2 * weight;
        etd->mt[sid].y += delta_y2 * weight;
        input_mt_slot(dev, sid);
        input_report_abs(dev, ABS_MT_POSITION_X, etd->mt[sid].x);
        input_report_abs(dev, ABS_MT_POSITION_Y, etd->mt[sid].y);
        printk("motion for id:%d, x[%d],y[%d]\r\n", sid, etd->mt[sid].x, etd->mt[sid].y);
    }
    MOTION_OUT:

    input_sync_v4(dev);
}

static void elantech_report_absolute_v4(struct vtp_dev *vtp_dev1,
                                        int packet_type) {
    switch (packet_type) {
        case PACKET_V4_STATUS:
            process_packet_status_v4(vtp_dev1);
            break;

        case PACKET_V4_HEAD:
            process_packet_head_v4(vtp_dev1);
            break;

        case PACKET_V4_MOTION:
            process_packet_motion_v4(vtp_dev1);
            break;

        case PACKET_UNKNOWN:
        default:
            /* impossible to get here */
            break;
    }
}

void configure_device(struct vtp_dev *vtp_dev1) {


    unsigned char *data = vtp_dev1->packet;
    struct device_info info;
    info.max_x_mm = data[2] | ((data[5] & 0xf0U) << 4U);
    info.max_y_mm = data[4] | ((data[5] & 0xfU) << 8U);
    info.res_x = data[0];
    info.res_y = data[1];
    info.connect = ((data[3] & 0x8) != 0);
    info.isTouchPad = true;
    if (info.connect) {
        inited = true;
    } else {
        //TODO sometime bit flip?
        //inited=false;
        printk(" error[%d] \r\n", __LINE__);
        return;

    }
    printk("x:%d,y:%d,resx:%d,resy:%d,connect:%d\r\n", info.max_x_mm, info.max_y_mm, info.res_x, info.res_y,
           info.connect);

    if (vtp_dev1->etd->tp_dev == NULL) {
        /***init touchpad device***/
        struct input_dev *input_dev_tp = input_allocate_device();
        if (input_dev_tp == NULL) {
            printk("Bad input_alloc_device()\n");
            return;
        }
        input_dev_tp->name = "Virtual Touch Pad";
        input_dev_tp->phys = "vtp";
        input_dev_tp->id.bustype = BUS_VIRTUAL;
        input_dev_tp->id.vendor = 0x0000;
        input_dev_tp->id.product = 0x0000;
        input_dev_tp->id.version = 0x0000;
        setup_dev(input_dev_tp, &info);
        if ((result = input_register_device(input_dev_tp)) != 0) {
            return;
        }
        vtp_dev1->etd->tp_dev = input_dev_tp;
    } else {
        setup_dev(vtp_dev1->etd->tp_dev, &info);
        input_reset_device(vtp_dev1->etd->tp_dev);
    }

    if (vtp_dev1->etd->ts_dev == NULL) {
        struct input_dev *input_dev_ts = input_allocate_device();

        if (input_dev_ts == NULL) {
            printk("Bad input_alloc_device()\n");
            return;
        }
        input_dev_ts->name = "Virtual Touch Screen";
        input_dev_ts->phys = "vts";
        input_dev_ts->id.bustype = BUS_VIRTUAL;
        input_dev_ts->id.vendor = 0x0000;
        input_dev_ts->id.product = 0x0000;
        input_dev_ts->id.version = 0x0000;

        info.isTouchPad = false;
        setup_dev(input_dev_ts, &info);

        if ((result = input_register_device(input_dev_ts)) != 0) {
            return;
        }
        vtp_dev1->etd->ts_dev = input_dev_ts;
    } else {
        setup_dev(vtp_dev1->etd->ts_dev, &info);
        input_reset_device(vtp_dev1->etd->ts_dev);
    }
}

static int elantech_packet_check_v4(unsigned char *data) {

    unsigned char *packet = data;
    unsigned char packet_type = packet[3] & 0x07;

    switch (packet_type) {
        case 0:
            return PACKET_V4_STATUS;

        case 1:
            return PACKET_V4_HEAD;

        case 2:
            return PACKET_V4_MOTION;
        case 3:
            return PACKET_CONFIG;
    }

    return PACKET_UNKNOWN;
}

static int elantech_process_byte(struct vtp_dev *vtp_dev1) {
    //    struct elantech_data *etd = filp->private_data;
    unsigned char *data = vtp_dev1->packet;
    int packet_type = elantech_packet_check_v4(data);
    switch (packet_type) {
        case PACKET_UNKNOWN:
            return -1;

        case PACKET_CONFIG:
            printk("CONFIG\n");
            configure_device(vtp_dev1);
        default:
            if (!inited) {
                printk("haven't inited\n");
                return 0;
            }
            elantech_report_absolute_v4(vtp_dev1, packet_type);
            break;
    }

    return 0;
};

static const unsigned short msg_bytes = 6 * sizeof(char);

static ssize_t vtp_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
    const char *tmp;
    char *write_buffer = (char *) kmalloc(msg_bytes, GFP_KERNEL);

    int result;
    tmp = buf;
    if ((result = copy_from_user(write_buffer, tmp, msg_bytes)) > 0) {
        printk("vtp_write: %ld bytes could not be read. buf: %x\n", result, buf);
        kfree(write_buffer);
        return -EFAULT;
    }

    ((struct vtp_dev *) filp->private_data)->packet = write_buffer;

    elantech_process_byte(filp->private_data);


#ifdef VTP_DEBUG
    printk("vtp_write: received: buf:%s\n",write_buffer);
#endif


    kfree(write_buffer);
    return count;
}

static void setup_dev(struct input_dev *target_input_dev, struct device_info *info) {

    //__set_bit(INPUT_PROP_POINTER,target_input_dev->propbit);
    __set_bit(EV_ABS, target_input_dev->evbit);
    __set_bit(EV_KEY, target_input_dev->evbit);
    __clear_bit(EV_REL, target_input_dev->evbit);


    __set_bit(BTN_TOUCH, target_input_dev->keybit);

    //TODO get x_min y_min x_max y_max
    int xmax = info->max_x_mm;
    int ymax = info->max_y_mm;


    /* For X to recognize me as touchpad. */
    input_set_abs_params(target_input_dev, ABS_X, 0, xmax, 0, 0);
    input_set_abs_params(target_input_dev, ABS_Y, 0, ymax, 0, 0);

    input_set_abs_params(target_input_dev, ABS_PRESSURE, PMIN,
                         PMAX, 0, 0);
    input_set_abs_params(target_input_dev, ABS_TOOL_WIDTH, WMIN,
                         WMAX, 0, 0);
    /* Multi-touch capable pad, up to 5 fingers. */
    if (info->isTouchPad) {
        input_mt_init_slots(target_input_dev, VTP_MAX_FINGER, INPUT_MT_POINTER);
    } else {
        input_mt_init_slots(target_input_dev, VTP_MAX_FINGER, INPUT_MT_DIRECT);

    }
    input_set_abs_params(target_input_dev, ABS_MT_POSITION_X, 0, xmax, 0, 0);
    input_set_abs_params(target_input_dev, ABS_MT_POSITION_Y, 0, ymax, 0, 0);
    input_set_abs_params(target_input_dev, ABS_MT_PRESSURE, PMIN,
                         PMAX, 0, 0);
    /*
     * The firmware reports how many trace lines the finger spans,
     * convert to surface unit as Protocol-B requires.
     */
    input_set_abs_params(target_input_dev, ABS_MT_TOUCH_MAJOR, 0,
                         WMAX * 2, 0, 0);
    input_set_abs_params(target_input_dev, ABS_MT_WIDTH_MAJOR, 0,
                         WMAX * 2, 0, 0);

    input_abs_set_res(target_input_dev, ABS_X, info->res_x);
    input_abs_set_res(target_input_dev, ABS_Y, info->res_y);

    input_abs_set_res(target_input_dev, ABS_MT_POSITION_X, info->res_x);
    input_abs_set_res(target_input_dev, ABS_MT_POSITION_Y, info->res_y);


}

static int __init

vtp_init(void) {
    if (vtp_major) {
        vtp_dev_num = MKDEV(vtp_major, 0);
        result = register_chrdev_region(vtp_dev_num, 1, VTP_DEVICE_NAME);
    } else {
        result = alloc_chrdev_region(&vtp_dev_num, 0, 1, VTP_DEVICE_NAME);
        vtp_major = MAJOR(vtp_dev_num);
    }
    vtp_minor = MINOR(vtp_dev_num);
    if (result < 0) {
        printk("vtp_init:Bad chrdev_region(), major nr: %d\n", vtp_major);
        return result;
    }

    vtp_read_data_dev = kmalloc(sizeof(struct vtp_dev), GFP_KERNEL);
    memset(vtp_read_data_dev, 0, sizeof(struct vtp_dev));
    if (!vtp_read_data_dev) {
        result = -ENOMEM;
        goto fail;
    }

    cdev_init(&vtp_read_data_dev->mcdev, &vtp_fops);

    vtp_read_data_dev->mcdev.owner = THIS_MODULE;
    vtp_read_data_dev->mcdev.ops = &vtp_fops;

    result = cdev_add(&vtp_read_data_dev->mcdev, vtp_dev_num, 1);
    if (result < 0) {
        goto fail;
    }
    vtp_class = class_create(THIS_MODULE, VTP_DEVICE_NAME);
    if (IS_ERR(vtp_class)) {
        goto fail;
    }
    struct device *vtp_device = device_create(vtp_class, NULL, vtp_dev_num, NULL, VTP_DEVICE_NAME, vtp_minor);
    if (IS_ERR(vtp_device)) {
        result = PTR_ERR(vtp_device);
        printk(KERN_WARNING
               "[target] Error %d while trying to create %s%d",
               result, VTP_DEVICE_NAME, vtp_minor);
        //cdev_del(&dev->cdev);
        return result;
    }

    void *etd = kmalloc(sizeof(struct elantech_data), GFP_KERNEL);
    if (etd == NULL) {
        printk("vtp_init:Could not alloc mem of etd\n");
        goto fail;
    }
    vtp_read_data_dev->etd = etd;
    vtp_read_data_dev->etd->tp_dev = NULL;
    vtp_read_data_dev->etd->ts_dev = NULL;


    printk("__init:Virtual Touchpad/TouchScreen Driver Initialized.\n");
    return 0;

    fail:
    return -1;

}

static void vtp_exit(void) {
    if (vtp_read_data_dev->etd->tp_dev != NULL) {
        input_unregister_device(vtp_read_data_dev->etd->tp_dev);
    }
    if (vtp_read_data_dev->etd->ts_dev != NULL) {
        input_unregister_device(vtp_read_data_dev->etd->ts_dev);
    }

    cdev_del(&vtp_read_data_dev->mcdev);
    if (vtp_class) {
        device_destroy(vtp_class, vtp_dev_num);
        class_destroy(vtp_class);
    }
    unregister_chrdev_region(vtp_dev_num, 1);
    printk("Virtual Touchpad Driver unloaded.\n");
}

ssize_t vtp_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {

    return copy_to_user(buf, &inited, count);
};

module_init(vtp_init);
module_exit(vtp_exit);
