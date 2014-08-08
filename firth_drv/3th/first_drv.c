#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include <linux/device.h>

int major;
volatile unsigned int *gpbcon = NULL;
volatile unsigned int *gpbdat = NULL;
static struct class *firstdrv_class;
static struct class_device *firstdrv_class_dev;

static int first_drv_open(struct inode *inode, struct file *file)
{
	printk("first_drv_open\n");
	return 0;
}

static ssize_t first_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	int val;
	if (copy_from_user(&val, buf, count) != 0)
		return -1;
	
	printk("*buf = 0x%04x\n", *buf);
	printk("first_drv_write, val = %d\n", val);

	return 0;
}

static struct file_operations first_ops = {
	.owner	= THIS_MODULE,
	.open 	= first_drv_open,
	.write  = first_drv_write,
};

static int first_drv_init(void)
{
	long err;
	
	major = register_chrdev(0, "my_first_drv", &first_ops);	

	firstdrv_class = class_create(THIS_MODULE, "firstdrv");	
	if (IS_ERR(firstdrv_class))
	{
		printk("class create error\n");
		return -1;
	}

	firstdrv_class_dev = class_device_create(firstdrv_class, NULL, MKDEV(major, 0), NULL, "xyz"); /* /dev/xyz */
	if (IS_ERR(firstdrv_class_dev)) 
	{
		printk("class create device error\n");
		err = PTR_ERR(firstdrv_class_dev);
		goto err_out;
	}

	gpbcon = (unsigned int *)ioremap(0x56000010, 1);	
	gpbdat = gpbcon + 1;

	printk("first_drv_init ok, major = %d\n", major);
	return 0;

err_out:
	unregister_chrdev(major, "my_first_drv");	
	class_destroy(firstdrv_class);
	return -1;
}

static void first_drv_exit(void)
{
	unregister_chrdev(major, "my_first_drv");
	class_device_unregister(firstdrv_class_dev);
	class_destroy(firstdrv_class);	
	iounmap(gpbcon);

	printk("first_drv_exit ok\n");
}

module_init(first_drv_init);
module_exit(first_drv_exit);

MODULE_LICENSE("GPL");
