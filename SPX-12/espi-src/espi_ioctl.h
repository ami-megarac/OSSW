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

struct espi_data_t {
	unsigned char	channel_num;	// Channel number 
	unsigned int	header;			// 
	unsigned int	buf_len;		// number of bytes
	unsigned char	*buffer;
};

#define ESPIIOC_BASE		'P'

#define READ_ESPI_DATA				_IOR(ESPIIOC_BASE, 0x00, struct espi_data_t)
#define READ_ESPI_SLAVE_REGS		_IOW(ESPIIOC_BASE, 0x01, struct espi_data_t)

#endif /* _ESPI_IOCTL_H_ */

