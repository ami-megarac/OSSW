--- uboot_old/arch/arm/cpu/astcommon/ast_wdt.c	1970-01-01 05:30:00.000000000 +0530
+++ uboot/arch/arm/cpu/astcommon/ast_wdt.c	2017-03-16 14:49:14.253829467 +0530
@@ -0,0 +1,102 @@
+
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
+ ***                                                            **
+ ***            All Rights Reserved.                            **
+ ***                                                            **
+ ***        6145-F, Northbelt Parkway, Norcross,                **
+ ***                                                            **
+ ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ ***                                                            **
+ *****************************************************************
+ *****************************************************************
+
+ *******************************************************************/
+#if defined(CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING) || defined(CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY)
+#include "soc_hw.h"
+#include <common.h>
+
+#define WDT_ENABLE              0x01
+#define WDT_TIMEOUT             0x11E1A300              // 5 Minutes (300 secs * 1MHz)
+#define WDT_TRIGGER             0x00004755              // Trigger Passcode
+#define WDT_CLOCK               0x10                    // 1MHz Clock for WDT
+#define WDT_RESET_SYSTEM        0x02                    // Enable System-Reset-On-Timeout
+#define WDT_CLR_EVT_COUNTER     0x76                    // Clear the Timeout Counter & Boot Source code
+#define WDT2_SECOND_BOOT_CODE   0x80                    // Enable Second Boot Code upon reset
+#define WDT_CUR_BOOTSRC_MASK    0x00000002
+
+#ifdef CONFIG_SPX_FEATURE_HW_FAILSAFE_BOOT 
+extern unsigned char broken_spi_banks;// specify the number of broken SPI flash bank
+#endif
+
+int Enable_watchdog(unsigned long *wdt_timeout_status)
+{
+       /* Setting the Watchdog Timer to prevent dead locks during booting */
+       /* Disable the Watchdog Timer */
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
+
+       /* Set the Timeout value to max possible 5 Minutes */
+       *((volatile unsigned long *)(WDT2_RELOAD_REG)) = WDT_TIMEOUT;
+
+       /* Set the Trigger value to restart Timer freshly */
+       *((volatile unsigned long *)(WDT2_CNT_RESTART_REG)) = WDT_TRIGGER;
+
+       /* Set the Watchdog clock to 1MHz clock */
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) |= (WDT_CLOCK);
+
+       /* Reading the Watchdog Timeout event counter*/
+       *wdt_timeout_status = *((volatile unsigned long *)(WDT2_TIMEOUT_STAT_REG));
+#ifdef CONFIG_SPX_FEATURE_DEDICATED_SPI_FLASH_BANK
+       /*clear boot source to restore address mapping... */
+#ifdef CONFIG_SPX_FEATURE_HW_FAILSAFE_BOOT
+		if ((broken_spi_banks == 2) && (*wdt_timeout_status & WDT_CUR_BOOTSRC_MASK))
+			broken_spi_banks = 1;
+		else if (broken_spi_banks == 0)
+#endif
+       {
+           *((volatile unsigned long *)(WDT2_CLR_TIMEOUT_STAT_REG)) = 0x00000001 ; 
+       }
+#endif
+
+#ifdef CONFIG_SPX_FEATURE_HW_FAILSAFE_BOOT
+       /* Enabling the Second Boot code upon reset */
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) |= (WDT2_SECOND_BOOT_CODE);
+#else
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) &= ~(WDT2_SECOND_BOOT_CODE);
+#endif
+       /* Enable back the Watchdog timer to start the Watchdog */
+       /* Also set the Flag to reset the CPU on Timeout */
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) |= (WDT_RESET_SYSTEM | WDT_ENABLE);
+
+#ifdef CONFIG_SPX_FEATURE_HW_FAILSAFE_BOOT 
+	if (broken_spi_banks == 1) 
+           return 1;// if bank#0 is broken then dont clear address mapping and bootimage 1 
+       else 
+#endif
+           return (((*wdt_timeout_status) & (WDT_CUR_BOOTSRC_MASK)) >> 1 ) + 1 ;
+}
+
+/**
+* @fn Disable_watchdog
+* @brief Disables watchdog reset.
+* @param[in] void.
+* @retval  0 - on success.
+*/
+int Disable_watchdog(void)
+{
+
+    if(*((volatile unsigned long *)(WDT2_CONTROL_REG)) & (WDT_ENABLE | WDT_RESET_SYSTEM))
+    {
+       printf("Disabling Watchdog 2 Timer\n");
+       *((volatile unsigned long *)(WDT2_CONTROL_REG)) &= ~(WDT_RESET_SYSTEM | WDT_ENABLE);
+    }
+#ifdef CONFIG_SPX_FEATURE_HW_FAILSAFE_BOOT
+	if ((broken_spi_banks == 2) && (*((volatile unsigned long *)(WDT2_TIMEOUT_STAT_REG)) & WDT_CUR_BOOTSRC_MASK))
+		broken_spi_banks = 1;
+#endif
+    return 0;
+}
+
+#endif

