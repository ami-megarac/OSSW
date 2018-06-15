--- u-boot-2013.07/include/configs/orion.h	1969-12-31 19:00:00.000000000 -0500
+++ mywork/include/configs/orion.h	2015-03-24 06:17:58.404534828 -0400
@@ -0,0 +1,90 @@
+/*-------------------------------------------------------------------------------------------------------------------*/
+/* ----------------------------------------------ORION Configuration -------------------------------------------*/
+/*-------------------------------------------------------------------------------------------------------------------*/
+#ifndef __ORION_H__
+#define __ORION_H__
+
+/*---------------------------------------------------------------
+ *      Board and Processor Identification
+ *---------------------------------------------------------------*/
+#define CONFIG_PILOT4   1 /* Pilot4 SOC */
+
+#include <configs/board.cfg>
+#include <configs/pilot.cfg>
+#include <configs/common.cfg>
+
+/* File Systems to be suported */
+#define CONFIG_FS_FAT   1
+#define CONFIG_CMD_FAT    1
+#define CONFIG_CMD_EXT2   1
+#define CONFIG_CMD_EXT4   1
+#define CONFIG_FS_EXT2    1
+#define CONFIG_FS_EXT4    1
+#define CONFIG_FS_GENERIC 1
+
+/*----------------------------------------------------------------
+ *        Override Defaults
+ *---------------------------------------------------------------*/
+#undef DEBUG
+#undef CONFIG_SYS_ALT_MEMTEST
+
+
+/*---------------------------------------------------------------
+ *      Serial Port Configuration
+ *---------------------------------------------------------------*/
+#define   CONFIG_SERIAL2  1/* Use Serial Port 2 as output on HORNET*/ 
+
+/*----------------------------------------------------------------
+ *        Default Memory Test Range
+ *---------------------------------------------------------------*/
+#define CONFIG_SYS_MEMTEST_START  0x80A00000
+#define CONFIG_SYS_MEMTEST_END    0x841FFFFF
+
+/*----------------------------------------------------------------
+ *           Timer Clock Frequency
+ *---------------------------------------------------------------*/
+#define UART_CLOCK         1846100   /* UART CLOCK in  HORNET*/
+#define SYSTEM_CLOCK       200000000 /*System CPU Clock - 200 MHz*/
+#define SYS_CLK            SYSTEM_CLOCK
+#define AHB_CLK            (SYS_CLK/2)
+#define P_CLK              25000000
+//#define CONFIG_SYS_HZ    P_CLK /* 25MHZ = 1sec commented to avoid rollover problems */
+#define CONFIG_SYS_HZ      1000 /* 1KHZ = 1sec to avoid roll over problems */
+
+/*----------------------------------------------------------------
+ *        Network Configuration
+ *---------------------------------------------------------------*/
+#define CONFIG_PHY_GIGE       1 /* Include GbE speed/duplex detection */
+#define CONFIG_PHY_CMD_DELAY  1
+#ifdef CONFIG_PILOT4
+#define CONFIG_PHY_ADDR0      0x2    //On Orion board, MAC0 corresponds to PHY ADDR 0x2
+#define CONFIG_PHY_ADDR1      0x1    //On Orion board, MAC1 corresponds to PHY ADDR 0x1
+#else
+#define CONFIG_PHY_ADDR0      0x1
+#define CONFIG_PHY_ADDR1      0x2 
+#endif
+/*-----------------------------------------------------------------
+ *             Physical Memory Map of SDRAM and Flash
+ *----------------------------------------------------------------*/
+#define CONFIG_BIGPHYSAREA     "4096"
+#define CONFIG_SYS_LOAD_ADDR   0x80A00000
+
+/*-----------------------------------------------------------------
+ * Board Specific Configs
+ *----------------------------------------------------------------*/
+#define CONFIG_ORION
+#define CONFIG_BOARD_NAME               "ORION"
+#define CONFIG_MEMTEST_ENABLE           0
+#define CONFIG_MEMTEST_RESULT           "idle"
+#define CONFIG_BOOT_SELECTOR            1
+#define CONFIG_MOST_RECENTLY_PROG_FW    1
+
+
+/*-----------------------------------------------------------------
+ * YAFU Specific Configs
+ *----------------------------------------------------------------*/
+#define YAFU_IMAGE_UPLOAD_LOCATION    (0x80A00000)
+#define YAFU_TEMP_SECTOR_BUFFER       (0x82A40000) 
+
+#endif
+
