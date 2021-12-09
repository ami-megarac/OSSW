--- linux/drivers/mtd/spichips/macronix.c	2019-03-21 17:14:12.388126100 +0800
+++ linux.new/drivers/mtd/spichips/macronix.c	2019-03-25 20:19:59.917513090 +0800
@@ -37,7 +37,9 @@
 #define ADDRESS_LO3_HI4_BYTE 0x02
 #define MX25L25x35E_MFR_ID 0xC2 
 #define MX25L25x35E_DEV_ID 0x1920
-
+#define MX25L51245G_MX66L51235F_MFR_ID 0xC2    //Macronix MX66L51235F MX25L51245G
+#define MX25L51245G_MX66L51235F_DEV_ID 0x1A20  //Macronix MX66L51235F MX25L51245G
+#define CMD_RDSFDP  0x5A
 
 /* Name, ID1, ID2 , Size, Clock, Erase regions, address mode,{ Offset, Erase Size, Erase Block Count } */
 /* address mode:  0x00 -3 byte address
@@ -52,12 +54,29 @@
 	{ "Macronix MX25L12805D", 0xC2, 0x1820, 0x00010001, 0x1000000, 50 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 256 },} },
    	{ "Macronix MX25L25635E", 0xC2, 0x1920, 0x002F002F, 0x2000000, 50 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 512 },} },
     	{ "Macronix MX25L25735E", 0xC2, 0x1920, 0x002F002F, 0x2000000, 50 * 1000000, 1, 0x01, {{ 0, 64  * 1024, 512 },} },      
-      { "Macronix MX66L51235F", 0xC2, 0x1A20, 0x002F002F, 0x4000000, 50 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 1024 },} },      
+      { "Macronix MX66L51235F", 0xC2, 0x1A20, 0x002F002F, 0x4000000, 50 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 1024 },} },
+	{ "Macronix MX25L51245G", 0xC2, 0x1A20, 0x002F002F, 0x4000000, 66 * 1000000, 1, 0x02, { {0, 64 * 1024, 1024},}},      
       { "Macronix MX66L1G45G", 0xC2, 0x1B20, 0x002F002F, 0x8000000, 50 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 2048 },} },
     	{ "EON EN25QH256",        0x1C, 0x1970, 0x0001000F, 0x2000000, 50 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 512 },} },      
 
 };
-
+static
+int read_rdsfdp_addr32(int bank, struct spi_ctrl_driver *ctrl_drv , u8 *addressByteNumber)
+{
+	u8 code[5] = {0};
+	int retval;
+	unsigned char rdsfdp={0};
+	
+	code[0] = CMD_RDSFDP;
+	code[1] = 0x0;
+	code[2] = 0x0;
+	code[3] = 0x32;
+	code[4] = 0x0;
+	/* Issue Controller Transfer Routine*/ 
+	retval = ctrl_drv->spi_transfer(bank,&code[0], 5,SPI_READ,&rdsfdp, 1);
+	*addressByteNumber = rdsfdp;
+	return retval;
+}
 /* to dinstinguish between MX25L25635/MX25L25735 E and F type */
 static int read_security_register(int bank, struct spi_ctrl_driver *ctrl_drv)
 {
@@ -122,7 +141,7 @@
 	int retval;
 	int address_mode = 0;
 	int i=0;
-	
+	u8 rsfdpAdd32 = 0;
 	retval = spi_generic_probe(bank,ctrl_drv,chip_info,"macronix",
 						macronix_data,ARRAY_SIZE(macronix_data));	
 
@@ -158,7 +177,29 @@
 		}
 	
 	}
+	if((chip_info->mfr_id == MX25L51245G_MX66L51235F_MFR_ID) && (chip_info->dev_id == MX25L51245G_MX66L51235F_DEV_ID))
+	{
+		if( read_rdsfdp_addr32(bank, ctrl_drv,&rsfdpAdd32) > -1 )
+		{
+			for (i = 0; i < (ARRAY_SIZE(macronix_data)); i++)
+			{
+				if((macronix_data[i].mfr_id == MX25L51245G_MX66L51235F_MFR_ID) && (macronix_data[i].dev_id == MX25L51245G_MX66L51235F_DEV_ID))
+				{
 
+					if ((strncmp(macronix_data[i].name, "Macronix MX66L51235F", sizeof("Macronix MX66L51235F")) == 0) && (rsfdpAdd32 == 0xF3))
+					{
+						memcpy(chip_info,&macronix_data[i],sizeof(struct spi_flash_info));
+						break;
+					}
+					if ((strncmp(macronix_data[i].name, "Macronix MX25L51245G", sizeof("Macronix MX25L51245G")) == 0) && (rsfdpAdd32 == 0xFB))
+					{
+						memcpy(chip_info,&macronix_data[i],sizeof(struct spi_flash_info));
+						break;
+					}
+				}
+			}
+		}
+	}
 	if ((ctrl_drv->fast_read == 5) && (ctrl_drv->fast_write == 5))
 	{
 		// QE, Quad Enable bit 6 in status register
