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

struct my_led
{
	char swich;
	char name;
};

static int first_drv_open(struct inode *inode, struct file *file)
{
	printk("first_drv_open\n");
	*gpbcon &= ~((0x3<<(5*2)) | (0x3<<(6*2)) | (0x3<<(7*2)) | (0x3<<(8*2)));
	*gpbcon |= ((0x1<<(5*2)) | (0x1<<(6*2)) | (0x1<<(7*2)) | (0x1<<(8*2)));

	return 0;
}

static ssize_t first_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	struct my_led ml;

	if (copy_from_user(&ml, buf, count) != 0)
		return -1;
	
	printk("first_drv_write, ml.swich = %d, ml.name = %d\n", ml.swich, ml.name);

	if (ml.swich == 1)
	{
		switch (ml.name)
		{
			case 1:
				*gpbdat &= ~(1 << 5);
				break;
			case 2:
				*gpbdat &= ~(1 << 6);
				break;
			case 3:
				*gpbdat &= ~(1 << 7);
				break;
			case 4:
				*gpbdat &= ~(1 << 8);
				break;
			case 5:
				*gpbdat &= ~((1<<5) | (1<<6) | (1<<7) | (1<<8));
				break;
			default:
				printk("Error\n");
				break;
		};
	}
	else
	{
		switch (ml.name)
		{
			case 1:
				*gpbdat |= (1 << 5);
				break;
			case 2:
				*gpbdat |= (1 << 6);
				break;
			case 3:
				*gpbdat |= (1 << 7);
				break;
			case 4:
				*gpbdat |= (1 << 8);
				break;
			case 5:
				*gpbdat |= (1<<5) | (1<<6) | (1<<7) | (1<<8);
				break;
			default:
				printk("Error\n");
				break;
		};
	}
		
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
