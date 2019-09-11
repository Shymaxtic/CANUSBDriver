#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static int __init canusbdriver_init(void) {

}

static void __exit canusbdriver_exit(void) {

}

module_init(canusbdriver_init);
module_exit(canusbdriver_exit);

MODULE_AUTHOR("Shymaxtic CAN USB");
MODULE_DESCRIPTION("CAN USB driver");
MODULE_LICENSE("GPL");