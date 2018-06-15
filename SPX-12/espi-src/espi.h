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

#ifndef __ESPI_H__
#define __ESPI_H__

#include <linux/types.h>

// IPMI Msg
typedef struct 
{
	u8				eSPIINuse;
	spinlock_t		espi_lock;
	u8				eSPISeqNo;
} eSPIBuf_T;

typedef struct 
{
	u8	Channel;
} __attribute__((packed)) IPMICmdMsg_T;

typedef struct 
{
	unsigned char (*num_espi_ch) (void);
	void (*reset_espi) (void);
	void (*read_espi_data_in) (u8 ch_num, char *buf);
	void (*read_espi_slave_regs) (u32 *gen_status, u32 *ch_status);
} espi_hal_operations_t;


typedef struct
{
	void (*get_espi_core_data) ( int dev_id );
} espi_core_funcs_t;


struct espi_hal
{
	eSPIBuf_T *pespi_buf;
	IPMICmdMsg_T* pipmi_cmd_msg;
	espi_hal_operations_t *pespi_hal_ops;
};


struct espi_dev
{
	struct espi_hal *pespi_hal;
	unsigned char ch_num;
};

#endif

