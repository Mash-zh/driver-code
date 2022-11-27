#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int num = 10;
module_param(num, int, 0660);

static int __init hello_init(void)
{
    printk("hello world!\n");
    printk("num is %d\n", num);
    return 0;
}

static void __exit hello_exit(void)
{
    printk("num is %d\n", num);
    printk("goodble!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
