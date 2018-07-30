/****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************/

/****************************************************************
 *
 * bthw_mod.c
 * AST BT controller driver
 *
*****************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/reboot.h>
#include <linux/delay.h>
#include "driver_hal.h"
#include "bt.h"
#include "bt_hw.h"
#include "dbgout.h"
#include "coreTypes.h"
#include "reset.h"

#ifdef DEBUG
#define dbgprint(fmt, args...)       printk (KERN_INFO fmt, ##args) 
#else
#define dbgprint(fmt, args...)		 
#endif

#define BT_DRIVER_NAME		"AST_iBT"
#define BT_CHANNEL_NUM		1

typedef int (*HostResetHandler) (void);

static void *bt_virt_base;
static bt_core_funcs_t *bt_core_funcs = NULL;
static int bt_hal_id;
static bool bt_enabled_by_user = FALSE;

static int	notify_reboot0 (struct notifier_block * this, unsigned long code, void * unused);

static struct notifier_block bthwreboot_notifier = 
{ 
	.notifier_call = notify_reboot0,
}; 

static int BtHwResetHandler0 (void);
static HostResetHandler BtHwResetHandler = BtHwResetHandler0;
static void set_bmc_busy (void);	
static void clr_bmc_busy (void);

static int 
notify_reboot0 (struct notifier_block *this, unsigned long code, void *unused) 
{ 
	if (code == SYS_DOWN || code == SYS_HALT) 
	{ 
		set_bmc_busy ();	
	} 
	return NOTIFY_DONE; 
} 

static int
BtHwResetHandler0 (void)
{
	if (bt_core_funcs)
	{
		bt_core_funcs->process_bt_intr (bt_hal_id, BT_CHANNEL_NUM - 1, 0xFF);
	}

	return 0;
}

static inline uint8_t
bt_read_reg(uint16_t reg)
{
	return ioread8(bt_virt_base + reg);
}

static inline void
bt_write_reg(uint8_t data, uint16_t reg)
{
	iowrite8(data, bt_virt_base + reg);
}

static inline uint32_t
bt_read_reg32(uint16_t reg)
{
	return ioread32(bt_virt_base + reg);
}

static inline void
bt_write_reg32(uint32_t data, uint16_t reg)
{
	iowrite32(data, bt_virt_base + reg);
}

static void
bt_enable_channel(void)
{
	volatile uint32_t reg32;

	reg32 = bt_read_reg32(LPC_iBTCR0);
	reg32 |= LPC_iBTCR0_EnableiBT | LPC_iBTCR0_EnClrSvWrP | LPC_iBTCR0_EnClrSvRdP;
	bt_write_reg32(reg32, LPC_iBTCR0);
}

static void
bt_disable_channel(void)
{
	volatile uint32_t reg32;

	reg32 = bt_read_reg32(LPC_iBTCR0);
	reg32 &= ~LPC_iBTCR0_EnableiBT;
	bt_write_reg32(reg32, LPC_iBTCR0);
}

static void
bt_configure_io_port_addr(void)
{
	volatile uint32_t reg32;
	
	reg32 = 0;
	reg32 |= (CONFIG_SPX_FEATURE_BT_PORT_ADDRESS << 16);
	bt_write_reg32(reg32, LPC_iBTCR0);
}

static uint8_t
bt_num_ch(void)
{
	return BT_CHANNEL_NUM;
}

static void
bt_enable_interrupt(void)
{
	volatile uint32_t reg32;

	reg32 = bt_read_reg32(LPC_iBTCR1);
	reg32 |= LPC_iBTCR1_H2BI;
	bt_write_reg32(reg32, LPC_iBTCR1);
	
}

static void
bt_disable_interrupt(void)
{
	volatile uint32_t reg32;

	reg32 = bt_read_reg32(LPC_iBTCR1);
	reg32 &= ~LPC_iBTCR1_H2BI;
	bt_write_reg32(reg32, LPC_iBTCR1);
}

static void
bt_enable_fifo (void)
{

}

static void
bt_disable_fifo (void)
{

}

#if 0
static void
bt_enable_h2b_interrupt (void)
{
	bt_enable_interrupt();
}
#endif

static void
bt_disable_h2b_interrupt (void)
{
	bt_disable_interrupt();
}

static void 
bthw_interrupt_enable_user (void)
{

	clr_bmc_busy ();
	if (bt_enabled_by_user == FALSE)
	{
		bt_enabled_by_user = TRUE;
		bt_enable_interrupt();
	}
}

static void
bthw_interrupt_disable_user (void)
{
	bt_enabled_by_user = FALSE;
	bt_disable_interrupt();
}

static void
bt_set_sms_bit (u8 Ch)
{
	uint8_t reg = 0;
	
	if (Ch >= BT_CHANNEL_NUM)
	{
		TCRIT ("Invalid BT Channel Number\n");
		return;
	}
	reg = bt_read_reg (LPC_iBTCR4);
	if (reg & LPC_iBTCR4_SMS_ATN)
	{
		TCRIT ("SMS_ATN bit is already set\n");
		return;
	}

	/* Only set SMS_ATN bit */
	reg = 0;
	reg |= LPC_iBTCR4_SMS_ATN;
	bt_write_reg (reg, LPC_iBTCR4);
}

static void
bt_clr_sms_bit (u8 Ch)
{

}

#if 0
static void
clr_read_fifo_ptr (void)
{
	volatile uint8_t reg;
	reg = bt_read_reg (LPC_iBTCR4);
	reg |= LPC_iBTCR4_CLR_RD_PTR;
	bt_write_reg (reg, LPC_iBTCR4);
}
#endif

static void
clr_write_fifo_ptr (void)
{
	volatile uint8_t reg;
	reg = bt_read_reg (LPC_iBTCR4);
	reg |= LPC_iBTCR4_CLR_WR_PTR;
	bt_write_reg (reg, LPC_iBTCR4);
}

static void
set_b2h_attention (void)	
{
	volatile uint8_t reg = 0;
	/* Only set B2H_ATN bit */
	reg |= LPC_iBTCR4_B2H_ATN;
	bt_write_reg32 (reg, LPC_iBTCR4);
}

static void
bthw_write_bt_data_buf (u8 Ch, u8* Buf, u32 Len)
{
	volatile uint8_t reg = 0;
	
	if (Ch >= BT_CHANNEL_NUM)
	{
		TCRIT ("Invalid BT Channel Number\n");
		return;
	}

	dbgprint ("DRes: %x %x %x %x %x %x\n", Buf[0], Buf[1], Buf[2], Buf[3], Buf[4], Buf[5]);

	reg = bt_read_reg (LPC_iBTCR4);
	if (reg & LPC_iBTCR4_H_BUSY)
	{
		TCRIT ("Host is busy... and this should not happen\n");
	}

	bt_write_reg ((uint8_t)Len, LPC_iBTCR5);
	while (Len)
	{
		bt_write_reg (*Buf, LPC_iBTCR5);
		Buf += 1;
		Len -= 1;
	}

	set_b2h_attention ();
	clr_write_fifo_ptr();
}

static void
set_bmc_busy (void)	
{
	volatile uint8_t reg = 0;

	reg = bt_read_reg (LPC_iBTCR4);
	if (reg & LPC_iBTCR4_B_BUSY)
	{
		return;
	}

	/* Only set B_BUSY bit */
	reg = 0;
	reg |= LPC_iBTCR4_B_BUSY;
	bt_write_reg (reg, LPC_iBTCR4);
}

static void
clr_bmc_busy (void)	
{
	volatile uint8_t reg = 0;

	reg = bt_read_reg (LPC_iBTCR4);
	if (reg & LPC_iBTCR4_B_BUSY)
	{
		/* Only clear B_BUSY bit */
		reg = 0;
		reg |= LPC_iBTCR4_B_BUSY;
		bt_write_reg (reg, LPC_iBTCR4);
	}
}

static void
clr_h2b_intr_status (void)	
{
	volatile uint32_t reg32 = 0;

	/* Only clear H2BI */
	reg32 |= LPC_iBTCR2_H2BI;
	bt_write_reg (reg32, LPC_iBTCR2);
}

static void
clr_h2b_attention (void)	
{
	volatile uint8_t reg = 0;

	/* Only set H2B_ATN */
	reg |= LPC_iBTCR4_H2B_ATN;
	bt_write_reg (reg, LPC_iBTCR4);
}

static void
bthw_read_bt_data_buf (u8 Ch, u8* Buf)
{
	volatile uint8_t Len = 0;
	Buf[0] = 0;
	if (Ch >= BT_CHANNEL_NUM)
	{
		TCRIT ("Invalid BT Channel Number\n");
		return;
	}

	set_bmc_busy ();

	clr_h2b_attention ();

	Len = bt_read_reg (LPC_iBTCR5);
	*Buf = Len;
	Buf++;
	while (Len)
	{
		*Buf = bt_read_reg (LPC_iBTCR5);
		Len -= 1;
		Buf += 1;
	}

	dbgprint ("DReq: %x %x %x %x\n", testbuf[0], testbuf[1], testbuf[2], testbuf[3]);

	clr_bmc_busy ();
}

static bt_hal_operations_t bt_hw_ops = {
	.num_bt_ch = bt_num_ch,
	.bt_interrupt_enable_user = bthw_interrupt_enable_user,
	.bt_interrupt_disable_user = bthw_interrupt_disable_user,
	.set_sms_bit = bt_set_sms_bit,
	.clr_sms_bit = bt_clr_sms_bit,
	.write_bt_data_buf = bthw_write_bt_data_buf,
	.read_bt_data_buf = bthw_read_bt_data_buf,
};

static hw_hal_t bt_hw_hal = {
	.dev_type = EDEV_TYPE_BT,
	.owner = THIS_MODULE,
	.devname = BT_DRIVER_NAME,
	.num_instances = BT_CHANNEL_NUM,
	.phal_ops = (void *) &bt_hw_ops
};


static irqreturn_t
bt_irq_handler(int irq, void *dev_id)
{
	int ret = 0;
	volatile uint32_t reg32 = 0;
	
	
	reg32 = bt_read_reg32 (LPC_iBTCR2);
	if (reg32 & LPC_iBTCR2_H2BI)
	{
		clr_h2b_intr_status ();

		if ((bt_core_funcs) && (bt_enabled_by_user == TRUE))
		{
			ret = bt_core_funcs->process_bt_intr (bt_hal_id, BT_CHANNEL_NUM - 1, 0);
			if (ret != 0)
			{
				TCRIT("BT core IRQ handler failed\n");
			}
		}
	}
	else
	{
		return IRQ_NONE;
	}

	return IRQ_HANDLED;
}

static void
bt_init_hw(void)
{
	set_bmc_busy();

	bt_configure_io_port_addr();
	bt_disable_interrupt();
	bt_disable_channel();
	bt_enable_channel();
	bt_enable_fifo();
}

static int
bt_module_init(void)
{
	int ret;
	extern int bt_core_loaded;

	if (!bt_core_loaded)
			return -1;

	bt_hal_id = register_hw_hal_module(&bt_hw_hal, (void **) &bt_core_funcs);
	if (bt_hal_id < 0)
	{
		TCRIT("%s: register BT HAL module failed\n", BT_DRIVER_NAME);
		return bt_hal_id;
	}

	bt_virt_base = ioremap(BT_REG_BASE, BT_REG_SIZE);
	if (!bt_virt_base)
	{
		TCRIT("%s: ioremap failed\n", BT_DRIVER_NAME);
		ret = -ENOMEM;
		goto out_mem_region;
	}

	IRQ_SET_HIGH_LEVEL(BT_IRQ);
	IRQ_SET_LEVEL_TRIGGER(BT_IRQ);

	ret = request_irq(BT_IRQ, bt_irq_handler, IRQF_SHARED, BT_DRIVER_NAME, bt_virt_base);
	if (ret)
	{
		TCRIT("%s: request irq failed\n", BT_DRIVER_NAME);
		goto out_iomap;
	}
	
	bt_init_hw();

	install_reset_handler (BtHwResetHandler);
	register_reboot_notifier(&bthwreboot_notifier); 
 
	clr_bmc_busy ();
	
	printk ("BT hw module loaded\n");
	return 0;

out_iomap:
	iounmap(bt_virt_base);
out_mem_region:
	release_mem_region(BT_REG_BASE, BT_REG_SIZE);

	return ret;
}

static void
bt_module_exit(void)
{
	set_bmc_busy ();	
	bt_disable_interrupt();
	bt_disable_channel();
	bt_disable_fifo();
	bt_disable_h2b_interrupt();

	if (bt_virt_base)
	{
		free_irq(BT_IRQ, bt_virt_base);
		iounmap(bt_virt_base);
	}

	unregister_hw_hal_module(EDEV_TYPE_BT, bt_hal_id);
	uninstall_reset_handler (BtHwResetHandler);
	unregister_reboot_notifier(&bthwreboot_notifier);
	printk ("BT hw module unloaded\n");
}

module_init(bt_module_init);
module_exit(bt_module_exit);

MODULE_AUTHOR("Rama Rao Bisa");
MODULE_DESCRIPTION("AST2300 BT controller driver");
MODULE_LICENSE("GPL");
