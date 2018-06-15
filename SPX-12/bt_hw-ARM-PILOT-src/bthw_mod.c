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
 * PILOT BT controller driver
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
#include "driver_hal.h"
#include "bt.h"
#include "bt_hw.h"
#include "dbgout.h"
#include "coreTypes.h"
#include "reset.h"

#define BT_DRIVER_NAME		"PILOT_bt"

typedef int (*HostResetHandler) (void);

static int	notify_reboot0 (struct notifier_block * this, unsigned long code, void * unused);
static int BtHwResetHandler0 (void);
static HostResetHandler BtHwResetHandler = BtHwResetHandler0;

static void *bt_virt_base = (void*)SE_KCS5_BT_SMIC_MAIL_VA_BASE;
static bt_core_funcs_t *bt_core_funcs;
static int bt_hal_id;
static bool bt_enabled_by_user = FALSE;
static int btflag = 0;
static int kcsflag = 0;
static int mailboxflag = 0;
static struct notifier_block bthwreboot_notifier = 
{ 
	.notifier_call = notify_reboot0,
}; 


static inline uint8_t
bt_read_reg(uint8_t reg)
{
	return ioread8(bt_virt_base + reg);
}

static inline void
bt_write_reg(uint8_t data, uint8_t reg)
{
	iowrite8(data, bt_virt_base + reg);
}

static void
set_bmc_busy (void)	
{
	volatile uint8 reg;
	
	reg = bt_read_reg (BTCR);
	if (reg & B_BUSY)
	{
		return;
	}
	reg |= B_BUSY;
	bt_write_reg (reg, BTCR);
}

static void
clr_bmc_busy (void)	
{

	volatile uint8 reg;

	reg = bt_read_reg (BTCR);
	reg |= CLR_RD_PTR | CLR_WR_PTR;
	bt_write_reg (reg, BTCR);
	
	reg = bt_read_reg (BTCR);
	if (!(reg & B_BUSY))
	{
		return;
	}
	reg |= B_BUSY;
	bt_write_reg (reg, BTCR);

}

static int 
notify_reboot0 (struct notifier_block *this, unsigned long code, void *unused) 
{ 
	if (code == SYS_DOWN || code == SYS_HALT) 
	{ 
		set_bmc_busy ();	
	} 
	return NOTIFY_DONE; 
} 

static uint8_t
bt_num_ch(void)
{
	return BT_CHANNEL_NUM;
}

static void
bt_enable_interrupt(void)
{
	volatile uint8 reg;

	clr_bmc_busy ();
	
	reg = bt_read_reg (BTIMSR);
	reg |= H2B_IRQ_EN;
	bt_write_reg (reg, BTIMSR);

	reg = bt_read_reg (KCSIER);
	reg |= BTINTEN;
	bt_write_reg (reg, KCSIER);
}

static int
BtHwResetHandler0 (void)
{
	bt_core_funcs->process_bt_intr (bt_hal_id, BT_CHANNEL_NUM - 1, 0xFF);
//	bt_enable_interrupt();
	return 0;
}

static void
bt_disable_interrupt(void)
{
	volatile uint8 reg;
	
	reg = bt_read_reg (BTIMSR);
	reg &= ~H2B_IRQ_EN;
	bt_write_reg (reg, BTIMSR);

	reg = bt_read_reg (KCSIER);
	reg &= ~BTINTEN;
	bt_write_reg (reg, KCSIER);
	
}

static void 
bthw_interrupt_enable_user (void)
{
//	if (bt_enabled_by_user == FALSE)
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
	volatile uint8 reg;

	if (Ch >= BT_CHANNEL_NUM)
	{
		TCRIT ("Invalid BT Channel Number\n");
		return;
	}
	reg = bt_read_reg (BTCR);
	reg |= SMS_ATN;
	bt_write_reg (reg, BTCR);
}

static void
bt_clr_sms_bit (u8 Ch)
{
	if (Ch >= BT_CHANNEL_NUM)
	{
		TCRIT ("Invalid BT Channel Number\n");
		return;
	}
}

static void 
clr_write_fifo_ptr (void) 
{ 
	volatile uint8_t reg; 

	reg = bt_read_reg (BTCR); 
	reg |= CLR_WR_PTR; 
	bt_write_reg (reg, BTCR); 
} 

static void 
set_b2h_attention (void)         
{ 
	volatile uint8_t reg; 

	reg = bt_read_reg (BTCR); 
	if (reg & B2H_ATN)
    { 
		return;
	} 
	reg |= B2H_ATN; 
    bt_write_reg (reg, BTCR); 
} 

static void
bthw_write_bt_data_buf (u8 Ch, u8* Buf, u32 Len)
{
	if (Ch >= BT_CHANNEL_NUM) 
	{
		TCRIT ("Invalid BT Channel Number\n"); 
		return; 
	} 
	clr_write_fifo_ptr (); 
	bt_write_reg (Len, BTDR); 
	while (Len) 
	{ 
		bt_write_reg (*Buf, BTDR); 
		Buf += 1; 
		Len -= 1; 
	} 
	set_b2h_attention (); 
}

static void 
clr_h2b_attention (void)         
{ 
	volatile uint8_t reg; 

	reg = bt_read_reg (BTCR); 
	if (!(reg & H2B_ATN)) 
    { 
		return;
	} 
	reg |= H2B_ATN; 
    bt_write_reg (reg, BTCR); 
} 

static void
bthw_read_bt_data_buf (u8 Ch, u8* Buf)
{
	volatile uint8_t Len; 

	Buf[0] = 0; 
	if (Ch >= BT_CHANNEL_NUM) 
	{ 
		TCRIT ("Invalid BT Channel Number\n"); 
		return; 
	} 
	set_bmc_busy (); 

	Len = bt_read_reg (BTDR); 
	*Buf = Len;
	Buf++;
	while (Len) 
	{ 
		*Buf = bt_read_reg (BTDR); 
		Len -= 1; 
		Buf += 1; 
    } 

    clr_h2b_attention (); 
    clr_bmc_busy (); 

}

static bt_hal_operations_t bt_hw_ops = 
{
	.num_bt_ch = bt_num_ch,
	.bt_interrupt_enable_user = bthw_interrupt_enable_user,
	.bt_interrupt_disable_user = bthw_interrupt_disable_user,
	.set_sms_bit = bt_set_sms_bit,
	.clr_sms_bit = bt_clr_sms_bit,
	.write_bt_data_buf = bthw_write_bt_data_buf,
	.read_bt_data_buf = bthw_read_bt_data_buf,
};

static hw_hal_t bt_hw_hal = 
{
	.dev_type = EDEV_TYPE_BT,
	.owner = THIS_MODULE,
	.devname = BT_DRIVER_NAME,
	.num_instances = BT_CHANNEL_NUM,
	.phal_ops = (void *) &bt_hw_ops
};

static int
IsSpuriousIntr (void)
{
	unsigned char intr_stat;

	kcsflag = 0;
	btflag = 0;
	mailboxflag = 0;

	intr_stat = bt_read_reg (KCSISR);
	if (intr_stat & 0x20)
		btflag = 1;
	if (intr_stat & 0x1C)
		kcsflag = 1;
	if (intr_stat & 0x80)
		mailboxflag = 1;

	if (kcsflag || btflag || mailboxflag)
		return 0;
	else
		return 1;
}

static irqreturn_t
bt_irq_handler(int irq, void *dev_id)
{
	int ret = 0;
	
	if (IsSpuriousIntr())
	{
		return IRQ_HANDLED;
	}
	if (btflag)
	{
		ret = bt_core_funcs->process_bt_intr (bt_hal_id, BT_CHANNEL_NUM - 1, 0);
	}
	if (ret != 0) 
	{ 
		TCRIT ("BT core IRQ handler failed\n"); 
	}
	if (kcsflag || mailboxflag)
		return IRQ_NONE;
	else
		return IRQ_HANDLED;
}

static void
bt_init_hw(void)
{
	bt_disable_interrupt();
	clr_bmc_busy ();
}

static int
bt_module_init(void)
{
	int ret;

	bt_hal_id = register_hw_hal_module(&bt_hw_hal, (void **) &bt_core_funcs);
	if (bt_hal_id < 0)
	{
		TCRIT("%s: register BT HAL module failed\n", BT_DRIVER_NAME);
		return bt_hal_id;
	}
	bt_init_hw();
	ret = request_irq(BT_IRQ, bt_irq_handler, IRQF_DISABLED | IRQF_SHARED, BT_DRIVER_NAME, (void*)bt_enable_interrupt);
	if (ret)
	{
		TCRIT("%s: request irq failed\n", BT_DRIVER_NAME);
		goto out_iomap;
	}
	install_reset_handler (BtHwResetHandler);
	register_reboot_notifier(&bthwreboot_notifier); 
 
	printk ("BT hw module loaded\n");
	return 0;

out_iomap:
	unregister_hw_hal_module(EDEV_TYPE_BT, bt_hal_id);

	return ret;
}

static void
bt_module_exit(void)
{
	bt_disable_interrupt();
	set_bmc_busy ();
	free_irq(BT_IRQ, (void*)bt_enable_interrupt);
	unregister_hw_hal_module(EDEV_TYPE_BT, bt_hal_id);
	uninstall_reset_handler (BtHwResetHandler);
	unregister_reboot_notifier(&bthwreboot_notifier);
	printk ("BT hw module unloaded\n");
}

module_init(bt_module_init);
module_exit(bt_module_exit);

MODULE_AUTHOR("Rama Rao Bisa");
MODULE_DESCRIPTION("PILOT BT controller driver");
MODULE_LICENSE("GPL");
