/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/

/****************************************************************
 *
 * pilot_watchdog.c
 * ServerEngines Pilot-II/Pilot-III watchdog timer driver
 *
 ****************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/bitops.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/delay.h>
#include "driver_hal.h"
#include "watchdog_core.h"
#include "pilot_watchdog.h"
#include "../../../Build/kernel/linux/drivers/mtd/spichips/spiflash.h"

/******************************************************************************************
 *Note:-Text from pilot4_specification_v0.23 
 *The watchdog counter will always reloads with the SYSWCFR1[15:0] after expires, and watchdog 
 *counter keep decrements and generates reset to modules continuously until watchdog is disable.
 *So Explicit watchdog disable is needed for watchdog 1/2/3 to stop.
*******************************************************************************************/

u32 g_reset_enable_low[MAX_WDT_DEVICES];
u32 g_reset_enable_high[MAX_WDT_DEVICES];

int set_cur_wdtdev_reset_mask0(const char *val, const struct kernel_param *kp)
{
        u32 reset_mask = 0;
        int ret = 0;

        ret = kstrtou32(val ,0 ,&reset_mask);
        if(ret)
             goto exit;

        g_reset_enable_low[current_wdt_device - 1] = reset_mask;
exit:   
        return ret;
}

int get_cur_wdtdev_reset_mask0(char *val, const struct kernel_param *kp)
{
        return (snprintf(val,64,"0x%x",g_reset_enable_low[current_wdt_device - 1]));
}

const struct kernel_param_ops current_wdt_reset_mask0 =
{
     .set = &set_cur_wdtdev_reset_mask0, 
     .get = &get_cur_wdtdev_reset_mask0,
};

module_param_cb(current_wdt_reset_mask0,
                &current_wdt_reset_mask0,
                NULL,
                0644);

int set_cur_wdtdev_reset_mask1(const char *val, const struct kernel_param *kp)
{
        u32 reset_mask = 0;
        int ret = 0;

        ret = kstrtou32(val ,0 ,&reset_mask);
        if(ret)
             goto exit;

        g_reset_enable_high[current_wdt_device - 1] = reset_mask;
exit:   
        return ret;
}

int get_cur_wdtdev_reset_mask1(char *val, const struct kernel_param *kp)
{
        return (snprintf(val, 64,"0x%x",g_reset_enable_high[current_wdt_device - 1]));
}

const struct kernel_param_ops current_wdt_reset_mask1 =
{
     .set = &set_cur_wdtdev_reset_mask1, 
     .get = &get_cur_wdtdev_reset_mask1,
};

module_param_cb(current_wdt_reset_mask1,
                &current_wdt_reset_mask1,
                NULL,
                0644);

int cur_wd_set_ushort(const char *val, const struct kernel_param *kp)
{
    int res=0;
    if(*val < min_wdt_num+48 || *val > max_wdt_num+48)
    {
        printk(KERN_INFO "Current supported Watchdog device number:%d -%d \n",min_wdt_num,max_wdt_num);
        return -1;
    }
    res = param_set_ushort(val, kp); // Use helper for write variable
    return res;
}

const struct kernel_param_ops cur_wddev_ops_ushort = 
{
    .set = &cur_wd_set_ushort, // Use setter funtion to validate input
    .get = &param_get_ushort,
};

module_param_cb(current_wdt_device,    /*filename*/
                &cur_wddev_ops_ushort, /*operations*/
                &current_wdt_device,               /* pointer to variable, contained parameter's value */
                0644    /*permissions on file*/
);

MODULE_PARM_DESC(current_wdt_device,"Current watch dog timer dev no");

static void pilot_watchdog_set_value(int value)
{
#ifdef SOC_PILOT_IV     
        uint32_t reg;
#endif  
        if(current_wdt_device == 1)
        {
                iowrite32(value & 0x000000FF, pilot_watchdog_virt_base + PILOT_WATCHDOG_WDCNT_LO);
                iowrite32((value & 0x0000FF00)>>8, pilot_watchdog_virt_base + PILOT_WATCHDOG_WDCNT_HI);
        }
#ifdef SOC_PILOT_IV
        else
        {
                if((current_wdt_device > MIN_WDT_DEVICE) && (current_wdt_device <= MAX_WDT_DEVICES))
                {
                        reg =((value * COUNT_PERSEC) & 0xffff) | (PILOT_WATCHDOG_PRETRIGGER_TIMEOUT << 16);
                        iowrite32(reg , (void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWCFR);
                }
        }
#endif
}

static void pilot_watchdog_count(void)
{
        uint32_t reg;

        if(current_wdt_device == 1)
        {
                reg = ioread32((void __iomem*)pilot_watchdog_virt_base + PILOT_WATCHDOG_WDCTL);
                reg |= PILOT_WATCHDOG_WDCTL_TRIGGER;
                iowrite32(reg, (void __iomem*)pilot_watchdog_virt_base + PILOT_WATCHDOG_WDCTL);
        }
#ifdef SOC_PILOT_IV
        else
        {
                if((current_wdt_device > MIN_WDT_DEVICE) && (current_wdt_device <= MAX_WDT_DEVICES))
                {
                        reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWCR);
                        reg|= PILOT_WATCHDOG_WDCTL_TRIGGER_NEW_WDT;
                        iowrite32(reg , (void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWCR);
                }
        }
#endif  
}

static void pilot_watchdog_enable(void)
{
        uint32_t reg;

        if (current_wdt_device == 1)
        {
                reg = ioread32((void __iomem*)pilot_watchdog_virt_base + PILOT_WATCHDOG_WDCTL);
                reg |= PILOT_DELAYED_WDO_ENABLE | PILOT_WATCHDOG_WDCTL_RUN;
                iowrite32(reg, (void __iomem*)pilot_watchdog_virt_base + PILOT_WATCHDOG_WDCTL);
#if defined SOC_PILOT_III || defined SOC_PILOT_IV
                reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + PILOT_RESET_CONTROL_SYSRCR);
                reg |= PILOT_WATCHDOG_RESET_ENABLE;
                iowrite32(reg, (void __iomem*)SE_RES_DEB_VA_BASE + PILOT_RESET_CONTROL_SYSRCR);
                reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + PILOT_WATCHDOG_SYSWRER);

                /* pilot spec recommends to reset only ARM processor*/
                //reg |= ~PILOT_WATCHDOG_SYSWRER_VALUE;
                reg = g_reset_enable_low[current_wdt_device - 1];
                iowrite32(reg, (void __iomem*)SE_RES_DEB_VA_BASE + PILOT_WATCHDOG_SYSWRER);
#endif
                /* When the firmware reset is occurred due to hard reset and after that,
                when the watchdog timeout occurs, the firmware gets halt.
                This is because the Power-On-Reset(This is set whenever there is hard reset) is not cleared 
                So the fix is to clear the Power-On-Reset */
                *((volatile unsigned long *)(SE_SYS_CLK_VA_BASE  + 0x00)) &= 0xFFFFFFFC;
        }
#ifdef SOC_PILOT_IV
        else
        {
                if((current_wdt_device > MIN_WDT_DEVICE) && (current_wdt_device <= MAX_WDT_DEVICES))
                {

                
                    /* Clear the SYSRST_STATUS bit 0 to make sure the status bits are cleared */
                    reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + SYSRST_STATUS);
                    reg |=(1 << (current_wdt_device - 2));  
                    iowrite32(reg , (void __iomem*)SE_RES_DEB_VA_BASE + SYSRST_STATUS);             
                   
                    /* Set the watchdog timer in stopped state by setting SYSWCR1[0]= 0 */
                    reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWCR);
                    reg  &=  ~PILOT_WATCHDOG_RESET_ENABLE;
                    iowrite32(reg , (void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWCR);
                    
                    /*Program the count value in register SYSWCFR1[15:0]*/
                    // Watchdog timer counter value is already set in "pilot_watchdog_set_value" function        

                    /* select modules which need to be reset from the registers SYSWRERL1 and SYSWRERH1 */
                    // Here we reset the SPI along with ARM and other modules specified in "pilot_watchdog_init" function
                    // Please refer EIP 259001 for SPI reset . 
                    reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWRERL);
                    reg = g_reset_enable_low[current_wdt_device - 1];
                    iowrite32(reg , (void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWRERL);

                    reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWRERH);
                    reg = g_reset_enable_high[current_wdt_device - 1];
                    iowrite32(reg , (void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWRERH);

                         
                    /* Set the watchdog timer in running state by setting SYSWCR1[0]=1 */
                    reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWCR);
                    reg |= PILOT_WATCHDOG_PRETRIGGER_ENABLE | PILOT_WATCHDOG_RESET_ENABLE;
                    iowrite32(reg , (void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWCR);

                    /* Trigger the watchdog timer by setting SYSWCR1[23]=1. This starts the countdown from the counter value loaded in SYSWCFR1[15:0] register */
                    // This action is done in "pilot_watchdog_count" function 

                     /*Clearing power on reset detected bit*/
                    *((volatile unsigned long *)(SE_SYS_CLK_VA_BASE  + 0x00)) &= 0xFFFFFFFC;
                }
        }
#endif
}

static void pilot_watchdog_disable(void)
{
        uint32_t reg;
 
        if(current_wdt_device == 1)
        {
                reg = ioread32((void __iomem*)pilot_watchdog_virt_base + PILOT_WATCHDOG_WDCTL);
                reg &= ~PILOT_WATCHDOG_WDCTL_RUN;
                iowrite32(reg, (void __iomem*)pilot_watchdog_virt_base + PILOT_WATCHDOG_WDCTL);

#if defined SOC_PILOT_III || defined SOC_PILOT_IV
                reg = 0;//clearing watchdog Reset register
                iowrite32(reg, (void __iomem*)SE_RES_DEB_VA_BASE + PILOT_WATCHDOG_SYSWRER);

                reg = RESET_I2C_INTERFACES;
                iowrite32(reg, (void __iomem*)SE_RES_DEB_VA_BASE + PILOT_SOFTWARE_SYSSRER);
#endif
        }
#ifdef SOC_PILOT_IV
        else
        {
                if((current_wdt_device > MIN_WDT_DEVICE) && (current_wdt_device <= MAX_WDT_DEVICES))
                {
                        reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWCR);
                        reg &= ~PILOT_WATCHDOG_RESET_ENABLE;
                        iowrite32(reg , (void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWCR);
                        reg = 0;
                        iowrite32(reg , (void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWRERL);
                        iowrite32(reg , (void __iomem*)SE_RES_DEB_VA_BASE + (current_wdt_device - 2)*0x10 + SYSWRERH);
                }
        }
#endif
}

static struct watchdog_hal_ops_t pilot_watchdog_ops = {
                .set_value = pilot_watchdog_set_value,
                .count = pilot_watchdog_count,
                .enable = pilot_watchdog_enable,
                .disable = pilot_watchdog_disable
};

static hw_hal_t pilot_watchdog_hal = {
                .dev_type = EDEV_TYPE_WATCHDOG,
                .owner = THIS_MODULE,
                .devname = PILOT_WATCHDOG_DRIVER_NAME,
                .num_instances = 1,
                .phal_ops = (void *) &pilot_watchdog_ops
};

static irqreturn_t pilot_watchdog_irq_handler(int irq, void *dev_id)
{

        volatile uint32_t reg;

        


#ifdef SOC_PILOT_IV
        int i;

        /*Set watchdog fired bit based on pre-trigger status*/
        for (i = 0; i < MAX_WDT_DEVICES-1; i++)
        {
                reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + (i)*0x10 + SYSWCR);
                if(reg & PRETRIGGER_VALUE_REACHED_STATUS)
                {
                        // Default the Boot SPI device (bank 0) if SPI or ARM reset is enabled . 
                        reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + (i)*0x10 + SYSWRERL);    
                        if ((reg & RESET_BOOT_SPI_IFC) || (reg & RESET_ARM_PROCESSOR)) 
                        {

                            // Reset the SPI controller when SPI module reset or ARM reset is enabled 
                            reg |= RESET_BOOT_SPI_IFC;
                            iowrite32(reg, (void __iomem*)SE_RES_DEB_VA_BASE + SYSWRERL);

                            // Default the Boot SPI device (bank 0)
                            restore_spidevice_to_default_state(0);  
                        }
                        
                        reg |= PRETRIGGER_VALUE_REACHED_STATUS; 
			            iowrite32(reg, (void __iomem*)SE_RES_DEB_VA_BASE + i*0x10 + SYSWCR);
                        return IRQ_HANDLED;
               }
        }
#endif
        /* 
           We cannot reset the Controller SPI or any other module along with watchdog timer expiration using  BMC watchdog timer 1 
           But this can be done for watchdog timer 2/3/4. 

           Due to the BMC watchdog timer 1 limitations, We reset the SPI module sepreately using Software Reset Controller.
           before BMC watchog 1 expiration. This achieved by delayng the watchdog hardware signal to 50ms ie) enabling 
           'Delayed WDO enable' bit in PILOT_WATCHDOG_WDCTL register.
           
           Enabling  'Delayed WDO enable' bit is done in "pilot_watchdog_enable" function . Please refer EIP 259001 for SPI reset 
        */
        // Default the Boot SPI device (bank 0) .
        restore_spidevice_to_default_state(0);

        // Set the bit 7 for the SPI Module reset 
        reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + PILOT_SOFTWARE_SYSSRER); 
        reg |= RESET_BOOT_SPI_IFC;
        iowrite32(reg, (void __iomem*)SE_RES_DEB_VA_BASE + PILOT_SOFTWARE_SYSSRER); 

        // Clear the reset status register  
        reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE + PILOT_RESET_STATUS_SYSRST); 
        reg |= 0x00000007;
        iowrite32(reg, (void __iomem*)SE_RES_DEB_VA_BASE + PILOT_RESET_STATUS_SYSRST); 

        // Set the default Software reset pulse 24 ms (0x30) and also enable software enable bit (bit 2)
        // of Software Reset Controller. 
        reg = ioread32((void __iomem*)SE_RES_DEB_VA_BASE +  PILOT_RESET_CONTROL_SYSRCR); 
        reg |= 0x3000 | PILOT_SOFTWARE_RESET_ENABLE ;
        iowrite32(reg, (void __iomem*)SE_RES_DEB_VA_BASE + PILOT_RESET_CONTROL_SYSRCR); 

        reg = ioread32(pilot_watchdog_virt_base + PILOT_WATCHDOG_WDCTL);
        reg |= PILOT_WATCHDOG_WDCTL_FIRE;
        iowrite32(reg, pilot_watchdog_virt_base + PILOT_WATCHDOG_WDCTL);
        return IRQ_HANDLED;
}

static int __init pilot_watchdog_init(void)
{
        int ret;
#ifdef SOC_PILOT_IV
        int i;
#endif

        pilot_watchdog_hal_id = register_hw_hal_module(&pilot_watchdog_hal, (void **) &watchdog_core_ops);
        if (pilot_watchdog_hal_id < 0) {
                printk(KERN_WARNING "%s: register HAL HW module failed\n", PILOT_WATCHDOG_DRIVER_NAME);
                return pilot_watchdog_hal_id;
        }

        if (!request_mem_region(PILOT_WATCHDOG_REG_BASE, PILOT_WATCHDOG_REG_LEN, PILOT_WATCHDOG_DRIVER_NAME)) {
                printk(KERN_ERR PILOT_WATCHDOG_DRIVER_NAME ": Can't request memory region\n");
                ret = -EBUSY;
                goto out_hal_register;
        }

        pilot_watchdog_virt_base = ioremap(PILOT_WATCHDOG_REG_BASE, PILOT_WATCHDOG_REG_LEN);
        if (!pilot_watchdog_virt_base) {
                ret = -ENOMEM;
                goto out_mem_region;
        }

        ret = request_irq(PILOT_WATCHDOG_IRQ, pilot_watchdog_irq_handler, IRQF_DISABLED, PILOT_WATCHDOG_DRIVER_NAME, pilot_watchdog_virt_base);
        if (ret) {
                printk(KERN_WARNING "%s: request irq failed\n", PILOT_WATCHDOG_DRIVER_NAME);
                goto out_iomap;
        }
	g_reset_enable_low[0] = (RESET_ARM_PROCESSOR |RESET_I2C_INTERFACES);
#ifdef SOC_PILOT_IV
        for (i = 1; i < MAX_WDT_DEVICES; i++)
        {
            g_reset_enable_low[i] = (RESET_ARM_PROCESSOR | RESET_I2C_INTERFACES_0_TO_7 | RESET_BOOT_SPI_IFC);
            g_reset_enable_high[i] = (RESET_I2C_INTERFACES_8_TO_9 | RESET_EMMC);
        }    
#endif        
        return 0;

out_iomap:
        iounmap(pilot_watchdog_virt_base);
out_mem_region:
        release_mem_region(PILOT_WATCHDOG_REG_BASE, PILOT_WATCHDOG_REG_LEN);
out_hal_register:
        unregister_hw_hal_module(EDEV_TYPE_WATCHDOG, pilot_watchdog_hal_id);

        return ret;
}

static void __exit pilot_watchdog_exit(void)
{
        free_irq(PILOT_WATCHDOG_IRQ, pilot_watchdog_virt_base);
        iounmap(pilot_watchdog_virt_base);
        release_mem_region(PILOT_WATCHDOG_REG_BASE, PILOT_WATCHDOG_REG_LEN);
        unregister_hw_hal_module(EDEV_TYPE_WATCHDOG, pilot_watchdog_hal_id);
}

module_init(pilot_watchdog_init);
module_exit(pilot_watchdog_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("ServerEngines Pilot-II/Pilot-III watchdog timer driver");
MODULE_LICENSE("GPL");
