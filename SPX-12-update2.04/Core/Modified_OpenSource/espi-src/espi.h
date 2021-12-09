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

typedef struct 
{
    unsigned char (*num_espi_ch) (void);
    void (*reset_espi) (void);
    void (*set_vw_bmc2host_group_val) (u8 ind, u8 val);
    void (*get_vw_host2bmc_group_val) (u8 ind, u8 *buf);
    void (*get_channel_status) (u32 *gen_status, u32 *ch_status, u32 *PeripheralChMaxReadReqSize, u32 *PeripheralChMaxPayloadSizeSelected, u32 *PeripheralChMaxPayloadSizeSupported );
} espi_hal_operations_t;


typedef struct
{
    void (*get_espi_core_data) ( int dev_id );
} espi_core_funcs_t;


struct espi_hal
{
    espi_hal_operations_t *pespi_hal_ops;
};


struct espi_dev
{
    struct espi_hal *pespi_hal;
    unsigned char ch_num;
};

#endif

