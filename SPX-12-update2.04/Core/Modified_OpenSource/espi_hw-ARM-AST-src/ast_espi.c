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
#include <mach/platform.h>

#include "driver_hal.h"
#include "espi.h"
#include "espi_ioctl.h"
#include "ast_espi.h"
#include "helper.h"

static int espi_irq;
static int gpio_irq;

static void *ast_espi_virt_base;
static void *ast_gpio_virt_base;

static int ast_espi_hal_id;
static espi_core_funcs_t *espi_core_ops;
static int espi_hw_driver_init = 0;

struct espi_ch_data        p_rx_channel;
#if 0 //no application
struct espi_ch_data        p_tx_channel;
struct espi_ch_data        np_tx_channel;    
#endif

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

static void ast_espi_set_vw_bmc2host_group_val(u8 ind, u8 val)
{
    u32 vw_val = 0;

    /* Converter the data format of Intel Visual Wire Registers in to the related AST Registers */
    switch(ind)
    {
        case 0x04: /* System Event */
            vw_val = ast_espi_read_reg(AST_ESPI_SYS_EVENT);
            /* Clear the value of bits */
            vw_val = vw_val & (~(1<<16));
            vw_val = vw_val | ((val & 0x1) << 16);
            ast_espi_write_reg(vw_val, AST_ESPI_SYS_EVENT);
            break;

        case 0x05: /* System Event */
            vw_val = ast_espi_read_reg(AST_ESPI_SYS_EVENT);
            /* Clear the value of bits */
            vw_val = vw_val & (~(0x0F<<20));
            vw_val = vw_val | ((val & 0x0F) << 20);
            ast_espi_write_reg(vw_val, AST_ESPI_SYS_EVENT);
            break;

        case 0x06: /* System Event */
            vw_val = ast_espi_read_reg(AST_ESPI_SYS_EVENT);
            /* Clear the value of bits */
            vw_val = vw_val & (~(0x3<<26));
            vw_val = vw_val | ((val & 0xc) << 24);
            ast_espi_write_reg(vw_val, AST_ESPI_SYS_EVENT);
            break;

        case 0x40:
            vw_val = ast_espi_read_reg(AST_ESPI_SYS1_EVENT);
            /* Clear the value of bits */
            vw_val = vw_val & (~(0x1<<20));
            vw_val = vw_val | ((val & 0x1) << 20);
            ast_espi_write_reg(vw_val, AST_ESPI_SYS1_EVENT);
            break;

        case 0x45:
        case 0x46:
            /* No Application */
            break;
    }
}

static void ast_espi_get_vw_host2bmc_group_val(u8 ind, u8 *buf)
{
    u32 vw_val = 0;

    /* providing the response by following the data format of Intel Visual Wire Registers */
    switch(ind)
    {
        case 0x02: /* System Event */
            vw_val = ast_espi_read_reg(AST_ESPI_SYS_EVENT);
            *buf = (vw_val & 0x7);
            break;

        case 0x03: /* System Event */
            vw_val = ast_espi_read_reg(AST_ESPI_SYS_EVENT);
            *buf = ((vw_val >> 4) & 0x7);
            break;

        case 0x07: /* System Event */
            vw_val = ast_espi_read_reg(AST_ESPI_SYS_EVENT);
            *buf = ((vw_val >> 8) & 0x7);
            break;

        case 0x41:
            vw_val = ast_espi_read_reg(AST_ESPI_SYS1_EVENT);
            *buf = (vw_val & 0x3) | ((vw_val >> 1) & 0x4);
            break;

        case 0x43:
            vw_val = ast_espi_read_reg(AST_ESPI_SYS1_EVENT);
            *buf = ((vw_val >> 4) & 0x3);
            break;

        case 0x47:
            vw_val = ast_espi_read_reg(AST_ESPI_SYS1_EVENT);
            *buf = ((vw_val >> 16) & 0x1);
            break;

        case 0x44:
            /* No Application */
        default:
            /* Failure or not in the range */
            *buf = 0xFF;
            break;
    }
}

static void ast_espi_get_channel_status(u32 *gen_status, u32 *ch_status, u32 *PeripheralChMaxReadReqSize, u32 *PeripheralChMaxPayloadSizeSelected, u32 *PeripheralChMaxPayloadSizeSupported )
{    
    *gen_status = ast_espi_read_reg(AST_ESPI_GEN_CAPCONF);
    
    *ch_status = 0;
    
    if (ast_espi_read_reg(AST_ESPI_CH0_CAPCONF) & 0x3)
        *ch_status |= PERIPHERAL_CHANNEL_SUPPORTED;

    *PeripheralChMaxReadReqSize = (ast_espi_read_reg(AST_ESPI_CH0_CAPCONF) & 0x7000) >> 12;
    *PeripheralChMaxPayloadSizeSelected = (ast_espi_read_reg(AST_ESPI_CH0_CAPCONF) & 0x700) >> 8;
    *PeripheralChMaxPayloadSizeSupported = (ast_espi_read_reg(AST_ESPI_CH0_CAPCONF) & 0x70) >> 4;

    if (ast_espi_read_reg(AST_ESPI_CH1_CAPCONF) & 0x3)
        *ch_status |= VWIRE_CHANNEL_SUPPORTED;
    
#ifdef ESPI_DEBUG
    // Slave Registers
    printk("AST_ESPI_GEN_CAPCONF    (0A0h) 0x%x\n", (unsigned int)ast_espi_read_reg(AST_ESPI_GEN_CAPCONF));
    printk("AST_ESPI_CH0_CAPCONF    (0A4h) 0x%x\n", (unsigned int)ast_espi_read_reg(AST_ESPI_CH0_CAPCONF));
    printk("AST_ESPI_CH1_CAPCONF    (0A8h) 0x%x\n", (unsigned int)ast_espi_read_reg(AST_ESPI_CH1_CAPCONF));
    printk("AST_ESPI_CH2_CAPCONF    (0ACh) 0x%x\n", (unsigned int)ast_espi_read_reg(AST_ESPI_CH2_CAPCONF));
    printk("AST_ESPI_CH3_CAPCONF    (0B0h) 0x%x\n", (unsigned int)ast_espi_read_reg(AST_ESPI_CH3_CAPCONF));
#endif
}

// Channel 0: Peripheral
static void ast_espi_pcp_rx(void)
{
    int i = 0;
    u32 ctrl = ast_espi_read_reg(AST_ESPI_PCP_RX_CTRL);
    //printk("cycle type = %x , tag = %x, len = %d byte \n",GET_CYCLE_TYPE(ctrl), GET_TAG(ctrl), GET_LEN(ctrl));    

    p_rx_channel.header = ctrl;

    //Message
    if((GET_CYCLE_TYPE(ctrl) & 0x10) == 0x10) {         //message
        p_rx_channel.buf_len = 5;    
        if(GET_CYCLE_TYPE(ctrl) & 0x1)                  //message with data
            p_rx_channel.buf_len += GET_LEN(ctrl);
    } else if((GET_CYCLE_TYPE(ctrl) & 0x09) == 0x09)    //success com with data
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

    // System Event, type 2 dual edge
    ast_espi_write_reg(0, AST_ESPI_SYS_INT_T0);
    ast_espi_write_reg(0, AST_ESPI_SYS_INT_T1);
    ast_espi_write_reg(SYS_HOST_RESET_WARN| SYS_OOB_RESET_WARN, AST_ESPI_SYS_INT_T2);
    // Enable all interrupt
    ast_espi_write_reg(0xffffffff, AST_ESPI_INTR_EN);
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
    // For A1
    ast_espi_write_reg(0x1, AST_ESPI_SYS1_INT_T0);
    ast_espi_write_reg(0x1, AST_ESPI_SYS1_EVENT_EN);

}

/* -------------------------------------------------- */
static espi_hal_operations_t ast_espi_hw_ops = {
    .num_espi_ch            = ast_espi_num_ch,
    .reset_espi             = ast_espi_reset,
    .set_vw_bmc2host_group_val  = ast_espi_set_vw_bmc2host_group_val,
    .get_vw_host2bmc_group_val  = ast_espi_get_vw_host2bmc_group_val,
    .get_channel_status     = ast_espi_get_channel_status,
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
    unsigned int handled = 0;
    unsigned long status;
    unsigned long intrEn;
    unsigned long espi_intr;

    status = ast_espi_read_reg(AST_ESPI_INTR_STATUS);
    intrEn  = ast_espi_read_reg(AST_ESPI_INTR_EN);
    //printk(KERN_WARNING "%s: ESPI AST_ESPI_INTR_STATUS status %x intrEn %x\n", AST_ESPI_DRIVER_NAME, status, intrEn);
    espi_intr = status & intrEn;

    if(espi_intr & HW_RESET_EVENT) {
        //printk("HW_RESET_EVENT \n");
        ast_espi_write_reg(ast_espi_read_reg(AST_ESPI_SYS_EVENT) | SYS_SLAVE_BOOT_STATUS | SYS_SLAVE_BOOT_DONE, AST_ESPI_SYS_EVENT);
        ast_espi_write_reg(HW_RESET_EVENT, AST_ESPI_INTR_STATUS);
        handled |= HW_RESET_EVENT;
    } 

    // VW Channel
    if(espi_intr & VW_GPIO_EVENT) {
        //printk("VW_GPIO_EVENT \n");
        ast_espi_write_reg(VW_GPIO_EVENT, AST_ESPI_INTR_STATUS);
        handled |= VW_GPIO_EVENT;
    }

    if(espi_intr & VW_SYS_EVENT) {
        //printk("VW_SYS_EVENT \n");
        ast_sys_event();
        ast_espi_write_reg(VW_SYS_EVENT, AST_ESPI_INTR_STATUS);
        handled |= VW_SYS_EVENT;
    }

    if(espi_intr & VW_SYS_EVENT1) {
        //printk("VW_SYS_EVENT1 \n");        
        ast_sys1_event();
        ast_espi_write_reg(VW_SYS_EVENT1, AST_ESPI_INTR_STATUS);
        handled |= VW_SYS_EVENT1;
    }    

    // Peripheral Channel
    if(espi_intr & PCNP_TX_COMPLETE) {
        //printk("PCNP_TX_COMPLETE \n");
        ast_espi_write_reg(PCNP_TX_COMPLETE, AST_ESPI_INTR_STATUS);
        handled |= PCNP_TX_COMPLETE;
    }

    if(espi_intr & PCP_TX_COMPLETE) {
        //printk("PCP_TX_COMPLETE \n");
        ast_espi_write_reg(PCP_TX_COMPLETE, AST_ESPI_INTR_STATUS);
        handled |= PCP_TX_COMPLETE;
    }

    if(espi_intr & PCP_RX_COMPLETE) {
        //printk("PCP_RX_COMPLETE \n");        
        ast_espi_pcp_rx();
        ast_espi_write_reg(PCP_RX_COMPLETE, AST_ESPI_INTR_STATUS);
        handled |= PCP_RX_COMPLETE;
    }

    if(espi_intr & PCNP_TX_ERROR) {
        //printk("PCNP_TX_ERROR \n");
        ast_espi_write_reg(PCNP_TX_ERROR, AST_ESPI_INTR_STATUS);        
        handled |= PCNP_TX_ERROR;
    }

    if(espi_intr & PCP_TX_ERROR) {
        //printk("PCP_TX_ERROR \n");
        ast_espi_write_reg(PCP_TX_ERROR, AST_ESPI_INTR_STATUS);                
        handled |= PCP_TX_ERROR;
    }

    if(espi_intr & PCNP_TX_ABORT) {
        //printk("PCNP_TX_ABORT\n");
        ast_espi_write_reg(PCNP_TX_ABORT, AST_ESPI_INTR_STATUS);                                
        handled |= PCNP_TX_ABORT;
    }

    if(espi_intr & PCP_TX_ABORT) {
        //printk("PCP_TX_ABORT\n");
        ast_espi_write_reg(PCP_TX_ABORT, AST_ESPI_INTR_STATUS);                        
        handled |= PCP_TX_ABORT;
    }

    if(espi_intr & PCNP_RX_ABORT) {
        //printk("PCNP_RX_ABORT\n");
        ast_espi_write_reg(PCNP_RX_ABORT, AST_ESPI_INTR_STATUS);
        handled |= PCNP_RX_ABORT;
    }

    if(espi_intr & PCP_RX_ABORT) {
        //printk("PCP_RX_ABORT \n");
        ast_espi_write_reg(PCP_RX_ABORT, AST_ESPI_INTR_STATUS);        
        handled |= PCP_RX_ABORT;
    }

    if ((espi_intr & (~handled)) == 0)
        return IRQ_HANDLED;
    else
        return IRQ_NONE;
}

static irqreturn_t ast_espi_reset_handler(int this_irq, void *dev_id)
{
    volatile uint32_t intr_value = 0;
    volatile uint32_t sw_mode = 0;
    
    if(this_irq == IRQ_GPIO)
    {
        intr_value = ioread32((void * __iomem)ast_gpio_virt_base + GPIO_AC_INT_STATUS);
        if ((intr_value >> 7) & 0x1)
        {
            // Power Down
            if (espi_hw_driver_init)
            {
                // System Event 1, type 0 rising edge
                // Before changing to rising edge, check Suspend Warn first. If so, respond SUS_ACK back.
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

                sw_mode = ast_espi_read_reg(AST_ESPI_CTRL) & ESPI_CTRL_SW_FLASH_READ;
                /* 
                 * We listen to ESPIRST# (GPIOAC7) to do ESPI controller reset.
                 * During the interrupt happens, ASPEED SDK do ESPI SCU register reset too.
                 * With that process, if we do Chassis Power Cycle, when Host power on, KCS will not work.
                 * Verified on Intel Wolfpass platform.
                */
                ast_espi_hw_init();
                ast_espi_write_reg(AST_ESPI_CTRL, ast_espi_read_reg(AST_ESPI_CTRL) | sw_mode);
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
    
    // Detect eSPI mode
    *(volatile u32 *)(IO_ADDRESS(AST_SCU_REG_BASE)) = (0x1688A8A8); //Unlock SCU register

    status = *(volatile u32 *)(IO_ADDRESS( (AST_SCU_REG_BASE + 0x70) ));
    if(!(status & (0x1 << 25)))
    {
        printk(KERN_WARNING "%s: eSPI mode is not enable\n", AST_ESPI_DRIVER_NAME);
        return -EPERM;
    }
    *(volatile u32 *)(IO_ADDRESS(AST_SCU_REG_BASE)) = 0;            //Lock SCU register

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
   
     espi_irq = GetIrqFromDT("ami_espi", AST_ESPI_IRQ);
 
    ret = request_irq(espi_irq, ast_espi_handler, IRQF_SHARED, AST_ESPI_DRIVER_NAME, ast_espi_virt_base);
    if (ret) {
        printk(KERN_WARNING "%s: AST_ESPI_IRQ request irq failed\n", AST_ESPI_DRIVER_NAME);
    }
    
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


     gpio_irq = GetIrqFromDT("ami_gpio", IRQ_GPIO);
    ret = request_irq(gpio_irq, ast_espi_reset_handler, IRQF_SHARED, "ast_espi_reset", &ast_espi_reset_handler);
    if (ret) {
        printk(KERN_WARNING "%s: IRQ_GPIO request irq failed\n", AST_ESPI_DRIVER_NAME);
    }

    p_rx_channel.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
#if 0 // No Application
    p_tx_channel.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
    np_tx_channel.buff = kmalloc(MAX_XFER_BUFF_SIZE, GFP_KERNEL);
#endif

    ast_espi_hw_init();
    espi_hw_driver_init = 1;

    printk("The eSPI HW Driver is loaded successfully.\n" );
    return 0;
}

void ast_espi_exit(void)
{
    kfree(p_rx_channel.buff);
#if 0 // No Application
    kfree(p_tx_channel.buff);
    kfree(np_tx_channel.buff);
#endif
    
    free_irq(espi_irq, ast_espi_virt_base);
    free_irq(gpio_irq, &ast_espi_reset_handler);
    
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
