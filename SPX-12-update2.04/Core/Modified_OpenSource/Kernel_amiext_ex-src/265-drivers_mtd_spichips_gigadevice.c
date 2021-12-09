--- linux.old/drivers/mtd/spichips/gigadevice.c	1970-01-01 08:00:00.000000000 +0800
+++ linux/drivers/mtd/spichips/gigadevice.c	2019-10-24 18:15:35.880768651 +0800
@@ -0,0 +1,194 @@
+/*
+ * Copyright (C) 2007 American Megatrends Inc
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
+#ifdef __UBOOT__	
+#include <common.h>
+#endif
+#include "spiflash.h"
+#ifdef	CFG_FLASH_SPI_DRIVER
+
+/* Name, ID1, ID2 , Size, Clock, Erase regions, address mode,{ Offset, Erase Size, Erase Block Count } */
+/* address mode:  0x00 -3 byte address
+			 	0x01 - 4 byte address	
+			 	0x02 - Low byte: 3 byte address, High byte: 4 byte address*/
+static struct spi_flash_info gigadevice_data [] = 
+{
+	{ "GigaDevice GD25Q256D/GD25B256D"   , 0xC8, 0x1940, 0x001F001F, 0x2000000, 50 * 1000000,  1, 0x02, {{ 0, 64  * 1024, 512 },} },
+};
+
+
+static
+int read_status_register_2(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval;
+	u8 reg_data;
+	u8 qe_enable = 0;
+	unsigned char command[5]={0};
+	int cmd_size = 0;
+	
+	command[0] = 0x35; // Read Status Register S15 ~ S8 
+	cmd_size = 1;
+	retval = ctrl_drv->spi_transfer(bank,command, cmd_size ,SPI_READ, &reg_data, 1);
+	if (retval < 0)
+	{
+		printk ("Could not read status register 2\n");
+		return -1;
+	}
+	
+	qe_enable = reg_data; // S9(QE) field
+	
+	return qe_enable;
+}	
+
+static
+int write_status_register_2(int bank,struct spi_ctrl_driver *ctrl_drv, unsigned char status)
+{
+	int retval;
+	u8 code = 0x0;
+
+	/* Issue Write Enable */
+	code = 0x06;
+	retval = ctrl_drv->spi_transfer(bank,&code, 1, SPI_NONE, NULL, 0);
+	if (retval < 0)
+	{
+		printk ("Could not issue write enable\n");
+		return -1;
+	}
+
+	/* Issue Write Status Register 2 */
+	code = 0x31;
+	retval = ctrl_drv->spi_transfer(bank,&code, 1, SPI_WRITE, &status, 1);
+	if (retval < 0)
+	{
+		printk ("Could not read status register 2\n");
+		return -1;
+	}
+
+	return 0;
+}
+
+static
+int 
+gigadevice_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info)
+{
+	int retval;
+	unsigned char status;
+	u8 qe_enable = 0;
+
+	retval =  spi_generic_probe(bank, ctrl_drv,chip_info,"gigadevice",gigadevice_data,ARRAY_SIZE(gigadevice_data));
+	if (retval == -1)
+		return retval;
+
+	if (spi_generic_read_status(bank,ctrl_drv,&status) < 0)
+	{
+		printk("gigadevice: Read SR Failed.Cannot Unprotect all sectors\n");
+		return retval;
+	}
+
+	/* If SRPL = 1 (Bit 7)and WP/ = 0 (Bit 4), then it is hardware locked */
+	if ((status & 0x80) && (!(status & 0x10)))
+	{
+		printk("gigadevice: Hardware Locked\n");
+		return retval;
+	}
+
+	// setup QE bit
+	if ((ctrl_drv->fast_read == 4) && (ctrl_drv->fast_write == 4))
+	{
+		qe_enable = read_status_register_2(bank, ctrl_drv);
+		if (!(qe_enable & 0x2))
+		{
+			qe_enable |= 0x2;
+			if (write_status_register_2(bank, ctrl_drv, qe_enable) < 0)
+				printk("gigadevice: Unable to write status register 2\n");
+		}
+	}
+	else
+	{
+		qe_enable = read_status_register_2(bank, ctrl_drv);
+		if (qe_enable & 0x2)
+		{
+			qe_enable &= (~(1<<1));
+			if (write_status_register_2(bank, ctrl_drv, qe_enable) < 0)
+				printk("gigadevice: Unable to write status register 2\n");
+		}
+	}	
+
+	/* Check if already unprotected */
+	if ((status & 0xC) == 0)
+		return retval;
+
+	/* if SRPL is set, we have to disable SRPL before unprotect */
+	if (status & 0x80)
+	{
+		if (spi_generic_write_status(bank,ctrl_drv,status& 0x7F) < 0)
+		{
+			printk("gigadevice: Clearing SRPL failed .Cannot Unprotect all sectors\n");
+			return retval;
+		}
+	}
+		
+	/* Unprotect all sectors */
+	if (spi_generic_write_status(bank,ctrl_drv,0x0) < 0)
+		printk("gigadevice: Unable to Unprotect all sectors\n");
+
+	return retval;
+}
+
+struct spi_chip_driver gigadevice_driver =
+{
+	.name 		= "gigadevice",
+	.module 	= THIS_MODULE,
+	.probe	 	= gigadevice_probe,
+	.erase_sector 	= spi_generic_erase,
+	.read_bytes	= spi_generic_read,
+	.write_bytes	= spi_generic_write,
+	/* gigadevice supports individual protect and unprotect of sectors */
+	/* if needed implement the functions and add here */
+};
+
+int 
+gigadevice_init(void)
+{
+	sema_init(&gigadevice_driver.lock, 1);
+#ifdef __UBOOT__	/* MIPS */
+	gigadevice_driver.probe	 		= gigadevice_probe;
+	gigadevice_driver.erase_sector 	= spi_generic_erase;
+	gigadevice_driver.read_bytes		= spi_generic_read;
+	gigadevice_driver.write_bytes	= spi_generic_write;
+#endif
+	register_spi_chip_driver(&gigadevice_driver);
+	return 0;
+}
+
+void 
+gigadevice_exit(void)
+{
+	sema_init(&gigadevice_driver.lock, 1);
+	unregister_spi_chip_driver(&gigadevice_driver);
+	return;
+}
+
+module_init(gigadevice_init);
+module_exit(gigadevice_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for gigadevice flash chips");
+
+#endif
