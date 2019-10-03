#ifndef _USB_CAN_API_H_
#define  _USB_CAN_API_H_
#include "usb_can_data.h"

int usb_can_request_data(int type, struct usb_can_dev_t* dev);

#endif