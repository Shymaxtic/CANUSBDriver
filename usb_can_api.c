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
    buf->type = type;
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