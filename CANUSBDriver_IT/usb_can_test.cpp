// Copyright (C) 2019 Shymaxtic
// 
// This file is part of CANUSBdriver.
// 
// CANUSBdriver is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// CANUSBdriver is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with CANUSBdriver.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include "../usb_can_file_ioctl.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#define SHYMAXTIC_USB_DEV   "/dev/can0"

int main(int argc, char** argv) {
    if (argc <= 1) {
        std::cout << "Invalid argument, need device name\n";
        return -EINVAL;
    } 
    char dev_name[64];
    strcpy(dev_name, "/dev/");
    strcat(dev_name, argv[1]);

    // Open device file.
    int ret = 0;
    int fd = 0;
    fd = open(dev_name, O_RDWR);
    if (fd < 0) {
        std::cerr << "ERR: Failed to open file with error: " << (int) fd << std::endl;
        return fd;
    }
    uint32_t dummyVal = 0;
    ret = ioctl(fd, USB_CAN_FILE_IOCTL_PING, &dummyVal);
    if (ret != 0) {
        std::cerr << "ERR: Failed to ioctl file with error: " << (int) ret << std::endl;
    }
    int tryTime = 100;
    while (tryTime--)
    {
        uint64_t baudrate = 0;
        ret = ioctl(fd, USB_CAN_FILE_IOCTL_GET_BAUDRATE, &baudrate);
        if (ret) {
            std::cerr << "ERR: Failed to USB_CAN_FILE_IOCTL_GET_BAUDRATE file with error" << (int) ret << std::endl;
        } else {
            std::cout << "INFO: baudrate " << (int)baudrate << std::endl;
        }
        usleep(10000);
    }
    ioctl_can_frame_param_t can_frame;
    ret = ioctl(fd, USB_CAN_FILE_IOCTL_GET_CAN_FRAME, &can_frame);
    if (ret) {
        std::cerr << "ERR: Failed to USB_CAN_FILE_IOCTL_GET_BAUDRATE file with error" << (int) ret << std::endl;
    } else {
        std::cout << "INFO: number frame: [" << (int)can_frame.u8frame_nums << "] {[" << (int)can_frame.frame_info[0].u32id << "][" << (int)can_frame.frame_info[0].u8info 
        << "]}" << std::endl;
    } 
    
    close(fd);
    std::cout << "Finished test\n";
    return 0;
}