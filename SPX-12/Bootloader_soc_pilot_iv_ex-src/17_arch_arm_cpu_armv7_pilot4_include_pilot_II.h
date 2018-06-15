--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/include/pilot_II.h	1969-12-31 19:00:00.000000000 -0500
+++ mywork/arch/arm/cpu/armv7/pilot4/include/pilot_II.h	2015-03-23 10:03:49.646700244 -0400
@@ -0,0 +1,215 @@
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
+#include "pilot_types.h"
+//#include "buffer.h"
+//#include "int.h"
+#include "pilot.h"
+//#include "lmem.h"
+//#include "adc.h"
+//#include "i2c.h"
+//#include "mailbox.h"
+//#include "bomber.h"
+//#include "psr.h"
+//#include "gpio.h"
+//#include "pwm.h"
+//#include "rtc.h"
+//#include "uart.h"
+//#include "tap.h"
+//#include "spi_common.h"
+//#include "lpcspi.h"
+//#include "uart.h"
+//#include "wdt.h"
+//#include "host.h"
+//#include "peci.h"
+//#include "bootspi.h"
+//#include "fgb_sec.h"
+//#include "usb2.h"
+//#include "usb.h"
+//#include "usb_host.h" 
+//#include "usb_hub.h" 
+//#include "fgb_top.h"
+//#include "fgb_tse.h"
+//#include "fgb_tfe.h"
+//#include "fgb_grc.h"
+//#include "fgb_dma.h"
+//#include "fgb_bs.h"
+//#include "fgb_ssp.h"
+//#include "sec_gdma.h"
+//#include "sec_top.h"
+//#include "timer.h"
+//#include "swc.h"
+//#include "mac.h"
+//#include "mac12common.h"
+//#include "mac_pkt_defs.h"
+//#include "pilotII_sec.h"
+//#include "pilotII_hw_sec.h"
+//#include "pilotII_dbg.h"
+//#include "modexp.h"
+//#include "i2c.h"
+//#include "PS2.h"
+//#include "xdma.h"
+//#include "sio.h"
+//#include "sdhc.h"
+//#include "ahb2pci.h"
+#include "sysctrl.h"
+#include "ddr.h"
+//#include "function1_pcie.h"
+//#include "nand.h"
+//#include "pinmux.h"  
+//
+extern void wait_for_previous_print(void);
+extern void STOPSIMULATIONSC(void);
+extern void SetIRQServiceC (tU8 intno, tU32 addr);
+extern void SetFIQServiceC (tU8 intno, tU32 addr);
+extern void CWriteStr(int);  
+extern void EnableInterruptC (tU8 intno, tU8 inttype);
+extern void DisableInterruptC (tU8 intno, tU8 inttype);
+extern void SYSCTRL_ENABLEINTRC (tU8 intno, tU8 inttype);
+extern void delay(tU32);  
+extern void WRITE_TO_SIMULATORC (tU32); 
+extern void nc_printchar (unsigned char c);
+extern int nc_printf(const char *format, ...);
+extern void WRITEREG_WORD(tVU32 Addrs,tVU32 data); 
+extern void WRITEREG_BYTE(tVU32 Addrs,tVU8 data);
+extern void RMWRITEREG_WORD(tVU32 Addrs,tVU32 data);
+extern void RMWRITEREG_BYTE(tVU32 Addrs,tVU8 data);
+extern tU32 READWORD(tVU32 Addrs);
+extern tU8 READBYTE(tVU32 Addrs);
+extern void FlushDcache(void);
+extern tU8 readchar_uart (void);
+extern tU16 gdmacopy (tU32, tU32, tU32, tU32);
+extern tU32 getrandomno (tPVU32, tU32);
+extern tU32 powerC(tU32 x, tU32 y);
+extern tU8 verilog_mem_compare (tU32 , tU32 , tU32 , tU32 , tU32 , tU32 , tU32 , tU32 ) ;
+extern tU16 verilog_calc_chksum (tU32 , tU32 , tU16 ) ;
+extern tU8 p2_readbyte (tU32) ;
+extern tVU32 gen_random_bits (tPVU32, tVU32);
+extern tVU32 nc_gen_random_bits (tPVU32, tVU32);
+extern void write_simulator(tU32 cmd);
+extern tU32 verilog_get_randomno (tPVU32, tU32);
+extern void int11_commonhandler(void);
+extern void int13_commonhandler(void);
+extern unsigned int update_crc (unsigned int, char);
+extern void empty_memcon_wr_q (void);
+
+extern void int14_commonhandler(void);
+extern void hw_hangInthandler(void);
+extern void pri_ahb_bus(void);
+extern void ins_ahb_bus(void);
+extern void dma_ahb_bus(void);
+extern void reset_arm(tU32, tU8);
+extern void int14_fiqcommonhandler(void);
+extern tU8 xdma_lsi_fifo_training (void);
+
+extern void Put_Ssp_In_Reset(void);
+extern void Icache_Invalidate_All(void);
+extern void Dcache_Invalidate_All(void);
+extern void ldma_disable (void);
+extern void Do_ssp_cctrl_reset(void);
+extern void Set_base_limits(void);
+extern void Ssp_init(void);
+extern void Enable_Ssp_I_CCtl(void);
+extern  void Enable_Ssp_D_CCtl(void);
+extern void Prog_I_CSize(tU32 CSize);
+extern void Prog_D_CSize(tU32 CSize); 
+extern void Bring_Ssp_Out_Reset(void);
+
+void do_arm_soft_reset(void);
+void do_soft_reset(tU8, tU8);
+int disableMacTx_Rx(int);
+void fgb_disable(void);
+void gdma_disable(void);
+void usb1_dma_disable(void);
+void disable_bomber(void);
+void nand_not_busy(void);
+void sdhc_not_busy(void);
+void usb2_dma_disable(void);
+void firmware_upgrade_reset(void) ;
+tU32 (* arm_reset)(void);
+
+//peci functions
+
+extern void peci_set_wriamsr_cmd_7(tU8, tU8);
+extern void peci_set_wriamsr_cmd_8(tU8, tU8);
+extern void peci_set_wriamsr_cmd_A(tU8, tU8);
+extern void peci_set_wriamsr_cmd_E(tU8, tU8);
+extern void peci_set_wrpciconfig_cmd_8(tU8, tU8);
+extern void peci_set_wrpciconfig_cmd_9(tU8, tU8);
+extern void peci_set_wrpciconfig_cmd_B(tU8, tU8);
+extern void peci_set_wrpciconfiglocal_7(tU8, tU8);
+extern void peci_set_wrpciconfiglocal_8(tU8, tU8);
+extern void peci_set_wrpciconfiglocal_A(tU8, tU8);
+extern void set_xmit_q_wriamsr_7(tU8, tU8, tU8, tU8);
+extern void set_xmit_q_wriamsr_8(tU8, tU8, tU8, tU8);
+extern void set_xmit_q_wriamsr_A(tU8, tU8, tU8, tU8);
+extern void set_xmit_q_wriamsr_8(tU8, tU8, tU8, tU8);
+extern void set_xmit_q_8(tU8, tU8, tU8, tU8);
+extern void set_xmit_q_9(tU8, tU8, tU8, tU8);
+extern void set_xmit_q_B(tU8, tU8, tU8, tU8);
+extern void set_xmit_q_local_7(tU8, tU8, tU8, tU8);
+extern void set_xmit_q_local_8(tU8, tU8, tU8, tU8);
+extern void set_xmit_q_local_A(tU8, tU8, tU8, tU8);
+extern void set_xmit_q_wriamsr_E(tU8, tU8, tU8, tU8);
+
+//peci monitor
+
+extern void peci_control_register_pre_simulation_checker(tU32);
+extern tU8 peci_recover_value(void);
+extern tU32 peci_status_register_post_simulation_checker(void);
+//PCIE Function 2 
+ 
+extern void FUNCTION1_EN(void);
+extern void FUNCTION1_CFG_EN(void);
+extern void pcie_function1_inthandler(void);
+
+//General
+
+extern tU32 Test_Finish(void);
+
+//USB2
+
+extern void usb2_dma_pause(void);
+extern void clear_dma_pause(void); 
+
+
+#define READ_REG8(a)      (*((tPVU8)  (a)))
+#define READ_REG16(a)     (*((tPVU16) (a)))
+#define READ_REG32(a)     (*((tPVU32) (a)))
+                                                                                                                                                                        
+#define WRITE_REG8(a,d)   (*((tPVU8) (a)) = d)
+#define WRITE_REG16(a,d)  (*((tPVU16) (a)) = d)
+#define WRITE_REG32(a,d)  (*((tPVU32) (a)) = d)
+                                                                                                                                                                        
+#define SET_BIT8(a,b)     (WRITE_REG8 (a, (READ_REG8(a)  | (tU8) (1<<b))))
+#define SET_BIT16(a,b)    (WRITE_REG16(a, (READ_REG16(a) | (tU16)(1<<b))))
+#define SET_BIT32(a,b)    (WRITE_REG32(a, (READ_REG32(a) | (tU32) (1<<b))))
+#define SET_BIT(a,b)      (SET_BIT32(a,b))
+                                                                                                                                                                        
+#define CLEAR_BIT8(a,b)   (WRITE_REG8 (a, (READ_REG8(a) & (tU8)(~(1<<b)))))
+#define CLEAR_BIT16(a,b)  (WRITE_REG16(a, (READ_REG16(a) & (tU16)(~(1<<b)))))
+#define CLEAR_BIT32(a,b)  (WRITE_REG32(a, (READ_REG32(a) & (tU32)(~(1<<b)))))
+#define CLEAR_BIT(a,b)    (CLEAR_BIT32(a,b))
+//                                                                                                                                                                        
+#define TOGGLE_BIT8(a,b)  (WRITE_REG8 (a, (READ_REG8(a)  ^ (tU8) (1<<b))))
+#define TOGGLE_BIT16(a,b) (WRITE_REG16(a, (READ_REG16(a) ^ (tU16)(1<<b))))
+#define TOGGLE_BIT32(a,b) (WRITE_REG32(a, (READ_REG32(a) ^ (tU32)(1<<b))))
+#define TOGGLE_BIT(a,b)   (TOGGLE_BIT32(a,b))
+
