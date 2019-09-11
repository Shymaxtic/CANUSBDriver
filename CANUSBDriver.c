#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>

#define D_VENDOR_ID         0x08f7
#define D_PRODUCT_ID        0x0002

static struct usb_device_id g_id_table[] = {
    {USB_DEVICE(D_VENDOR_ID, D_PRODUCT_ID)},
    {},
};
MODULE_DEVICE_TABLE(usb, g_id_table);

// function for interact to usb device.
static canusbdriver_probe(struct usb_interface* interface, const struct usb_device_id* id) {

}

static void canusbdriver_disconnect(struct usb_interface* interface) {

}

static struct usb_driver canusbdriver = {
    .name = "Shymaxtic CAN USB",
    .probe = canusbdriver_probe,
    .disconnect = canusbdriver_disconnect,
    .id_table = g_id_table,
};

// function for modprobe module.
static int __init canusbdriver_init(void) {

}

static void __exit canusbdriver_exit(void) {

}

module_init(canusbdriver_init);
module_exit(canusbdriver_exit);

MODULE_AUTHOR("Shymaxtic CAN USB");
MODULE_DESCRIPTION("CAN USB driver");
MODULE_LICENSE("GPL");