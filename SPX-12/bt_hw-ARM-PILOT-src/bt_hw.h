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
 * bt_hw.h
 * PILOT LPC controller BT-related
 * definitions, macros, prototypes
 *
*****************************************************************/

#ifndef ___BT_HW_H__
#define ___BT_HW_H__

#define BT_IRQ			IRQ_KCS_BT_SMIC_MAIL

#define BT_CHANNEL_NUM		1

#define BTCR 			0x08
#define BTDR 			0x09
#define BTIMSR 			0x0A
#define KCSIER			0x39
#define KCSISR			0x38

#define CLR_WR_PTR 			(1<<0)
#define CLR_RD_PTR			(1<<1)
#define B_BUSY 				(1<<7)
#define H2B_IRQ_EN 			(1<<2)
#define H2B_ATN 			(1<<2)
#define B2H_ATN 			(1<<3)
#define BTINTEN				(1<<5)
#define SMS_ATN				(1<<4)

#endif /* ! ___BT_HW_H__ */
