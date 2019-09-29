#ifndef USB_CAN_FILE_IOCTL_H
#define USB_CAN_FILE_IOCTL_H
#ifdef USER_APP
#include <stdint.h>
#include <sys/ioctl.h>
#endif

typedef struct {
    uint32_t    len;
    uint8_t     *buf; 
} ioctl_rx_buffer_params;

#define USB_CAN_FILE_IOCTL_PING             _IOW('q', 102, uint32_t)
#define USB_CAN_FILE_IOCTL_GET_BAUDRATE     _IOR('q', 103, uint64_t)
#define USB_CAN_FILE_IOCTL_GET_RX_BUFFER    _IOR('q', 104, ioctl_rx_buffer_params)

#endif
