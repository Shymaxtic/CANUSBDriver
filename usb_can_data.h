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


#ifndef _USB_CAN_DATA_H_
#define _USB_CAN_DATA_H_


#define D_USB_PACKET_MAX_LEN        64 // equal sizeof endpoint

typedef enum {
    E_USB_CAN_PING = 0,
    E_USB_CAN_GET_BAUDRATE,
    E_USB_CAN_GET_CAN_FRAME
} usb_can_packet_type_t;

/*
  Info of chunk of can frames sent via usb.
 | Frame num  (1 byte) |[| ID (4 bytes) | info (1 byte) | data (8 bytes) |...]
*/
typedef struct {
    uint32_t     u32id;
    uint8_t      u8info;
    uint8_t      au8data[8];
} __attribute__ ((packed)) usb_can_frame_info_t; // 13 bytes

#define D_USB_PACKET_DATA_LEN       (D_USB_PACKET_MAX_LEN - sizeof(uint8_t))

// data packet for my usb. maximum is equal 64 byte endpoint of TM4C123GH6PGE
typedef struct usb_can_packet_t {
    uint8_t u8type;
    uint8_t au8data[D_USB_PACKET_DATA_LEN];
}   __attribute__ ((packed)) usb_can_packet_t;


#endif