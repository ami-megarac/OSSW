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
 * ast_watchdog.c
 * ASPEED AST2050/2100/2150/2200 watchdog timer driver
 *
 ****************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/bitops.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "driver_hal.h"
#include "watchdog_core.h"

#define AST_WATCHDOG_REG_BASE   0x1E785000
#define AST_WATCHDOG_REG_LEN    SZ_4K

/* registers offset */
#define AST_WATCHDOG_CNT        0x00
#define AST_WATCHDOG_VALUE      0x04
#define AST_WATCHDOG_RESTART    0x08
#define AST_WATCHDOG_CTRL       0x0C
#define AST_WATCHDOG_TIMEOUT    0x10
#define AST_WATCHDOG_CLEAR      0x14
#define AST_WATCHDOG_WIDTH      0x18

#define AST_WATCHDOG2_CNT               0x20
#define AST_WATCHDOG2_VALUE             0x24
#define AST_WATCHDOG2_RESTART   0x28
#define AST_WATCHDOG2_CTRL              0x2C
#define AST_WATCHDOG2_TIMEOUT   0x30
#define AST_WATCHDOG2_CLEAR             0x34
#define AST_WATCHDOG2_WIDTH             0x38

/* bits of watchdog control register */
#define AST_WATCHDOG_CTRL_CLOCK         0x10
#define AST_WATCHDOG_CTRL_EXTERNAL      0x08
#define AST_WATCHDOG_CTRL_INTERRUPT     0x04
#define AST_WATCHDOG_CTRL_RESET         0x02
#define AST_WATCHDOG_CTRL_ENABLE        0x01

#define AST_WATCHDOG_RELOAD_MAGIC       0x4755

#define AST_WATCHDOG_DRIVER_NAME "ast_watchdog"
#define AST_WATCHDOG_COUNT_PER_SEDOND 1000000 /* we use external 1MHz clock source */

static void *ast_watchdog_virt_base;
static struct watchdog_core_funcs_t *watchdog_core_ops;
static int ast_watchdog_hal_id;

#if defined(SOC_AST2500)
#define MAX_WDT_DEVICES 3
#else
#if defined (SOC_AST2400)
#define MAX_WDT_DEVICES 2
#else
#define MAX_WDT_DEVICES 1
#endif
#endif
#define MIN_WDT_DEVICE 1

static int current_wdt_device = 1;

#if defined(SOC_AST2500)
#define AST_WATCHDOG_RESET_MASK 0x1c

u32 g_wdtdev_reset_mask[MAX_WDT_DEVICES];

int set_cur_wdtdev_reset_mask(const char *val, const struct kernel_param *kp)
{
        u32 reset_mask = 0;
        int ret = 0;

        ret = kstrtou32(val ,0 ,&reset_mask);
        if(ret)
             goto exit;

        g_wdtdev_reset_mask[current_wdt_device - 1] = reset_mask;
exit:   
        return ret;
}

int get_cur_wdtdev_reset_mask(char *val, const struct kernel_param *kp)
{
        return (snprintf(val,64,"0x%x",g_wdtdev_reset_mask[current_wdt_device - 1]));
}

const struct kernel_param_ops current_wdt_reset_mask0 =
{
     .set = &set_cur_wdtdev_reset_mask, 
     .get = &get_cur_wdtdev_reset_mask,
};

module_param_cb(current_wdt_reset_mask0,
                &current_wdt_reset_mask0,
                &g_wdtdev_reset_mask,
                0644);

#endif


static const int min_wdt_num = 1, max_wdt_num = MAX_WDT_DEVICES; 


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

MODULE_PARM_DESC(current_wdt_device,"Current Watchdog timer dev no");

static void ast_watchdog_set_value(int value)
{
#ifdef CONFIG_SPX_FEATURE_SELECT_WDT2
        iowrite32(value * AST_WATCHDOG_COUNT_PER_SEDOND, ast_watchdog_virt_base + AST_WATCHDOG2_VALUE);
#else
        iowrite32(value * AST_WATCHDOG_COUNT_PER_SEDOND, ast_watchdog_virt_base +(current_wdt_device - 1)*0x20 + AST_WATCHDOG_VALUE);
#endif
}

static int ast_watchdog_get_status(void) 
{ 
    /* return watchdog timeout status to determine system reset from 2nd boot source or not */ 
    return ((ioread32(ast_watchdog_virt_base + AST_WATCHDOG_TIMEOUT + ((current_wdt_device - 1)*0x20)) & 0x00000002) >> 1 ); 
} 

static void ast_watchdog_count(void)
{
#ifdef CONFIG_SPX_FEATURE_SELECT_WDT2
        iowrite32(AST_WATCHDOG_RELOAD_MAGIC, ast_watchdog_virt_base + AST_WATCHDOG2_RESTART);
#else
        iowrite32(AST_WATCHDOG_RELOAD_MAGIC, ast_watchdog_virt_base + (current_wdt_device - 1)*0x20 + AST_WATCHDOG_RESTART);
#endif
}

static void ast_watchdog_enable(void)
{
        /* we use external 1MHz clock source */
#ifdef CONFIG_SPX_FEATURE_SELECT_WDT2
        iowrite32(AST_WATCHDOG_CTRL_CLOCK | AST_WATCHDOG_CTRL_RESET | AST_WATCHDOG_CTRL_ENABLE, ast_watchdog_virt_base + AST_WATCHDOG2_CTRL);
#else
        iowrite32(AST_WATCHDOG_CTRL_CLOCK | AST_WATCHDOG_CTRL_RESET | AST_WATCHDOG_CTRL_ENABLE, ast_watchdog_virt_base + (current_wdt_device - 1)*0x20 + AST_WATCHDOG_CTRL);
#endif
#if defined(SOC_AST2500)
        iowrite32(g_wdtdev_reset_mask[current_wdt_device - 1], ast_watchdog_virt_base + (current_wdt_device - 1)*0x20 + AST_WATCHDOG_RESET_MASK);
#endif  
}

static void ast_watchdog_disable(void)
{
#ifdef CONFIG_SPX_FEATURE_SELECT_WDT2
        iowrite32(ioread32(ast_watchdog_virt_base + AST_WATCHDOG2_CTRL) & ~AST_WATCHDOG_CTRL_ENABLE, ast_watchdog_virt_base + AST_WATCHDOG2_CTRL);
#else
        iowrite32(ioread32(ast_watchdog_virt_base + AST_WATCHDOG_CTRL) & ~AST_WATCHDOG_CTRL_ENABLE, ast_watchdog_virt_base + (current_wdt_device - 1)*0x20 + AST_WATCHDOG_CTRL);
#endif
}

static struct watchdog_hal_ops_t ast_ops = {
        .set_value = ast_watchdog_set_value,
        .count = ast_watchdog_count,
        .get_status = ast_watchdog_get_status,
        .enable = ast_watchdog_enable,
        .disable = ast_watchdog_disable,
};

static hw_hal_t ast_hal = {
        .dev_type = EDEV_TYPE_WATCHDOG,
        .owner = THIS_MODULE,
        .devname = AST_WATCHDOG_DRIVER_NAME,
        .num_instances = 1,
        .phal_ops = (void *) &ast_ops
};

static int __init ast_watchdog_init(void)
{
        int ret;
#if defined(SOC_AST2500)
        int i;
#endif 

        extern int watchdog_core_loaded;
        if (!watchdog_core_loaded)
                        return -1;

        ast_watchdog_hal_id = register_hw_hal_module(&ast_hal, (void **) &watchdog_core_ops);
        if (ast_watchdog_hal_id < 0) {
                printk(KERN_WARNING "%s: register HAL HW module failed\n", AST_WATCHDOG_DRIVER_NAME);
                return ast_watchdog_hal_id;
        }

        if (!request_mem_region(AST_WATCHDOG_REG_BASE, AST_WATCHDOG_REG_LEN, AST_WATCHDOG_DRIVER_NAME)) {
                printk(KERN_ERR AST_WATCHDOG_DRIVER_NAME ": Can't request memory region\n");
                ret = -EBUSY;
                goto out_hal_register;
        }

        ast_watchdog_virt_base = ioremap(AST_WATCHDOG_REG_BASE, AST_WATCHDOG_REG_LEN);
        if (!ast_watchdog_virt_base) {
                ret = -ENOMEM;
                goto out_mem_region;
        }
#if defined(SOC_AST2500)
        for (i = 0; i < MAX_WDT_DEVICES; i++)
        {
                g_wdtdev_reset_mask[i] = ioread32(ast_watchdog_virt_base + (i * 0x20) + AST_WATCHDOG_RESET_MASK); 
        }
#endif  
        
        return 0;

out_mem_region:
        release_mem_region(AST_WATCHDOG_REG_BASE, AST_WATCHDOG_REG_LEN);
out_hal_register:
        unregister_hw_hal_module(EDEV_TYPE_WATCHDOG, ast_watchdog_hal_id);

        return ret;
}

static void __exit ast_watchdog_exit(void)
{
        iounmap(ast_watchdog_virt_base);
        release_mem_region(AST_WATCHDOG_REG_BASE, AST_WATCHDOG_REG_LEN);
        unregister_hw_hal_module(EDEV_TYPE_WATCHDOG, ast_watchdog_hal_id);
}

module_init(ast_watchdog_init);
module_exit(ast_watchdog_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2050/AST2100/2150/2200 watchdog timer driver");
MODULE_LICENSE("GPL");
