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
 * File name: ast_jtag.c
 * JTAG hardware driver is implemented in software mode instead of hardware mode.
 * Driver controlls TCK, TMS, and TDIO by software directly when software mode is enable.
 * The CLK is 12KHz.
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "driver_hal.h"
#include "jtag.h"
#include "jtag_ioctl.h"
#include "ast_jtag.h"


#define AST_JTAG_DRIVER_NAME	"ast_jtag"
#define bit_delay() udelay(2) // slower udelay(42)

volatile u8		*ast_jtag_v_add=0;
#ifdef SOC_AST2300
 volatile u8		*ast_gpio_v_add=0;
#endif
static int ast_jtag_hal_id;
static jtag_core_funcs_t *jtag_core_ops;

int g_is_Support_LoopFunc=0;

// jtag pin set for ALTERA
#define TDI_PIN 0 
#define TDO_PIN	1 
#define TCK_PIN	2 
#define TMS_PIN	3 

typedef enum {
	SET_LOW = 0,
	SET_HIGH,
	
	READ_VAL,
	
	FORCE_SET_LOW = 0x10,
	FORCE_SET_HIGH	
	
}eJtagPinactions;

/*
 * ast_tdo_pin
 * Note: Connect a GPIO(G5) to replace TDO, and read it to get data back from device. A workaround for AST2300/AST1050.
 */ 
static int ast_tdo_pin(void){
	int tdo_bit = 0;
	
	#ifdef SOC_AST2300
	 if(ioread32((void * __iomem)ast_gpio_v_add + GPIO_DATA_VALUE) & GPIO_TDO_BIT) {
	#else
	 if(ioread32((void * __iomem)ast_jtag_v_add + JTAG_STATUS) & SOFTWARE_TDIO_BIT) {
	#endif
		 tdo_bit = 1;
	}

	return tdo_bit;
}
	 
int jtag_io(int pin, int act){
	// TDI_PIN 0(w), TDO_PIN 1(r), TCK_PIN 2(w), TMS_PIN 3(w)
	static unsigned char Pins[4]={0,};
	int reVal = 0;
    unsigned int op_code = 0;
    unsigned int force_action = 0;
    
    switch(act){
    
    	case FORCE_SET_LOW:
       	case FORCE_SET_HIGH:
       		force_action = 1;
    	case SET_LOW:
    	case SET_HIGH:
			if(force_action == 1 || Pins[pin] != act) 
			{
				Pins[pin] =  act;

				if (Pins[TCK_PIN]) 
					op_code |= SOFTWARE_TCK_BIT; 
				if (Pins[TMS_PIN])
					op_code |= SOFTWARE_TMS_BIT; 
				if (Pins[TDI_PIN])
					op_code |= SOFTWARE_TDIO_BIT;

				//printk("P%X D%X\n",pin,act);
				iowrite32(SOFTWARE_MODE_ENABLE | op_code, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
			}
		break;
    	case READ_VAL:
    		if( pin == TDO_PIN){
				 reVal = ast_tdo_pin();
				 //printk("TDO %X\n",reVal);
			 }
		 break;
    }
    return reVal;
}

/*
 * jtag_sir
 * Send Jtag instruction
 */
void jtag_sir(unsigned short bits, unsigned int tdi){
	
	int i = 0;
	//uint32_t tdo;
	// go to DRSCAN
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	// go to IRSCAN
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	// go to CaptureIR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	// go to ShiftIR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	
	for(i = 0; i< bits; i++) {
		if(i == (bits - 1)) {
			if(tdi & (0x1 << i)) {
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
			} 
			else {
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
			}
		}
		else {
			if(tdi & (0x1 << i)) {
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
			}
			else {
				iowrite32(SOFTWARE_MODE_ENABLE, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
			}
		}
		//tdo = ioread32((void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	}
	
	// go to UpdateIR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();

	// go to IDLE
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT,(void * __iomem) ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
}


/*
 * jtag_sdr
 * Read data back and send instruction
 * Note: Connect a GPIO(G5) to replace TDO, and read it to get data back from device. A workaround for AST2300/AST1050.
 */
void jtag_sdr(unsigned short bits, unsigned int *TDI,unsigned int *TDO){
	
	unsigned int index = 0;
	u32 shift_bits =0;
	u32 dr_data;

	//go to DR Scan
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	//go to Capture DR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	
	// go to ShiftDR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	
	while (bits) {
		if( !(TDO) ) {
			//Write
			dr_data = (TDI[index] >> (shift_bits % 32)) & (0x1);
			if(bits == 1) {
				// go to Exit1DR
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | (dr_data << 16), (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | (dr_data << 16), (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
			}
			else {
				iowrite32(SOFTWARE_MODE_ENABLE | (dr_data << 16), (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | (dr_data << 16), (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
			}
		}
		else {
			//Read
			if(bits == 1) {
				// go to Exit1DR
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
			}
			else {
				iowrite32(SOFTWARE_MODE_ENABLE, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
				iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
				bit_delay();
			}
			#ifdef SOC_AST2300
			 if(ioread32((void * __iomem)ast_gpio_v_add + GPIO_DATA_VALUE) & GPIO_G5_BIT) {
			#else
			 if(ioread32((void * __iomem)ast_jtag_v_add + JTAG_STATUS) & SOFTWARE_TDIO_BIT) {
			#endif
				dr_data = 1;
				TDO[index] |= (0x1 << (shift_bits % 32));
			}
			else {
				dr_data = 0;
				TDO[index] &= ~(0x1 << (shift_bits % 32));
			}
		}
		shift_bits++;
		bits--;
		if((shift_bits % 32) == 0)
			index ++;
	}
	// go to UpdateDR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();

	// go to IDLE
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
}


/*
 * ast_jtag_reset
 * Generate at least 9 TMS high and 1 TMS low to force devices into Run-Test/Idle State
 */
void ast_jtag_reset (void)
{
	int i = 0;
	//State from test logic reset to Run-Test/Idle State 
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	for(i=0;i<10;i++) {
		iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
		bit_delay();
		iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
		bit_delay();
	}
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
}


/*
 * jtag_dr_pause
 * Switch JTAG status from IDLE to DRPAUSE and DRUPDATE to IDLE without going through ShiftDR status.
 * When call this function, the jtag status should be idle status.
 */
void jtag_dr_pause(unsigned int min_mSec){
	
	// start state is run test idle
	// go to DRSCAN
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	//go to Capture DR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	// go to Exit1DR skip ShiftDR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	// go to Pause DR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	//
	if (min_mSec != 0)
		mdelay(min_mSec);

	// go to Exit2DR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	// go to UpdateDR
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TMS_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	// go to IDLE
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	bit_delay();

}


/*
 * jtag_runtest_idle
 */
void jtag_runtest_idle(unsigned int tcks, unsigned int min_mSec)
{
	int i = 0;
	static unsigned int idle_count = 0;

	for(i = 0; i< tcks; i++) {
		iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
		bit_delay();
		iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TCK_BIT | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
		bit_delay();
		iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
		bit_delay();
	}
	
	if (min_mSec != 0){ 
		mdelay(min_mSec);
	}
	//msleep :: for kernel switch other task.
	if((idle_count ++ ) % 128 == 0){
		msleep(0);
 	}
}


/* -------------------------------------------------- */
static jtag_hal_operations_t ast_jtag_hw_ops = {
	.reset_jtag = ast_jtag_reset,
};

static hw_hal_t ast_jtag_hw_hal = {
	.dev_type = EDEV_TYPE_JTAG,
	.owner = THIS_MODULE,
	.devname = AST_JTAG_DRIVER_NAME,
	.num_instances = AST_JTAG_PORT_NUM,
	.phal_ops = (void *) &ast_jtag_hw_ops
};


int ast_jtag_init(void)
{
	int status;
	uint32_t reg;
	
	extern int jtag_core_loaded;
	if (!jtag_core_loaded)
		return -1;
	
	ast_jtag_hal_id = register_hw_hal_module(&ast_jtag_hw_hal, (void **) &jtag_core_ops);
	if (ast_jtag_hal_id < 0) {
		printk(KERN_WARNING "%s: register HAL HW module failed\n", AST_JTAG_DRIVER_NAME);
		return ast_jtag_hal_id;
	}
	
	ast_jtag_v_add = ioremap_nocache(AST_JTAG_REG_BASE, 0x40);
	if (!ast_jtag_v_add) {
		printk(KERN_WARNING "%s: ioremap failed\n", AST_JTAG_DRIVER_NAME);
	unregister_hw_hal_module(EDEV_TYPE_JTAG, ast_jtag_hal_id);
		return -ENOMEM;
	}
	
	#ifdef SOC_AST2300
	ast_gpio_v_add = ioremap_nocache(AST_GPIO_REG_BASE, 0x40);
	if (!ast_gpio_v_add) {
		printk(KERN_WARNING "%s: AST_GPIO_REG_BASE ioremap failed\n", AST_JTAG_DRIVER_NAME);
		unregister_hw_hal_module(EDEV_TYPE_JTAG, ast_jtag_hal_id);
		return -ENOMEM;
	}
	#endif
	
	//memset (&JTAG_device_information, 0, sizeof(JTAG_DEVICE_INFO));
	*(volatile u32 *)(IO_ADDRESS(0x1E6E2000)) = (0x1688A8A8); //Unlock SCU register
	
	#ifdef SOC_AST2300
	status = *(volatile u32 *)(IO_ADDRESS(0x1E6E2084));
	if(status & (0x1 << 5))
		printk(KERN_WARNING "%s: Watchdog WDTRST2 output function\n", AST_JTAG_DRIVER_NAME);
	else
	{
		reg = ioread32((void * __iomem)ast_gpio_v_add + GPIO_DIRECTION);
		reg = reg & ~(GPIO_G5_BIT);
		iowrite32(reg, (void * __iomem)ast_gpio_v_add + GPIO_DIRECTION);
		barrier();
	}
	#endif
	status = *(volatile u32 *)(IO_ADDRESS(0x1E6E2004));
	*(volatile u32 *)(IO_ADDRESS(0x1E6E2004)) = status &= ~(0x00400000); //Set JTAG Master Enable in SCU Reset Register
	*(volatile u32 *)(IO_ADDRESS(0x1E6E2000)) = 0; //Lock SCU register
	
	iowrite32(AST_JTAG_CTRL_ENABLE, (void * __iomem)ast_jtag_v_add + JTAG_CONTROL); // Enable Engine
	barrier();
	iowrite32(SOFTWARE_MODE_ENABLE | SOFTWARE_TDIO_BIT, (void * __iomem)ast_jtag_v_add + JTAG_STATUS);
	barrier();
	
	reg = ioread32((void * __iomem)ast_jtag_v_add + JTAG_INTERRUPT);
	reg = reg & ~(AST_JTAG_INTR_STATUS_MASK | AST_JTAG_INTR_ENABLE_MASK);
	iowrite32( reg, (void * __iomem)ast_jtag_v_add + JTAG_INTERRUPT); //Disable Interrupt
	barrier();
	mdelay(1); //let last data output.
	
	ast_jtag_reset();
	
	return 0;
}

void ast_jtag_exit(void)
{
	iowrite32(AST_JTAG_CTRL_ENG_OUT_DIS, (void * __iomem)ast_jtag_v_add + JTAG_CONTROL); // Disable Engine
	barrier();
	
	#ifdef SOC_AST2300
	iounmap (ast_gpio_v_add);
	#endif
	iounmap (ast_jtag_v_add);
	unregister_hw_hal_module(EDEV_TYPE_JTAG, ast_jtag_hal_id);
	
	return;
}

EXPORT_SYMBOL(g_is_Support_LoopFunc);
EXPORT_SYMBOL(ast_jtag_reset);
EXPORT_SYMBOL(jtag_sdr);
EXPORT_SYMBOL(jtag_sir);
EXPORT_SYMBOL(jtag_runtest_idle);
EXPORT_SYMBOL(jtag_dr_pause);
EXPORT_SYMBOL(jtag_io);

module_init (ast_jtag_init);
module_exit (ast_jtag_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("ASPEED AST SoC JTAG Driver");
MODULE_LICENSE ("GPL");
