--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/include/pilot.h	1969-12-31 19:00:00.000000000 -0500
+++ mywork/arch/arm/cpu/armv7/pilot4/include/pilot.h	2015-02-06 05:06:37.000000000 -0500
@@ -0,0 +1,167 @@
+/*******************************************************************************
+ *
+ *
+ * Copyright (C) 2004-2014 Emulex. All rights reserved.
+ * EMULEX is a trademark of Emulex.
+ * www.emulex.com
+ *
+ * This program is free software; you can redistribute it and/or modify it under
+ * the terms of version 2 of the GNU General Public License as published by the
+ * Free Software Foundation.
+ * This program is distributed in the hope that it will be useful. ALL EXPRESS
+ * OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY IMPLIED
+ * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
+ * NON-INFRINGEMENT, ARE DISCLAIMED, EXCEPT TO THE EXTENT THAT SUCH DISCLAIMERS
+ * ARE HELD TO BE LEGALLY INVALID. See the GNU General Public License for more
+ * details, a copy of which can be found in the file COPYING included
+ * with this package.
+ *
+ ********************************************************************************/
+
+#define TLPCROM				0x00000000                        
+#define DDRRAM				0x80000000              
+#define SRAM				0x02000000              
+#if defined (ZYNQ)
+  #define LMEMSTART			0x50000000              
+#else
+  #define LMEMSTART			0x10000000              
+#endif
+#define VIC				0x40000000              
+#define INTbase				0x40000000              
+#define LPCREGBASE     	        	0x40100000              
+#define SYSCONTROL			0x40100100              
+#define INTRAMBASE                  0x40100200
+#define INTRAMBASE_END              0x4010027C
+#define BOOTSPI_BASE  		        0x40200000              
+#define DDR_REG_BASE     	        0x40300000              
+#define AHBSecAHBBridge		        0x40400000 
+#define AHBPeriAPBBridge  	        0x40430000              
+#define Ethernet 		        0x40500000              
+#define FrameGrabber      	        0x40900000              
+#define FGBBASE                         FrameGrabber
+#define FGB_SECBASE      	        0x40A00000              
+#define SDHC_BASE                       0x40C00000
+#define SDHC1_BASE                       0x40C80000
+#define NAND_BASE                       0x40D00000
+#define DMAController     	        0x40400000              
+#define AHBSysAPBBridge		        0x40410000              
+#define AHBSIOAPBBridge   	        0x40420000              
+#define GPIO          		        0x40420000              
+#define UART1          		        0x40421000              
+#define UART2          		        0x40422000              
+#define PSR               	        0x40426500              
+#define SystemWakeup      	        0x40425000              
+#define SWCBASE				SystemWakeup            
+#define Mailbox          	        0x40426420              
+#define WatchdogTimerMSFT 	        0x40427000              
+#define RealTimeClock     	        0x40428000              
+#define RTCbase         	        RealTimeClock
+#define RTCSTDbase         	        0x40428100
+#define RTCEXTbase         	        0x40428180              //d
+#define WatchdogTimer     	        0x40410000              
+#define Timer1Base        	        0x40411000              
+#define PS2Interface      	        0x40412000              
+#define FanTachandPWM     	        0x40413000              
+#define ADCandThermal     	        0x40414000              
+#define VirtualUART1      	        0x40415000              
+#define VirtualUART2      	        0x40416000              
+#define Bomber                          0x40417000              
+#define PECIBASE                        0x40418000              
+#define UART3          		        0x40430000              
+#define UART4          		        0x40431000              
+#define UART5          		        0x40432000              
+#define I2C1          		        0x40433000              
+#define I2C2          		        0x40434000              
+#define I2C3          		        0x40435000              
+#define I2C4            	        0x40436000              
+#define I2C5            	        0x40437000              
+#define I2C6            	        0x40438000              
+#define I2C7            	        0x40439000              
+#define I2C8            	        0x4043A000              
+#define I2C9            	        0x4043B000              
+#define I2C10            	        0x4043C000              
+#define BKUP_SPIBASE			0x40280000
+#define HSPIBASE		        0x40429000              
+#define KCS1BASE			0x40426000              
+#define KCS3BASE			0x40426200              
+#define KCS4BASE			0x40426300              
+#define KCS5BASE			0x40426400              
+#define USB1BASE                        0x40700000              
+#define USB2BASE                        0x40800000              
+#define I2C1base                        I2C1
+#define I2C2base                        I2C2
+#define I2C3base                        I2C3
+#define I2C4base                        I2C4
+#define I2C5base                        I2C5
+#define I2C6base                        I2C6
+#define I2C7base                        I2C7
+#define I2C8base                        I2C8
+#define I2C9base                        I2C9
+#define I2C10base                       I2C10
+#define UART1base                       UART1
+#define UART2base                       UART2
+#define UART3base                       UART3
+#define UART4base                       UART4
+#define UART5base                       UART5
+#define UART6base       		VirtualUART1
+#define UART7base       		VirtualUART2
+#define AHB2PCIBase			0x12000000
+#define AHB2PCIRegBase			0x40B00000
+#define FUNCTION1_BASE                  0x40440000
+#define TOPLEVEL_PINCTRL                0x40100900  
+//we donot have tap master,used to validate our tap slaves 
+//#define TAPBASE                         0x30000000 
+#define TAPBASE                         0x40429100
+
+//;PIOLOT VERIFICATION.EQU
+#define Mode_USR32			0x10
+#define Mode_FIQ32			0x11
+#define Mode_IRQ32			0x12
+#define Mode_ABT32			0x17
+#define Mode_UND32			0x1b
+#define Mode_SVC32			0x13
+
+#define I_Bit				0x80
+#define F_Bit				0x40
+
+//; See SDT reference manual section 8 (Angel)
+//; Angel SWIs
+#define AngelSWI_ARMSWI			0x123456
+#define AngelSWI_ThumbSWI		0xAB
+#define AngelSWI_SYS_OPEN		0x01
+#define AngelSWI_SYS_CLOSE		0x02
+#define AngelSWI_SYS_WRITEC		0x03
+#define AngelSWI_SYS_WRITE0		0x04
+#define AngelSWI_SYS_WRITE		0x05
+#define AngelSWI_SYS_READ 		0x06
+#define AngelSWI_SYS_READC		0x07
+#define AngelSWI_SYS_ISERROR		0x08
+#define AngelSWI_SYS_ISTTY		0x09
+#define AngelSWI_SYS_SEEK		0x0A
+#define AngelSWI_SYS_FLEN		0x0C
+#define AngelSWI_SYS_TMPNAM		0x0D
+#define AngelSWI_SYS_REMOVE		0x0E
+#define AngelSWI_SYS_RENAME		0x0F
+#define AngelSWI_SYS_CLOCK		0x10
+#define AngelSWI_SYS_TIME		0x11
+#define AngelSWI_SYS_SYSTEM		0x12
+#define AngelSWI_SYS_ERRNO		0x13
+#define AngelSWI_SYS_GET_CMDLINE 	0x15
+#define AngelSWI_SYS_HEAPINFO		0x16
+#define AngelSWI_EnterSVC		0x17
+#define AngelSWI_ReportException 	0x18
+#define AngelSWI_ApplDevice		0x19
+#define AngelSWI_LateStartup		0x20
+#define AngelSWI_ADPAppExit		0x20026
+
+#define fopenReadbin			1
+
+#define TubeBase                       0x40000000    //temporay
+#define TubeCharOff			0x00
+#define TubeHexOff			0x04
+#define TubeQuadOff			0x08
+#define TubeAddressChar			TubeBase + TubeCharOff
+#define TubeAddressHex			TubeBase + TubeHexOff
+#define TubeAddressQuad			TubeBase + TubeQuadOff
+#define TubeNewline			10
+#define TubeTerminate			4
