/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
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
 * platform_neoncity.c
 * platform-specific initialization module for neoncity
 *
 ****************************************************************/
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/types.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include "platform_neoncity.h"


#define PLATFORM_MAJOR           145
#define PLATFORM_MAX_CHANNELS    1
#define PLATFORM_DEV_NAME        "platform"

#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
  #define USE_UNLOCKED_IOCTL  
  #endif
#endif

#ifdef USE_UNLOCKED_IOCTL
static long platform_module_ioctl(struct file *file, uint cmd, ulong arg);
#else
static int platform_module_ioctl(struct inode *inode, struct file *file, uint cmd, ulong arg);
#endif

/* Module Variables */
static struct file_operations platform_ops = {
        owner:      THIS_MODULE,
        read:       NULL,
        write:      NULL,
#ifdef HAVE_UNLOCKED_IOCTL
        unlocked_ioctl: platform_module_ioctl,
#else
        ioctl:      platform_module_ioctl,
#endif
        open:       NULL,
        release:    NULL,
};

int __init init_module(void)
{
    uint32_t reg1;
	uint8 reg;
    printk("Initializing platform driver for PILOT-III\n");

    if (register_chrdev(PLATFORM_MAJOR,PLATFORM_DEV_NAME,&platform_ops) < 0)
    {
        printk("ERROR: Unable to register Platform Driver\n");
        return -EBUSY;
    }

    // Changing GPIO_2 (Front Panel ID LED) Pin direction into output
    reg= ioread8((void __iomem*)SE_GPIO_VA_BASE + PILOT_GPIO_2_ENABLE_OUTPUT_DIRECTION);
    reg |= 0x01;
    reg |= GPIO_ENABLE_5; //Enabling the Event Debounce 
    iowrite8(reg,(void __iomem*)SE_GPIO_VA_BASE + PILOT_GPIO_2_ENABLE_OUTPUT_DIRECTION);

    // Changing GPIO_2 (Front Panel ID LED) Pin Data into Low
    reg= ioread8((void __iomem*)SE_GPIO_VA_BASE + PILOT_GPIO_2_ENABLE_DATA_LOW);
    reg &= ~GPIO_ENABLE_2;
    iowrite8(reg,(void __iomem*)SE_GPIO_VA_BASE + PILOT_GPIO_2_ENABLE_DATA_LOW);

    // Changing GPIO_15 (SystemFault Green LED) Pin direction into output
    reg = 0;
    reg = ioread8((void __iomem*)SE_GPIO_VA_BASE + PILOT_GPIO_15_ENABLE_OUTPUT_DIRECTION);
    reg |= 0x01;
    reg |= GPIO_ENABLE_5; //Enabling the Event Debounce
    iowrite8(reg, (void __iomem*)SE_GPIO_VA_BASE + PILOT_GPIO_15_ENABLE_OUTPUT_DIRECTION); 

    // Changing GPIO_15 (SystemFault Green LED) Pin Data into Low
    reg = ioread8((void __iomem*)SE_GPIO_VA_BASE + PILOT_GPIO_15_ENABLE_DATA_LOW);
    reg &= ~GPIO_ENABLE_7;
    iowrite8(reg, (void __iomem*)SE_GPIO_VA_BASE + PILOT_GPIO_15_ENABLE_DATA_LOW);

   // Stopping the Front Panel LED (GPIO_2) Blinking Rate	
    reg = ioread8((void __iomem*) SE_GPIO_VA_BASE + PILOT_GPIO_2_LED_BLINK_RATE);
    reg &= 0xcf;
    iowrite8(reg,(void __iomem*)SE_GPIO_VA_BASE + PILOT_GPIO_2_LED_BLINK_RATE);

   // Stopping the Status Fault LED (GPIO_15) Blinking Rate
    reg = ioread8((void __iomem*) SE_GPIO_VA_BASE + PILOT_GPIO_15_LED_BLINK_RATE);
    reg &= 0x3f;
    iowrite8(reg,(void __iomem*)SE_GPIO_VA_BASE + PILOT_GPIO_15_LED_BLINK_RATE);

   // Converting the SD PINs into GPIOs for Fan Presence
    reg1 = ioread32((void __iomem*) SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL);
    reg1 |= 0x0000fe00;
    iowrite32(reg1,(void __iomem*) SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL);

     // Converting the NAND_DQS PIN into GPIO for Fan Presence
    reg1=0;
    reg1 = ioread32((void __iomem*) SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE);
    reg1 |= 0x80000000;
    iowrite32(reg1,(void __iomem*) SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE);

#if 0
    /* enable PS/2 keyboard and mouse pins */
    reg = ioread32(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL);
    reg |= (PILOT_TOP_LEVEL_PIN_CTRL_KEYBD_DATA | PILOT_TOP_LEVEL_PIN_CTRL_KEYBD_CLK | PILOT_TOP_LEVEL_PIN_CTRL_MOUSE_DATA | PILOT_TOP_LEVEL_PIN_CTRL_MOUSE_CLK);
    iowrite32(reg, SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL);
#endif
    return 0;
}

void __exit cleanup_module(void)
{
    /* unregister char device */
    unregister_chrdev(PLATFORM_MAJOR,PLATFORM_DEV_NAME);
}

#ifdef USE_UNLOCKED_IOCTL
static long platform_module_ioctl(struct file *file, uint cmd, ulong arg)
#else
static int platform_module_ioctl(struct inode *inode, struct file *file, uint cmd, ulong arg)
#endif
{
    uint32_t reg,hardreg,sysreg;
    platform_value_t platform_value;

    reg = hardreg = sysreg = 0;
    switch(cmd)
    {
     case GPIO_MUX_CTRL:
        if (copy_from_user ((void*)&platform_value, (void *)arg, sizeof (platform_value)))
        {
            printk("GPIO Pin Multiplexing: Error copying data from user\n");
            return -EFAULT;
        }
        if((platform_value.AltFunc > 3)||(platform_value.AltFunc < 0))
        {
            printk("GPIO Pin Multiplexing: Invalid Select/De-select GPIO Option\n");
            return -EINVAL;
        }
        
        switch(platform_value.PortNum)
        {
            case GPIO_PORT_2:
                if(platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_2 (Category: "GPIO", GIPO_16-23) as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_0;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_1;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_2;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                       reg = ~GPIO_ENABLE_3;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                       reg = ~GPIO_ENABLE_4;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                       reg = ~GPIO_ENABLE_5;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                      reg = ~GPIO_ENABLE_6;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                      reg = ~GPIO_ENABLE_7;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_2);
                        return -EINVAL;
                    }

                   *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) &= reg;
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_2 (Category: "GPIO", GPIO_16-23) as PWM related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_0;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_1;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_2;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = GPIO_ENABLE_3;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = GPIO_ENABLE_4;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = GPIO_ENABLE_5;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = GPIO_ENABLE_6;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = GPIO_ENABLE_7;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_2);
                        return -EINVAL;
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) |= reg;
                }
                break;

            case GPIO_PORT_3:
                /* Enabling GPIO_PORT_3 (Category: "GPIO", GPIO_24-31) as per product specification */
                if(platform_value.PinNum != 0xFF)
                {
                    printk("Cannot Enable/Disable Specific Pins in Port %d\n",GPIO_PORT_3);
                    return -EINVAL;
                }
                sysreg = *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_DATA));
                hardreg = *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL));
                if(platform_value.AltFunc == 1)
                {
                    /* Reading MAC1_STRAP and RGMII_1_STRAP using Software Strap Control Register*/
                    /* Enabling GPIO_PORT_3 (Category: "GPIO", GIPO_24-31) as per product specification*/
                    if (sysreg & MAC1_STRAP)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            printk("Inside Hard register override\n");
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~MAC1_STRAP;
                        }
                    }
                    if (sysreg & RGMII_1_MODE_STRAP)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~RGMII_1_MODE_STRAP;
                        }
                    }
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_3 (Category: "GPIO", GPIO_24-31) as RGMII_1 related pins */
                    if ((sysreg & MAC1_STRAP) == 0)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= MAC1_STRAP;
                        }
                    }
                    if ((sysreg & RGMII_1_MODE_STRAP) == 0)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= RGMII_1_MODE_STRAP;
                        }
                    }
                }
                else if(platform_value.AltFunc == 3)
                {
                    /* Enabling GPIO_PORT_3 (Category: "GPIO", GPIO_24-31) as MAC1 related pins */
                    if ((sysreg & MAC1_STRAP) == 0)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= MAC1_STRAP;
                        }
                    }
                    if (sysreg & RGMII_1_MODE_STRAP)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~RGMII_1_MODE_STRAP;
                        }
                    }
                }
                break;

            case GPIO_PORT_14:
                /* Enable RGMII_1 as GPIO Pins (GPIO Port 14 Pin 0 to 4) */
                sysreg = *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_DATA));
                hardreg = *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL));

                if (platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_14 (Category: "Add1 GPIO/RGMII_1", GIPO_32-36 or GPIO_113-116) as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_8;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_9;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_10;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = ~GPIO_ENABLE_11;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = 0xff;
                        if(sysreg & RGMII_0_MODE_STRAP)
                        {
                            if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                            {
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~RGMII_0_MODE_STRAP;
                            }
                        }
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_14);
                        return -EINVAL;
                    }

                    /* Reading RGMII_1_STRAP using Software Strap Control Register*/
                    if (sysreg & RGMII_1_MODE_STRAP)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~RGMII_1_MODE_STRAP;
                        }
                    }
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) &= reg;
                }
                else if (platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_14 (Category: "Add1 GPIO/RGMII_1", GIPO_32-36 or GPIO_112-116) as Fan Tach related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_8;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_9;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_10;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = GPIO_ENABLE_11;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        /* Enabling GPIO_PORT_14_4 as RGMII_REFCLK pin */
                        reg = 0;
                        if((sysreg & RGMII_0_MODE_STRAP) == 0)
                        {
                            if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                            {
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= RGMII_0_MODE_STRAP;
                            }
                        }
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_14);
                        return -EINVAL;
                    }

                    /* Reading RGMII_1_STRAP using Software Strap Control Register*/
                    if (sysreg & RGMII_1_MODE_STRAP)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~RGMII_1_MODE_STRAP;
                        }
                    }
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) |= reg;
                }
                else if (platform_value.AltFunc == 3)
                {
                    /* Enabling GPIO_PORT_14 (Category: "Add1 GPIO/RGMII_1", GIPO_32-36 or GPIO_112-116) as RGMII related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_8;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_9;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_10;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = ~GPIO_ENABLE_11;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        /* Enabling GPIO_PORT_14_4 as RGMII_REFCLK pin */
                        reg = 0xff;
                        if(sysreg & RGMII_0_MODE_STRAP)
                        {
                            if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                            {
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~RGMII_0_MODE_STRAP;
                            }
                        }
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_14);
                        return -EINVAL;
                    }

                    /* Reading RGMII_1_STRAP using Software Strap Control Register*/
                    if ((sysreg & RGMII_1_MODE_STRAP) == 0)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= RGMII_1_MODE_STRAP;
                        }
                    }
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) &= reg;
                }
                break;

            case GPIO_PORT_16:
                /* Enable SD Card as GPIO Pins ( GPIO Port 16 Pin 0 to 7) */
                sysreg = *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_DATA));
                hardreg = *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL));

                if(platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_16 (Category: "SD Card") as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_0;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_1;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_2;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = GPIO_ENABLE_3;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = GPIO_ENABLE_4;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = GPIO_ENABLE_5;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = GPIO_ENABLE_6;
                    }
                    else if(platform_value.PinNum  == 7)
                    {
                        reg = GPIO_ENABLE_7;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_16);
                        return -EINVAL;
                    }

                    if(sysreg & SDCARD_STRAP)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~SDCARD_STRAP;
                        }
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE)) |= reg;
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_16 (Category: "SD Card") as SD-CARD related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg &= ~GPIO_ENABLE_0;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg &= ~GPIO_ENABLE_1;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg &= ~GPIO_ENABLE_2;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg &= ~GPIO_ENABLE_3;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg &= ~GPIO_ENABLE_4;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg &= ~GPIO_ENABLE_5;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg &= ~GPIO_ENABLE_6;
                    }
                    else if(platform_value.PinNum  == 7)
                    {
                        reg &= ~GPIO_ENABLE_7;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_16);
                        return -EINVAL;
                    }

                    if((sysreg & SDCARD_STRAP) == 0)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= SDCARD_STRAP;
                        }
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE)) &= reg;
                }
                else if(platform_value.AltFunc == 3)
                {
                    /* Enabling GPIO_PORT_16 (Category: "SD Card") as SPI related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg &= ~GPIO_ENABLE_0;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg &= ~GPIO_ENABLE_1;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg &= ~GPIO_ENABLE_4;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg &= ~GPIO_ENABLE_5;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_16);
                        return -EINVAL;
                    }

                    if(sysreg & SDCARD_STRAP)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~SDCARD_STRAP;
                        }
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE)) &= reg;
                }
                break;

            case GPIO_PORT_17:
                /* Enable RMII as GPIO Pins (GPIO Port 17 Pin 0 to 7) */
                sysreg = *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_DATA));
                hardreg = *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL));

                if(platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_17 (Category: "RGMII") as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_8;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_9;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_10;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = GPIO_ENABLE_11;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = GPIO_ENABLE_12;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = GPIO_ENABLE_13;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = GPIO_ENABLE_14;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = GPIO_ENABLE_15;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_17);
                        return -EINVAL;
                    }

                    if(sysreg & RGMII_0_MODE_STRAP)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~RGMII_0_MODE_STRAP;
                        }
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE)) |= reg;
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_17 (Category: "RMII") as RGMII related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_8;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_9;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_10;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = ~GPIO_ENABLE_11;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = ~GPIO_ENABLE_12;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = ~GPIO_ENABLE_13;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = ~GPIO_ENABLE_14;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = ~GPIO_ENABLE_15;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_17);
                        return -EINVAL;
                    }

                    if((sysreg & RGMII_0_MODE_STRAP) == 0)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= RGMII_0_MODE_STRAP;
                        }
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE)) &= reg;
                }
                else if(platform_value.AltFunc == 3)
                {
                    /* Enabling GPIO_PORT_17 (Category: "RMII") as MAC0 related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_8;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_9;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_10;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = ~GPIO_ENABLE_11;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = ~GPIO_ENABLE_12;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = ~GPIO_ENABLE_13;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = ~GPIO_ENABLE_14;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = ~GPIO_ENABLE_15;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_17);
                        return -EINVAL;
                    }

                    if(sysreg & RGMII_0_MODE_STRAP)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~RGMII_0_MODE_STRAP;
                        }
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE)) &= reg;
                }
                break;

            case GPIO_PORT_18:
                /* Enable RMII as GPIO Pins (GPIO Port 18 Pin 0 to 1) */
                sysreg = *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_DATA));
                hardreg = *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL));

                if(platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_18 (Category: "RMII","RGMII_0","NAND/External Bus Interface") as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                       reg = GPIO_ENABLE_16;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                       reg = GPIO_ENABLE_17;
                    }
                    else if (platform_value.PinNum > 7)
                    {
                       printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_18);
                       return -EINVAL;
                    }

                    /* Enable RGMII_0  MAC0_TXD_2,MAC0_TXD_3,MAC0_RXD_2,MAC0_RXD_3 ( GPIO Port 18 Pin 2 to 5) */
                    if((sysreg & RGMII_0_MODE_STRAP) && (platform_value.PinNum >= 0) && (platform_value.PinNum<=5))
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~RGMII_0_MODE_STRAP;
                        }
                    }

                    /* for GPIO_PORT_18_0-1, enabling GPIO also need to pull down RGMII_1_MODE_STRAP & MAC1_STRAP */
                    if ((platform_value.PinNum == 0) || (platform_value.PinNum == 1))
                    {
                        if (sysreg & RGMII_1_MODE_STRAP)
                            if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                            {
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~RGMII_1_MODE_STRAP;
                            }
                        if (sysreg & MAC1_STRAP)
                            if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                            {
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~MAC1_STRAP;
                            }
                    }

                    /* Enable NAND/External Bus Interface as GPIO (GPIO Port 18 Pin 6 to 7) */
                    if((sysreg & EXT_BUS_STRAP) && ((platform_value.PinNum == 6) || (platform_value.PinNum == 7)))
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~EXT_BUS_STRAP;
                        }
                    }

                    if(reg > 0)
                    {
                        *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE)) |= reg;
                    }
                }
                else if(platform_value.AltFunc == 2)
                {
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_16;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_17;
                    }
                    else if (platform_value.PinNum > 7)
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_18);
                        return -EINVAL;
                    }

                    /* Enable RGMII_0  MAC0_TXD_2,MAC0_TXD_3,MAC0_RXD_2,MAC0_RXD_3 ( GPIO Port 18 Pin 2 to 5) */
                    if(((sysreg & RGMII_0_MODE_STRAP) == 0) && (platform_value.PinNum >= 2) && (platform_value.PinNum<=5))
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= RGMII_0_MODE_STRAP;
                        }
                    }

                    /* Enable NAND/External Bus Interface as GPIO (GPIO Port 18 Pin 6 to 7) */
                    if(((sysreg & EXT_BUS_STRAP) == 0) && ((platform_value.PinNum == 6) || (platform_value.PinNum == 7)))
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= EXT_BUS_STRAP;
                        }
                    }

                    if(reg > 0)
                    {
                        *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE)) &= reg;
                    }
                }
                break;

            case GPIO_PORT_19:
            case GPIO_PORT_20:
                if(platform_value.PinNum != 0xFF)
                {
                    printk("Cannot Enable/Disable Specific Pins in Port %d\n", platform_value.PortNum);
                    return -EINVAL;
                }
                /* Enable NAND/External Bus Interface as GPIO (GPIO_PORT_19 & GPIO_PORT_20, Pin 0 to 7) */
                sysreg = *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_DATA));
                hardreg = *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL));
                if(platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_19 & GPIO_PORT_20 (Category: "NAND/External Bus Interface") as per product specification*/
                    if(sysreg & EXT_BUS_STRAP)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~EXT_BUS_STRAP;
                        }
                    }
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_19 & GPIO_PORT_20 (Category: "NAND/External Bus Interface") as NAND related pins */
                    if((sysreg & EXT_BUS_STRAP) == 0)
                    {
                        if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= EXT_BUS_STRAP;
                        }
                    }
                }
                break;

            case GPIO_PORT_21:
                if(platform_value.AltFunc == 1)
                {
                     /* Enabling GPIO_PORT_21_0-1 (Category: "Boot SPI") as per product specification*/
                     if(platform_value.PinNum == 0)
                     {
                        reg = GPIO_ENABLE_18;
                     }
                     else if(platform_value.PinNum == 1)
                     {
                        reg = GPIO_ENABLE_19;
                     }
                     else
                     {
                         printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_21);
                         return -EINVAL;
                     }

                   *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE)) |= reg;
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_21_0-1 (Category: "Boot SPI") as SPI related pins */
                    if(platform_value.PinNum == 0)
                    {
                       reg = ~GPIO_ENABLE_18;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                       reg = ~GPIO_ENABLE_19;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_21);
                        return -EINVAL;
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO0_ENABLE)) &= reg;
                }
                break;

            case GPIO_PORT_22:
                if(platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_22 (Category: "UART1") as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_0;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_1;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_2;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = GPIO_ENABLE_3;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = GPIO_ENABLE_4;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = GPIO_ENABLE_5;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = GPIO_ENABLE_6;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = GPIO_ENABLE_7;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_22);
                        return -EINVAL;
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO1_ENABLE)) |= reg;
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_22 (Category: "UART1") as UART1 related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_0;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_1;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_2;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                       reg = ~GPIO_ENABLE_3;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                       reg = ~GPIO_ENABLE_4;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                       reg = ~GPIO_ENABLE_5;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                      reg = ~GPIO_ENABLE_6;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                      reg = ~GPIO_ENABLE_7;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_22);
                        return -EINVAL;
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO1_ENABLE)) &= reg;
                }
                break;

            case GPIO_PORT_23:
                if(platform_value.AltFunc == 1)
                {
                    /* Enable FAN Tach UART2_CTS_N,UART2_DCD_N,UART2_DSR_N,UART2_RI_N as GPIO (GPIO Port 23 Pin 0 to 3) */
                    /* Enabling GPIO_PORT_23 (Category: "FAN_TACH","BMC UART","PWM") as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_8;
                        hardreg = ~GPIO_ENABLE_12;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_9;
                        hardreg = ~GPIO_ENABLE_13;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_10;
                        hardreg = ~GPIO_ENABLE_14;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = GPIO_ENABLE_11;
                        hardreg = ~GPIO_ENABLE_15;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                       reg = GPIO_ENABLE_12;
                       hardreg = 0;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                       reg = GPIO_ENABLE_13;
                       hardreg = ~GPIO_ENABLE_16;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                       reg = GPIO_ENABLE_14;
                       hardreg = ~GPIO_ENABLE_17;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                       reg = GPIO_ENABLE_15;
                       hardreg = 0;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_23);
                        return -EINVAL;
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO1_ENABLE)) |= reg;

                    if(hardreg > 0)
                    {
                        *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) &= hardreg;
                    }
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_23 (Category: "FAN_TACH" & "BMC UART" & "PWM") as UART related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_8;
                        hardreg = ~GPIO_ENABLE_12;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_9;
                        hardreg = ~GPIO_ENABLE_13;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_10;
                        hardreg = ~GPIO_ENABLE_14;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = ~GPIO_ENABLE_11;
                        hardreg = ~GPIO_ENABLE_15;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                       reg = ~GPIO_ENABLE_12;
                       hardreg = 0;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                       reg = ~GPIO_ENABLE_13;
                       hardreg = ~GPIO_ENABLE_16;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                       reg = ~GPIO_ENABLE_14;
                       hardreg = ~GPIO_ENABLE_17;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                       reg = ~GPIO_ENABLE_15;
                       hardreg = 0;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_23);
                        return -EINVAL;
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO1_ENABLE)) &= reg;

                    if(hardreg > 0)
                    {
                        *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) &= hardreg;
                    }
                }
                else if(platform_value.AltFunc == 3)
                {
                    /* Enabling GPIO_PORT_23 (Category: "FAN_TACH" & "BMC UART" & "PWM") as FAN TACH or PWM related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_12;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_13;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_14;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                       reg = GPIO_ENABLE_15;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                       reg = GPIO_ENABLE_16;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                       reg = GPIO_ENABLE_17;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_23);
                        return -EINVAL;
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE+PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) |= reg;
                }
                break;

            case GPIO_PORT_24:
                    /* Enable SGPIO GPIO Pins (GPIO Port 24 Pin 0 to 3) */
                    sysreg = *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_DATA));
                    hardreg =*((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL));
                    if(platform_value.AltFunc == 1)
                    {
                        /* Enabling GPIO_PORT_24 (Category: "SGPIO","Host SPI") as per product specification*/
                        if(platform_value.PinNum == 0)
                        {
                            reg = GPIO_ENABLE_16;
                        }
                        else if(platform_value.PinNum == 1)
                        {
                            reg = GPIO_ENABLE_17;
                        }
                        else if(platform_value.PinNum == 2)
                        {
                            reg = GPIO_ENABLE_18;
                        }
                        else if(platform_value.PinNum == 3)
                        {
                            reg = GPIO_ENABLE_19;
                        }
                        else if((platform_value.PinNum >=4) && (platform_value.PinNum <= 7))
                        {
                           reg = 0;
                            if(sysreg & HSPI_EN_STRAP)
                            {
                                if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                                {
                                   *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                                   *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                                   *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~HSPI_EN_STRAP;
                                }
                            }
                        }
                        else
                        {
                            printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_24);
                            return -EINVAL;
                        }

                        if(reg > 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO1_ENABLE)) |= reg;
                        }
                    }
                    else if(platform_value.AltFunc == 2)
                    {
                        /* Enabling GPIO_PORT_24 (Category: "Host SPI") as SGPIO or HSPI related pins */
                        if(platform_value.PinNum == 0)
                        {
                            reg = ~GPIO_ENABLE_16;
                        }
                        else if(platform_value.PinNum == 1)
                        {
                            reg = ~GPIO_ENABLE_17;
                        }
                        else if(platform_value.PinNum == 2)
                        {
                            reg = ~GPIO_ENABLE_18;
                        }
                        else if(platform_value.PinNum == 3)
                        {
                            reg = ~GPIO_ENABLE_19;
                        }
                        else if((platform_value.PinNum >=4) && (platform_value.PinNum <= 7))
                        {
                            /* Disable Host SPI GPIO Pins (GPIO Port 24 Pin 4 to 7) */
                            reg = 0;
                            if((sysreg & HSPI_EN_STRAP) == 0)
                            {
                                    if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                                    {
                                        *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                                        *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                                        *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= HSPI_EN_STRAP;
                                    }
                            }
                        }
                        else
                        {
                            printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_24);
                            return -EINVAL;
                        }

                        if(reg > 0)
                        {
                            *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO1_ENABLE)) &= reg;
                        }
                    }
                    break;

            case GPIO_PORT_25:
                if(platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_25 (Category: "SWC(MISC) & UART3") as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_24;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_25;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_26;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = GPIO_ENABLE_27;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = GPIO_ENABLE_28;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = GPIO_ENABLE_29;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = GPIO_ENABLE_30;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = GPIO_ENABLE_31;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_25);
                        return -EINVAL;
                    }
                    /* Enable SWC(MISC) & UART3 GPIO Pins (GPIO Port 25,Pin 0 to 7) */
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO1_ENABLE)) |= reg;
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_25 (Category: "SWC(MISC) & UART3") as PASSTHRU or PWRBT related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_24;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_25;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_26;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = ~GPIO_ENABLE_27;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = ~GPIO_ENABLE_28;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = ~GPIO_ENABLE_29;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = ~GPIO_ENABLE_30;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = ~GPIO_ENABLE_31;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_25);
                        return -EINVAL;
                    }
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO1_ENABLE)) &= reg;
                }
                break;

            case GPIO_PORT_26:
                if(platform_value.AltFunc == 1)
                {
                   /* Enabling GPIO_PORT_26 (Category: "SWC(MISC) & UART3") as per product specification*/
                   if(platform_value.PinNum == 0)
                   {
                       reg = GPIO_ENABLE_0;
                       hardreg = 0;
                   }
                   else if(platform_value.PinNum == 1)
                   {
                       reg = GPIO_ENABLE_1;
                       hardreg = 0;
                   }
                   else if(platform_value.PinNum == 2)
                   {
                       reg = GPIO_ENABLE_2;
                       hardreg = ~GPIO_ENABLE_24;
                   }
                   else if(platform_value.PinNum == 3)
                   {
                       reg = GPIO_ENABLE_3;
                       hardreg = 0;
                   }
                   else if(platform_value.PinNum == 4)
                   {
                       reg = GPIO_ENABLE_4;
                       hardreg = 0;
                   }
                   else if(platform_value.PinNum == 5)
                   {
                       reg = GPIO_ENABLE_5;
                       hardreg = 0;
                   }
                   else if(platform_value.PinNum == 6)
                   {
                       reg = GPIO_ENABLE_6;
                       hardreg = ~GPIO_ENABLE_22;
                   }
                   else if(platform_value.PinNum == 7)
                   {
                       reg = GPIO_ENABLE_7;
                       hardreg = ~GPIO_ENABLE_23;
                   }
                   else
                   {
                       printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_26);
                       return -EINVAL;
                   }
                    /* Enable SWC(MISC) & UART3 GPIO Pins (GPIO Port 26,Pin 0,1,3,4,5) */
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) |= reg;

                    /* Enable SWC(MISC) & UART3 GPIO Pins (GPIO Port 26,Pin 2,Pin 7,Pin 6) */
                    if(hardreg > 0)
                    {
                        *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) &= hardreg;
                    }
                }
                else if(platform_value.AltFunc == 2)
                {
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_0;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_1;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        /* Enabling GPIO_PORT_26_2 (Category: "SWC(MISC) & UART3") as LDRQ_N pin */
                        reg = ~GPIO_ENABLE_2;
                        hardreg = GPIO_ENABLE_24;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = ~GPIO_ENABLE_3;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = ~GPIO_ENABLE_4;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = ~GPIO_ENABLE_5;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        /* Enabling GPIO_PORT_26_6 (Category: "SWC(MISC) & UART3") as BMC Watchdog pin */
                        reg = ~GPIO_ENABLE_6;
                        hardreg = GPIO_ENABLE_22;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        /* Enabling GPIO_PORT_26_7 (Category: "SWC(MISC) & UART3") as SIO Watchdog pin */
                        reg = ~GPIO_ENABLE_7;
                        hardreg = GPIO_ENABLE_23;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_26);
                        return -EINVAL;
                    }
                     *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) &= reg;

                     if(hardreg > 0)
                     {
                         *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) |= hardreg;
                     }
                }
                else if(platform_value.AltFunc == 3)
                {
                    if(platform_value.PinNum == 2)
                    {
                        /* Enabling GPIO_PORT_26_2 (Category: "SWC(MISC) & UART3") as SIO_SCI_N pin */
                        reg = ~GPIO_ENABLE_2;
                        hardreg = ~GPIO_ENABLE_24;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        /* Enabling GPIO_PORT_26_6 (Category: "SWC(MISC) & UART3") as UART3_RXD pin */
                        reg = ~GPIO_ENABLE_6;
                        hardreg = ~GPIO_ENABLE_22;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        /* Enabling GPIO_PORT_26_7 (Category: "SWC(MISC) & UART3") as UART3_TXD pin */
                        reg = ~GPIO_ENABLE_7;
                        hardreg = ~GPIO_ENABLE_23;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_26);
                        return -EINVAL;
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) &= hardreg;
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) &= reg;
                }
                break;

            case GPIO_PORT_27:
                /* Enable I2C-0 Pins as GPIO (GPIO Port 27, Pin 0,1,2,3,4,5,6,7) */
                sysreg = *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_DATA));
                hardreg = *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL));
                if(platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_27 (Category: "I2C0","I2C1","I2C2","I2C3") as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_8;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_9;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_10;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = GPIO_ENABLE_11;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = GPIO_ENABLE_12;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = GPIO_ENABLE_13;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = GPIO_ENABLE_14;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = GPIO_ENABLE_15;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_27);
                        return -EINVAL;
                    }

                    if ((platform_value.PinNum == 0) || (platform_value.PinNum == 1))
                    {
                        if(sysreg & UART_STRAP)
                        {
                            if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                            {
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~UART_STRAP;
                            }
                        }
                    }
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) |= reg;
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_27 (Category: "I2C0","I2C1","I2C2","I2C3") as I2C0-I2C3 related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_8;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_9;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_10;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = ~GPIO_ENABLE_11;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = ~GPIO_ENABLE_12;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = ~GPIO_ENABLE_13;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = ~GPIO_ENABLE_14;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = ~GPIO_ENABLE_15;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_27);
                        return -EINVAL;
                    }

                    if ((platform_value.PinNum == 0) || (platform_value.PinNum == 1))
                    {
                        if(sysreg & UART_STRAP)
                        {
                            if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                            {
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) &= ~UART_STRAP;
                            }
                        }
                    }
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) &= reg;
                }
                else if(platform_value.AltFunc == 3)
                {
                    /* Enabling GPIO_PORT_27_0-1 (Category: "I2C0") as UART4 related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_8;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_9;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_27);
                        return -EINVAL;
                    }

                    if ((platform_value.PinNum == 0) || (platform_value.PinNum == 1))
                    {
                        if((sysreg & UART_STRAP) == 0)
                        {
                            if((hardreg & ENABLE_OVERRIDE_STRAP_OPTION) == 0)
                            {
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_SYS_STRAP_CONFIG_REG))|= ENABLE_OVERRIDE_STRAP_OPTION;
                                *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_STRAPCTL)) |= UART_STRAP;
                            }
                        }
                    }
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) &= reg;
                }
                break;

            case GPIO_PORT_28:
                if(platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_28 (Category: "I2C4","I2C5","I2C6","I2C7") as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_16;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_17;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_18;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = GPIO_ENABLE_19;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = GPIO_ENABLE_20;
                        hardreg = ~GPIO_ENABLE_19;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = GPIO_ENABLE_21;
                        hardreg = ~GPIO_ENABLE_18;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = GPIO_ENABLE_22;
                        hardreg = ~GPIO_ENABLE_21;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = GPIO_ENABLE_23;
                        hardreg = ~GPIO_ENABLE_20;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_28);
                        return -EINVAL;
                    }
                    /* Enable I2C-4 as GPIO Pin 0,1,2,3 in Port 28 */
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) |= reg;

                    /* Enable I2C-6 as GPIO Pin 4,5,6,7 in Port 28 */
                    if(hardreg > 0)
                    {
                        *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) &= hardreg;
                    }
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_28 (Category: "I2C4","I2C5","I2C6","I2C7") as I2C4-I2C7 related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_16;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_17;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_18;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = ~GPIO_ENABLE_19;
                        hardreg = 0;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = ~GPIO_ENABLE_20;
                        hardreg = ~GPIO_ENABLE_19;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = ~GPIO_ENABLE_21;
                        hardreg = ~GPIO_ENABLE_18;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = ~GPIO_ENABLE_22;
                        hardreg = ~GPIO_ENABLE_21;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = ~GPIO_ENABLE_23;
                        hardreg = ~GPIO_ENABLE_20;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_28);
                        return -EINVAL;
                    }
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) &= reg;

                    if(hardreg > 0)
                    {
                        *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) &= hardreg;
                    }
                }
                else if(platform_value.AltFunc == 3)
                {
                    /* Enabling GPIO_PORT_28_4-7 (Category: "I2C6","I2C7") as PS2_DATA related pins */
                    if(platform_value.PinNum == 4)
                    {
                        reg = ~GPIO_ENABLE_20;
                        hardreg = GPIO_ENABLE_19;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = ~GPIO_ENABLE_21;
                        hardreg = GPIO_ENABLE_18;
                    }
                    else if(platform_value.PinNum == 6)
                    {
                        reg = ~GPIO_ENABLE_22;
                        hardreg = GPIO_ENABLE_21;
                    }
                    else if(platform_value.PinNum == 7)
                    {
                        reg = ~GPIO_ENABLE_23;
                        hardreg = GPIO_ENABLE_20;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_28);
                        return -EINVAL;
                    }

                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_PINMUXCTL)) |= hardreg;
                   *((uint32_t *)(SE_SYS_CLK_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) &= reg;
                }
                break;

            case GPIO_PORT_29:
                if(platform_value.AltFunc == 1)
                {
                    /* Enabling GPIO_PORT_29 (Category: "PWM & FANTACH") as per product specification*/
                    if(platform_value.PinNum == 0)
                    {
                        reg = GPIO_ENABLE_24;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = GPIO_ENABLE_25;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = GPIO_ENABLE_26;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = GPIO_ENABLE_27;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = GPIO_ENABLE_28;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = GPIO_ENABLE_29;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_29);
                        return -EINVAL;
                    }
                    /* Enable PWM FAN TACH GPIO Pins (GPIO Port 29 Pin 0 to 5)*/
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) |= reg;
                }
                else if(platform_value.AltFunc == 2)
                {
                    /* Enabling GPIO_PORT_29 (Category: "PWM & FANTACH") as PWM or FAN_TACH related pins */
                    if(platform_value.PinNum == 0)
                    {
                        reg = ~GPIO_ENABLE_24;
                    }
                    else if(platform_value.PinNum == 1)
                    {
                        reg = ~GPIO_ENABLE_25;
                    }
                    else if(platform_value.PinNum == 2)
                    {
                        reg = ~GPIO_ENABLE_26;
                    }
                    else if(platform_value.PinNum == 3)
                    {
                        reg = ~GPIO_ENABLE_27;
                    }
                    else if(platform_value.PinNum == 4)
                    {
                        reg = ~GPIO_ENABLE_28;
                    }
                    else if(platform_value.PinNum == 5)
                    {
                        reg = ~GPIO_ENABLE_29;
                    }
                    else
                    {
                        printk("Invalid pin number given for Alternate Function %d in Port %d\n",platform_value.AltFunc,GPIO_PORT_29);
                        return -EINVAL;
                    }
                    *((uint32_t *)(SE_TOP_LVL_PIN_VA_BASE + PILOT_TOP_LEVEL_PIN_CTRL_GPIO2_ENABLE)) &= reg;
                }
                break;

            default :
                printk("GPIO Pin Multiplexing: Invalid Gpio Pin Multiplexing Port Number\n");
                return -EINVAL;
        }
        break;

        default :
            printk("GPIO Pin Multiplexing: Invalid IOCTL\n");
            return -EINVAL;
}

return 0;

}

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("platform-specific initialization module for NeonCity");
MODULE_LICENSE("GPL");
