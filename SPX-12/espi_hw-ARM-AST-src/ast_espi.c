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
 * File name: ast_espi.c
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
#include "espi.h"
#include "espi_ioctl.h"
#include "ast_espi.h"


static void *ast_espi_virt_base;
static void *ast_gpio_virt_base;

static int ast_espi_hal_id;
static espi_core_funcs_t *espi_core_ops;
static int espi_hw_driver_init = 0;

struct espi_ch_data		p_rx_channel;
struct espi_ch_data		p_tx_channel;
struct espi_ch_data		np_tx_channel;	
struct espi_ch_data		oob_rx_channel;
struct espi_ch_data		oob_tx_channel;
struct espi_ch_data		flash_rx_channel;
struct espi_ch_data		flash_tx_channel;
u32						vw_gpio;
u32						vw_sysevent;

/* -------------------------------------------------- */

static inline u32 ast_espi_read_reg(u32 offset)
{
	return( ioread32( (void * __iomem)ast_espi_virt_base + offset ) );
}

static inline void ast_espi_write_reg(u32 value, u32 offset) 
{
	iowrite32(value, (void * __iomem)ast_espi_virt_base + offset);
}

/* -------------------------------------------------- */

static unsigned char ast_espi_num_ch(void)
{
    return AST_ESPI_CHANNEL_NUM;
}

void ast_espi_reset (void)
{
	// Reset eSPI controller
	printk(KERN_WARNING "%s: ast_espi_reset\n", AST_ESPI_DRIVER_NAME);
}

static void ast_espi_read_data(u8 channel, char *buf)
{
	char vw_index = *buf;
	
	switch (channel)
	{
		case 0: // Peripheral 
			memcpy(buf, p_rx_channel.buff, p_rx_channel.buf_len);
			break;
		case 1: // Virtual Wire
			if (vw_index == 1)		// System Event
				*buf = vw_sysevent;
			else if (vw_index == 2)	// GPIO
				*buf = vw_gpio;
			break;
		case 2: // OOB 
			memcpy(buf, oob_rx_channel.buff, oob_rx_channel.buf_len);
			break;
		case 3: // Runtime flash 
			memcpy(buf, flash_rx_channel.buff, flash_rx_channel.buf_len);
			break;
		default:
			break;
	}
}

static void ast_espi_slave_registers(u32 *gen_status, u32 *ch_status )
{	
	*gen_status = ast_espi_read_reg(AST_ESPI_GEN_CAPCONF);
	
	*ch_status = 0;
	
	if (ast_espi_read_reg(AST_ESPI_CH0_CAPCONF) & 0x3)
		*ch_status |= (0x1 << 0);
	if (ast_espi_read_reg(AST_ESPI_CH1_CAPCONF) & 0x3)
		*ch_status |= (0x1 << 1);
	if (ast_espi_read_reg(AST_ESPI_CH2_CAPCONF) & 0x3)
		*ch_status |= (0x1 << 2);
	if (ast_espi_read_reg(AST_ESPI_CH3_CAPCONF) & 0x3)
		*ch_status |= (0x1 << 3);
	
#ifdef ESPI_DEBUG
	// Slave Registers
	printk("AST_ESPI_GEN_CAPCONF	(0A0h) 0x%x\n", (unsigned int)ast_espi_read_reg(AST_ESPI_GEN_CAPCONF));
	printk("AST_ESPI_CH0_CAPCONF	(0A4h) 0x%x\n", (unsigned int)ast_espi_read_reg(AST_ESPI_CH0_CAPCONF));
	printk("AST_ESPI_CH1_CAPCONF	(0A8h) 0x%x\n", (unsigned int)ast_espi_read_reg(AST_ESPI_CH1_CAPCONF));
	printk("AST_ESPI_CH2_CAPCONF	(0ACh) 0x%x\n", (unsigned int)ast_espi_read_reg(AST_ESPI_CH2_CAPCONF));
	printk("AST_ESPI_CH3_CAPCONF	(0B0h) 0x%x\n", (unsigned int)ast_espi_read_reg(AST_ESPI_CH3_CAPCONF));
#endif
}

// Channel 3: Runtime Flash
static void ast_espi_flash_rx(void)
{
	int i = 0;
	struct espi_ch_data	 *flash_rx = &flash_rx_channel;
	u32 ctrl = ast_espi_read_reg(AST_ESPI_FLASH_RX_CTRL);
	//printk("cycle type = %x , tag = %x, len = %d byte \n", GET_CYCLE_TYPE(ctrl), GET_TAG(ctrl), GET_LEN(ctrl));

	flash_rx->full = 1;
	flash_rx->header = ctrl;
	if((GET_CYCLE_TYPE(ctrl) == 0x00) || (GET_CYCLE_TYPE(ctrl) == 0x02))
		flash_rx->buf_len = 4;
	else if (GET_CYCLE_TYPE(ctrl) == 0x01)
		flash_rx->buf_len = GET_LEN(ctrl) + 4;
	else if ((GET_CYCLE_TYPE(ctrl) & 0x09) == 0x09)
		flash_rx->buf_len = GET_LEN(ctrl);
	else
		flash_rx->buf_len = 0;

	for(i = 0;i< flash_rx->buf_len; i++) 
		flash_rx->buff[i] = ast_espi_read_reg(AST_ESPI_FLASH_RX_DATA);
}

#if 0 // No application
static void ast_espi_flash_tx(void)
{
	int i=0;	

	for(i = 0;i < flash_tx_channel.buf_len; i++)
		ast_espi_write_reg(flash_tx_channel.buff[i], AST_ESPI_FLASH_TX_DATA);		

	ast_espi_write_reg(TRIGGER_TRANSACTION | flash_tx_channel.header, AST_ESPI_FLASH_TX_CTRL);
}
#endif

// Channel 2: OOB
static void ast_espi_oob_rx(void)
{
	int i = 0;
	u32 ctrl = ast_espi_read_reg(AST_ESPI_OOB_RX_CTRL);
	//printk("cycle type = %x , tag = %x, len = %d byte \n",GET_CYCLE_TYPE(ctrl), GET_TAG(ctrl), GET_LEN(ctrl));

	oob_rx_channel.header = ctrl;
	oob_rx_channel.buf_len = GET_LEN(ctrl);

	for(i = 0;i< oob_rx_channel.buf_len; i++) 
		oob_rx_channel.buff[i] = ast_espi_read_reg(AST_ESPI_OOB_RX_DATA);
}

#if 0 // No application
static void ast_espi_oob_tx(void)
{
	int i=0;	

	for(i = 0;i < oob_tx_channel.buf_len; i++)
		ast_espi_write_reg(oob_tx_channel.buff[i] , AST_ESPI_OOB_TX_DATA);		

	ast_espi_write_reg(TRIGGER_TRANSACTION | oob_tx_channel.header , AST_ESPI_OOB_TX_CTRL);
}
#endif

// Channel 0: Peripheral
static void ast_espi_pcp_rx(void)
{
	int i = 0;
	u32 ctrl = ast_espi_read_reg(AST_ESPI_PCP_RX_CTRL);
	//printk("cycle type = %x , tag = %x, len = %d byte \n",GET_CYCLE_TYPE(ctrl), GET_TAG(ctrl), GET_LEN(ctrl));	

	p_rx_channel.header = ctrl;

	//Message
	if((GET_CYCLE_TYPE(ctrl) & 0x10) == 0x10) {		//message
		p_rx_channel.buf_len = 5;	
		if(GET_CYCLE_TYPE(ctrl) & 0x1)					//message with data
			p_rx_channel.buf_len += GET_LEN(ctrl);
	} else if((GET_CYCLE_TYPE(ctrl) & 0x09) == 0x09)	//success com with data
		p_rx_channel.buf_len = GET_LEN(ctrl);
	else
		p_rx_channel.buf_len = 0;

	for(i = 0;i< p_rx_channel.buf_len; i++)
		p_rx_channel.buff[i] = ast_espi_read_reg(AST_ESPI_PCP_RX_DATA);
}

#if 0 // No application
static void ast_espi_pcp_tx(void)
{
	int i=0;	
	
	for(i = 0;i < p_tx_channel.buf_len; i++)
		ast_espi_write_reg(p_tx_channel.buff[i] , AST_ESPI_PCP_TX_DATA);		

	ast_espi_write_reg(TRIGGER_TRANSACTION | p_tx_channel.header, AST_ESPI_PCP_TX_CTRL);
}

static void ast_espi_pcnp_tx(void)
{
	int i=0;	

	for(i = 0;i < np_tx_channel.buf_len; i++)
		ast_espi_write_reg(np_tx_channel.buff[i] , AST_ESPI_PCNP_TX_DATA);		

	ast_espi_write_reg(TRIGGER_TRANSACTION | np_tx_channel.header, AST_ESPI_PCNP_TX_CTRL);
}
#endif

static void ast_sys_event(void)
{
	u32 status = ast_espi_read_reg(AST_ESPI_SYS_EVENT_STATUS);
	u32 sys_event = ast_espi_read_reg(AST_ESPI_SYS_EVENT);

	//printk("status %x, sys_event %x\n", status, sys_event);	

	if(status & SYS_HOST_RESET_WARN) {
		if(sys_event & SYS_HOST_RESET_WARN)
			ast_espi_write_reg(sys_event | SYS_HOST_RESET_ACK, AST_ESPI_SYS_EVENT);
		else 
			ast_espi_write_reg(sys_event & ~SYS_HOST_RESET_ACK, AST_ESPI_SYS_EVENT);
		ast_espi_write_reg(SYS_HOST_RESET_WARN, AST_ESPI_SYS_EVENT_STATUS);
	}

	if(status & SYS_OOB_RESET_WARN) {
		if(sys_event & SYS_OOB_RESET_WARN)
			ast_espi_write_reg(sys_event | SYS_OOB_RESET_ACK, AST_ESPI_SYS_EVENT);
		else 
			ast_espi_write_reg(sys_event & ~SYS_OOB_RESET_ACK, AST_ESPI_SYS_EVENT);
		ast_espi_write_reg(SYS_OOB_RESET_WARN, AST_ESPI_SYS_EVENT_STATUS);
	}

	if(status & ~(SYS_OOB_RESET_WARN | SYS_HOST_RESET_WARN)) {
		//printk("new status %x \n",status);
		ast_espi_write_reg(status, AST_ESPI_SYS_EVENT_STATUS);
	}
}

static void 
ast_sys1_event(void)
{
	u32 status = ast_espi_read_reg(AST_ESPI_SYS1_EVENT_STATUS);
	if(status & SYS1_SUS_WARN) {
		ast_espi_write_reg(ast_espi_read_reg(AST_ESPI_SYS1_EVENT) | SYS1_SUS_ACK, AST_ESPI_SYS1_EVENT);
		ast_espi_write_reg(SYS1_SUS_WARN, AST_ESPI_SYS1_EVENT_STATUS);
	}

	if(status & ~(SYS1_SUS_WARN)) {
		//printk("new sys1 status %x \n",status);
		ast_espi_write_reg(status, AST_ESPI_SYS1_EVENT_STATUS);
	}
	
}

static void ast_espi_hw_init(void) 
{
	
	// espi initial 
	ast_espi_write_reg(ast_espi_read_reg(AST_ESPI_CTRL) | 0xff, AST_ESPI_CTRL);

	// Enable all interrupt
	ast_espi_write_reg(0xffffffff, AST_ESPI_INTR_EN);
	
	// System Event, type 2 dual edge
	ast_espi_write_reg(0, AST_ESPI_SYS_INT_T0);
	ast_espi_write_reg(0, AST_ESPI_SYS_INT_T1);
	ast_espi_write_reg(SYS_HOST_RESET_WARN| SYS_OOB_RESET_WARN, AST_ESPI_SYS_INT_T2);
	ast_espi_write_reg(0xffffffff, AST_ESPI_SYS_EVENT_EN);
	
	// System Event 1, type 0 rising edge
	// Before changing to rising edge, check Suspend Warn first. If so, respond SUS_ACK back.
	if ((ast_espi_read_reg(AST_ESPI_SYS1_EVENT) & ~(SYS1_SUS_ACK)) && (ast_espi_read_reg(AST_ESPI_SYS1_EVENT) & SYS1_SUS_WARN))
	{
		ast_espi_write_reg(ast_espi_read_reg(AST_ESPI_SYS1_EVENT) | SYS1_SUS_ACK, AST_ESPI_SYS1_EVENT);
		ast_espi_write_reg(SYS1_SUS_WARN, AST_ESPI_SYS1_EVENT_STATUS);
	}
	
	ast_espi_write_reg(0, AST_ESPI_SYS1_INT_T1);
	ast_espi_write_reg(0, AST_ESPI_SYS1_INT_T2);
	ast_espi_write_reg(0x1, AST_ESPI_SYS1_INT_T0);
	ast_espi_write_reg(0x1, AST_ESPI_SYS1_EVENT_EN);

}

/* -------------------------------------------------- */
static espi_hal_operations_t ast_espi_hw_ops = {
	.num_espi_ch			= ast_espi_num_ch,
	.reset_espi				= ast_espi_reset,
	.read_espi_data_in		= ast_espi_read_data,
	.read_espi_slave_regs	= ast_espi_slave_registers,
};

static hw_hal_t ast_espi_hw_hal = {
	.dev_type = EDEV_TYPE_ESPI,
	.owner = THIS_MODULE,
	.devname = AST_ESPI_DRIVER_NAME,
	.num_instances = AST_ESPI_CHANNEL_NUM,
	.phal_ops = (void *) &ast_espi_hw_ops
};

static irqreturn_t ast_espi_handler(int this_irq, void *dev_id)
{
	unsigned long status;
	unsigned long intrEn;
	unsigned long espi_intr;

	status = ast_espi_read_reg(AST_ESPI_INTR_STATUS);
	intrEn  = ast_espi_read_reg(AST_ESPI_INTR_EN);
	//printk(KERN_WARNING "%s: AST_ESPI_INTR_STATUS status %x intrEn %x\n", AST_ESPI_DRIVER_NAME, status, intrEn);
	espi_intr = status & intrEn;

	if(espi_intr & HW_RESET_EVENT) {
		//printk("HW_RESET_EVENT \n");
		ast_espi_write_reg(ast_espi_read_reg(AST_ESPI_SYS_EVENT) | SYS_SLAVE_BOOT_STATUS | SYS_SLAVE_BOOT_DONE, AST_ESPI_SYS_EVENT);

		ast_espi_write_reg(HW_RESET_EVENT, AST_ESPI_INTR_STATUS);
	} 
	
	// Flash Channel
	if(espi_intr & FLASH_TX_COMPLETE) {
		//printk("FLASH_TX_COMPLETE \n");
		ast_espi_write_reg(FLASH_TX_COMPLETE, AST_ESPI_INTR_STATUS);
	}

	if(espi_intr & FLASH_RX_COMPLETE) {
		//printk("FLASH_RX_COMPLETE \n");		
		ast_espi_flash_rx();
		ast_espi_write_reg(FLASH_RX_COMPLETE, AST_ESPI_INTR_STATUS);
	}
	
	if(espi_intr & FLASH_TX_ERROR) {
		//printk("FLASH_TX_ERROR \n");
		ast_espi_write_reg(FLASH_TX_ERROR, AST_ESPI_INTR_STATUS);					
	}
	
	if(espi_intr & FLASH_TX_ABORT) {
		//printk("FLASH_TX_ABORT \n");
		ast_espi_write_reg(FLASH_TX_ABORT, AST_ESPI_INTR_STATUS);					
	}
	
	if(espi_intr & FLASH_RX_ABORT) {
		//printk("FLASH_RX_ABORT \n");
		ast_espi_write_reg(FLASH_RX_ABORT, AST_ESPI_INTR_STATUS);				
	}

	// OOB Channel
	if(espi_intr & OOB_TX_COMPLETE) {
		//printk("OOB_TX_COMPLETE \n");
		ast_espi_write_reg(OOB_TX_COMPLETE, AST_ESPI_INTR_STATUS);
	}

	if(espi_intr & OOB_RX_COMPLETE) {
		//printk("OOB_RX_COMPLETE \n");		
		ast_espi_oob_rx();
		ast_espi_write_reg(OOB_RX_COMPLETE, AST_ESPI_INTR_STATUS);
	}

	if(espi_intr & OOB_TX_ERROR) {
		//printk("OOB_TX_ERROR \n");
		ast_espi_write_reg(OOB_TX_ERROR, AST_ESPI_INTR_STATUS);					
	}

	if(espi_intr & OOB_TX_ABORT) {
		//printk("OOB_TX_ABORT \n");
		ast_espi_write_reg(OOB_TX_ABORT, AST_ESPI_INTR_STATUS);			
	}
	
	if(espi_intr & OOB_RX_ABORT) {
		//printk("OOB_RX_ABORT");
		ast_espi_write_reg(OOB_RX_ABORT, AST_ESPI_INTR_STATUS);		
	}
	
	// VW Channel
	if(espi_intr & VW_GPIO_EVENT) {
		//printk("VW_GPIO_EVENT \n");
		vw_gpio = ast_espi_read_reg(AST_ESPI_GPIO_THROUGH_VW);
		ast_espi_write_reg(VW_GPIO_EVENT, AST_ESPI_INTR_STATUS);
	}

	if(espi_intr & VW_SYS_EVENT) {
		//printk("VW_SYS_EVENT \n");
		vw_sysevent = ast_espi_read_reg(AST_ESPI_SYS_EVENT);
		ast_sys_event();
		ast_espi_write_reg(VW_SYS_EVENT, AST_ESPI_INTR_STATUS);
	}

	if(espi_intr & VW_SYS_EVENT1) {
		//printk("VW_SYS_EVENT1 \n");		
		ast_sys1_event();
		ast_espi_write_reg(VW_SYS_EVENT1, AST_ESPI_INTR_STATUS);
	}	

	// Peripheral Channel
	if(espi_intr & PCNP_TX_COMPLETE) {
		//printk("PCNP_TX_COMPLETE \n");
		ast_espi_write_reg(PCNP_TX_COMPLETE, AST_ESPI_INTR_STATUS);
	}

	if(espi_intr & PCP_TX_COMPLETE) {
		//printk("PCP_TX_COMPLETE \n");
		ast_espi_write_reg(PCP_TX_COMPLETE, AST_ESPI_INTR_STATUS);
	}

	if(espi_intr & PCP_RX_COMPLETE) {
		//printk("PCP_RX_COMPLETE \n");		
		ast_espi_pcp_rx();
		ast_espi_write_reg(PCP_RX_COMPLETE, AST_ESPI_INTR_STATUS);
	}
	
	if(espi_intr & PCNP_TX_ERROR) {
		//printk("PCNP_TX_ERROR \n");
		ast_espi_write_reg(PCNP_TX_ERROR, AST_ESPI_INTR_STATUS);		
	}

	if(espi_intr & PCP_TX_ERROR) {
		//printk("PCP_TX_ERROR \n");
		ast_espi_write_reg(PCP_TX_ERROR, AST_ESPI_INTR_STATUS);				
	}
	
	if(espi_intr & PCNP_TX_ABORT) {
		//printk("PCNP_TX_ABORT\n");
		ast_espi_write_reg(PCNP_TX_ABORT, AST_ESPI_INTR_STATUS);								
	}

	if(espi_intr & PCP_TX_ABORT) {
		//printk("PCP_TX_ABORT\n");
		ast_espi_write_reg(PCP_TX_ABORT, AST_ESPI_INTR_STATUS);						
	}
	
	if(espi_intr & PCNP_RX_ABORT) {
		//printk("PCNP_RX_ABORT\n");
		ast_espi_write_reg(PCNP_RX_ABORT, AST_ESPI_INTR_STATUS);
	}
	
	if(espi_intr & PCP_RX_ABORT) {
		//printk("PCP_RX_ABORT \n");
		ast_espi_write_reg(PCP_RX_ABORT, AST_ESPI_INTR_STATUS);		
	}

	return IRQ_HANDLED;
}

static irqreturn_t ast_espi_reset_handler(int this_irq, void *dev_id)
{
	volatile uint32_t intr_value = 0;
	
	if(this_irq == IRQ_GPIO)
	{
		intr_value = ioread32((void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_STATUS);
		if ((intr_value >> 7) & 0x1)
		{
			// Power Down
			if (espi_hw_driver_init)
			{
				if (ast_espi_read_reg(AST_ESPI_SYS1_EVENT_STATUS) & SYS1_SUS_WARN)
				{
					if ((ast_espi_read_reg(AST_ESPI_SYS1_EVENT) & ~(SYS1_SUS_ACK)) && (ast_espi_read_reg(AST_ESPI_SYS1_EVENT) & SYS1_SUS_WARN))
					{
						ast_espi_write_reg(ast_espi_read_reg(AST_ESPI_SYS1_EVENT) | SYS1_SUS_ACK, AST_ESPI_SYS1_EVENT);
						ast_espi_write_reg(SYS1_SUS_WARN, AST_ESPI_SYS1_EVENT_STATUS);
					}
				}
				
				if (ast_espi_read_reg(AST_ESPI_SYS1_EVENT_STATUS) & SYS1_SUS_PWDOWN_ACK)
				{
					ast_espi_write_reg(SYS1_SUS_PWDOWN_ACK, AST_ESPI_SYS1_EVENT_STATUS);
				}
			}
		}
		iowrite32(intr_value, (void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_STATUS);
	}
	return IRQ_HANDLED;
}

int ast_espi_init(void)
{
	int ret;
	uint32_t reg;
	uint32_t status;	

	extern int espi_core_loaded;
	if (!espi_core_loaded)
		return -1;
	
	ast_espi_hal_id = register_hw_hal_module(&ast_espi_hw_hal, (void **) &espi_core_ops);
	if (ast_espi_hal_id < 0) {
		printk(KERN_WARNING "%s: register HAL HW module failed\n", AST_ESPI_DRIVER_NAME);
		return ast_espi_hal_id;
	}
	
	ast_espi_virt_base = ioremap_nocache(AST_ESPI_REG_BASE, SZ_4K);
	if (!ast_espi_virt_base) {
		printk(KERN_WARNING "%s: ioremap failed\n", AST_ESPI_DRIVER_NAME);
		unregister_hw_hal_module(EDEV_TYPE_ESPI, ast_espi_hal_id);
		return -ENOMEM;
	}
	ast_gpio_virt_base = ioremap_nocache(AST_GPIO_REG_BASE, SZ_4K);
	if (!ast_gpio_virt_base) {
		printk(KERN_WARNING "%s: AST_GPIO_REG_BASE ioremap failed\n", AST_ESPI_DRIVER_NAME);
		unregister_hw_hal_module(EDEV_TYPE_ESPI, ast_espi_hal_id);
		return -ENOMEM;
	}
	
	ret = request_irq(AST_ESPI_IRQ, ast_espi_handler, IRQF_SHARED, "ast_espi", ast_espi_virt_base);
	if (ret) {
		printk(KERN_WARNING "%s: AST_ESPI_IRQ request irq failed\n", AST_ESPI_DRIVER_NAME);
	}
	
	// Enable eSPI mode
	*(volatile u32 *)(IO_ADDRESS(AST_SCU_REG_BASE)) = (0x1688A8A8); //Unlock SCU register

	status = *(volatile u32 *)(IO_ADDRESS( (AST_SCU_REG_BASE + 0x70) ));
	if(!(status & (0x1 << 25)))
	{
		printk(KERN_WARNING "%s: eSPI mode is not enable\n", AST_ESPI_DRIVER_NAME);
	}
	*(volatile u32 *)(IO_ADDRESS(AST_SCU_REG_BASE)) = 0;			//Lock SCU register
	
	// Set GPIO_AC7 Pin sensitivity, falling edge
	reg = ioread32((void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_SENS_0);
	reg = (reg & ~(0x1 << GPIO_AC_7));
	iowrite32(reg, (void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_SENS_0);

	reg = ioread32((void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_SENS_1);
	reg = (reg & ~(0x1 << GPIO_AC_7));
	iowrite32(reg, (void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_SENS_1);

	reg = ioread32((void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_SENS_2);
	reg = (reg & ~(0x1 << GPIO_AC_7));
	iowrite32(reg, (void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_SENS_2);
	
	// Set Input Pin
	reg = ioread32((void * __iomem)ast_gpio_virt_base + GPIO_AC_DIRECTION);
	reg = (reg & ~(0x1 << GPIO_AC_7));
	iowrite32(reg, (void * __iomem)ast_gpio_virt_base + GPIO_AC_DIRECTION);
	
	// Enable GPIO_AC7 Pin interrupt
	reg = ioread32((void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_EN);
	if (!(reg & (0x1 << GPIO_AC_7)) )
	{
		reg = (0x1 << GPIO_AC_7);
		iowrite32(reg, (void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_EN);
	}
	
	// Request interrupt for RESET (GPIO_AC7) Pin
	ret = request_irq(IRQ_GPIO, ast_espi_reset_handler, IRQF_SHARED, "ast_espi_reset", ast_gpio_virt_base);
	if (ret) {
		printk(KERN_WARNING "%s: IRQ_GPIO request irq failed\n", AST_ESPI_DRIVER_NAME);
	}
	
	p_rx_channel.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
	p_tx_channel.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
	np_tx_channel.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
	oob_rx_channel.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
	oob_tx_channel.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
	flash_rx_channel.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
	flash_tx_channel.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
	
	ast_espi_hw_init();
	espi_hw_driver_init = 1;

	printk("The eSPI HW Driver is loaded successfully.\n" );
	return 0;
}

void ast_espi_exit(void)
{
	kfree(p_rx_channel.buff);
	kfree(p_tx_channel.buff);
	kfree(np_tx_channel.buff);
	kfree(oob_rx_channel.buff);
	kfree(oob_tx_channel.buff);
	kfree(flash_rx_channel.buff);
	kfree(flash_tx_channel.buff);
	
	free_irq(AST_ESPI_IRQ, ast_espi_virt_base);
	free_irq(IRQ_GPIO, ast_gpio_virt_base);
	
	iounmap (ast_espi_virt_base);
	iounmap (ast_gpio_virt_base);
	unregister_hw_hal_module(EDEV_TYPE_ESPI, ast_espi_hal_id);
	
	return;
}

module_init (ast_espi_init);
module_exit (ast_espi_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("ASPEED AST SoC eSPI Driver");
MODULE_LICENSE ("GPL");
