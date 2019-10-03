#ifndef _USB_CAN_DATA_H_
#define _USB_CAN_DATA_H_
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define D_USB_PACKET_MAX_LEN        32
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

typedef enum {
    E_USB_CAN_PING = 0,
    E_USB_CAN_GET_BAUDRATE
} usb_can_packet_type_t;
// data packet for my usb.
typedef struct usb_can_packet_t {
    u8 type;
    u8 data[D_USB_PACKET_MAX_LEN];
}   __attribute__ ((packed)) usb_can_packet_t;


#endif