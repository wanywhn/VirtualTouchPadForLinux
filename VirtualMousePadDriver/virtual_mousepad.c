#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/pci.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/input/mt.h>

#include "virtual_mousepad.h"

//vtp=virtual touch pad
#ifndef vtp_MAJOR
#define vtp_MAJOR 60
#endif



#define EV_COORDS 0
#define EV_BTN_LEFT_PRESS 1
#define EV_BTN_LEFT_RELEASE 2
#define EV_BTN_RIGHT_PRESS 3
#define EV_BTN_RIGHT_RELEASE 4
#define EV_BTN_MIDDLE_PRESS 5
#define EV_BTN_MIDDLE_RELEASE 6
#define EV_SCROLL_HORIZ 7
#define EV_SCROLL_VERT 8

MODULE_LICENSE("GPL");

// Uncomment to print debugging messages
//#define VTP_DEBUG

dev_t vtp_dev_num;


struct vtp_dev *mvtp_dev;

static int vtp_major = vtp_MAJOR;

static ssize_t vtp_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int vtp_open(struct inode *inode,struct file *filp);
static void vtp_exit(void);
static int vtp_init(void);

struct file_operations vtp_fops = {
	write:vtp_write,
	open:vtp_open
};
int result;


static int vtp_open(struct inode*inode,struct file *filp){
    struct vtp_dev *vtpdev;
    vtpdev=container_of(inode->i_cdev,struct vtp_dev,mcdev);
    filp->private_data=vtpdev;

    return 0;

}
static void elantech_input_sync_v4(struct vtp_dev*vtp_dev1)
{
	struct input_dev *dev = vtp_dev1->etd->tp_dev;
	struct elantech_data *etd = vtp_dev1->etd;
	unsigned char *packet = vtp_dev1->packet;

	/* For clickpads map both buttons to BTN_LEFT */
		input_report_key(dev, BTN_LEFT, packet[0] & 0x01);
		input_report_key(dev, BTN_RIGHT, packet[0] & 0x02);
		input_report_key(dev, BTN_MIDDLE, packet[0] & 0x04);

	input_mt_sync_frame(dev);
	// input_mt_report_pointer_emulation(dev, true);
	input_sync(dev);
}

static void process_packet_status_v4(struct vtp_dev*vtp_dev1)
{
	printk("in process_packet_status_v4\n");
	struct input_dev *dev = vtp_dev1->etd->tp_dev;
	unsigned char *packet = vtp_dev1->packet;
	unsigned fingers;
	int i;

	/* notify finger state change */
	fingers = packet[1] & 0x1f;
	printk("process_packet_status_v4: get fingers:%x\n",fingers);
	for (i = 0; i <VTP_MAX_FINGER; i++) {
		if ((fingers & (1 << i)) == 0) {
			input_mt_slot(dev, i);
			input_mt_report_slot_state(dev, MT_TOOL_FINGER, false);
		}
	}
	printk("leave process_packet_status_v4\n");

	elantech_input_sync_v4(vtp_dev1);
}

static void process_packet_head_v4(struct vtp_dev *vtp_dev1)
{
	printk("in process_packet_head_v4\n");
	struct input_dev *dev = vtp_dev1->etd->tp_dev;
	struct elantech_data *etd = vtp_dev1->etd;
	unsigned char *packet = vtp_dev1->packet;
	int id = ((packet[3] & 0xe0) >> 5) - 1;
	int pres, traces;

	if (id < 0)
		return;

	etd->mt[id].x = ((packet[1] & 0x0f) << 8) | packet[2];
	etd->mt[id].y = etd->y_max - (((packet[4] & 0x0f) << 8) | packet[5]);
	pres = (packet[1] & 0xf0) | ((packet[4] & 0xf0) >> 4);
	traces = (packet[0] & 0xf0) >> 4;

	input_mt_slot(dev, id);
	input_mt_report_slot_state(dev, MT_TOOL_FINGER, true);

	input_report_abs(dev, ABS_MT_POSITION_X, etd->mt[id].x);
	input_report_abs(dev, ABS_MT_POSITION_Y, etd->mt[id].y);
	input_report_abs(dev, ABS_MT_PRESSURE, pres);
	input_report_abs(dev, ABS_MT_TOUCH_MAJOR, traces * etd->width);
	/* report this for backwards compatibility */
	input_report_abs(dev, ABS_TOOL_WIDTH, traces);

	printk("leave process_packet_head_v4\n");
	elantech_input_sync_v4(vtp_dev1);
}

static void process_packet_motion_v4(struct vtp_dev*vtp_dev1)
{
	printk("in process_packet_motion_v4\n");
	struct input_dev *dev = vtp_dev1->etd->tp_dev;
	struct elantech_data *etd = vtp_dev1->etd;
	unsigned char *packet = vtp_dev1->packet;
	int weight, delta_x1 = 0, delta_y1 = 0, delta_x2 = 0, delta_y2 = 0;
	int id, sid;

	id = ((packet[0] & 0xe0) >> 5) - 1;
	if (id < 0)
		return;

	sid = ((packet[3] & 0xe0) >> 5) - 1;
	weight = (packet[0] & 0x10) ? ETP_WEIGHT_VALUE : 1;
	/*
	 * Motion packets give us the delta of x, y values of specific fingers,
	 * but in two's complement. Let the compiler do the conversion for us.
	 * Also _enlarge_ the numbers to int, in case of overflow.
	 */
	delta_x1 = (signed char)packet[1];
	delta_y1 = (signed char)packet[2];
	delta_x2 = (signed char)packet[4];
	delta_y2 = (signed char)packet[5];
	
	printk("process_packet_motion_v4:deltx1:%d",delta_x1);
	printk("process_packet_motion_v4:delty1:%d",delta_y1);
	printk("process_packet_motion_v4:deltx2:%d",delta_x2);
	printk("process_packet_motion_v4:delty2:%d",delta_y2);
	

	etd->mt[id].x += delta_x1 * weight;
	etd->mt[id].y -= delta_y1 * weight;
	input_mt_slot(dev, id);
	input_report_abs(dev, ABS_MT_POSITION_X, etd->mt[id].x);
	input_report_abs(dev, ABS_MT_POSITION_Y, etd->mt[id].y);

	if (sid >= 0) {
		etd->mt[sid].x += delta_x2 * weight;
		etd->mt[sid].y -= delta_y2 * weight;
		input_mt_slot(dev, sid);
		input_report_abs(dev, ABS_MT_POSITION_X, etd->mt[sid].x);
		input_report_abs(dev, ABS_MT_POSITION_Y, etd->mt[sid].y);
	}

	elantech_input_sync_v4(vtp_dev1);
	printk("leave process_packet_motion_v4\n");
}

static void elantech_report_absolute_v4(struct  vtp_dev *vtp_dev1,
										int packet_type)
{
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
/*
 * Process byte stream from mouse and handle complete packets
 */



static int elantech_packet_check_v4(unsigned char *data)
{

	unsigned char *packet = data;
	unsigned char packet_type = packet[3] & 0x03;

	printk("elantech_packet_check_v4: packet_type is :%d\n",packet_type);


	/*
	 * Sanity check based on the constant bits of a packet.
	 * The constant bits change depending on the value of
	 * the hardware flag 'crc_enabled' and the version of
	 * the IC body, but are the same for every packet,
	 * regardless of the type.
	 */


	switch (packet_type) {
		case 0:
			return PACKET_V4_STATUS;

		case 1:
			return PACKET_V4_HEAD;

		case 2:
			return PACKET_V4_MOTION;
	}

	return PACKET_UNKNOWN;
}
static int elantech_process_byte(struct  vtp_dev *vtp_dev1)
{
//    struct elantech_data *etd = filp->private_data;
unsigned char *data=vtp_dev1->packet;
	printk("in elantech_process_byte\n");
    int packet_type= elantech_packet_check_v4(data);
            switch (packet_type) {
                case PACKET_UNKNOWN:
                    return -1;

                default:
                    elantech_report_absolute_v4(vtp_dev1, packet_type);
                    break;
            }
	    printk("leave elantech_process_byte\n");

    return 0;
};

static const unsigned short msg_bytes=6*sizeof(char);
static ssize_t vtp_write( struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
    //TODO count should be 6


	const char *tmp;
	char *write_buffer=(char *)kmalloc(msg_bytes,GFP_KERNEL);

	int result;
	tmp=buf;
	if ((result = copy_from_user(write_buffer,tmp,msg_bytes )) > 0) {
		printk("vtp_write: %ld bytes could not be read. buf: %x\n",result,buf);
		kfree(write_buffer);
		return -EFAULT;
	}

	printk("vtp_write:get buffer:%x\n",*write_buffer);
    ((struct vtp_dev *)filp->private_data)->packet=write_buffer;

    printk("write end ,go to elantech_process_byte\n");
	elantech_process_byte(filp->private_data);


#ifdef VTP_DEBUG
	printk("vtp_write: received: buf:%s\n",write_buffer);
#endif



	kfree(write_buffer);
	return count;
}

static int __init vtp_init(void)
{
	if (vtp_major) {
		vtp_dev_num = MKDEV(vtp_major, 0);
		result = register_chrdev_region(vtp_dev_num, 1, "virtual_mousepad");
	} else {
		result = alloc_chrdev_region(&vtp_dev_num, 0, 1,"virtual_mousepad");
		vtp_major = MAJOR(vtp_dev_num);
	}
	if (result < 0) {
		printk("vtp_init:Bad chrdev_region(), major nr: %d\n",vtp_major);
		return result;
	}

    mvtp_dev=kmalloc(sizeof(struct vtp_dev),GFP_KERNEL);
    memset(mvtp_dev,0, sizeof(struct vtp_dev));
    if(!mvtp_dev){
        result=-ENOMEM;
        goto fail;
    }

    cdev_init(&mvtp_dev->mcdev,&vtp_fops);
    mvtp_dev->mcdev.owner=THIS_MODULE;
    mvtp_dev->mcdev.ops=&vtp_fops;

	result = cdev_add(&mvtp_dev->mcdev, vtp_dev_num, 1);
	if (result < 0) {
	    goto fail;
	}
	void * etd=kmalloc(sizeof(struct elantech_data),GFP_KERNEL);
	if(etd==NULL){
	    printk("vtp_init:Could not allock mem of etd\n");
	    goto fail;
	}
	mvtp_dev->etd=etd;

	struct input_dev *input_dev1=input_allocate_device();

	if (input_dev1== NULL) {
		printk("<3>Bad input_alloc_device()\n");
		goto fail;
		return -1;
	}

	input_dev1->name = "Virtual Touch Pad";
	input_dev1->phys = "vtp";
	input_dev1->id.bustype = BUS_VIRTUAL;
	input_dev1->id.vendor = 0x0000;
	input_dev1->id.product = 0x0000;
	input_dev1->id.version = 0x0000;

	__set_bit(INPUT_PROP_POINTER,input_dev1->propbit);
	__set_bit(EV_ABS, input_dev1->evbit);
	__set_bit(EV_KEY,input_dev1->evbit);
	__clear_bit(EV_REL,input_dev1->evbit);

//	set_bit(REL_X, input_dev1->relbit);
//	set_bit(REL_Y, input_dev1->relbit);
//	set_bit(REL_WHEEL, input_dev1->relbit);
//	set_bit(REL_HWHEEL, input_dev1->relbit);

//	set_bit(EV_KEY, input_dev1->evbit);
	__set_bit(BTN_LEFT, input_dev1->keybit);
	__set_bit(BTN_RIGHT, input_dev1->keybit);
	__set_bit(BTN_MIDDLE, input_dev1->keybit);



	__set_bit(BTN_TOUCH,input_dev1->keybit);
    __set_bit(BTN_TOOL_FINGER,input_dev1->keybit);
    __set_bit(BTN_TOOL_DOUBLETAP,input_dev1->keybit);
    __set_bit(BTN_TOOL_TRIPLETAP,input_dev1->keybit);
    __set_bit(BTN_TOOL_QUADTAP,input_dev1->keybit);
    __set_bit(BTN_TOOL_QUINTTAP,input_dev1->keybit);

   // set_bit(ABS_X,input_dev1->absbit);
   // set_bit(ABS_Y,input_dev1->absbit);
   // set_bit(ABS_PRESSURE,input_dev1->absbit);
   // set_bit(ABS_TOOL_WIDTH,input_dev1->absbit);

    //TODO get x_min y_min x_max y_max
    int x_min=0,y_min=0;
    int x_max=1920,y_max=1080;

    /* For X to recognize me as touchpad. */
    input_set_abs_params(input_dev1, ABS_X, x_min, x_max, 0, 0);
    input_set_abs_params(input_dev1, ABS_Y, y_min, y_max, 0, 0);
    /*
     * range of pressure and width is the same as v2,
     * report ABS_PRESSURE, ABS_TOOL_WIDTH for compatibility.
     */
    input_set_abs_params(input_dev1, ABS_PRESSURE, PMIN,
                         PMAX, 0, 0);
    input_set_abs_params(input_dev1, ABS_TOOL_WIDTH, WMIN,
                         WMAX, 0, 0);
    /* Multitouch capable pad, up to 5 fingers. */
    input_mt_init_slots(input_dev1, VTP_MAX_FINGER, 0);
    input_set_abs_params(input_dev1, ABS_MT_POSITION_X, x_min, x_max, 0, 0);
    input_set_abs_params(input_dev1, ABS_MT_POSITION_Y, y_min, y_max, 0, 0);
    input_set_abs_params(input_dev1, ABS_MT_PRESSURE, PMIN,
                         PMAX, 0, 0);
    /*
     * The firmware reports how many trace lines the finger spans,
     * convert to surface unit as Protocol-B requires.
     */
    input_set_abs_params(input_dev1, ABS_MT_TOUCH_MAJOR, 0,
                         WMAX* 2, 0, 0);

    input_abs_set_res(input_dev1,ABS_X,X_RES);
    input_abs_set_res(input_dev1,ABS_Y,Y_RES);

    input_abs_set_res(input_dev1,ABS_MT_POSITION_X,X_RES);
    input_abs_set_res(input_dev1,ABS_MT_POSITION_Y,Y_RES);



	if ( (result = input_register_device(input_dev1)) != 0 ) {
	    goto fail;
	}


    mvtp_dev->etd->tp_dev=input_dev1;

	printk("__init: Android Virtual Mouse Driver Initialized.\n");
	return 0;

    fail:
    return -1;

}

static void vtp_exit(void)
{
	input_unregister_device(mvtp_dev->etd->tp_dev);
	cdev_del(&mvtp_dev->mcdev);
	unregister_chrdev_region(vtp_dev_num, 1);
	printk("<3>Android Virtual Mouse Driver unloaded.\n");
};

module_init(vtp_init);
module_exit(vtp_exit);
