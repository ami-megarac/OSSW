--- linux/drivers/mtd/spichips/macronix.c	2019-04-16 10:54:19.882898063 +0800
+++ linux.new/drivers/mtd/spichips/macronix.c	2019-04-16 10:12:39.866834000 +0800
@@ -142,6 +142,7 @@
 	int address_mode = 0;
 	int i=0;
 	u8 rsfdpAdd32 = 0;
+	unsigned char status_reg ;
 	retval = spi_generic_probe(bank,ctrl_drv,chip_info,"macronix",
 						macronix_data,ARRAY_SIZE(macronix_data));	
 
@@ -202,9 +203,14 @@
 	}
 	if ((ctrl_drv->fast_read == 5) && (ctrl_drv->fast_write == 5))
 	{
-		// QE, Quad Enable bit 6 in status register
-		if (spi_generic_write_status(bank, ctrl_drv, 0x40) < 0)
-			printk("macronix: Unable to set QE\n");
+		spi_generic_read_status(bank, ctrl_drv, &status_reg);
+		if ( !(0x40 & status_reg) )
+		{
+			status_reg |= 0x40;
+			// QE, Quad Enable bit 6 in status register
+			if (spi_generic_write_status(bank, ctrl_drv, status_reg) < 0)
+				printk("macronix: Unable to set QE\n");
+		}
 	}
 	else 
 	{
