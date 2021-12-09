diff -Naur linux_org/drivers/mtd/spichips/generic.c linux_new/drivers/mtd/spichips/generic.c
--- linux_org/drivers/mtd/spichips/generic.c	1969-12-31 19:00:00.000000000 -0500
+++ linux_new/drivers/mtd/spichips/generic.c	2016-10-03 17:38:15.420980429 -0400
@@ -0,0 +1,1591 @@
+/*
+ * Copyright (C) 2007-2013 American Megatrends Inc
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation; either version 2 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
+ */
+
+
+#ifdef __UBOOT__
+#include <common.h>
+#endif
+#include "spiflash.h"
+#ifdef	CFG_FLASH_SPI_DRIVER
+
+/* Flash opcodes. */
+#define	OPCODE_WREN		0x06	/* Write enable */
+#define	OPCODE_WRDI		0x04	/* Write disable*/
+#define	OPCODE_RDID		0x9F	/* Read JEDEC ID */
+#define	OPCODE_RDSR		0x05	/* Read status register */
+#define OPCODE_WRSR		0x01	/* Write status register */
+#define	OPCODE_READ		0x03	/* Read data bytes */
+#define	OPCODE_FAST_READ	0x0B	/* Read Fast read */
+#define	OPCODE_DREAD	0x3B	/* Dual Read Mode */
+#define	OPCODE_2READ	0xBB	/* 2 x I/O Read Mode */
+#define	OPCODE_QREAD	0x6B	/* Quad Read Mode */
+#define	OPCODE_QWRITE	0x32	/* Quad Write Mode */
+#define	OPCODE_4READ	0xEB	/* 4 x I/O Read Mode */
+#define	OPCODE_4WRITE	0x38	/* 4 x I/O Write Mode */
+#define	OPCODE_PP		0x02	/* Page program */
+#define	OPCODE_SE		0xD8	/* Sector erase */
+#define OPCODE_DP		0xB9	/* Deep Power Down */
+#define	OPCODE_RES		0xAB	/* Read Electronic Signature */
+
+/* Status Register bits. */
+#define	SR_WIP			0x01	/* Write in progress */
+#define	SR_WEL			0x02	/* Write enable latch */
+#define	SR_BP0			0x04	/* Block protect 0 */
+#define	SR_BP1			0x08	/* Block protect 1 */
+#define	SR_BP2			0x10	/* Block protect 2 */
+#define	SR_SRWD			0x80	/* SR write protect */
+
+#define PROGRAM_PAGE_SIZE	256	/* Max Program Size */
+
+#define ADDR_16MB 		0x1000000
+#define CMD_MX25XX_EN4B		0xb7	/* Enter 4-byte address mode */
+#define CMD_MX25XX_EX4B		0xe9	/* Exit 4-byte address mode */
+#define CMD_ISSI_EX4B		0x29	/* Exit 4-byte address mode */
+
+
+#define ADDRESS_3BYTE	0x00
+#define ADDRESS_4BYTE	0x01
+#define ADDRESS_LO3_HI4_BYTE 0x02
+
+#define ADDRESS_DIE_LO3_HI4_BYTE 0x06
+#define ADDR_32MB 		0x2000000
+#define CMD_WX25XX_CS		0xc2	/* Die select */
+
+extern unsigned long ractrends_spiflash_flash_id[MAX_SPI_BANKS];
+extern unsigned char ractrends_spiflash_address_mode [MAX_SPI_BANKS];
+extern unsigned char total_active_spi_banks;
+
+static int wait_till_ready(int bank,struct spi_ctrl_driver *ctrl_drv);
+int spansion_sector_handler(int protect_state, int bank, loff_t addr_ori, size_t bytes, struct spi_ctrl_driver *ctrl_drv);
+int spi_spansion_extended_address(int bank, int enable, struct spi_ctrl_driver *ctrl_drv);
+
+p_soc_spi_transfer_t p_soc_spi_transfer = NULL;
+
+/* Define max times to check status register before we give up. */
+#define	MAX_READY_WAIT_COUNT	4000000
+
+int spansion_erase_size = 0;
+
+static
+int inline
+spi_error(int retval)
+{
+	printk("SPI Chip %s (%d) : Error (%d)\n",__FILE__,__LINE__,retval);
+	return retval;
+}
+
+static int
+spi_generic_read_flag_status(int bank, struct spi_ctrl_driver *ctrl_drv,unsigned char *status)
+{
+	int  retval;
+	u8 code = 0x70;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,status, 1);
+
+	if (retval < 0)
+		return spi_error(retval);
+
+	return 0;
+}
+
+
+
+int
+spi_generic_read_status(int bank, struct spi_ctrl_driver *ctrl_drv,unsigned char *status)
+{
+	int  retval;
+	u8 code = OPCODE_RDSR;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,status, 1);
+
+	if (retval < 0)
+		return spi_error(retval);
+
+	return 0;
+}
+
+int
+spi_generic_write_status(int bank,struct spi_ctrl_driver *ctrl_drv, unsigned char status)
+{
+	int retval;
+	u8 code = OPCODE_WRSR;
+
+	/* Send write enable */
+	spi_generic_write_enable(bank,ctrl_drv);
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_WRITE,&status, 1);
+	if (retval < 0) 
+		return spi_error(retval);
+
+	return 0;
+}
+
+
+int
+spi_generic_write_enable(int bank,struct spi_ctrl_driver *ctrl_drv)
+{
+	u8 code = OPCODE_WREN;
+	int retval;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_NONE, NULL, 0);
+	if (retval < 0)
+		return spi_error(retval);
+	return 0;
+}
+
+int
+spi_generic_write_disable(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	u8 code = OPCODE_WRDI;
+	int retval;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_NONE, NULL, 0);
+	if (retval < 0)
+		return spi_error(retval);
+	return 0;
+}
+
+int spi_generic_select_die(int bank, u8 die_num, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval;
+	u8 command[2];
+
+	command[0] = CMD_WX25XX_CS;
+	command[1] = die_num;
+
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		return -1;
+	}
+
+	retval = ctrl_drv->spi_transfer(bank, command, 2, SPI_NONE, NULL, 0);
+
+	if (retval < 0)
+	{
+		printk ("Could not select die.\n");
+		return spi_error(retval);
+	}
+	return 0;
+}
+
+int enter_4byte_addr_mode(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	//enable 32 MB Address mode
+	u8 code = CMD_MX25XX_EN4B;
+	int retval;
+
+	//printf("<ENTER> 4 BYTE\n");
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		return -1;
+	}
+
+
+	/* Issue Controller Transfer Routine */
+	if ((ractrends_spiflash_flash_id[bank] == 0x002019BA) || (ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA))
+		spi_generic_write_enable(bank,ctrl_drv);
+	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0);
+	if ((ractrends_spiflash_flash_id[bank] == 0x002019BA) || (ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA)) 
+		spi_generic_write_disable(bank,ctrl_drv);
+	if (retval < 0)
+	{
+		printk ("Could not Enter into 4-byte address mode\n");
+		return spi_error(retval);
+	}
+	return 0;
+}
+
+/*
+
+ Default the SPI device to 3 byte, if the SPI device supports 'both' 3 and 4 byte mode.
+
+ This function does the exactly what "exit_4byte_addr_mode" does, but without the need of 
+ Linux core SPI private structures and variables  ie) without  'spi_ctrl_driver'
+ or 'map_info' or 'mtd_info'
+ 
+ restore_spidevice_to_default_state will be called during SOC reset or wdt expiration 
+ or during kernel panic.    
+
+*/
+
+int
+restore_spidevice_to_default_state(int bank)
+{
+
+	unsigned char status;
+	unsigned char spi_addr_mode;
+	unsigned char code = 0;
+	int retval=0;
+	unsigned long  count;
+
+	// p_soc_spi_transfer would be initialised in the SOC specific calls. 
+	// For piot soc it would point to 'pilot3spi_transfer' function 
+	if (p_soc_spi_transfer == NULL) {
+		printk("restore_spidevice_to_default_state :  p_soc_spi_transfer is NULL \n");
+		return 1;
+	}
+
+	//for (bank=0; bank < total_active_spi_banks; bank++)
+	//{
+
+		spi_addr_mode = ractrends_spiflash_address_mode [bank];
+
+		printk("restore_spidevice_to_default_state: bank =%d total_active_spi_banks=%d spi_addr_mode=%d \n", bank, total_active_spi_banks, spi_addr_mode);
+
+
+		// Default to 3 byte address mode only if the Boot SPI is supports 3 and 4 byte mode . 
+		// We don`t have to do anything for Boot SPI that supports 'only' 3 byte or 'only' 4 byte mode 
+		if (spi_addr_mode != ADDRESS_LO3_HI4_BYTE) 
+		{
+			//continue;
+			return 1;
+		}
+
+		/* wait_till_ready */
+		code = OPCODE_RDSR;
+
+		for (count = 0; count < MAX_READY_WAIT_COUNT; count++)
+		{
+			// read spi_generic_read_status 
+			if (p_soc_spi_transfer(bank, &code, 1, SPI_READ, &status, 1) < 0)
+			{
+				printk("Error reading SPI Status Register\n");
+				//break;
+				return 1;
+			}
+			else
+			{
+				if (!(status & SR_WIP)) 
+				{
+					break;				
+				}  
+			}
+		}
+
+
+		/* spi_generic_write_enable */
+		if ((ractrends_spiflash_flash_id[bank] == 0x002019BA) || (ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA))
+		{
+
+			printk("restore_spidevice_to_default_state: spi_generic_write_enable \n");
+
+			code = OPCODE_WREN;
+
+			retval = p_soc_spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0);
+			if (retval < 0) 
+			{
+				printk("Error spi_generic_write_enable on bank %d \n", bank);
+				//spi_error(retval);
+				return spi_error(retval);
+			}
+			else 
+			{
+				/* Default the SPI to 3 byte mode */
+				code = CMD_MX25XX_EX4B;
+				p_soc_spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0);
+				if (retval < 0)
+				{ 
+					printk("Error Default the SPI to 3 byte mode on bank %d  \n", bank);
+					spi_error(retval);
+				}
+
+				printk("restore_spidevice_to_default_state: spi_generic_write_disable \n");
+				/*  spi_generic_write_disable . Issue Write Disable Cmd even if above command (default to 3 byte ie) CMD_MX25XX_EX4B) fails or succeeds */
+				if ((ractrends_spiflash_flash_id[bank] == 0x002019BA) || (ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA))
+				{
+					code = OPCODE_WRDI;
+
+					retval = p_soc_spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0);
+					if (retval < 0) 
+					{
+						printk("Error spi_generic_write_disable on bank %d  \n", bank);
+						//spi_error(retval);
+						return spi_error(retval);
+					}
+				}
+			}
+		}
+	//}	
+
+	return 0;
+}
+
+
+int exit_4byte_addr_mode(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	//Disable 32 MB Address mode
+	u8 code = CMD_MX25XX_EX4B;
+	int retval;
+
+	//printf("<EXIT> 4 BYTE\n");
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		return -1;
+	}
+
+	/* Exit 4 byte addresing for issi chip */
+	if(ractrends_spiflash_flash_id[bank] >> 16 == 0x9D)
+	code = CMD_ISSI_EX4B;
+
+	/* Issue Controller Transfer Routine */
+	if ((ractrends_spiflash_flash_id[bank] == 0x002019BA) || (ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA)) 
+		spi_generic_write_enable(bank,ctrl_drv);
+	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0);
+	if ((ractrends_spiflash_flash_id[bank] == 0x002019BA) || (ractrends_spiflash_flash_id[bank] == 0x002020BA) || (ractrends_spiflash_flash_id[bank] == 0x002021BA)) 
+		spi_generic_write_disable(bank,ctrl_drv);
+	if (retval < 0)
+	{
+		printk ("Could not Exit from 4-byte address mode\n");
+		return spi_error(retval);
+	}
+	return 0;
+}
+#if 0
+int spi_generic_extended_address(int bank, SPI_DIR dir, u8 addr, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval;
+
+	if (dir == SPI_READ)
+	{
+		u8 code = 0xC8;
+		u8 reg_data;
+
+		ctrl_drv->spi_transfer(bank, &code, 1, SPI_READ, &reg_data, 1);
+		retval = (int) reg_data;
+	}
+	else if (dir == SPI_WRITE)
+	{
+		u8 command[2];
+
+		command[0] = 0xC5;
+		command[1] = addr;
+		spi_generic_write_enable(bank, ctrl_drv);
+		ctrl_drv->spi_transfer(bank, command, 2, SPI_NONE, NULL, 0);
+		spi_generic_write_disable(bank, ctrl_drv);
+		retval = command[1];
+	}
+	else // SPI_NONE
+	{
+		retval = 0;
+	}
+
+	return retval;
+}
+#endif
+// the function just for WINBOND W25Q256 only, always revise the extended address to the defalut
+int w25q256_force_extended_address(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval;
+	u8 code;
+	u8 reg_data;
+	u8 command[5];
+
+	code = 0xC8; // "Read Extended Address Register"
+	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_READ, &reg_data, 1);
+	if (reg_data == 0x01)
+	{
+		spi_generic_write_enable(bank,ctrl_drv);
+		command[0] = 0xC5; // "Write Extended Address Register" with the force address 0x00
+		command[1] = command[2] = command[3] = command[4] = 0x00;
+		retval = ctrl_drv->spi_transfer(bank, command, 5, SPI_NONE, NULL, 0);
+		spi_generic_write_disable(bank,ctrl_drv);
+	}
+	return 0;
+}
+
+
+
+static int
+wait_till_ready(int bank,struct spi_ctrl_driver *ctrl_drv)
+{
+	unsigned long  count;
+	unsigned char sr;
+
+	for (count = 0; count < MAX_READY_WAIT_COUNT; count++)
+	{
+		if (spi_generic_read_status(bank,ctrl_drv,&sr) < 0)
+		{
+			printk("Error reading SPI Status Register\n");
+			break;
+		}
+		else
+		{
+			if (!(sr & SR_WIP))
+				return 0;
+		}
+	}
+
+	printk("spi_generic: Waiting for Ready Failed\n");
+	return 1;
+}
+
+static int
+require_read_flag_status(int bank,struct spi_ctrl_driver *ctrl_drv)
+{
+	unsigned long count;
+	unsigned char sr;
+
+	for (count = 0; count < MAX_READY_WAIT_COUNT; count++)
+	{
+		if (spi_generic_read_flag_status(bank,ctrl_drv,&sr) < 0)
+		{
+			printk("Error reading SPI Status Register\n");
+			break;
+		}
+		else
+		{
+			if (sr & SR_SRWD)
+				return 0;
+		}
+	}
+
+	printk("spi_generic %s() : Waiting for Ready Failed\n", __func__);
+	return 1;
+}
+
+
+
+int
+spi_generic_erase(struct map_info *map, unsigned long sect_addr)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+	int bank = map->map_priv_1;
+	struct spi_ctrl_driver *ctrl_drv = priv->ctrl_drv;	
+	int retval;
+	unsigned char command[5];
+	int cmd_size;	
+	u8 address32 = priv->address32;
+	//unsigned long flash_size = priv->size;
+	u8 had_switch_die = 0;
+	int iomode_write = priv->iomode_write;
+	
+	/* For Spansion SPI  Flash*/
+	unsigned long addr_ori = sect_addr;
+ 	int protect_state ;
+ 	size_t bytes = spansion_erase_size;
+
+	down(&priv->chip_drv->lock);
+	
+
+ 	/* Spantion SPI unprotect sector */
+	if (ractrends_spiflash_flash_id[bank] == 0x00011902)
+	{
+		protect_state = 0;//spansion_sector_handler do unprotect sector
+		if (spansion_sector_handler(protect_state, bank, addr_ori, bytes, priv->ctrl_drv) != 0)
+		{
+			printk("spansion unprotect sector unsuccessful\n");
+			return -1;	
+		}
+	}
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		up(&priv->chip_drv->lock);
+		return -1;
+	}
+
+	if (address32 == ADDRESS_DIE_LO3_HI4_BYTE) {
+		if(sect_addr>=ADDR_32MB){
+			spi_generic_select_die( bank, 1,ctrl_drv);
+			had_switch_die = 1;
+			sect_addr-=ADDR_32MB;
+		}
+	}
+
+	/* Logic for 4 byte address mode Enter */
+	if ((iomode_write == IOMODE_4XIO) || (iomode_write == IOMODE_QUAD) || ( ((sect_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE )||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE)))
+	{
+		retval = enter_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to enter 4 byte address mode\n");
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank]  == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+
+	if ((iomode_write == IOMODE_4XIO) || (iomode_write == IOMODE_QUAD) || (((sect_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE)))
+	{
+		/* Set up command buffer. */
+		command[0] = OPCODE_SE;
+		if (ractrends_spiflash_flash_id[bank] == 0x00011902) command[0] = 0xDC; // ERASE command in 4byte mode [spansion only]
+		if (address32 == ADDRESS_DIE_LO3_HI4_BYTE) command[0] = 0xDC; // ERASE command in 4byte mode
+		command[1] = sect_addr >> 24;
+		command[2] = sect_addr >> 16;
+		command[3] = sect_addr >> 8;
+		command[4] = sect_addr;
+
+		cmd_size = 5;
+	}
+	else {
+		/* Set up command buffer. */
+		command[0] = OPCODE_SE;
+		command[1] = sect_addr >> 16;
+		command[2] = sect_addr >> 8;
+		command[3] = sect_addr;
+
+		cmd_size = 4;
+	}
+
+	/* Issue Controller Transfer Routine */
+	spi_generic_write_enable(bank,ctrl_drv); /* Send write enable */
+	retval = ctrl_drv->spi_transfer(bank,command, cmd_size ,SPI_NONE, NULL, 0);
+	spi_generic_write_disable(bank,ctrl_drv); /* Send write disable */
+
+	if (ractrends_spiflash_flash_id[bank] == 0x002020BA || ractrends_spiflash_flash_id[bank] == 0x002021BA)
+	{
+		/* requires the read flag status with at latest one byte. */
+		if (require_read_flag_status(bank,ctrl_drv))
+		{
+			up(&priv->chip_drv->lock);
+			return -1;
+		}
+	}
+
+	if (retval < 0)
+	{
+		//if 4 byte mode exit
+		if ((iomode_write == IOMODE_4XIO) || (iomode_write == IOMODE_QUAD) || ( ((sect_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE)))
+		{
+			retval = exit_4byte_addr_mode(bank, ctrl_drv);
+			if (retval < 0)
+			{
+				printk ("Unable to exit 4 byte address mode\n");
+			}
+		}
+
+		if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+
+		if(had_switch_die == 1)
+		{
+			spi_generic_select_die( bank, 0,ctrl_drv);
+		}
+		up(&priv->chip_drv->lock);
+		return spi_error(retval);
+	}
+
+	if ((iomode_write == IOMODE_4XIO) || (iomode_write == IOMODE_QUAD) || ( ((sect_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE)))
+	{
+		retval = exit_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to exit 4 byte address mode\n");
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+
+	if(had_switch_die == 1)
+	{
+		spi_generic_select_die( bank, 0,ctrl_drv);
+	}
+	up(&priv->chip_drv->lock);
+
+	/* Spantion SPI protect sector */
+	if (ractrends_spiflash_flash_id[bank] == 0x00011902)
+	{	
+		protect_state = 1;//spansion_sector_handler do protect sector
+		if (spansion_sector_handler(protect_state, bank, addr_ori, bytes, priv->ctrl_drv) != 0)
+		{
+			printk("spansion protect sector unsuccessful\n");
+			return -1;	
+		}
+	}
+	return retval;
+}
+
+
+int
+spi_generic_read(struct map_info *map, loff_t addr, size_t bytes, unsigned char *buff)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+	int bank = map->map_priv_1;
+	struct spi_ctrl_driver *ctrl_drv = priv->ctrl_drv;	
+	int retval = 0;
+	size_t transfer;
+	unsigned char command[8];
+	int cmd_size;
+	int  (*readfn)(int bank,unsigned char *,int , SPI_DIR, unsigned char *, unsigned long); //unsigned long);
+	int end_addr = (addr+bytes-1);	
+	u8 address32 = priv->address32;
+	//unsigned long flash_size = priv->size;
+	u8 had_switch_die = 0;
+	int iomode_read = priv->iomode_read;
+	
+	/* Some time zero bytes length are sent */
+	if (bytes==0)
+		return 0;
+
+	if (address32 == ADDRESS_DIE_LO3_HI4_BYTE)
+	{ 
+		if (addr < ADDR_32MB && end_addr >= ADDR_32MB)
+		{
+			int ErrorCode;
+			transfer = (ADDR_32MB - addr);
+			ErrorCode = spi_generic_read(map, addr, transfer, buff);
+			if (ErrorCode != 0) return ErrorCode;
+			 
+			 //fix address
+			bytes-=transfer;
+			addr+=transfer;
+			buff+=transfer;
+			
+			end_addr = (addr+bytes-1);
+			if (bytes==0) return 0;
+		}
+	}
+	down(&priv->chip_drv->lock);
+	
+	
+	
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		up(&priv->chip_drv->lock);
+		return -1;
+	}
+
+	if (address32 == ADDRESS_DIE_LO3_HI4_BYTE){
+		if(addr >= ADDR_32MB){
+			spi_generic_select_die( bank, 1,ctrl_drv);
+			had_switch_die = 1;
+			addr-=ADDR_32MB;
+			end_addr = (addr+bytes-1);
+		}
+	}
+	
+	if (ctrl_drv->spi_burst_read)
+		readfn = ctrl_drv->spi_burst_read;
+	else
+		readfn = ctrl_drv->spi_transfer;
+
+	transfer=bytes;
+
+
+	/* Logic for 4 byte address mode Enter */
+	if ((iomode_read == IOMODE_4XIO) || (iomode_read == IOMODE_QUAD) || ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE )||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE)))
+	{
+		//printk ("Trying to enter 4 byte mode\n");
+		retval = enter_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to enter 4 byte address mode\n");
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+
+	while (bytes)
+	{
+		if (ctrl_drv->spi_burst_read)
+			transfer=bytes;
+		else
+		{
+			if ((iomode_read == IOMODE_QUAD) || (iomode_read == IOMODE_4XIO))
+				transfer = bytes;
+			else 
+			{
+				transfer=ctrl_drv->max_read;
+				if (transfer > bytes)
+					transfer = bytes;
+			}
+		}
+
+		switch (iomode_read)
+		{
+			case IOMODE_NORMAL:
+			if ((( end_addr  >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{
+				if (ractrends_spiflash_flash_id[bank] == 0x00011902) 
+				{
+					/*Spasion  enable extended address*/
+					int enable = 1;
+					if (spi_spansion_extended_address(bank, enable, priv->ctrl_drv) != 0)
+					{
+						printk("spansion access extended address unsuccessful\n");
+						return -1;	
+					}	
+		
+
+				}
+				/* Set up command buffer. */	/* Normal Read */
+				command[0] = OPCODE_READ;
+				if (ractrends_spiflash_flash_id[bank] == 0x00011902) command[0] = 0x13; // READ command in 4byte mode [spansion only]
+				if (address32 == ADDRESS_DIE_LO3_HI4_BYTE) command[0] = 0x13; // READ command in 4byte mode
+				command[1] = addr >> 24;
+				command[2] = addr >> 16;
+				command[3] = addr >> 8;
+				command[4] = addr;
+
+				cmd_size = 5;
+			}
+			else {
+
+				/* Set up command buffer. */	/* Normal Read */
+				command[0] = OPCODE_READ;
+				command[1] = addr >> 16;
+				command[2] = addr >> 8;
+				command[3] = addr;
+
+				cmd_size = 4;
+			}
+			/* Issue Controller Transfer Routine */
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+			break;
+			
+			case IOMODE_FAST: // Need to check Fast Read in 4 byte address mode
+			if ((( end_addr  >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{ 
+				if (ractrends_spiflash_flash_id[bank] == 0x00011902) 
+				{
+					/*Spasion  enable extended address*/
+					int enable = 1;
+					if (spi_spansion_extended_address(bank, enable, priv->ctrl_drv) != 0)
+					{
+						printk("spansion access extended address unsuccessful\n");
+						return -1;	
+					}	
+		
+
+				}
+				/* Set up command buffer. */   /* Fast Read */
+				command[0] = OPCODE_FAST_READ;
+				if (ractrends_spiflash_flash_id[bank] == 0x00011902) command[0] = 0x0C; // FAST_READ command in 4byte mode [spansion only]
+				if (address32 == ADDRESS_DIE_LO3_HI4_BYTE) command[0] = 0x0C; // FAST_READ command in 4byte mode
+				command[1] = addr >> 24;
+				command[2] = addr >> 16;
+				command[3] = addr >> 8;
+				command[4] = addr;
+				command[5] = 0;			/* dummy data */
+
+				cmd_size = 6;
+			}
+			else
+			{
+				/* Set up command buffer. */   /* Fast Read */
+				command[0] = OPCODE_FAST_READ;
+				command[1] = addr >> 16;
+				command[2] = addr >> 8;
+				command[3] = addr;
+				command[4] = 0;			/* dummy data */
+
+				cmd_size = 5;
+			}
+			/* Issue Controller Transfer Routine */
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+			break;
+			
+			case IOMODE_DUAL: // Need to check Dual Read in 4 byte address mode
+			if ((( end_addr  >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{ 
+				if (ractrends_spiflash_flash_id[bank] == 0x00011902) 
+				{
+					/*Spasion  enable extended address*/
+					int enable = 1;
+					if (spi_spansion_extended_address(bank, enable, priv->ctrl_drv) != 0)
+					{
+						printk("spansion access extended address unsuccessful\n");
+						return -1;	
+					}	
+		
+
+				}
+				/* Set up command buffer. */   /* Dual Read */
+				command[0] = OPCODE_DREAD;
+				command[1] = addr >> 24;
+				command[2] = addr >> 16;
+				command[3] = addr >> 8;
+				command[4] = addr;
+				command[5] = 0;			/* dummy data */
+
+				cmd_size = 6;
+			}
+			else
+			{ 
+				/* Set up command buffer. */   /* Dual Read */
+				command[0] = OPCODE_DREAD;
+				command[1] = addr >> 16;
+				command[2] = addr >> 8;
+				command[3] = addr;
+				command[4] = 0;			/* dummy data */
+
+				cmd_size = 5;
+			}
+			/* Issue Controller Transfer Routine */
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+			break;
+			
+			case IOMODE_2XIO: // Need to check 2xI/O Read in 4 byte address mode 
+			if ((( end_addr  >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{ 
+				if (ractrends_spiflash_flash_id[bank] == 0x00011902) 
+				{
+					/*Spasion  enable extended address*/
+					int enable = 1;
+					if (spi_spansion_extended_address(bank, enable, priv->ctrl_drv) != 0)
+					{
+						printk("spansion access extended address unsuccessful\n");
+						return -1;	
+					}	
+		
+
+				}
+				/* Set up command buffer. */   /* 2xI/O Read */
+				command[0] = OPCODE_2READ;
+				command[1] = addr >> 24;
+				command[2] = addr >> 16;
+				command[3] = addr >> 8;
+				command[4] = addr;
+				command[5] = 0;			/* dummy data */
+				if (ractrends_spiflash_flash_id[bank]  >> 16 == 0x20) command[6] = 0;	/* dummy data for Micron*/
+
+				cmd_size = 6;
+				if (ractrends_spiflash_flash_id[bank]  >> 16 == 0x20) cmd_size = 7;		/* cmd_size for Micron*/
+			}
+			else
+			{ 
+				/* Set up command buffer. */   /* 2xI/O Read */
+				command[0] = OPCODE_2READ;
+				command[1] = addr >> 16;
+				command[2] = addr >> 8;
+				command[3] = addr;
+				command[4] = 0;			/* dummy data */
+				if (ractrends_spiflash_flash_id[bank]  >> 16 == 0x20) command[5] = 0;	/* dummy data for Micron*/
+
+				cmd_size = 5;
+				if (ractrends_spiflash_flash_id[bank]  >> 16 == 0x20) cmd_size = 6;			/* cmd_size data for Micron*/
+			}
+			/* Issue Controller Transfer Routine */
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+			break;
+			
+			case IOMODE_QUAD:
+			/* Set up command buffer. */   /* Quad Read */
+			command[0] = OPCODE_QREAD;
+			command[1] = addr >> 24;
+			command[2] = addr >> 16;
+			command[3] = addr >> 8;
+			command[4] = addr;
+			command[5] = 0;		/* dummy data */
+			cmd_size = 6;
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+			break;
+
+			case IOMODE_4XIO:
+			/* Set up command buffer. */   /* 4xI/O Read */
+			command[0] = OPCODE_4READ;
+			command[1] = addr >> 24;
+			command[2] = addr >> 16;
+			command[3] = addr >> 8;
+			command[4] = addr;
+			command[5] = 0;		/* dummy data */
+			command[6] = 0;		/* dummy data */
+			command[7] = 0;		/* dummy data */
+			cmd_size = 8;
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+			break;
+			
+			default: 
+			break;
+		}
+
+		if (retval < 0)
+		{
+			//if 4 byte mode, exit
+			if ((iomode_read == IOMODE_4XIO) || (iomode_read == IOMODE_QUAD) || ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE)))
+			{
+				retval = exit_4byte_addr_mode(bank, ctrl_drv);
+				if (retval < 0)
+				{
+					printk ("Unable to exit 4 byte address mode\n");
+				}
+			}
+
+			if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+
+		bytes-=transfer;
+		addr+=transfer;
+		buff+=transfer;
+	}
+
+	//if 4 byte mode exit
+	if ((iomode_read == IOMODE_4XIO) || (iomode_read == IOMODE_QUAD) || ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE)))
+	{
+		//printk ("Trying to exit 4 byte mode\n");
+		retval = exit_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to exit 4 byte address mode\n");
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank]  == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+
+	if(had_switch_die == 1)
+	{
+		spi_generic_select_die( bank, 0,ctrl_drv);
+	}
+	up(&priv->chip_drv->lock);
+
+	/*Spasion  disable extended address*/
+	if (ractrends_spiflash_flash_id[bank] == 0x00011902) 
+	{
+		int enable = 0 ;
+		if (spi_spansion_extended_address(bank, enable, priv->ctrl_drv) != 0)
+		{
+			printk("spansion access extended address unsuccessful\n");
+			return -1;	
+		}	
+	}
+	return 0;
+}
+
+
+int
+spi_generic_write(struct map_info *map, loff_t addr, size_t bytes, const unsigned char *buff)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+	int bank = map->map_priv_1;
+	struct spi_ctrl_driver *ctrl_drv = priv->ctrl_drv;
+	
+	int retval = 0;
+	unsigned char command[5]={0};
+	size_t transfer;
+	int cmd_size = 0;
+	int end_addr = (addr+bytes-1);
+	u8 address32 = priv->address32;
+	//unsigned long flash_size = priv->size;
+	u8 had_switch_die = 0;
+	int iomode_write = priv->iomode_write;
+	/* For Spantion SPI  */
+	unsigned long addr_ori = addr;
+    	int protect_state ;
+    	size_t ori_bytes = bytes;
+
+	/* Spansion SPI unprotect sector */
+	if (ractrends_spiflash_flash_id[bank] == 0x00011902)
+	{
+		protect_state = 0;//spansion_sector_handler do unprotect sector
+		if (spansion_sector_handler(protect_state, bank, addr_ori, ori_bytes, priv->ctrl_drv) != 0)
+		{
+			printk("spansion unprotect sector unsuccessful\n");
+			return -1;	
+		}
+			
+	}
+	
+	/* Some time zero bytes length are sent */
+	if (bytes==0)
+		return 0;
+
+	if (address32 == ADDRESS_DIE_LO3_HI4_BYTE)
+	{ 
+		if (addr < ADDR_32MB && end_addr >= ADDR_32MB)
+		{
+			int ErrorCode;
+			transfer = (ADDR_32MB - addr);
+			ErrorCode = spi_generic_write(map, addr, transfer, buff);
+			if (ErrorCode != 0) return ErrorCode;
+			 
+			 //fix address
+			bytes-=transfer;
+			addr+=transfer;
+			buff+=transfer;
+			
+			end_addr = (addr+bytes-1);
+			if (bytes==0) return 0;
+		}
+	}
+
+	down(&priv->chip_drv->lock);
+	
+	if (address32 == ADDRESS_DIE_LO3_HI4_BYTE){
+		if(addr >= ADDR_32MB){
+			spi_generic_select_die( bank, 1,ctrl_drv);
+			had_switch_die = 1;
+			addr-=ADDR_32MB;
+			end_addr = (addr+bytes-1);
+		}
+	}
+	
+	/* Logic for 4 byte address mode Enter */
+	if ((iomode_write == IOMODE_4XIO) || (iomode_write == IOMODE_QUAD) || ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE)))
+	{
+		retval = enter_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to enter 4 byte address mode\n");
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank]  == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+
+	while (bytes)
+	{
+		/* Wait until finished previous command. */
+		if (wait_till_ready(bank,ctrl_drv))
+		{
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return -1;
+		}
+
+		transfer = PROGRAM_PAGE_SIZE;
+		if (bytes <  transfer)
+			transfer = bytes;
+
+		switch (iomode_write)
+		{
+			case IOMODE_NORMAL:
+            if (((end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE))
+			{
+				/* Set up command buffer. */
+				command[0] = OPCODE_PP;
+				if (ractrends_spiflash_flash_id[bank]  == 0x00011902) command[0] = 0x12; // PROGRAM command in 4byte mode [spansion only]
+				if (address32 == ADDRESS_DIE_LO3_HI4_BYTE) command[0] = 0x12; // PROGRAM command in 4byte mode
+				command[1] = addr >> 24;
+				command[2] = addr >> 16;
+				command[3] = addr >> 8;
+				command[4] = addr;
+				cmd_size = 5;
+			}
+			else {
+				/* Set up command buffer. */
+				command[0] = OPCODE_PP;
+				command[1] = addr >> 16;
+				command[2] = addr >> 8;
+				command[3] = addr;
+				cmd_size = 4;
+			}
+			break;
+			
+			case IOMODE_QUAD:
+			/* Set up command buffer. */
+			command[0] = OPCODE_QWRITE;
+			command[1] = addr >> 24;
+			command[2] = addr >> 16;
+			command[3] = addr >> 8;
+			command[4] = addr;
+			cmd_size = 5;
+			break;
+
+			case IOMODE_4XIO:
+			/* Set up command buffer. */
+			command[0] = OPCODE_4WRITE;
+			command[1] = addr >> 24;
+			command[2] = addr >> 16;
+			command[3] = addr >> 8;
+			command[4] = addr;
+			cmd_size = 5;
+			break;
+			
+			default:
+			break;
+		}
+
+		/* Issue Controller Transfer Routine */
+		spi_generic_write_enable(bank,ctrl_drv); /* Send write enable */
+		retval = ctrl_drv->spi_transfer(bank,command,cmd_size ,SPI_WRITE,
+						(unsigned char *)buff, transfer);
+		spi_generic_write_disable(bank,ctrl_drv); /* Send write disable */
+
+		if (ractrends_spiflash_flash_id[bank]  == 0x002020BA || ractrends_spiflash_flash_id[bank]  == 0x002021BA)
+		{
+			/* requires the read flag status with at latest one byte. */
+			if (require_read_flag_status(bank,ctrl_drv))
+			{
+				up(&priv->chip_drv->lock);
+				return -1;
+			}
+		}
+
+		if (retval < 0)
+		{
+			//if 4 byte mode exit
+			if ((iomode_write == IOMODE_4XIO) || (iomode_write == IOMODE_QUAD) || ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE)))
+			{
+				retval = exit_4byte_addr_mode(bank, ctrl_drv);
+				if (retval < 0)
+				{
+					printk ("Unable to exit 4 byte address mode\n");
+				}
+			}
+
+			if (ractrends_spiflash_flash_id[bank]  == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+
+			if(had_switch_die == 1)
+			{
+				spi_generic_select_die( bank, 0,ctrl_drv);
+			}
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+		addr+=(transfer-retval);
+		buff+=(transfer-retval);
+		bytes-=(transfer-retval);
+	}
+
+	//if 4 byte mode exit
+	if ((iomode_write == IOMODE_4XIO) || (iomode_write == IOMODE_QUAD) || ( (( end_addr >= ADDR_16MB) && ((address32 == ADDRESS_LO3_HI4_BYTE)||(address32 == ADDRESS_DIE_LO3_HI4_BYTE))) || (address32 == ADDRESS_4BYTE)))
+	{
+		retval = exit_4byte_addr_mode(bank, ctrl_drv);
+		if (retval < 0)
+		{
+			printk ("Unable to exit 4 byte address mode\n");
+		}
+	}
+
+	if (ractrends_spiflash_flash_id[bank]  == 0x00EF1940) w25q256_force_extended_address(bank, ctrl_drv);
+
+	if(had_switch_die == 1)
+	{
+		spi_generic_select_die( bank, 0,ctrl_drv);
+	}
+	up(&priv->chip_drv->lock);
+
+	/* Spantion SPI protect sector */
+	if (ractrends_spiflash_flash_id[bank] == 0x00011902)
+	{
+		protect_state = 1;//spansion_sector_handler do protect sector
+		if (spansion_sector_handler(protect_state, bank, addr_ori, ori_bytes, priv->ctrl_drv) != 0)
+		{
+			printk("spansion protect sector unsuccessful\n");
+			return -1;	
+		}
+	}
+	return 0;
+}
+
+int 
+spansion_sector_handler(int protect_state,int bank, loff_t addr_ori, size_t bytes, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval = 0;
+	int spi_program_buffer_size = 256;
+	int unprotect_size = 0;
+	unsigned long sect_addr_for = 0;
+    	unsigned char command_sec[6];
+    	unprotect_size = bytes;
+
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		return -1;
+	}
+	
+		
+		for(sect_addr_for = addr_ori; sect_addr_for<addr_ori+unprotect_size; sect_addr_for+=spi_program_buffer_size)
+		{
+    			unsigned char dyb, dyb_c;//debug
+    			command_sec[0] = 0xe0;
+    			command_sec[1] = sect_addr_for >> 24;
+    			command_sec[2] = sect_addr_for >> 16;
+    			command_sec[3] = sect_addr_for >> 8;
+    			command_sec[4] = sect_addr_for;
+
+	    		ctrl_drv->spi_transfer(bank,command_sec, 5,SPI_READ,&dyb, 1);
+	    		if(protect_state == 0)//protect_state = 0 = Need  Unprotect System
+	    		{
+	    			if(0xff == dyb) continue;	
+	    			command_sec[5] = 0xff;
+	    		}
+	    		else if(protect_state == 1)//protect_state = 0 = Need  Protect System
+	    		{
+	    			if(0x0== dyb) continue;
+	    			command_sec[5] = 0x0;		
+	    		}
+    			
+	    
+    			command_sec[0] = 0xe1;
+			spi_generic_write_enable(bank,ctrl_drv); /* Send write enable */
+			retval = ctrl_drv->spi_transfer(bank,command_sec, 6 ,SPI_NONE, NULL, 0);
+			if(retval < 0)
+			return -1;
+			spi_generic_write_disable(bank,ctrl_drv); /* Send write disable */
+
+    			command_sec[0] = 0xe0;
+    			ctrl_drv->spi_transfer(bank,command_sec, 5,SPI_READ,&dyb_c, 1);
+		}
+	return 0;
+}
+int
+spi_spansion_extended_address(int bank, int enable, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval = 0;
+	u8 brwr_code = 0x17;
+	u8 extadd = 0;
+	if(enable == 1)
+	{
+		extadd = 0x80;
+	}else{
+		extadd = 0x0;
+	}
+	retval = ctrl_drv->spi_transfer(bank,&brwr_code, 1,SPI_WRITE, &extadd, 1);//enable extended address
+	if(retval < 0)
+		return -1;
+	return 0;
+}
+
+/***********************************************************************************/
+extern int spi_verbose;
+int
+spi_generic_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info,
+			char *spi_name,struct spi_flash_info *spi_list, int spi_list_len)
+{
+	int  retval;
+	u32 val;
+	int i;
+	u16 opread;
+	u16 opwrite;
+	u8 code = OPCODE_RDID;
+	//int address_mode = 0;
+#if defined(CONFIG_SOC_AST2400)
+	/*Varible for check HW isn't it support Quad IO function */
+	unsigned char command[6] = {0};
+	int cmd_size = 0;
+	unsigned char env_addr_val[0xff]={0};
+	int env_addr_num =  0;
+	u32 env_addr = 0;
+	u8 status_reg = 0;
+	static int check_one_time = 1;
+	static int hw_unsupport_quad = 0;
+#endif
+	
+	if (spi_verbose == 2)
+		printk("SPI: probing for %s devices ...\n",spi_name);
+
+	/* Send write enable */
+	retval =spi_generic_write_enable(bank,ctrl_drv);
+	if (retval < 0)
+	 	return -1;
+	
+	/* Issue Controller Transfer Routine */
+	val = 0;
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,(unsigned char *)&val, 3);
+	val &= 0x00FFFFFF;
+
+	if (retval < 0)
+	{
+		spi_error(retval);
+		return -1;
+	}
+
+	/* Send write disable */
+	retval = spi_generic_write_disable(bank,ctrl_drv);
+	if (retval < 0)
+		return -1;
+
+	/* Match the ID against the table entries */
+	for (i = 0; i < spi_list_len; i++)
+	{
+		if ((spi_list[i].mfr_id == ((val)& 0xFF)) && (spi_list[i].dev_id == ((val >> 8)& 0xFFFF)))
+		{
+			/* Check Operation Mode */
+			//for Read Operation
+			opread = (spi_list[i].operationmode & 0xFFFF);
+			opread &= ctrl_drv->operation_mode_mask;
+			if (0x1F < opread)
+			{
+				ctrl_drv->fast_read = IOMODE_4XIO;
+			}
+			else if ((0xF < opread) && (opread  <= 0x1F))
+			{
+				ctrl_drv->fast_read = IOMODE_QUAD;
+			}
+			else if ((0x7 < opread) && (opread  <= 0xF))
+			{
+				ctrl_drv->fast_read = IOMODE_2XIO;
+			}
+			else if ((0x3 < opread) && (opread  <= 0x7))
+			{
+				ctrl_drv->fast_read = IOMODE_DUAL;
+			}
+			else if ((0x1 < opread) && (opread <= 0x3))
+			{
+				ctrl_drv->fast_read = IOMODE_FAST;
+			}
+			else if (opread <= 0x1)
+			{
+				ctrl_drv->fast_read = IOMODE_NORMAL;
+			}
+			
+			//for Write Operation
+			opwrite = (spi_list[i].operationmode >> 16);
+			opwrite &= (ctrl_drv->operation_mode_mask >> 16);
+			if (0x1F < opwrite)
+			{
+				ctrl_drv->fast_write = IOMODE_4XIO;
+			}
+			else if ((0xF < opwrite) && (opwrite <= 0x1F))
+			{
+				ctrl_drv->fast_write = IOMODE_QUAD;
+			}
+			else if (opwrite <= 0x1)
+			{
+				ctrl_drv->fast_write = IOMODE_NORMAL;
+			}
+			
+		  break;
+		}
+	}
+
+	if (i == spi_list_len)
+	{
+//		if (spi_verbose == 2)
+//			printk("%s : Unrecognized ID (0x%x) got \n",spi_name,val);
+		return -1;
+	}
+	memcpy(chip_info,&spi_list[i],sizeof(struct spi_flash_info));
+
+	/*When SPI Flash mfr_id is Spansion,  then get spansion erase block size*/
+	if(spi_list[i].mfr_id ==  0x01)
+	{
+		spansion_erase_size =  spi_list[i].regions[0].erasesize;
+	}
+
+#if defined(CONFIG_SOC_AST2400)	
+	/*Check HW Circuit isn't it supported Quad  IO Mode*/
+	if((ctrl_drv->fast_read == IOMODE_QUAD) || (ctrl_drv->fast_read == IOMODE_4XIO) || (ctrl_drv->fast_write == IOMODE_QUAD) || (ctrl_drv->fast_write == IOMODE_4XIO) )
+	{
+		if(check_one_time == 1)
+		{
+
+			env_addr = CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_START;
+			env_addr &= 0x0FFFFFFF;// get environment address
+
+			/*QE bit enable*/ 
+			if(spi_list[i].mfr_id == 0xEF)//Winbond QE bit enable
+			{
+				int retval;
+				u8 code = 0x35; // Read Status Register 2 Command 
+				
+				/* Read Status Register 2*/
+				retval = ctrl_drv->spi_transfer(bank, &code, 1,SPI_READ, &status_reg, 1);
+				if (retval < 0)
+					printk ("Could not read status register 2\n");
+
+				if (!(0x02 & status_reg))//if QE bit not enable, enable QE bit
+				{
+					code = 0x31;// Write Status Register 2 Command
+					status_reg |= 0x02;//enable QE bit
+
+					/* Write Status Register 2*/
+					spi_generic_write_enable(bank,ctrl_drv);//Send write enable
+					retval = ctrl_drv->spi_transfer(bank, &code, 1,SPI_WRITE,&status_reg, 1);
+					if (retval < 0)
+						printk ("Could not write status register 2\n");
+				}
+			}
+
+			if(spi_list[i].mfr_id == 0xC2 || spi_list[i].mfr_id == 0x1C)//Macronix QE bit enable
+			{
+				spi_generic_read_status(bank, ctrl_drv, &status_reg);
+				if ( !(0x40 & status_reg) )
+				{
+					status_reg |= 0x40;
+					// QE, Quad Enable bit 6 in status register
+					if (spi_generic_write_status(bank, ctrl_drv, status_reg ) < 0)
+						printk("macronix: Unable to set QE\n");
+				}
+			}
+			
+			enter_4byte_addr_mode(bank, ctrl_drv);
+
+			command[0] = OPCODE_QREAD;
+			command[1] = env_addr >> 24;
+			command[2] = env_addr >> 16;
+			command[3] = env_addr >> 8;
+			command[4] = env_addr;
+			command[5] = 0;
+			cmd_size = 6;
+
+			ctrl_drv->spi_transfer(0, command, cmd_size,SPI_READ,(unsigned char *)env_addr_val, 4);//get env_addr_val
+			
+			/*Check is get right env_addr_val value or  wrong env_addr_val value */
+			/*****************************************************************
+			if get env_addr_val had value over 3 = get right value,  HW Support Quad IO.
+			if get env_addr_val all value smaller than 3 = get wrong value,  HW UnSupport Quad IO.
+			******************************************************************/ 
+			for(env_addr_num = 0; env_addr_num < 255; env_addr_num++)
+			{
+
+				if((env_addr_val[env_addr_num]&0x0f) > 0x03 || ((env_addr_val[env_addr_num]>>4)&0x0f) > 0x03)
+				{
+					hw_unsupport_quad = 0;
+					break;//break for loop 
+				}
+				else if(env_addr_num == 0xfe)
+				{
+					printk("Hardware UnSupported Quad IO Mode, So IO Mode Used Fast Read & Normal Write\n");
+					hw_unsupport_quad = 1;
+				}
+			}
+
+			exit_4byte_addr_mode(bank, ctrl_drv);
+			
+			if(hw_unsupport_quad == 1)
+			{
+				//QE bit disable
+				if(spi_list[i].mfr_id == 0xEF)//Winbond QE bit disable
+				{
+					int retval;
+					u8 code = 0x35; // Read Status Register 2 Command 
+				
+					/* Read Status Register 2*/
+					retval = ctrl_drv->spi_transfer(bank, &code, 1,SPI_READ, &status_reg, 1);
+					if (retval < 0)
+						printk ("Could not read status register 2\n");
+
+					code = 0x31;// Write Status Register 2 Command
+					status_reg &= (~(0x02));//disable QE bit
+
+					/* Write Status Register 2*/
+					spi_generic_write_enable(bank,ctrl_drv);//Send write enable
+					retval = ctrl_drv->spi_transfer(bank, &code, 1,SPI_WRITE,&status_reg, 1);
+					if (retval < 0)
+						printk ("Could not write status register 2\n");
+
+				}
+			
+				if(spi_list[i].mfr_id == 0xC2 || spi_list[i].mfr_id == 0x1C)//Macronix QE bit disable
+				{
+					spi_generic_read_status(bank, ctrl_drv, &status_reg);
+					status_reg &= (~(0x40));
+
+					if (spi_generic_write_status(bank, ctrl_drv, status_reg) < 0)// QE, Quad Disable bit 6 in status register
+						printk("macronix: Unable to Clear QE\n");
+
+				}
+			}
+		}
+
+		check_one_time = 0;
+	}
+
+	/*If  Unsupported Quad IO Mode Do Following Behavior*/
+	if(hw_unsupport_quad == 1)
+	{
+		ctrl_drv->fast_read = IOMODE_FAST;
+		ctrl_drv->fast_write = IOMODE_NORMAL;
+	}
+#endif
+
+	if( (spi_list[i].mfr_id == 0xEF) && (spi_list[i].dev_id == 0x2140) )
+	{
+		ctrl_drv->fast_read = IOMODE_FAST;
+		ctrl_drv->fast_write = IOMODE_NORMAL;
+	}	
+
+	if (spi_verbose > 0)
+	{
+		printk(KERN_INFO"Found SPI Chip %s(0x%0x) ", spi_list[i].name,spi_list[i].dev_id);
+		switch (ctrl_drv->fast_read) 
+		{
+			case IOMODE_NORMAL:
+				printk("NORMAL READ, ");
+			break;
+			case IOMODE_FAST:
+				printk("FAST READ, ");
+			break;
+			case IOMODE_DUAL:
+				printk("DUAL READ, ");
+			break;
+			case IOMODE_2XIO:
+				printk("2x I/O READ, ");
+			break;
+			case IOMODE_QUAD:
+				printk("QUAD READ, ");
+			break;
+			case IOMODE_4XIO:
+				printk("4x I/O READ, ");
+			break;
+			default:
+			break;
+		}
+		switch (ctrl_drv->fast_write) 
+		{
+			case IOMODE_NORMAL:
+				printk("NORMAL WRITE\n");
+			break;
+			case IOMODE_QUAD:
+				printk("QUAD WRITE\n");
+			break;
+			case IOMODE_4XIO:
+				printk("4x I/O WRITE\n");
+			break;
+			default:
+				printk("\n");
+			break;
+		}
+	}
+
+	return 0;
+
+}
+
+EXPORT_SYMBOL(spi_generic_probe);
+EXPORT_SYMBOL(spi_generic_erase);
+EXPORT_SYMBOL(spi_generic_read);
+EXPORT_SYMBOL(spi_generic_write);
+EXPORT_SYMBOL(spi_generic_write_disable);
+EXPORT_SYMBOL(spi_generic_write_enable);
+EXPORT_SYMBOL(spi_generic_read_status);
+EXPORT_SYMBOL(spi_generic_write_status);
+EXPORT_SYMBOL(restore_spidevice_to_default_state);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for Generic SPI flash chips");
+
+#endif
