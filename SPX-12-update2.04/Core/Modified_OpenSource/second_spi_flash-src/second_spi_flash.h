/*****************************************************************
 *****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 *****************************************************************
 *****************************************************************/

#ifndef __SECOND_SPI_FLASH_H__
#define __SECOND_SPI_FLASH_H__

#include <linux/version.h>

#define SET_SECOND_SPI_ACCESS     0
#define RESET_SPI               6

struct second_spi_flash_hal_ops_t {
	int (*max_read)(void); /* maximum data size to be used to read type functions in spi_transfer */
	int (*transfer)(unsigned char *cmd, unsigned int cmd_len, unsigned char *data, unsigned long data_len, int data_dir);
	int (*burst_read)(unsigned char *cmd, unsigned int cmd_len, unsigned char *data, unsigned long  data_len, int data_dir);
	int (*configure_clock)(unsigned int clock);
    int (*set_addr_mode)(unsigned int addr_mode);
    void (*second_spi_flash_change_mode)(int mode);
    int max_clock;
#if defined(SOC_AST2500)
    void (*set_cs_range)(int spi_size);
#endif
};

struct second_spi_flash_hal_t {
	struct second_spi_flash_hal_ops_t *ops;
	struct mtd_info *mtd;
	struct mtd_partition partitions;
};

struct second_spi_flash_dev
{
	struct second_spi_flash_hal_t *psecond_spi_flash_hal;
};
struct second_spi_flash_core_ops_t {
};

#endif /* __SECOND_SPI_FLASH_H__ */
