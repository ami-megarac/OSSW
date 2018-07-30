/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

 /*
 * File name: espimain.c
 * This driver provides common layer, independent of the hardware, for the eSPI driver.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>
#include <asm/io.h>
#include "helper.h"
#include "driver_hal.h"
#include "dbgout.h"
#include "espi.h"
#include "espi_ioctl.h"

#ifdef HAVE_UNLOCKED_IOCTL
  #if HAVE_UNLOCKED_IOCTL
  #define USE_UNLOCKED_IOCTL
  #endif
#endif

#define ESPI_MAJOR			176
#define ESPI_MINOR			0
#define ESPI_MAX_DEVICES	255
#define ESPI_DEV_NAME		"espi"


static int read_espi_data (struct espi_dev *pdev, unsigned long arg);
static int read_espi_slave_regs (struct espi_dev *pdev, unsigned long arg);

static struct cdev *espi_cdev;
static dev_t espi_devno = MKDEV(ESPI_MAJOR, ESPI_MINOR);


int register_espi_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
	struct espi_hal *pespi_hal;

	pespi_hal = (struct espi_hal*) kmalloc (sizeof(struct espi_hal), GFP_KERNEL);
	if (!pespi_hal)
	{
		return -ENOMEM;
	}
	// IPMI Msg
	pespi_hal->pespi_buf = (eSPIBuf_T*) kmalloc (num_instances * sizeof(eSPIBuf_T), GFP_KERNEL);
	if (!pespi_hal->pespi_buf)
	{
		kfree (pespi_hal);
		return -ENOMEM;
	}

	pespi_hal->pipmi_cmd_msg = (IPMICmdMsg_T*) vmalloc (num_instances * sizeof(IPMICmdMsg_T));
	if (!pespi_hal->pipmi_cmd_msg)
	{
		kfree (pespi_hal->pespi_buf);
		kfree (pespi_hal);
		return -ENOMEM;
	}
	
	pespi_hal->pespi_hal_ops = ( espi_hal_operations_t *) phal_ops;
	*phw_data = (void *) pespi_hal;	

	return 0;	
}


int unregister_espi_hal_module (void *phw_data)
{
	struct espi_hal *pespi_hal = (struct espi_hal*) phw_data;

	vfree (pespi_hal->pipmi_cmd_msg);
	kfree (pespi_hal->pespi_buf);
	kfree (pespi_hal);

	return 0;
}

int read_espi_data (struct espi_dev *pdev, unsigned long arg)
{
	struct espi_data_t espi_data;

	if  (copy_from_user (&espi_data, (void*) arg, sizeof(struct espi_data_t)))
		return -EFAULT;

	pdev->pespi_hal->pespi_hal_ops->read_espi_data_in (espi_data.channel_num, espi_data.buffer);

	if  (copy_to_user ((void*) arg, &espi_data, sizeof(struct espi_data_t)))
		return -EFAULT;

	return 0;
}

int read_espi_slave_regs (struct espi_dev *pdev, unsigned long arg)
{
	struct espi_data_t espi_data;
	unsigned int num_channel;
	unsigned int gen_status;
	unsigned int ch_status;

	if  (copy_from_user (&espi_data, (void*) arg, sizeof(struct espi_data_t)))
		return -EFAULT;
    
	num_channel = pdev->pespi_hal->pespi_hal_ops->num_espi_ch();
	printk ("Number of channel support %d \n", num_channel);
	pdev->pespi_hal->pespi_hal_ops->read_espi_slave_regs(&gen_status, &ch_status);

	if (gen_status & (0x1 << 0))
		printk("Peripheral Channel Supported\n");
	if (gen_status & (0x1 << 1))
		printk("Virtual Wire Channel Supported\n");
	if (gen_status & (0x1 << 2))
		printk("OOB Message Channel Supported\n");
	if (gen_status & (0x1 << 3))
		printk("Flash Access Channel Supported\n");
	
	if (ch_status & (0x1 << 0))
		printk("Peripheral, ");
	if (ch_status & (0x1 << 1))
		printk("Virtual Wire, ");
	if (ch_status & (0x1 << 2))
		printk("OOB Message, ");
	if (ch_status & (0x1 << 3))
		printk("Flash Access, ");
	if (ch_status == 0)
		printk("NULL ");
	printk("Channels enable and ready\n");
	
	return 0;
}

static int espi_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	struct espi_hal *pespi_hal;
	struct espi_dev *pdev;
	eSPIBuf_T* peSPIBuffer;
	hw_info_t espi_hw_info;
	unsigned char open_count;
	int ret;

	ret = hw_open (EDEV_TYPE_ESPI, minor,&open_count, &espi_hw_info);
	if (ret)
		return -ENXIO;

	pespi_hal = espi_hw_info.pdrv_data;

	pdev = (struct espi_dev*)kmalloc(sizeof(struct espi_dev), GFP_KERNEL);
	
	if (!pdev)
	{
		hw_close (EDEV_TYPE_ESPI, minor, &open_count);
		printk (KERN_ERR "%s: failed to allocate espi private dev structure for espi iminor: %d\n", ESPI_DEV_NAME, minor);
		return -ENOMEM;
	}

	peSPIBuffer = &pespi_hal->pespi_buf [espi_hw_info.inst_num];
	
	pdev->pespi_hal = pespi_hal;
	file->private_data = pdev;

	if (open_count == 1)
	{
		// initialization buffer
	}

	return 0;
}


static int espi_release(struct inode *inode, struct file *file)
{
	int ret;
	unsigned char open_count;
	struct espi_dev *pdev = (struct espi_dev*)file->private_data;
	eSPIBuf_T *peSPIBuffer = &pdev->pespi_hal->pespi_buf [pdev->ch_num];
	
	ret = hw_close (EDEV_TYPE_ESPI, iminor(inode), &open_count);
	if(ret) { return -1; }
	
	if (open_count == 0) 
		peSPIBuffer->eSPIINuse = 0;
	
	pdev->pespi_hal = NULL;
	file->private_data = NULL;

	kfree (pdev);
	return 0;
}

#ifdef USE_UNLOCKED_IOCTL
static long espi_ioctl(struct file *file,unsigned int cmd, unsigned long arg)
#else
static int espi_ioctl(struct inode *inode, struct file *file,unsigned int cmd, unsigned long arg)
#endif
{
	int ret = 0;
	struct espi_dev *pdev = (struct espi_dev*) file->private_data;
	//struct espi_data_t *xfer = (void __user *)arg;
	
	switch (cmd)
	{
		case READ_ESPI_DATA:
			ret = read_espi_data ( pdev, arg );
			
			break;
		case READ_ESPI_SLAVE_REGS:
			ret = read_espi_slave_regs ( pdev, arg );
			
			break;
		default:
			printk ( "Invalid eSPI IOCTL\n");
			return -EINVAL;
	}
	return ret;
}


/* ----- Driver registration ---------------------------------------------- */
static struct file_operations espi_ops = {
	owner:		THIS_MODULE,
	read:		NULL,
	write:		NULL,
#ifdef USE_UNLOCKED_IOCTL
	unlocked_ioctl: espi_ioctl,
#else
	ioctl:		espi_ioctl,
#endif
	open:		espi_open,
	release:	espi_release,
};


static espi_core_funcs_t espi_core_funcs = {
	.get_espi_core_data = NULL,
};

static core_hal_t espi_core_hal = {
	.owner					= THIS_MODULE,
	.name					= "ESPI CORE",
	.dev_type				= EDEV_TYPE_ESPI,
	.register_hal_module	= register_espi_hal_module,
	.unregister_hal_module	= unregister_espi_hal_module,
	.pcore_funcs			= (void *)&espi_core_funcs
};


/*
 * eSPI driver init function
 */
int __init espi_init(void)
{
	int ret =0 ;
  
	/* espi device initialization */ 
	if ((ret = register_chrdev_region (espi_devno, ESPI_MAX_DEVICES, ESPI_DEV_NAME)) < 0)
	{
		printk (KERN_ERR "failed to register espi device <%s> (err: %d)\n", ESPI_DEV_NAME, ret);
		return ret;
	}
	
	espi_cdev = cdev_alloc ();
	if (!espi_cdev)
	{
		unregister_chrdev_region (espi_devno, ESPI_MAX_DEVICES);
		printk (KERN_ERR "%s: failed to allocate espi cdev structure\n", ESPI_DEV_NAME);
		return -1;
	}
	
	cdev_init (espi_cdev, &espi_ops);
	
	espi_cdev->owner = THIS_MODULE;
	
	if ((ret = cdev_add (espi_cdev, espi_devno, ESPI_MAX_DEVICES)) < 0)
	{
		cdev_del (espi_cdev);
		unregister_chrdev_region (espi_devno, ESPI_MAX_DEVICES);
		printk	(KERN_ERR "failed to add <%s> char device\n", ESPI_DEV_NAME);
		ret = -ENODEV;
		return ret;
	}
	
	if ((ret = register_core_hal_module (&espi_core_hal)) < 0)
	{
		printk(KERN_ERR "failed to register the Core espi module\n");
		cdev_del (espi_cdev);
		unregister_chrdev_region (espi_devno, ESPI_MAX_DEVICES);
		ret = -EINVAL;
		goto out_no_mem;
	}

	printk("The eSPI Driver is loaded successfully.\n" );
	return 0;
  
out_no_mem:
	cdev_del (espi_cdev);
	unregister_chrdev_region (espi_devno, ESPI_MAX_DEVICES);	

	return ret;
}

/*!
 * eSPI driver exit function
 */
void __exit espi_exit(void)
{
	unregister_core_hal_module (EDEV_TYPE_ESPI);
	unregister_chrdev_region (espi_devno, ESPI_MAX_DEVICES);
	
	if (NULL != espi_cdev)
	{
		cdev_del (espi_cdev);
	}

	printk ( "Unregistered the eSPI driver successfully\n");

	return;	
}

int espi_core_loaded =1;
EXPORT_SYMBOL(espi_core_loaded);

module_init(espi_init);
module_exit(espi_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("eSPI Common Driver");
MODULE_LICENSE ("GPL");

