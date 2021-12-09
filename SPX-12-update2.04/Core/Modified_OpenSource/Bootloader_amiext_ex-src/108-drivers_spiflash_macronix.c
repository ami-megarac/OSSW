--- uboot/drivers/spiflash/macronix.c	2019-04-16 10:52:27.338895181 +0800
+++ uboot.new/drivers/spiflash/macronix.c	2019-04-16 10:02:30.106818000 +0800
@@ -210,8 +210,9 @@
 		spi_generic_read_status(bank, ctrl_drv, &status_reg);
 		if ( !(0x40 & status_reg) )
 		{
+			status_reg |= 0x40;
 			// QE, Quad Enable bit 6 in status register
-			if (spi_generic_write_status(bank, ctrl_drv, 0x40) < 0)
+			if (spi_generic_write_status(bank, ctrl_drv, status_reg) < 0)
 				printk("macronix: Unable to set QE\n");
 		}
 	}
