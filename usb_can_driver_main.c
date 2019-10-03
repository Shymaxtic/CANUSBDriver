/**
 * Copyright (C) 2019 quynhpp
 * 
 * This file is part of CANUSBdriver.
 * 
 * CANUSBdriver is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CANUSBdriver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CANUSBdriver.  If not, see <http://www.gnu.org/licenses/>.
 */



#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "usb_can_file_ioctl.h"
#include "usb_can_data.h"
#include "usb_can_api.h"

#define D_VENDOR_ID         0x1CBE
#define D_PRODUCT_ID        0x0003
#define D_MAX_USB_MEM_SIZE  512

static struct usb_device_id id_table[] = {
    {USB_DEVICE(D_VENDOR_ID, D_PRODUCT_ID)},
    {},
};
MODULE_DEVICE_TABLE(usb, id_table);

// get struct usb_can_dev_t containing kref.
#define to_usb_can_dev(kr)  container_of(kr, struct usb_can_dev_t, kref)

// the usb driver.
static struct usb_driver usb_can_driver;

// function to delete device struct when kref is zero.
static void usb_can_delete(struct kref *kref) {
    struct usb_can_dev_t *dev = to_usb_can_dev(kref);
    // release ref counter usb device in this driver.
    usb_put_dev(dev->udev);
    kfree(dev->bulk_in_buffer);
    kfree(dev);
}

// function to open device file
static int usb_can_open(struct inode *inode, struct file *file) {
    struct usb_can_dev_t *dev = NULL;
    struct usb_interface *interface = NULL;
    int subminor = 0;
    int ret = 0;

    subminor = iminor(inode);
    interface = usb_find_interface(&usb_can_driver, subminor);
    if (!interface) {
        pr_err("%s - error, can't find device for minor %d", __FUNCTION__, subminor);
        ret = -ENODEV;
        goto exit;
    }
    dev = usb_get_intfdata(interface);
    if (!dev) {
        ret = -ENODEV;
        goto exit;
    }
    // increment our usage count for the device.
    kref_get(&dev->kref);

    // save our object in the file's private struct.
    file->private_data = dev;
exit:
    return ret;
}

// function to close device file
static int usb_can_release(struct inode *inode, struct file *file) {
    struct usb_can_dev_t* dev = NULL;
    dev = (struct usb_can_dev_t*)file->private_data;
    if (!dev) {
        return -ENODEV;
    }
    // decrement the count on our device.
    kref_put(&dev->kref, usb_can_delete);
    return 0;
}

// function to read device file.
static ssize_t usb_can_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos) {
    int ret = 0;
    return ret;
}

// function to write device file.
static ssize_t usb_can_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos) {
    int ret = 0;
    return ret;
}

// function to ioctl device file.
static long usb_can_ioctl(struct file* file, unsigned int cmd, unsigned long arg__) {
    struct usb_can_dev_t *dev = NULL;
    int ret = 0;
    dev = (struct usb_can_dev_t*)file->private_data;
    switch (cmd) {
        case USB_CAN_FILE_IOCTL_PING: {     
            ret = usb_can_request_data(E_USB_CAN_PING, dev);
            if (ret) {
                goto error;
            }
        }
        break;
        case USB_CAN_FILE_IOCTL_GET_BAUDRATE: {
            // cast arg_ to address of user space variable.
            uint64_t __user* user_params = (int64_t*)arg__;
            if (unlikely(!user_params)) {
                ret = -EINVAL;
                break;
            }
            ret = usb_can_request_data(E_USB_CAN_GET_BAUDRATE, dev);
            if (ret) {
                goto error;
            } else {
                // put_user(dev->bulk_in_buffer, user_params);
                usb_can_packet_t rcvPck;
                memcpy(&rcvPck, dev->bulk_in_buffer, sizeof(usb_can_packet_t));
                if (copy_to_user(user_params, rcvPck.data, sizeof(uint64_t))) {
                    pr_err("%s - Faild to copy to user %d", __FUNCTION__, ret);
                    ret = -EFAULT;
                }
            } 
        }
        break;
        default:
        break;
    }
    return ret;
error:
    return ret;
}

static struct file_operations usb_can_fops = {
    .owner              = THIS_MODULE,
	.read               = usb_can_read,
	.write              = usb_can_write,
    .unlocked_ioctl     = usb_can_ioctl,
	.open               = usb_can_open,
	.release            = usb_can_release,
};

/* 
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with devfs and the driver core
 */
static struct usb_class_driver usb_can_class = {
	.name = "usb/can%d",
	.fops = &usb_can_fops,
	.minor_base = USB_CAN_MINOR_BASE,
};

static int usb_can_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_can_dev_t *dev = NULL;
    struct usb_host_interface *if_desc = NULL;
    struct usb_endpoint_descriptor *endpoint = NULL;
    size_t buffer_size;
    int i;
    int ret = -ENOMEM;

    // allocate memory for this device state and initialize it.
    dev = kzalloc(sizeof(struct usb_can_dev_t), GFP_KERNEL);
    if (!dev) {
        pr_err("Out of memory");
        goto error;
    }
    kref_init(&dev->kref);

    // get ref counter on usb device.
    dev->udev = usb_get_dev(interface_to_usbdev(interface));
    dev->uif = interface;

    // set up endpoint information.
    // use only the first bulk-in and bulk-out endpoints.

    // get current interface setting
    if_desc = interface->cur_altsetting;
    for (i = 0; i < if_desc->desc.bNumEndpoints; ++i) {
        endpoint = &if_desc->endpoint[i].desc;
        if (!dev->bulk_in_endpointAddr && 
            (endpoint->bEndpointAddress & USB_DIR_IN) &&
            ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
            == USB_ENDPOINT_XFER_BULK)) {
                // found a bulk in endpoint.
                buffer_size = endpoint->wMaxPacketSize;
                dev->bulk_in_size = buffer_size;
                dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
                dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
                if (!dev->bulk_in_buffer) {
                    pr_err("Failed to allocate bulk_in buffer");
                    goto error;
                }
            }

		if (!dev->bulk_out_endpointAddr &&
		    !(endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			/* we found a bulk out endpoint */
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
		}            
    }

    if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr)) {
		pr_err("Could not find both bulk-in and bulk-out endpoints");
		goto error;
	}

    // save our data pointer in this interface device.
    usb_set_intfdata(interface, dev);

    // ready to register device now.
    ret = usb_register_dev(interface, &usb_can_class);
    if (ret) {
        pr_err("Faild to get a minor for this device.");
        usb_set_intfdata(interface, NULL);
        goto error;
    }

    dev_info(&interface->dev, "USB CAN device now attached to /usb/can%d", interface->minor);
    return 0;

error:
    if (dev) {
        kref_put(&dev->kref, usb_can_delete);
    }
    return ret;
}

static void usb_can_disconnect(struct usb_interface *interface) {
    struct usb_can_dev_t *dev = NULL;
    int minor = interface->minor;

    dev = usb_get_intfdata(interface);
    usb_set_intfdata(interface, NULL);

    // give back our minor.
    usb_deregister_dev(interface, &usb_can_class);

    // decrement our usage count.
    kref_put(&dev->kref, usb_can_delete);

    dev_info(&interface->dev, "USB CAN device #%d now disconnected", minor);
}

static struct usb_driver usb_can_driver = {
    .name = "qusbcan",
    .id_table = id_table,
    .probe = usb_can_probe,
    .disconnect = usb_can_disconnect
};

static int __init usb_can_init(void) {
    int result;
    // register this driver with the USB subsystem.
    result = usb_register(&usb_can_driver);
    if (result) {
        pr_err("Failed to register usb can driver. Error number %d", result);
    }
    return result;
}

static void __exit usb_can_exit(void) {
    // deregister this driver with the USB subsystem
    usb_deregister(&usb_can_driver);
}

module_init(usb_can_init);
module_exit(usb_can_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shymaxtic CAN USB");
MODULE_DESCRIPTION("CAN USB driver");