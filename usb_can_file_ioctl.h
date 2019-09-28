#ifndef USB_CAN_FILE_IOCTL_H
#define USB_CAN_FILE_IOCTL_H
#ifdef USER_APP
#include <stdint.h>
#include <sys/ioctl.h>
#endif


#define USB_CAN_FILE_IOCTL_PING            _IOW('s', 102, uint32_t)
#define USB_CAN_FILE_IOCTL_GET_BAUDRATE    _IOR('s', 103, uint64_t)

#endif
