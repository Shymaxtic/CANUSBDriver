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

#ifndef USB_CAN_FILE_IOCTL_H
#define USB_CAN_FILE_IOCTL_H
#ifdef USER_APP
#include <stdint.h>
#include <sys/ioctl.h>
#endif
#include "usb_can_data.h"


#define D_MAX_FRAME_PACKED  4

typedef struct {
    uint8_t                 u8frame_nums;
    usb_can_frame_info_t    frame_info[D_MAX_FRAME_PACKED];         // list of usb_can_frame_info_t
} ioctl_can_frame_param_t;

#define USB_CAN_FILE_IOCTL_PING             _IOW('q', 102, uint32_t)
#define USB_CAN_FILE_IOCTL_GET_BAUDRATE     _IOR('q', 103, uint64_t)
#define USB_CAN_FILE_IOCTL_GET_CAN_FRAME    _IOR('q', 104, ioctl_can_frame_param_t)

#endif
