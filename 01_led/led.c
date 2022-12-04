#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/uaccess.h>

#define GPIOLED_CNT 1
#define GPIOLED_NAME "gpio_led"
#define LED_OFF 1
#define LED_ON 0

typedef struct {
    int major;                  //主设备号
    int minor;                  //次设备号
    dev_t devid;                //设备号
    struct device_node *pNode;  //设备节点
    int gpio_num;               //led使用的gpio编号
    struct cdev cdev;           //字符设备
    struct class *pClass;       //类
    struct device *pDevice;     //设备
}gpioled_dev;

gpioled_dev pGpio_led;

static int led_open(struct inode *pInode, struct file *pFilp)
{
    pFilp->private_data = &pGpio_led;
    return 0;
}

static ssize_t led_read(struct file *pFilp, char __user *pBuf, size_t cnt, loff_t *pOfft)
{
    return 0;
}

static ssize_t led_write(struct file *pFilp, const char __user *pBuf, size_t cnt, loff_t *pOfft)
{
    int ret = 0;
    unsigned char pDatabuf[1];
    unsigned char ledstat;
    gpioled_dev *pDev = pFilp->private_data;
    ret = copy_from_user(pDatabuf, pBuf, cnt);
    if(ret < 0){
        printk("copy from user failed!\n");
    }
    ledstat = pDatabuf[0];
    if(ledstat == LED_ON){
        gpio_set_value(pDev->gpio_num, 0);
    }else if(ledstat == LED_OFF){
        gpio_set_value(pDev->gpio_num, 1);
    }else{
        printk("error led status\n");
    }
    return ret;
}

static int led_release(struct inode *pInode, struct file *pFilp)
{
    return 0;
}

struct file_operations gpioled_fops = {
    .owner      = THIS_MODULE,
    .open       = led_open,
    .read       = led_read,
    .write      = led_write,
    .release    = led_release,
};

static int __init led_init(void)
{

    int ret = 0;
    printk("led init !\n");
    //gpio init
    pGpio_led.pNode = of_find_node_by_path("/led");
        if(pGpio_led.pNode == NULL){
        printk("not find node-led\n");
        return -1;
    }else{
        printk("find node-led\n");
    }

    pGpio_led.gpio_num = of_get_named_gpio(pGpio_led.pNode, "led-gpio", 0);
    if(pGpio_led.gpio_num < 0){
        printk("get gpio num failed!\n");
        return -1;
    }else{
        printk("gpio num is %d\n", pGpio_led.gpio_num);
    }

    ret = gpio_direction_output(pGpio_led.gpio_num, 1);
    if(ret < 0){
        printk("set gpio output type failed!\n");
        return ret;
    }

    //register char deivce driver
    if(pGpio_led.major){
        pGpio_led.devid = MKDEV(pGpio_led.major, 0);
        ret = register_chrdev_region(pGpio_led.devid, GPIOLED_CNT, GPIOLED_NAME);
    }else{
        ret = alloc_chrdev_region(&pGpio_led.devid, 0, GPIOLED_CNT, GPIOLED_NAME);
        pGpio_led.major = MAJOR(pGpio_led.devid);
        pGpio_led.minor = MINOR(pGpio_led.devid);
    }
    if(ret < 0){
        printk("require device id failed!\n");
        return ret;
    }else{
        printk("major is %d, minor is %d\n", pGpio_led.major, pGpio_led.minor);
    }

    //cdev init
    pGpio_led.cdev.owner = THIS_MODULE;
    cdev_init(&pGpio_led.cdev, &gpioled_fops);
    cdev_add(&pGpio_led.cdev, pGpio_led.devid, GPIOLED_CNT);

    //class init
    pGpio_led.pClass = class_create(THIS_MODULE, GPIOLED_NAME);
    if(IS_ERR(pGpio_led.pClass)){
        cdev_del(&pGpio_led.cdev);
        unregister_chrdev_region(pGpio_led.devid, GPIOLED_CNT);
        return PTR_ERR(pGpio_led.pClass);
    }else{
        printk("class create success!\n");
    }

    //cteate device
    pGpio_led.pDevice = device_create(pGpio_led.pClass, NULL, pGpio_led.devid, NULL, GPIOLED_NAME);
    if(IS_ERR(pGpio_led.pDevice)){
        class_destroy(pGpio_led.pClass);
        cdev_del(&pGpio_led.cdev);
        unregister_chrdev_region(pGpio_led.devid, GPIOLED_CNT);
        return PTR_ERR(pGpio_led.pDevice);
    }else{
        printk("device create success!\n");
    }

    return ret;
}

static void __exit led_exit(void)
{
    printk("led exit\n");

    device_destroy(pGpio_led.pClass, pGpio_led.devid);
    class_destroy(pGpio_led.pClass);
    cdev_del(&pGpio_led.cdev);
    unregister_chrdev_region(pGpio_led.devid, GPIOLED_CNT);
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MASH");