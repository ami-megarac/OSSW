--- u-boot-2013.07/drivers/spiflash/issi.c  1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/drivers/spiflash/issi.c       2013-12-05 12:17:55.215503948 -0500 
@@ -0,0 +1,158 @@
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
+#ifdef	CONFIG_FLASH_SPI_DRIVER
+
+/*IOMODE*/
+#define IOMODE_4XIO		5
+#define IOMODE_QUAD 	4
+
+#define	MAX_READY_WAIT_COUNT	4000000
+#define	SR_WIP			0x01	/* Write in progress */
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
+	printk("issi.c %s() : Waiting for Ready Failed\n", __func__);
+	return 1;
+}
+
+
+
+/* Name, ID1, ID2 , Size, Clock, Erase regions, address mode, { Offset, Erase Size, Erase Block Count } */
+/* address mode:  0x00 -3 byte address
+				0x01 - 4 byte address 	
+				0x02 - Low byte: 3 byte address, High byte: 4 byte address*/
+static struct spi_flash_info issi_data [] =
+{
+    	{ " ISSI IS25LP128F",        0x9D, 0x1860, 0x0011001F, 0x1000000, 50 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 256 },} },
+    	{ " ISSI IS25LP256D",        0x9D, 0x1960, 0x0011001F, 0x2000000, 50 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 512 },} },
+    	{ " ISSI IS25WP512M",        0x9D, 0x1A60, 0x0011001F, 0x4000000, 50 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 1024 },} },
+};
+
+static
+int
+issi_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info)
+{
+	int retval;
+	u8 status_reg = 0;
+
+	retval = spi_generic_probe(bank,ctrl_drv,chip_info,"issi",
+						issi_data,ARRAY_SIZE(issi_data));
+
+	if (retval == -1)
+		return retval;
+
+	// To determinate whether or not operate in Quad SPI mode
+	if ((ctrl_drv->fast_read == IOMODE_QUAD) || (ctrl_drv->fast_read == IOMODE_4XIO))
+	{
+		wait_till_ready(bank,ctrl_drv);
+		spi_generic_read_status(bank, ctrl_drv, &status_reg);
+		if ( !(0x40 & status_reg) )// If QE status disable
+		{
+			status_reg = status_reg | 0x40;
+			wait_till_ready(bank,ctrl_drv);
+			// QE, Quad Enable bit 6 in status register
+			if (spi_generic_write_status(bank, ctrl_drv, status_reg) < 0)
+				printk("issi: Unable to set QE enable\n");
+		}
+		wait_till_ready(bank,ctrl_drv);
+	}
+	else
+	{
+		wait_till_ready(bank,ctrl_drv);
+		spi_generic_read_status(bank, ctrl_drv, &status_reg);
+		if ( 0x40 & status_reg )// If QE status enable
+		{
+			status_reg = status_reg & (~0x40);
+			wait_till_ready(bank,ctrl_drv);
+			// QE, Quad Enable bit 6 in status register
+			if (spi_generic_write_status(bank, ctrl_drv, status_reg) < 0)
+				printk("issi: Unable to set QE disable\n");
+		}
+		wait_till_ready(bank,ctrl_drv);
+	}
+
+	return retval;
+}
+
+struct spi_chip_driver issi_driver =
+{
+	.name 		= "issi",
+	.module 	= THIS_MODULE,
+	.probe	 	= issi_probe,
+	.erase_sector 	= spi_generic_erase,
+	.read_bytes	= spi_generic_read,
+	.write_bytes	= spi_generic_write,
+};
+
+
+
+int
+issi_init(void)
+{
+	init_MUTEX(&issi_driver.lock);
+#ifdef __UBOOT__	/* MIPS */
+	issi_driver.probe	 		= issi_probe;
+	issi_driver.erase_sector 	= spi_generic_erase;
+	issi_driver.read_bytes	= spi_generic_read;
+	issi_driver.write_bytes	= spi_generic_write;
+#endif
+	register_spi_chip_driver(&issi_driver);
+	return 0;
+}
+
+
+void
+issi_exit(void)
+{
+	init_MUTEX(&issi_driver.lock);
+	unregister_spi_chip_driver(&issi_driver);
+	return;
+}
+
+
+module_init(issi_init);
+module_exit(issi_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for ISSI flash chips");
+
+#endif
