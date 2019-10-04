/**
 * Copyright (C) 2019 Shymaxtic
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

#ifndef _USB_CAN_DRIVER_H_
#define _USB_CAN_DRIVER_H_

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

// Minor range for my devices from the usb maintainer.
#define USB_CAN_MINOR_BASE  192


// my data struct 
struct usb_can_dev_t {
    struct usb_device *udev;            // usb device.
    struct usb_interface* uif;          // usb interface.
    unsigned char*  bulk_in_buffer;     // buffer to receive data.
    size_t  bulk_in_size;               // size of receive buffer.
    __u8    bulk_in_endpointAddr;       // in endpoint address.
    __u8    bulk_out_endpointAddr;      // out endpoint address.
    struct kref kref;
};


#endif