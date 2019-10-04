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

#ifndef _USB_CAN_API_H_
#define  _USB_CAN_API_H_
#include "usb_can_driver.h"
#include "usb_can_data.h"
#include "usb_can_file_ioctl.h"

int usb_can_request_data(int type, struct usb_can_dev_t* dev);

// deserialize usb packet data to ioctl params.
int usb_can_deserialize_to_ioclt_can_frame(ioctl_can_frame_param_t __user* user_params, const uint8_t* in_buff);
int usb_can_deserialize_to_ioclt_baudrate(uint64_t __user* user_params, const uint8_t* in_buff);

#endif