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
static struct class *sec_drv_class;
static struct class_device *sec_drv_class_dev;
volatile unsigned int *gpgcon;
volatile unsigned int *gpgdat;

static int sec_drv_open(struct inode *inode, struct file *file)
{
	/*
	 * K1,K2,K3,K4对应GPG0，GPG3，GPG5，GPG6
	 * 配置GPG0，GPG3，GPG5，GPG6为输入引脚 
	 */
	*gpgcon &= ~((0x3<<(0*2)) | (0x3<<(3*2)) | (0x3<<(5*2)) | (0x3<<(6*2) | (0x3<<(7*2) | (0x3<<(11*2)))));

	return 0;
}

ssize_t sec_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	/* 返回4个引脚的电平 */
	unsigned char key_vals[6];
	int regval;

//	printk("size = %d\n", size);
	if (size != sizeof(key_vals))
		return -EINVAL;

	regval = *gpgdat;
	key_vals[0] = (regval & (1<<0)) ? 1 : 0;
	key_vals[1] = (regval & (1<<3)) ? 1 : 0;
	key_vals[2] = (regval & (1<<5)) ? 1 : 0;
	key_vals[3] = (regval & (1<<6)) ? 1 : 0;
	key_vals[4] = (regval & (1<<7)) ? 1 : 0;
	key_vals[5] = (regval & (1<<11)) ? 1 : 0;

	if (copy_to_user(buf, key_vals, sizeof(key_vals)) != 0)
		return -1;
	
	return sizeof(key_vals);
}

static struct file_operations sec_drv_fops = {
    .owner  =   THIS_MODULE,
    .open   =   sec_drv_open,
	.read	=	sec_drv_read,
};

static int sec_drv_init(void)
{
	major = register_chrdev(0, "sec_drv", &sec_drv_fops);	
	
	sec_drv_class = class_create(THIS_MODULE, "sec_drv");
	sec_drv_class_dev = class_device_create(sec_drv_class, NULL, MKDEV(major, 0), NULL, "buttons"); /* /dev/buttons */

	gpgcon = (volatile unsigned int *)ioremap(0x56000060, 1);
	gpgdat = gpgcon + 1;
	
	return 0;
}

static void sec_drv_exit(void)
{
	unregister_chrdev(major, "sec_drv");
	class_device_unregister(sec_drv_class_dev);
	class_destroy(sec_drv_class);
	iounmap(gpgcon);
}

module_init(sec_drv_init);
module_exit(sec_drv_exit);
MODULE_LICENSE("GPL");
