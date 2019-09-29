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
    uint64_t baudrate = 0;
    ret = ioctl(fd, USB_CAN_FILE_IOCTL_GET_BAUDRATE, &baudrate);
    if (ret) {
        std::cerr << "ERR: Failed to USB_CAN_FILE_IOCTL_GET_BAUDRATE file with error" << (int) ret << std::endl;
    } 
    std::cout << "INFO: baudrate " << (int)baudrate << std::endl;
    close(fd);
    return 0;
}