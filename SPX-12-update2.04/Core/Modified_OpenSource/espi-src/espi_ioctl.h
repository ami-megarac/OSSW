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

#ifndef _ESPI_IOCTL_H_
#define _ESPI_IOCTL_H_

#include <linux/socket.h>
#include <linux/tcp.h>

#define PERIPHERAL_CHANNEL_SUPPORTED        (0x1<<0)
#define VWIRE_CHANNEL_SUPPORTED             (0x1<<1)

// AST_ESPI_CHx_CAPCONF
typedef enum
{
    AST_ESPI_CH_CAP_MAX_READ_REQ_SIZE_RESERVED = 0,
    AST_ESPI_CH_CAP_MAX_READ_REQ_SIZE_64_BYTES,
    AST_ESPI_CH_CAP_MAX_READ_REQ_SIZE_128_BYTES,
    AST_ESPI_CH_CAP_MAX_READ_REQ_SIZE_256_BYTES,
    AST_ESPI_CH_CAP_MAX_READ_REQ_SIZE_512_BYTES,
    AST_ESPI_CH_CAP_MAX_READ_REQ_SIZE_1024_BYTES,
    AST_ESPI_CH_CAP_MAX_READ_REQ_SIZE_2048_BYTES,
    AST_ESPI_CH_CAP_MAX_READ_REQ_SIZE_4096_BYTES,
    AST_ESPI_CH_CAP_MAX_READ_REQ_SIZE_EOF
} AST_ESPI_CH_CAP_MAX_READ_REQ_SIZE_T;

typedef enum
{
    AST_ESPI_CH_CAP_MAX_PAYLOAD_SIZE_RESERVED = 0,
    AST_ESPI_CH_CAP_MAX_PAYLOAD_SIZE_64_BYTES,
    AST_ESPI_CH_CAP_MAX_PAYLOAD_SIZE_128_BYTES,
    AST_ESPI_CH_CAP_MAX_PAYLOAD_SIZE_256_BYTES,
    AST_ESPI_CH_CAP_MAX_PAYLOAD_SIZE_EOF
} AST_ESPI_CH_CAP_MAX_PAYLOAD_SIZE_T;

struct espi_data_t {
    unsigned int    header;         //
    unsigned int    buf_len;        // number of bytes
    unsigned char   *buffer;
};

/* IO Command Data Structure */
struct espi_get_channel_stat_t {
    unsigned char   num_channel;
    unsigned char   gen_status;
    unsigned char   ch_status;
    unsigned char   PeripheralChMaxReadReqSize;
    unsigned char   PeripheralChMaxPayloadSizeSelected;
    unsigned char   PeripheralChMaxPayloadSizeSupported;
};

struct espi_get_vw_group_val_t {
    unsigned char   vw_grp_ind;
};

struct espi_set_vw_group_val_t {
    unsigned char   vw_grp_ind;
    unsigned char   vw_grp_val;
};

#define ESPIIOC_BASE        'p'

#define GET_PERI_VW_CHAN_STAT       _IOR(ESPIIOC_BASE, 0x00, struct espi_data_t)
#define GET_VW_HOST2BMC_GROUP_VAL   _IOR(ESPIIOC_BASE, 0x01, struct espi_data_t)
#define SET_VW_BMC2HOST_GROUP_VAL   _IOW(ESPIIOC_BASE, 0x02, struct espi_data_t)

#endif /* _ESPI_IOCTL_H_ */

