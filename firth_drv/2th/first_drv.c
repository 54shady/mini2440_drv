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

int major;
volatile unsigned int *gpbcon = NULL;
volatile unsigned int *gpbdat = NULL;

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
	major = register_chrdev(0, "my_first_drv", &first_ops);	

	gpbcon = (unsigned int *)ioremap(0x56000010, 1);	
	gpbdat = gpbcon + 1;

	printk("first_drv_init ok\n");
	return 0;
}

static void first_drv_exit(void)
{
	unregister_chrdev(major, "my_first_drv");
	iounmap(gpbcon);

	printk("first_drv_exit ok\n");
}

module_init(first_drv_init);
module_exit(first_drv_exit);

MODULE_LICENSE("GPL");
