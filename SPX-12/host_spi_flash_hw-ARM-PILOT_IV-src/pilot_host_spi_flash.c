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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>

#include "driver_hal.h"
#include "host_spi_flash.h"

/* base address of registers */
#define PILOT_HOST_SPI_REG_BASE			0x40429000
#define PILOT_HOST_SPI_REG_SIZE			SZ_256

/* address cycle mode */
#define BYTE_3_ADDR_MODE                0
#define BYTE_4_ADDR_MODE                1

/* commands of SPI flash */
#define SPI_FLASH_CMD_EXIT_4B_ADDRESS_MODE		0xE9    /* Exit 4-byte address mode */
#define SPI_FLASH_CMD_READ_DATA_FAST			0x0B
#define SPI_FLASH_CMD_READ_DATA_FAST_4B_MODE	0x0C    /* Fast Read 4-byte address mode */
#define SPI_FLASH_CMD_BLK_ERASE_64K				0xD8
#define SPI_FLASH_CMD_PAGE_PROGRAM				0x02
#define SPI_FLASH_CMD_PAGE_PROGRAM_4B_MODE		0x12    /* Program 4-byte address mode */

/* offset of registers */
#define PILOT_HOST_SPI_REG_ADDR			0x00
#define PILOT_HOST_SPI_REG_CMD			0x08
#define PILOT_HOST_SPI_REG_CTRL			0x10
#define PILOT_HOST_SPI_REG_CLK			0x14
#define PILOT_HOST_SPI_REG_STS			0x18
#define PILOT_HOST_SPI_REG_MISC			0x1C
#define PILOT_HOST_SPI_REG_FIFO			0x30
#define PILOT_HOST_SPI_REG_MEM_CFG		0x80

/* bits of command register */
#define PILOT_HOST_SPI_REG_CMD_REG_ACCESS		0x80000000 /* bit 31 */
#define PILOT_HOST_SPI_REG_CMD_FAST_READ		0x04000000 /* bit 26 */
#define PILOT_HOST_SPI_REG_CMD_CHIP_SELECT		0x01000000 /* bit 24 */
#define PILOT_HOST_SPI_REG_CMD_DIR				0x00001000 /* bit 12 */
#define PILOT_HOST_SPI_REG_CMD_LEN_MASK			0x00000F00 /* bit[11:8] */
#define PILOT_HOST_SPI_REG_CMD_LEN_SHIFT		8
#define PILOT_HOST_SPI_REG_CMD_OPCODE_MASK		0x000000FF /* bit[7:0] */
#define PILOT_HOST_SPI_REG_CMD_OPCODE_SHIFT		0

/* bits of control register */
#define PILOT_HOST_SPI_REG_CTRL_PRE_FETCH		0x00000010 /* bit 4 */
#define PILOT_HOST_SPI_REG_CTRL_DIRECT_WRITE	0x00000008 /* bit 3 */
#define PILOT_HOST_SPI_REG_CTRL_ENABLE			0x00000001 /* bit 0 */

/* bits of status register */
#define PILOT_HOST_SPI_REG_STS_FIFO_EMPTY		0x00000020 /* bit 5 */
#define PILOT_HOST_SPI_REG_STS_FIFO_FULL		0x00000010 /* bit 4 */
#define PILOT_HOST_SPI_REG_STS_BUSY				0x00000004 /* bit 2 */
#define PILOT_HOST_SPI_REG_STS_IN_PROGRESS		0x00000001 /* bit 0 */

/* bits of host memory configuration register */
#define PILOT_HOST_SPI_REG_CFG_FW_RD_CYCYLE		0x00000004 /* bit 2 */
#define PILOT_HOST_SPI_REG_CFG_HI_ADD			0x00000002 /* bit 1 */
#define PILOT_HOST_SPI_REG_CFG_LO_ADD			0x00000001 /* bit 0 */

/* bits of miscellaneous configuration register */
#define PILOT_HOST_SPI_ADDR_CYCLE_4BYTE			0x01000000 /* bit 24 */

/* Define max times to check status register before we give up. */
#define PILOT_HOST_SPI_FLASH_WAIT_MAX	100000

static void __iomem *pilot_host_spi_flash_virt;
static struct host_spi_flash_core_ops_t *host_spi_flash_core_ops;
static int pilot_host_spi_flash_hal_hw_id;

typedef enum
{
	NORMAL_MODE,
	ENTER_FOURBYTE,
	EXIT_FOURBYTE,
} ADDRESS_MODE;

static int  pilot_host_spi_flash_configure_clock(unsigned int clock)
{
	return 0;
}

static int pilot_host_spi_flash_addr_mode(unsigned int addr_mode)
{
	return 0;
}

static int pilot_host_spi_flash_set_addr_mode(unsigned int addr_mode)
{
	uint32_t reg;

	reg = ioread32(pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_MISC);

	if (addr_mode == BYTE_4_ADDR_MODE) 
	{
		reg |= PILOT_HOST_SPI_ADDR_CYCLE_4BYTE;
	} 
	else
	{ //BYTE_3_ADDR_MODE
		reg &= ~PILOT_HOST_SPI_ADDR_CYCLE_4BYTE;
	}

	iowrite32(reg, pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_MISC);	
	
	return 0;
}

static inline int pilot_host_spi_flash_determine_4bmode(unsigned long opcode) {

	if ((opcode == SPI_FLASH_CMD_READ_DATA_FAST_4B_MODE) || (opcode == SPI_FLASH_CMD_PAGE_PROGRAM_4B_MODE))
		return ENTER_FOURBYTE;
	else if (opcode == SPI_FLASH_CMD_EXIT_4B_ADDRESS_MODE)
		return EXIT_FOURBYTE;

	return NORMAL_MODE;
}

static void pilot_host_spi_flash_wait_idle(void)
{
	uint32_t reg;
	unsigned int count;

	count = 0;
	do {
		reg = ioread32(pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_STS);
		count ++;
		if (count >= PILOT_HOST_SPI_FLASH_WAIT_MAX) {
			printk("host spi flash wait idle timeout\n");
			break;
		}
	} while (reg & PILOT_HOST_SPI_REG_STS_IN_PROGRESS);
}

static void pilot_host_spi_flash_wait_empty(void)
{
	uint32_t reg;
	unsigned int count;

	count = 0;
	do {
		reg = ioread32(pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_STS);
		count ++;
		if (count >= PILOT_HOST_SPI_FLASH_WAIT_MAX) {
			printk("host spi flash wait empty timeout\n");
			break;
		}
	} while (reg & PILOT_HOST_SPI_REG_STS_FIFO_EMPTY);
}

static void pilot_host_spi_flash_wait_full(void)
{
	uint32_t reg;
	unsigned int count;

	count = 0;
	do {
		reg = ioread32(pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_STS);
		count ++;
		if (count >= PILOT_HOST_SPI_FLASH_WAIT_MAX) {
			printk("host spi flash wait full timeout\n");
			break;
		}
	} while (reg & PILOT_HOST_SPI_REG_STS_FIFO_FULL);
}

static void pilot_host_spi_flash_enable(int enable)
{
	uint32_t reg;

	reg = ioread32(pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_CTRL);
	if (enable) {
		reg |= PILOT_HOST_SPI_REG_CTRL_ENABLE;
	} else {
		reg &= ~PILOT_HOST_SPI_REG_CTRL_ENABLE;
	}
	iowrite32(reg, pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_CTRL);
}

static int pilot_host_spi_flash_transfer(unsigned char *cmd, unsigned int cmd_len, unsigned char *data, unsigned long data_len, int data_dir)
{
	uint32_t reg;
	int i;
	int AddrMode = pilot_host_spi_flash_determine_4bmode(cmd[0]);
	
	if (AddrMode == ENTER_FOURBYTE)
		pilot_host_spi_flash_set_addr_mode(BYTE_4_ADDR_MODE);
	else if(AddrMode == EXIT_FOURBYTE)
		pilot_host_spi_flash_set_addr_mode(BYTE_3_ADDR_MODE);
		
	/* address */
	if (cmd_len >= 4) 
	{
		if(cmd_len == 4) 
		{
			reg = (cmd[1] << 16) | (cmd[2] << 8) | cmd[3];
		}
		else
		{
            reg = (cmd[1] << 24) | (cmd[2] << 16) | (cmd[3] <<8) | cmd[4];
		}

		iowrite32(reg, pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_ADDR);
	}

	reg = ioread32(pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_CMD);
	reg &= ~PILOT_HOST_SPI_REG_CMD_REG_ACCESS;

	/* command code */
	reg &= ~PILOT_HOST_SPI_REG_CMD_OPCODE_MASK;
	reg |= cmd[0] << PILOT_HOST_SPI_REG_CMD_OPCODE_SHIFT;

	/* command length */
	reg &= ~PILOT_HOST_SPI_REG_CMD_LEN_MASK;
	reg |= cmd_len << PILOT_HOST_SPI_REG_CMD_LEN_SHIFT;

	/* direction */
	reg |= PILOT_HOST_SPI_REG_CMD_DIR;
	if ((data_dir == 0) && (data_len != 0))
		reg &= ~PILOT_HOST_SPI_REG_CMD_DIR;

	/* prepare command */
	pilot_host_spi_flash_wait_idle();
	iowrite32(reg, pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_CMD);

	wmb();

	/* issue command, switch to register access mode */
	reg |= PILOT_HOST_SPI_REG_CMD_REG_ACCESS;
	pilot_host_spi_flash_wait_idle();
	iowrite32(reg, pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_CMD);

	wmb();

	pilot_host_spi_flash_wait_idle();
	if (data_dir == 1) { /* write */
		for (i = 0; i < data_len; i ++) {
			pilot_host_spi_flash_wait_full();
			iowrite8(data[i], pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_FIFO);
		}
	} else { /* read */
		for (i = 0; i < data_len; i ++) {
			pilot_host_spi_flash_wait_empty();
			data[i] = ioread8(pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_FIFO);
		}
	}

	wmb();

	/* switch back to non-register mode */
	pilot_host_spi_flash_wait_idle();
	reg &= ~PILOT_HOST_SPI_REG_CMD_REG_ACCESS;
	iowrite32(reg, pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_CMD);

	if ((cmd[0] == SPI_FLASH_CMD_READ_DATA_FAST_4B_MODE) || (cmd[0] == SPI_FLASH_CMD_PAGE_PROGRAM_4B_MODE))
		pilot_host_spi_flash_set_addr_mode(BYTE_3_ADDR_MODE);
	
	return 0;
}

static int pilot_host_spi_flash_max_read(void)
{
	return (64 * 1024 * 1024);
}

struct host_spi_flash_hal_ops_t pilot_host_spi_flash_ops = {
	.max_read = pilot_host_spi_flash_max_read,
	.transfer = pilot_host_spi_flash_transfer,
	.burst_read = pilot_host_spi_flash_transfer,
	.configure_clock = pilot_host_spi_flash_configure_clock,
	.set_addr_mode = pilot_host_spi_flash_addr_mode,
};

hw_hal_t pilot_host_spi_flash_hw_hal = {
	.dev_type = EDEV_TYPE_HOST_SPI_FLASH,
	.owner = THIS_MODULE,
	.devname = "pilot_host_spi_flash",
	.num_instances = 1,
	.phal_ops = (void *) &pilot_host_spi_flash_ops
};

static void pilot_host_spi_flash_change_mode(int master_mode)
{
	uint32_t reg;

	reg = ioread32(pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_MEM_CFG);
	if (master_mode)
		reg &= ~(PILOT_HOST_SPI_REG_CFG_FW_RD_CYCYLE);
	else /* bypass */
		reg |= PILOT_HOST_SPI_REG_CFG_FW_RD_CYCYLE;
	iowrite32(reg, pilot_host_spi_flash_virt + PILOT_HOST_SPI_REG_MEM_CFG);
}

int __init pilot_host_spi_flash_init(void)
{
	int rc;

	if (!request_mem_region(PILOT_HOST_SPI_REG_BASE, PILOT_HOST_SPI_REG_SIZE, "pilot_host_spi_flash")) {
		return -EBUSY;
	}

	pilot_host_spi_flash_virt = ioremap(PILOT_HOST_SPI_REG_BASE, PILOT_HOST_SPI_REG_SIZE);
	if (!pilot_host_spi_flash_virt) {
		rc = -ENOMEM;
		goto out_release_mem_region;
	}

	pilot_host_spi_flash_enable(0);
	pilot_host_spi_flash_enable(1);
	pilot_host_spi_flash_change_mode(1);

	pilot_host_spi_flash_hal_hw_id = register_hw_hal_module(&pilot_host_spi_flash_hw_hal, (void **) &host_spi_flash_core_ops);
	if (pilot_host_spi_flash_hal_hw_id < 0) {
		pilot_host_spi_flash_change_mode(0);
		goto out_iounmap;
	}

	return 0;

out_iounmap:
	iounmap(pilot_host_spi_flash_virt);
out_release_mem_region:
	release_mem_region(PILOT_HOST_SPI_REG_BASE, PILOT_HOST_SPI_REG_SIZE);

	return rc;
}

void __exit pilot_host_spi_flash_exit(void)
{
	unregister_hw_hal_module(EDEV_TYPE_HOST_SPI_FLASH, pilot_host_spi_flash_hal_hw_id);

	pilot_host_spi_flash_change_mode(0);

	if (pilot_host_spi_flash_virt) {
		iounmap(pilot_host_spi_flash_virt);
		release_mem_region(PILOT_HOST_SPI_REG_BASE, PILOT_HOST_SPI_REG_SIZE);
	}
}

module_init(pilot_host_spi_flash_init);
module_exit(pilot_host_spi_flash_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("Hardware module of host SPI flash driver for Pilot-IV SoC");
MODULE_LICENSE("GPL");
