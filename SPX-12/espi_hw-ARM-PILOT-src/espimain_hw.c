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
 * File name: espimain_hw.c
 * eSPI hardware driver is implemented for hardware controller.
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#include "driver_hal.h"
#include "reset.h"
#include "espi.h"
#include "espi_ioctl.h"
#include "espi_hw.h"


static void *pilot_espi_virt_base;

static int pilot_espi_hal_id;
static espi_core_funcs_t *espi_core_ops;
static int espi_hw_driver_init = 0;

struct espi_ch_data		p_rx_data;
struct espi_ch_data		oob_rx_data;
struct espi_ch_data		flash_rx_data;
u32						vw_gpio;
u32						vw_sysevent;

static unsigned char first_vw_triggered = 0;

struct HostToBMC_VW_INDEX
{
	const unsigned char index;
	unsigned char prev_value;
	unsigned char curr_value;
};

struct HostToBMC_VW_INDEX Host2BMC_VW_INDEX[] = {
	{0x02, 0, 0},
	{0x03, 0, 0},
	{0x07, 0, 0},
	{0x41, 0, 0},
	{0x42, 0, 0},
	{0x43, 0, 0},
	{0x44, 0, 0},
	{0x47, 0, 0}
};

static unsigned char BMC2Host_VW_INDEX[] = {
	0x4,
	0x5,
	0x6,
	0x40,
	0x45,
	0x46
};

#define H2B_ARRAY_SIZE	(sizeof(Host2BMC_VW_INDEX)/(sizeof(struct HostToBMC_VW_INDEX)))
#define B2H_ARRAY_SIZE	(sizeof(BMC2Host_VW_INDEX)/(1*sizeof(unsigned char)))


int espi_irq[ PILOT_ESPI_CHANNEL_NUM ] =
{
	PILOT_ESPI_IRQ,
	IRQ_ESPI_VWIRE,
	IRQ_ESPI_OOB,
	IRQ_ESPI_SAFS,
};

typedef irqreturn_t (* HANDLER) ( int this_irq, void *dev_id);

static HANDLER espi_irq_handlers[ PILOT_ESPI_CHANNEL_NUM ] =
{
	espi_handler_0,
	espi_handler_1,
	espi_handler_2,
	espi_handler_3,
};

/* -------------------------------------------------- */

inline uint8_t pilot_espi_read8_reg(uint16_t offset)
{
	return ioread8((void * __iomem)pilot_espi_virt_base + offset);
}

inline void pilot_espi_write8_reg(uint8_t value, uint16_t offset)
{
	iowrite8(value, (void * __iomem)pilot_espi_virt_base + offset);
}

inline uint32_t pilot_espi_read32_reg(uint32_t offset)
{
	return( ioread32( (void * __iomem)pilot_espi_virt_base + offset ) );
}

inline void pilot_espi_write32_reg(uint32_t value, uint32_t offset) 
{
	iowrite32(value, (void * __iomem)pilot_espi_virt_base + offset);
}
/* -------------------------------------------------- */

static unsigned char pilot_espi_num_ch(void)
{
    return PILOT_ESPI_CHANNEL_NUM;
}

void pilot_espi_reset (void)
{
	// Reset eSPI controller
	printk(KERN_WARNING "%s: pilot_espi_reset\n", PILOT_ESPI_DRIVER_NAME);
}

static void pilot_espi_read_data(u8 channel, char *buf)
{
	char vw_index = *buf;
	
	switch (channel)
	{
		case 0: // Peripheral 
			memcpy(buf, p_rx_data.buff, p_rx_data.buf_len);
			break;
		case 1: // Virtual Wire
			if (vw_index == 1)		// System Event
				*buf = vw_sysevent;
			else if (vw_index == 2)	// GPIO
				*buf = vw_gpio;
			break;
		case 2: // OOB 
			memcpy(buf, oob_rx_data.buff, oob_rx_data.buf_len);
			break;
		case 3: // Runtime flash 
			memcpy(buf, flash_rx_data.buff, flash_rx_data.buf_len);
			break;
		default:
			break;
	}
}

static void pilot_espi_slave_registers(u32 *gen_status, u32 *ch_status )
{	
	*gen_status = pilot_espi_read32_reg(PILOT_ESPI_GEN_CAPCONF);
	
	*ch_status = 0;
	
	if (pilot_espi_read32_reg(PILOT_ESPI_CH0_CAPCONF) & 0x3)
		*ch_status |= (0x1 << 0);
	if (pilot_espi_read32_reg(PILOT_ESPI_CH1_CAPCONF) & 0x3)
		*ch_status |= (0x1 << 1);
	if (pilot_espi_read32_reg(PILOT_ESPI_CH2_CAPCONF) & 0x3)
		*ch_status |= (0x1 << 2);
	if (pilot_espi_read32_reg(PILOT_ESPI_CH3_CAPCONF) & 0x3)
		*ch_status |= (0x1 << 3);
	
#ifdef ESPI_DEBUG
	// Slave Registers
	printk("PILOT_ESPI_GEN_CAPCONF	(08h) 0x%x\n", (unsigned int)pilot_espi_read32_reg(PILOT_ESPI_GEN_CAPCONF));
	printk("PILOT_ESPI_CH0_CAPCONF	(10h) 0x%x\n", (unsigned int)pilot_espi_read32_reg(PILOT_ESPI_CH0_CAPCONF));
	printk("PILOT_ESPI_CH1_CAPCONF	(20h) 0x%x\n", (unsigned int)pilot_espi_read32_reg(PILOT_ESPI_CH1_CAPCONF));
	printk("PILOT_ESPI_CH2_CAPCONF	(30h) 0x%x\n", (unsigned int)pilot_espi_read32_reg(PILOT_ESPI_CH2_CAPCONF));
	printk("PILOT_ESPI_CH3_CAPCONF	(40h) 0x%x\n", (unsigned int)pilot_espi_read32_reg(PILOT_ESPI_CH3_CAPCONF));
#endif
}

static int auto_ack_enabled(unsigned char index, unsigned char value)
{
    if (index == VW_INDEX_05h)
    {
        if (((value & (1 << (SLAVE_BOOT_LOAD_STATUS + 4))) == (1 << (SLAVE_BOOT_LOAD_STATUS + 4))) &&
            ((*(volatile unsigned char*)(ESPI_VW_HW_ACK_CTL) & ESPI_HW_SLV_BLS) == 0))
        return 1;

        if (((value & (1 << (SLAVE_BOOT_LOAD_DONE + 4))) == (1 << (SLAVE_BOOT_LOAD_DONE + 4))) &&
            ((*(volatile unsigned char*)(ESPI_VW_HW_ACK_CTL) & ESPI_HW_SLV_BLD) == 0))
        return 1;
    }

    if ((index == VW_INDEX_40h) &&
        ((value & (1 << (SUSACK_N + 4))) == (1 << (SUSACK_N + 4))) &&
        ((*(volatile unsigned char*)(ESPI_VW_HW_ACK_CTL) & ESPI_HW_SUSACK) == 0))
        return 1;

    if ((index == VW_INDEX_04h) &&
        ((value & (1 << (OOB_RST_ACK + 4))) == (1 << (OOB_RST_ACK + 4))) &&
        ((*(volatile unsigned char*)(ESPI_VW_HW_ACK_CTL) & ESPI_HW_OOBACK) == 0))
        return 1;

    if ((index == VW_INDEX_06h) &&
        ((value & (1 << (HOST_RST_ACK + 4))) == (1 << (HOST_RST_ACK + 4))) &&
        ((*(volatile unsigned char*)(ESPI_VW_HW_ACK_CTL) & ESPI_HW_HSTACK) == 0))
        return 1;

    return 0;
}

static int pilot_send_vwire(unsigned char index, unsigned char value)
{
    int i = 0;

    if ((pilot_espi_read32_reg(PILOT_ESPI_CH1_CAPCONF) & CH1_VW_CHANNEL_EN) != CH1_VW_CHANNEL_EN)
    {
        printk(KERN_WARNING "%s: cannot send vwire(0x%x,0x%x), channel disabled\n", PILOT_ESPI_DRIVER_NAME, index, value);
        return -EAGAIN;
    }
	
    // Check if chip revision is A2
    if (((*(volatile u32 *)(IO_ADDRESS(SE_SYSCLK_VA_BASE + 0x50)) & 0xF) >= 2) && auto_ack_enabled(index, value))
    {
        printk(KERN_WARNING "%s: HW ACK enabled, returning\n", PILOT_ESPI_DRIVER_NAME);
        return 0;
    }
	
    for (i = 0; i < B2H_ARRAY_SIZE; i++)
    {
        if (BMC2Host_VW_INDEX[i] == index)
        {
			pilot_espi_write8_reg(value, (PILOT_ESPI_VW_INDEX0 + index));
            //printk("put_vwire index:0x%x value:0x%x\n", index, value);
            break;
        }
    }

    if (i == B2H_ARRAY_SIZE)
    {
        printk(KERN_WARNING "%s: Unsupported B2H vwire index :%x\n", PILOT_ESPI_DRIVER_NAME, index);
        return -EINVAL;
    }

    return 0;
}

static void pilot_vw_system_event(void)
{
    int i = 0, j = 0;
    volatile unsigned char vw_status = 0;
    unsigned char value;
    unsigned char temp = 0;

    for (i = 0; i < H2B_ARRAY_SIZE; i++)
    {
        vw_status = pilot_espi_read8_reg(PILOT_ESPI_VW_INDEX0 + Host2BMC_VW_INDEX[i].index);

        // Check if valid is set for any of the bits
        if ((vw_status & VW_IDX_VALID) == VW_IDX_VALID)
        {
            first_vw_triggered = 1;
            Host2BMC_VW_INDEX[i].prev_value = Host2BMC_VW_INDEX[i].curr_value;
            for (j = 0; j < 4; j++)
            {
                if ((Host2BMC_VW_INDEX[i].prev_value & (1 << j)) != (vw_status & (1 << j)))
                {
                    temp |= (1 << j);
                }
            }
            Host2BMC_VW_INDEX[i].curr_value &= ~temp;
            Host2BMC_VW_INDEX[i].curr_value |= (((temp & 0xF) << 4) | (vw_status & 0xF));
            //printk("vwire_valid Index(%x) hwsts:0x%x swsts:0x%x\n", Host2BMC_VW_INDEX[i].index, vw_status, Host2BMC_VW_INDEX[i].curr_value);
        }
        temp = 0;
    }
	
    for (i = 0; i < H2B_ARRAY_SIZE; i++)
    {
        if ((Host2BMC_VW_INDEX[i].index == VW_INDEX_41h) && ((Host2BMC_VW_INDEX[i].curr_value & 0x10) == 0x10))
        {
            value = Host2BMC_VW_INDEX[i].curr_value & 0x1;
            if (pilot_send_vwire(VW_INDEX_40h, SET_VWIRE(SUSACK_N, value)) == 0)
            {
                // clear the valid bits for this interrupt and make it as previous
                Host2BMC_VW_INDEX[i].prev_value = Host2BMC_VW_INDEX[i].curr_value;
                Host2BMC_VW_INDEX[i].curr_value &= 0xEF;
            }
        }

        if ((Host2BMC_VW_INDEX[i].index == VW_INDEX_03h) && ((Host2BMC_VW_INDEX[i].curr_value & 0x40) == 0x40))
        {
            value = (Host2BMC_VW_INDEX[i].curr_value >> 2) & 0x1;
            if (pilot_send_vwire(VW_INDEX_04h, SET_VWIRE(OOB_RST_ACK, value)) == 0)
            {
                // clear the valid bit for this interrupt and make it as previous
                Host2BMC_VW_INDEX[i].prev_value = Host2BMC_VW_INDEX[i].curr_value;
                Host2BMC_VW_INDEX[i].curr_value &= 0xBF;
            }
        }

        if ((Host2BMC_VW_INDEX[i].index == VW_INDEX_07h) && ((Host2BMC_VW_INDEX[i].curr_value & 0x10) == 0x10))
        {
            value = Host2BMC_VW_INDEX[i].curr_value & 0x1;
            if (pilot_send_vwire(VW_INDEX_06h, SET_VWIRE(HOST_RST_ACK, value)) == 0)
            {
                // clear the valid bit for this interrupt and make it as previous
                Host2BMC_VW_INDEX[i].prev_value = Host2BMC_VW_INDEX[i].curr_value;
                Host2BMC_VW_INDEX[i].curr_value &= 0xEF;
            }
        }
    }
}

static void pilot_espi_hw_init(void) 
{
	// espi hardware initialization
	if ( pilot_send_vwire( VW_INDEX_05h, (unsigned char)(SET_VWIRE(SLAVE_BOOT_LOAD_DONE, 1) | SET_VWIRE(SLAVE_BOOT_LOAD_STATUS, 1)) ) != 0)
		printk(KERN_WARNING "%s: failed to send virtual wire\n", PILOT_ESPI_DRIVER_NAME);

	//first_vw_triggered = 1;
	pilot_vw_system_event();
	
	// Enable interrupts
	pilot_espi_write32_reg(pilot_espi_read32_reg(PILOT_ESPI_VW_CTRL) | VW_INTR_EN,     PILOT_ESPI_VW_CTRL);
	pilot_espi_write32_reg(pilot_espi_read32_reg(PILOT_ESPI_OOB_CTRL) | H2B_INTR_EN,   PILOT_ESPI_OOB_CTRL);
	pilot_espi_write32_reg(pilot_espi_read32_reg(PILOT_ESPI_FLASH_CTRL) | H2B_INTR_EN, PILOT_ESPI_FLASH_CTRL);
}

/* -------------------------------------------------- */
static espi_hal_operations_t pilot_espi_hw_ops = {
	.num_espi_ch			= pilot_espi_num_ch,
	.reset_espi				= pilot_espi_reset,
	.read_espi_data_in		= pilot_espi_read_data,
	.read_espi_slave_regs	= pilot_espi_slave_registers,
};

static hw_hal_t pilot_espi_hw_hal = {
	.dev_type		= EDEV_TYPE_ESPI,
	.owner			= THIS_MODULE,
	.devname		= PILOT_ESPI_DRIVER_NAME,
	.num_instances	= PILOT_ESPI_CHANNEL_NUM,
	.phal_ops		= (void *) &pilot_espi_hw_ops
};

static void pilot_espi_handler(int channel_num)
{
	unsigned long status;
	
	if (channel_num == 1)// VW
	{	
		// In case the channel was not enabled at the time of driver init or after espi reset
		if (!first_vw_triggered)
		{
			// Set SLAVE_BOOT_LOAD_DONE
			pilot_send_vwire(VW_INDEX_05h, (unsigned char)(SET_VWIRE(SLAVE_BOOT_LOAD_DONE, 1) | SET_VWIRE(SLAVE_BOOT_LOAD_STATUS, 1)));
		}
		pilot_vw_system_event();
	}
	else if (channel_num == 2)// OOB
	{
		status = pilot_espi_read32_reg(PILOT_ESPI_OOB_STATUS);
		
		if ((status & H2B_INTR_STATUS) == H2B_INTR_STATUS)
		{
			pilot_espi_write32_reg((H2B_PACKET_VALID | H2B_INTR_STATUS), PILOT_ESPI_OOB_STATUS);
		}
	}
	else if (channel_num == 3)// Flash
	{
		status = pilot_espi_read32_reg(PILOT_ESPI_FLASH_STATUS);
		
		if ((status & H2B_INTR_STATUS) == H2B_INTR_STATUS)
		{
			pilot_espi_write32_reg((H2B_PACKET_VALID | H2B_INTR_STATUS), PILOT_ESPI_FLASH_STATUS);
		}
	}
	else
	{
		printk(KERN_WARNING "%s: Invalid interrupt\n", PILOT_ESPI_DRIVER_NAME);
	}
	
	return;
}

static int pilot_espi_reset_handler(void)
{
	int i = 0;
	
	// VW clears all index except 02 on a eSPI reset
	for (i = 0;i < H2B_ARRAY_SIZE; i++)
	{
		// Index 2 and 42 are not reset with eSPI reset
		if ((Host2BMC_VW_INDEX[i].index == 0x2) || (Host2BMC_VW_INDEX[i].index == 0x42))
			continue;
	
		Host2BMC_VW_INDEX[i].prev_value = 0;
		Host2BMC_VW_INDEX[i].curr_value = 0;
	}
	
	// Set SLAVE_BOOT_LOAD_DONE
	pilot_send_vwire(VW_INDEX_05h, (unsigned char)(SET_VWIRE(SLAVE_BOOT_LOAD_DONE, 1) | SET_VWIRE(SLAVE_BOOT_LOAD_STATUS, 1)) );
	
	first_vw_triggered = 0;
	
	// OOB clear the RD/WR pointer
	pilot_espi_write32_reg(pilot_espi_read32_reg(PILOT_ESPI_OOB_CTRL) | (B2H_WRPTR_CLEAR | B2H_RDPTR_CLEAR | H2B_WRPTR_CLEAR | H2B_RDPTR_CLEAR),   PILOT_ESPI_OOB_CTRL);
	
	// Flash clear the RD/WR pointer
	pilot_espi_write32_reg(pilot_espi_read32_reg(PILOT_ESPI_FLASH_CTRL) | (B2H_WRPTR_CLEAR | B2H_RDPTR_CLEAR | H2B_WRPTR_CLEAR | H2B_RDPTR_CLEAR), PILOT_ESPI_FLASH_CTRL);
	
	return 0;
}

irqreturn_t espi_handler_0( int this_irq, void *dev_id)
{
    pilot_espi_handler(0);
    return IRQ_HANDLED;
}

irqreturn_t espi_handler_1( int this_irq, void *dev_id)
{
    pilot_espi_handler(1);
    return IRQ_HANDLED;
}

irqreturn_t espi_handler_2( int this_irq, void *dev_id)
{
    pilot_espi_handler(2);
    return IRQ_HANDLED;
}

irqreturn_t espi_handler_3( int this_irq, void *dev_id)
{
    pilot_espi_handler(3);
    return IRQ_HANDLED;
}

static irqreturn_t oobdummy_irqhandler(int irq, void *dev_id)
{
    printk("%s: oobdummy handler\n", PILOT_ESPI_DRIVER_NAME);
    return IRQ_HANDLED;
}

int pilot_espi_init(void)
{
	int i;
	uint32_t status;

	extern int espi_core_loaded;
	if (!espi_core_loaded)
		return -1;
	
	status = *(volatile u32 *)(IO_ADDRESS(SE_SYSCLK_VA_BASE + 0x0C));
	if(!(status & (0x1 << 14)))
	{
		printk(KERN_WARNING "%s: eSPI mode is not enable 0x%x\n", PILOT_ESPI_DRIVER_NAME, (unsigned int)status);
		return -EIO;
	}
	
	pilot_espi_hal_id = register_hw_hal_module(&pilot_espi_hw_hal, (void **) &espi_core_ops);
	if (pilot_espi_hal_id < 0) {
		printk(KERN_WARNING "%s: register HAL HW module failed\n", PILOT_ESPI_DRIVER_NAME);
		return pilot_espi_hal_id;
	}
	
	pilot_espi_virt_base = ioremap_nocache(PILOT_ESPI_REG_BASE, SZ_4K);
	if (!pilot_espi_virt_base) {
		printk(KERN_WARNING "%s: ioremap failed\n", PILOT_ESPI_DRIVER_NAME);
		unregister_hw_hal_module(EDEV_TYPE_ESPI, pilot_espi_hal_id);
		return -ENOMEM;
	}
	
	// Request IRQ, VM, OOB and Flash
	for (i = 1; i < PILOT_ESPI_CHANNEL_NUM; i++)
	{
		if( request_irq( espi_irq[i], espi_irq_handlers[i], IRQF_DISABLED, "pilot_espi", (pilot_espi_virt_base + i)) < 0 )
			printk(KERN_WARNING "%s: request irq %d failed\n", PILOT_ESPI_DRIVER_NAME, espi_irq[i]);
	}
	
    // Need it for prior A2 chips 
    if ((*(volatile u32 *)(IO_ADDRESS(SE_SYSCLK_VA_BASE + 0x50)) & 0xF) < 2)
    {
        if (request_irq(IRQ_SIO_PSR, oobdummy_irqhandler, IRQF_SHARED, "pilot_espi_oobdummy", &oobdummy_irqhandler) < 0)
            printk(KERN_WARNING "%s: Failed to request irq %d", PILOT_ESPI_DRIVER_NAME, IRQ_SIO_PSR);
    }

	p_rx_data.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
	oob_rx_data.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
	flash_rx_data.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);

	pilot_espi_hw_init();
	
	install_reset_handler(pilot_espi_reset_handler);
	
	espi_hw_driver_init = 1;
	
	printk("The eSPI HW Driver is loaded successfully.\n" );
	return 0;
}

void pilot_espi_exit(void)
{
	int i;
	kfree(p_rx_data.buff);
	kfree(oob_rx_data.buff);
	kfree(flash_rx_data.buff);
	
	for (i = 1; i < PILOT_ESPI_CHANNEL_NUM; i++)
		free_irq(espi_irq[i], (pilot_espi_virt_base + i));
	
	if ((*(volatile u32 *)(IO_ADDRESS(SE_SYSCLK_VA_BASE + 0x50)) & 0xF) < 2)
		free_irq(IRQ_SIO_PSR, &oobdummy_irqhandler);
	
	iounmap (pilot_espi_virt_base);
	iounmap (pilot_espi_virt_base);
	
	uninstall_reset_handler(pilot_espi_reset_handler);
	
	unregister_hw_hal_module(EDEV_TYPE_ESPI, pilot_espi_hal_id);
	
	return;
}

module_init (pilot_espi_init);
module_exit (pilot_espi_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("PILOT SoC eSPI Driver");
MODULE_LICENSE ("GPL");
