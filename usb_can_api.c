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

#include "usb_can_api.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

// callback function when finish writing bulk .
static void usb_can_write_bulk_callback(struct urb *urb) {
    struct usb_can_dev_t *dev = urb->context;
    // sync/ asyn unlink faults are not errors.
    if (urb->status && !(urb->status == -ENOENT || 
	      urb->status == -ECONNRESET ||
	      urb->status == -ESHUTDOWN)) {
		dev_dbg(&dev->uif->dev,
			"%s - nonzero write bulk status received: %d",
			__FUNCTION__, urb->status);
    }

    /* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			urb->transfer_buffer, urb->transfer_dma);
}

int usb_can_request_data(int type, struct usb_can_dev_t* dev) {
    int ret = 0;
    struct urb *urb = NULL;
    usb_can_packet_t* buf = NULL;
    int rcvLen = 0;
    if (!dev) {
        ret = -ENOMEM;
        goto exit;
    }
    // creat an urb
    urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!urb) {
        ret = -ENOMEM;
        goto error;
    }
    // create buffer to receive or transmit data.
    buf = usb_alloc_coherent(dev->udev, sizeof(usb_can_packet_t), GFP_KERNEL, &urb->transfer_dma); //TODO: read transfer_dma
    if (!buf) {
        ret = -ENOMEM;
        goto error;
    }
    buf->u8type = type;
    // Transmition phase.---            
    // fill the urb
    usb_fill_bulk_urb(urb, dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
                    buf, sizeof(usb_can_packet_t), usb_can_write_bulk_callback, dev);
    urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
                
    // send the data out the bulk port.
    ret = usb_submit_urb(urb, GFP_KERNEL) ;
    if (ret) {
        pr_err("%s - Failed to submit write urb with error %d", __FUNCTION__, ret);
        goto error;
    }
    usb_free_urb(urb);
    // Receive phase.---
    ret = usb_bulk_msg(dev->udev,
                usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
                dev->bulk_in_buffer,
                min(dev->bulk_in_size, sizeof(usb_can_packet_t)),
                (int *) &rcvLen, HZ*10);
    // TODO: check mismatch len.       
exit:    
    return ret;
error:
    usb_free_coherent(dev->udev, sizeof(usb_can_packet_t), buf, urb->transfer_dma);
    kfree(buf);
    usb_free_urb(urb);
    return ret;    
}