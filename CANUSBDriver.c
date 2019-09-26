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

#define D_VENDOR_ID         0x090c
#define D_PRODUCT_ID        0x1000

static struct usb_device_id id_table[] = {
    {USB_DEVICE(D_VENDOR_ID, D_PRODUCT_ID)},
    {},
};
MODULE_DEVICE_TABLE(usb, id_table);

// Minor range for my devices from the usb maintainer.
#define USB_CAN_MINOR_BASE  192

// my data struct 
struct usb_can_driver_t {
    struct usb_device *udev;    // usb device.
    struct usb_interface* uif;  // usb interface.
    unsigned char*  bulk_in_buff;   // buffer to receive data.
    size_t  bulk_in_size;           // size of receive buffer.
    __u8    bulk_in_endpoint_addr;
    __u8    bulk_out_endpoint_addr;
    struct kref kref;
};

#define to_usb_can_dev(kr)  container_of(kr, struct usb_can_driver_t, kref)

static struct usb_driver usb_can_driver;

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