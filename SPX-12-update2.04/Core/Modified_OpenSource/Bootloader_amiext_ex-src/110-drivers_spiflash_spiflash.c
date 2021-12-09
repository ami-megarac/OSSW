--- uboot.old/drivers/spiflash/spiflash.c	2019-10-24 17:46:47.756768651 +0800
+++ uboot/drivers/spiflash/spiflash.c	2019-10-24 18:07:18.772768651 +0800
@@ -231,6 +231,7 @@
 extern int micron_init(void);
 extern int issi_init(void);
 extern int default_init(void);
+extern int gigadevice_init(void);
 #ifdef POST_SPI_INIT
 extern void post_spi_init(int bank, struct spi_flash_info *spi_info);
 #endif
@@ -282,7 +283,8 @@
     	s33_init();
     	micron_init();
     	issi_init();
-
+        gigadevice_init();
+        
         chip_drvs_are_init = 1;
     }
 
