--- uboot_old/oem/ami/standalone/cmd_nettest.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/cmd_nettest.c	2018-06-22 17:51:23.921169873 +0800
@@ -0,0 +1,98 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+
+#include <common.h>
+#include <command.h>
+#include "common_std.h"
+
+#define NETESTCMD_MAX_ARGS CONFIG_SYS_MAXARGS
+#define MODULE_NAME "testapps"
+
+static int do_mactest (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+    char module_name[] = MODULE_NAME;
+    int rcode;
+
+    rcode = load_app(module_name);
+    if (rcode != 0) {
+    	printf("load_app() failed\n");
+	return rcode;
+    }
+
+    return exec_app(argc + 1, argv - 1);
+}
+
+static int do_mactestd (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+    char module_name[] = MODULE_NAME;
+    int rcode;
+
+    rcode = load_app(module_name);
+    if (rcode != 0) {
+    	printf("load_app() failed\n");
+	return rcode;
+    }
+
+    return exec_app(argc + 1, argv - 1);
+}
+
+U_BOOT_CMD(
+    mactest,    NETESTCMD_MAX_ARGS, 0,  do_mactest,
+    "Dedicated LAN test program",
+    NULL
+);
+
+U_BOOT_CMD(
+    mactestd,    NETESTCMD_MAX_ARGS, 0,  do_mactestd,
+    "Dedicated LAN test program and display more information",
+    NULL
+);
+
+static int do_ncsitest (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+    char module_name[] = MODULE_NAME;
+    int rcode;
+
+    rcode = load_app(module_name);
+    if (rcode != 0) {
+    	printf("load_app() failed\n");
+	return rcode;
+    }
+
+    return exec_app(argc + 1, argv - 1);
+}
+
+static int do_ncsitestd (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+    char module_name[] = MODULE_NAME;
+    int rcode;
+
+    rcode = load_app(module_name);
+    if (rcode != 0) {
+    	printf("load_app() failed\n");
+	return rcode;
+    }
+
+    return exec_app(argc + 1, argv - 1);
+}
+
+U_BOOT_CMD(
+    ncsitest,    NETESTCMD_MAX_ARGS, 0,  do_ncsitest,
+    "Share LAN (NC-SI) test program",
+    NULL
+);
+
+U_BOOT_CMD(
+    ncsitestd,    NETESTCMD_MAX_ARGS, 0,  do_ncsitestd,
+    "Share LAN (NC-SI) test program and display more information",
+    NULL
+);
--- uboot_old/oem/ami/standalone/cmd_slt.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/cmd_slt.c	2018-06-22 17:51:23.921169873 +0800
@@ -0,0 +1,38 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+
+#include <common.h>
+#include <command.h>
+#include "common_std.h"
+
+#define MODULE_NAME "testapps"
+
+static int do_dramtest (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+    char module_name[] = MODULE_NAME;
+    int rcode;
+
+    rcode = load_app(module_name);
+    if (rcode != 0) {
+    	printf("load_app() failed\n");
+	return rcode;
+    }
+
+    return exec_app(argc + 1, argv - 1);
+}
+
+U_BOOT_CMD(
+    dramtest,   CONFIG_SYS_MAXARGS, 0,  do_dramtest,
+    "dramtest- Stress DRAM",
+    NULL
+);
+
--- uboot_old/oem/ami/standalone/common_std.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/common_std.c	2018-06-22 17:51:23.921169873 +0800
@@ -0,0 +1,35 @@
+#include <common.h>
+#include "../fmh/cmd_fmh.h"
+#include"common_std.h"
+
+int load_app(char * module_name)
+{
+    unsigned long img_addr;
+    unsigned long size;
+    int rcode;
+
+    rcode = GetFMHSectorLocationSize(module_name, &img_addr, &size);
+    if (rcode != 0) {
+    	printf("GetFMHSectorLocationSize() failed\n");
+	return rcode;
+    }
+
+    rcode = flash_read(img_addr, size, (char *)CONFIG_STANDALONE_LOAD_ADDR);
+    if (rcode != ERR_OK) {
+    	printf("flash_read() failed\n");
+    	return rcode;
+    }
+    return rcode;
+}
+
+int exec_app(int argc, char * const argv[])
+{
+    unsigned long rc;
+    int rcode = 0;
+
+    rc = do_go_exec ((void *)CONFIG_STANDALONE_LOAD_ADDR, argc, argv);
+    if (rc != 0) rcode = 1;
+    printf ("## Application terminated, rc = 0x%lX\n", rc);
+
+    return rcode;
+}
--- uboot_old/oem/ami/standalone/common_std.h	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/common_std.h	2018-06-22 17:51:23.921169873 +0800
@@ -0,0 +1,3 @@
+extern __attribute__((weak)) unsigned long do_go_exec(ulong (*entry)(int, char * const []), int argc, char * const argv[]);
+extern int load_app(char * module_name);
+extern int exec_app(int argc, char * const argv[]);
--- uboot_old/oem/ami/standalone/Makefile	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/Makefile	2018-06-22 17:51:23.921169873 +0800
@@ -0,0 +1,50 @@
+#
+# (C) Copyright 2013 
+# American Megatrends Inc
+#
+# This program is free software; you can redistribute it and/or
+# modify it under the terms of the GNU General Public License as
+# published by the Free Software Foundation; either version 2 of
+# the License, or (at your option) any later version.
+#
+# This program is distributed in the hope that it will be useful,
+# but WITHOUT ANY WARRANTY; without even the implied warranty of
+# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
+# GNU General Public License for more details.
+#
+# You should have received a copy of the GNU General Public License
+# along with this program; if not, write to the Free Software
+# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+# MA 02111-1307 USA
+#
+
+include $(TOPDIR)/config.mk
+
+LIB	:= $(obj)libnettest.o
+
+COBJS-y	+= common_std.o
+COBJS-y	+= cmd_nettest.o
+COBJS-y += cmd_slt.o
+
+
+#-------------------------------------------------------------------
+COBJS   := $(sort $(COBJS-y))
+SRCS	:= $(COBJS:.o=.c)
+OBJS	:= $(addprefix $(obj),$(COBJS))
+
+
+all:	$(obj).depend  $(LIB)
+
+$(LIB): $(obj).depend $(OBJS)
+	$(call cmd_link_o_target, $(OBJS))
+
+#########################################################################
+
+# defines $(obj).depend target
+include $(SRCTREE)/rules.mk
+
+sinclude $(obj).depend
+
+#########################################################################
+
+
--- uboot_old/oem/ami/standalone/nettest/cmd_nettest.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/cmd_nettest.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,421 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+
+/*  
+ *  for reference only
+ */
+#if 0
+
+#include <common.h>
+#include <command.h>
+#include "SWFUNC.H"
+#include "COMMINF.H"
+
+#ifdef SLT_UBOOT
+extern int main_function(int argc, char * const argv[], char mode);
+extern unsigned long int strtoul(char *string, char **endPtr, int base);
+
+#ifdef CMD_MACTEST
+static int do_mactest (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+    display_lantest_log_msg = 0;
+    return main_function( argc, argv, MODE_DEDICATED );
+}
+
+static int do_mactestd (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+    display_lantest_log_msg = 1;
+    return main_function( argc, argv, MODE_DEDICATED );
+}
+
+U_BOOT_CMD(
+    mactest,    NETESTCMD_MAX_ARGS, 0,  do_mactest,
+    "Dedicated LAN test program",
+    NULL
+);
+
+U_BOOT_CMD(
+    mactestd,    NETESTCMD_MAX_ARGS, 0,  do_mactestd,
+    "Dedicated LAN test program and display more information",
+    NULL
+);
+#endif //CMD_MACTEST
+
+// ------------------------------------------------------------------------------
+#ifdef CMD_NCSITEST
+static int do_ncsitest (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+    display_lantest_log_msg = 0;
+    return main_function( argc, argv, MODE_NSCI );
+}
+
+static int do_ncsitestd (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+    display_lantest_log_msg = 1;
+    return main_function( argc, argv, MODE_NSCI );
+}
+
+U_BOOT_CMD(
+    ncsitest,    NETESTCMD_MAX_ARGS, 0,  do_ncsitest,
+    "Share LAN (NC-SI) test program",
+    NULL
+);
+
+U_BOOT_CMD(
+    ncsitestd,    NETESTCMD_MAX_ARGS, 0,  do_ncsitestd,
+    "Share LAN (NC-SI) test program and display more information",
+    NULL
+);
+#endif //CMD_NCSITEST
+
+// ------------------------------------------------------------------------------
+#ifdef CMD_PHY_RW
+static int do_phyread (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+	MAC_ENGINE	MACENG;
+	MAC_ENGINE	*eng;
+	int     MACnum;
+	int     PHYreg;
+	ULONG   result_data;
+	int     ret = 0;
+	int     PHYaddr;
+	int     timeout = 0;
+	ULONG   MAC_040;
+
+	eng = &MACENG;
+    do {
+        if ( argc != 4 ) {
+            printf(" Wrong parameter number.\n");
+            printf(" phyr mac addr reg\n");
+            printf("   mac     : 0 or 1.   [hex]\n");
+            printf("   PHY addr: 0 to 0x1F.[hex]\n");
+            printf("   register: 0 to 0xFF.[hex]\n");
+            printf(" example: phyr 0 0 1\n");
+            ret = -1;
+            break;
+        }
+
+        MACnum  = strtoul(argv[1], NULL, 16);
+        PHYaddr = strtoul(argv[2], NULL, 16);
+        PHYreg  = strtoul(argv[3], NULL, 16);
+
+        if ( MACnum == 0 ) {
+            // Set MAC 0
+            eng->run.MAC_BASE = MAC_BASE1;
+        }
+        else if ( MACnum == 1 ) {
+                // Set MAC 1
+                eng->run.MAC_BASE = MAC_BASE2;
+        }
+        else {
+            printf("wrong parameter (mac number)\n");
+            ret = -1;
+            break;
+        }
+
+        if ( ( PHYaddr < 0 ) || ( PHYaddr > 31 ) ) {
+            printf("wrong parameter (PHY address)\n");
+            ret = -1;
+            break;
+        }
+
+        MAC_040 = Read_Reg_MAC_DD( eng, 0x40 );
+        eng->inf.NewMDIO = (MAC_040 & 0x80000000) ? 1 : 0;
+
+        if ( eng->inf.NewMDIO ) {
+            Write_Reg_MAC_DD( eng, 0x60, MAC_PHYRd_New | (PHYaddr << 5) | ( PHYreg & 0x1f ) );
+            while ( Read_Reg_MAC_DD( eng, 0x60 ) & MAC_PHYBusy_New ) {
+                if ( ++timeout > TIME_OUT_PHY_RW ) {
+                    ret = -1;
+                    break;
+                }
+            }
+#ifdef Delay_PHYRd
+            DELAY( Delay_PHYRd );
+#endif
+            result_data = Read_Reg_MAC_DD( eng, 0x64 ) & 0xffff;
+        }
+        else {
+            Write_Reg_MAC_DD( eng, 0x60, MDC_Thres | MAC_PHYRd | (PHYaddr << 16) | ((PHYreg & 0x1f) << 21) );
+            while ( Read_Reg_MAC_DD( eng, 0x60 ) & MAC_PHYRd ) {
+                if ( ++timeout > TIME_OUT_PHY_RW ) {
+                    ret = -1;
+                    break;
+                }
+            }
+#ifdef Delay_PHYRd
+            DELAY( Delay_PHYRd );
+#endif
+            result_data = Read_Reg_MAC_DD( eng, 0x64 ) >> 16;
+        }
+		printf(" PHY[%d] reg[0x%02X] = %04lx\n", PHYaddr, PHYreg, result_data );
+    } while ( 0 );
+
+    return ret;
+}
+
+static int do_phywrite (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+	MAC_ENGINE	MACENG;
+	MAC_ENGINE	*eng;
+	int     MACnum;
+	int     PHYreg;
+	int     PHYaddr;
+	ULONG   reg_data;
+	int     ret     = 0;
+	int     timeout = 0;
+	ULONG   MAC_040;
+
+	eng = &MACENG;
+
+    do {
+        if ( argc != 5 )
+        {
+            printf(" Wrong parameter number.\n");
+            printf(" phyw mac addr reg data\n");
+            printf("   mac     : 0 or 1.     [hex]\n");
+            printf("   PHY addr: 0 to 0x1F.  [hex]\n");
+            printf("   register: 0 to 0xFF.  [hex]\n");
+            printf("   data    : 0 to 0xFFFF.[hex]\n");
+            printf(" example: phyw 0 0 0 610\n");
+            ret = -1;
+            break;
+        }
+
+        MACnum   = strtoul(argv[1], NULL, 16);
+        PHYaddr  = strtoul(argv[2], NULL, 16);
+        PHYreg   = strtoul(argv[3], NULL, 16);
+        reg_data = strtoul(argv[4], NULL, 16);
+
+        if ( MACnum == 0 ) {
+            // Set MAC 0
+            eng->run.MAC_BASE  = MAC_BASE1;
+        }
+        else if ( MACnum == 1 ) {
+                // Set MAC 1
+                eng->run.MAC_BASE  = MAC_BASE2;
+        }
+        else {
+            printf("wrong parameter (mac number)\n");
+            ret = -1;
+            break;
+        }
+
+        if ( ( PHYaddr < 0 ) || ( PHYaddr > 31 ) ) {
+            printf("wrong parameter (PHY address)\n");
+            ret = -1;
+            break;
+        }
+
+        MAC_040 = Read_Reg_MAC_DD( eng, 0x40 );
+        eng->inf.NewMDIO = (MAC_040 & 0x80000000) ? 1 : 0;
+
+        if ( eng->inf.NewMDIO ) {
+            Write_Reg_MAC_DD( eng, 0x60, ( reg_data << 16 ) | MAC_PHYWr_New | (PHYaddr<<5) | (PHYreg & 0x1f) );
+
+            while ( Read_Reg_MAC_DD( eng, 0x60 ) & MAC_PHYBusy_New ) {
+                if ( ++timeout > TIME_OUT_PHY_RW ) {
+                    ret = -1;
+                    break;
+                }
+            }
+        }
+        else {
+            Write_Reg_MAC_DD( eng, 0x64, reg_data );
+            Write_Reg_MAC_DD( eng, 0x60, MDC_Thres | MAC_PHYWr | (PHYaddr<<16) | ((PHYreg & 0x1f) << 21) );
+
+            while ( Read_Reg_MAC_DD( eng, 0x60 ) & MAC_PHYWr ) {
+                if ( ++timeout > TIME_OUT_PHY_RW ) {
+                    ret = -1;
+                    break;
+                }
+            }
+        } // End if ( eng->inf.NewMDIO )
+
+		printf("Write: PHY[%d] reg[0x%02X] = %04lx\n", PHYaddr, PHYreg, reg_data );
+	} while ( 0 );
+
+	return ret;
+}
+
+static int do_phydump (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+	MAC_ENGINE	MACENG;
+	MAC_ENGINE	*eng;
+	int     MACnum;
+	int     PHYreg;
+	ULONG   result_data;
+	int     ret = 0;
+	int     PHYaddr;
+	int     timeout = 0;
+	ULONG   MAC_040;
+
+	eng = &MACENG;
+	do {
+		if ( argc != 3 ) {
+			printf(" Wrong parameter number.\n");
+			printf(" phyd mac addr\n");
+			printf("   mac     : 0 or 1.   [hex]\n");
+			printf("   PHY addr: 0 to 0x1F.[hex]\n");
+			printf(" example: phyd 0 0\n");
+			ret = -1;
+			break;
+		}
+
+		MACnum  = strtoul(argv[1], NULL, 16);
+		PHYaddr = strtoul(argv[2], NULL, 16);
+
+		if ( MACnum == 0 ) {
+			// Set MAC 0
+			eng->run.MAC_BASE = MAC_BASE1;
+		}
+		else if ( MACnum == 1 ) {
+			// Set MAC 1
+			eng->run.MAC_BASE = MAC_BASE2;
+		}
+		else {
+			printf("wrong parameter (mac number)\n");
+			ret = -1;
+			break;
+		}
+
+		if ( ( PHYaddr < 0 ) || ( PHYaddr > 31 ) ) {
+			printf("wrong parameter (PHY address)\n");
+			ret = -1;
+			break;
+		}
+
+		MAC_040 = Read_Reg_MAC_DD( eng, 0x40 );
+		eng->inf.NewMDIO = (MAC_040 & 0x80000000) ? 1 : 0;
+
+		if ( eng->inf.NewMDIO ) {
+			for ( PHYreg = 0; PHYreg < 32; PHYreg++ ) {
+				
+				Write_Reg_MAC_DD( eng, 0x60, MAC_PHYRd_New | (PHYaddr << 5) | ( PHYreg & 0x1f ) );
+				while ( Read_Reg_MAC_DD( eng, 0x60 ) & MAC_PHYBusy_New ) {
+					if ( ++timeout > TIME_OUT_PHY_RW ) {
+						ret = -1;
+						break;
+					}
+				}
+#ifdef Delay_PHYRd
+				DELAY( Delay_PHYRd );
+#endif
+				result_data = Read_Reg_MAC_DD( eng, 0x64 ) & 0xffff;
+				switch ( PHYreg % 4 ) {
+					case 0	: printf("%02d| %04lx ", PHYreg, result_data ); break;
+					case 3	: printf("%04lx\n", result_data ); break;
+					default	: printf("%04lx ", result_data ); break;
+				}
+			}
+		}
+		else {
+			for ( PHYreg = 0; PHYreg < 32; PHYreg++ ) {
+				Write_Reg_MAC_DD( eng, 0x60, MDC_Thres | MAC_PHYRd | (PHYaddr << 16) | ((PHYreg & 0x1f) << 21) );
+				while ( Read_Reg_MAC_DD( eng, 0x60 ) & MAC_PHYRd ) {
+					if ( ++timeout > TIME_OUT_PHY_RW ) {
+						ret = -1;
+						break;
+					}
+				}
+#ifdef Delay_PHYRd
+				DELAY( Delay_PHYRd );
+#endif
+				result_data = Read_Reg_MAC_DD( eng, 0x64 ) >> 16;
+				switch ( PHYreg % 4 ) {
+					case 0	: printf("%02d| %04lx ", PHYreg, result_data ); break;
+					case 3	: printf("%04lx\n", result_data ); break;
+					default	: printf("%04lx ", result_data ); break;
+				}
+			}
+		}
+	} while ( 0 );
+
+	return ret;
+}
+
+U_BOOT_CMD(
+	phyr,    NETESTCMD_MAX_ARGS, 0,  do_phyread,
+	"Read PHY register.  (phyr mac addr reg)",
+	NULL
+);
+
+U_BOOT_CMD(
+	phyw,    NETESTCMD_MAX_ARGS, 0,  do_phywrite,
+	"Write PHY register. (phyw mac addr reg data)",
+	NULL
+);
+
+U_BOOT_CMD(
+	phyd,    NETESTCMD_MAX_ARGS, 0,  do_phydump,
+	"Dump PHY register. (phyd mac addr)",
+	NULL
+);
+#endif //CMD_PHY_RW
+
+// ------------------------------------------------------------------------------
+#ifdef CMD_MACGPIO
+static int do_macgpio (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+	Write_Reg_GPIO_DD( 0x78 , Read_Reg_GPIO_DD( 0x78 ) & 0xf7bfffff );
+	Write_Reg_GPIO_DD( 0x7c , Read_Reg_GPIO_DD( 0x7c ) | 0x08400000 );
+	DELAY( 100 );
+	Write_Reg_GPIO_DD( 0x78 , Read_Reg_GPIO_DD( 0x78 ) | 0x08400000 );
+
+	return 0;
+}
+
+U_BOOT_CMD(
+	macgpio,    NETESTCMD_MAX_ARGS, 0,  do_macgpio,
+	"Setting GPIO to trun on the system for the MACTEST/NCSITEST (OEM)",
+	NULL
+);
+#endif //CMD_MACGPIO
+/*
+static int do_clkduty (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
+{
+	int temp;
+	int i;
+	char *re[10];
+
+	temp   = strtoul(argv[1], NULL, 16);
+	for ( i = 1; i < argc; i++ )
+	{
+		re[i] = argv[i + 1];
+		printf("arg[%d]= %s\n", i , re[i]);
+	}
+	argc--;
+
+	Write_Reg_SCU_DD( 0x1DC, 0 );
+	main_function( argc, re, MODE_DEDICATED );
+	printf("SCU1DC= %lx\n", Read_Reg_SCU_DD(0x1DC) );
+     
+     for ( i = 0; i < 64; i += temp )
+     {
+         Write_Reg_SCU_DD( 0x1DC, ( ((ULONG)(i + 0x40) << 16) | ((ULONG)(i + 0x40) << 8) ) );
+         printf("SCU1DC= %lx [%lx]\n", Read_Reg_SCU_DD(0x1DC) , (ULONG)temp );
+         main_function( argc, re, MODE_DEDICATED );
+     }
+     
+     return 0;
+}
+
+U_BOOT_CMD(
+    clkduty,    NETESTCMD_MAX_ARGS, 0,  do_clkduty,
+    "clkduty",
+    NULL
+);
+*/
+
+#endif // End SLT_UBOOT
+
+#endif
--- uboot_old/oem/ami/standalone/nettest/COMMINF.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/COMMINF.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,1135 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef COMMINF_H
+#define COMMINF_H
+
+#include "SWFUNC.H"
+
+#if defined(LinuxAP)
+  #include <stdio.h>
+#endif
+#if defined(SLT_UBOOT)
+    #ifdef AST1010_CHIP
+        #include <aspeed/ast_addr_remap.h>
+        #include <aspeed/ast_addr_controller_base.h>
+    #endif
+#endif
+#if defined(DOS_ALONE) || defined(SLT_NEW_ARCH)
+  #include <stdio.h>
+  #include <time.h>
+  #include <dos.h>    // For delay()
+#endif
+
+#include "TYPEDEF.H"
+#include "LIB.H"
+#include "MAC.H"
+#include "PHYSPECIAL.H"
+#include "PHYGPIO.H"
+
+//---------------------------------------------------------
+// Print Message
+//---------------------------------------------------------
+// for function
+#define FP_LOG                                   0
+#define FP_IO                                    1
+#define STD_OUT                                  2
+
+#if defined(ENABLE_LOG_FILE)
+  #define PRINTF(i, ...)                                   \
+      switch( i ) {                                        \
+          case FP_LOG:                                     \
+              fprintf(eng->fp_log, __VA_ARGS__);           \
+              break;                                       \
+          case FP_IO:                                      \
+              fprintf(eng->fp_io, __VA_ARGS__);            \
+              break;                                       \
+          case STD_OUT:                                    \
+              fprintf(stdout, __VA_ARGS__);                \
+              break;                                       \
+          default : break; }
+#elif defined (SLT_UBOOT)
+  #define PRINTF(i, ...)                                          \
+     do {                                                         \
+         if (i == STD_OUT) {                                      \
+             fprintf(stdout, __VA_ARGS__);                        \
+             break;                                               \
+         }                                                        \
+         if ( (display_lantest_log_msg != 0) && (i == FP_LOG) ) { \
+             fprintf(stdout, "[Log]:   ");                        \
+             fprintf(stdout, __VA_ARGS__);                        \
+         }                                                        \
+     } while ( 0 );
+#else
+  #define PRINTF(i, ...) do {} while ( 0 );
+#endif
+
+//---------------------------------------------------------
+// Function
+//---------------------------------------------------------
+  #define SWAP_4B( x )                                                             \
+                                                 ( ( ( ( x ) & 0xff000000 ) >> 24) \
+                                                 | ( ( ( x ) & 0x00ff0000 ) >>  8) \
+                                                 | ( ( ( x ) & 0x0000ff00 ) <<  8) \
+                                                 | ( ( ( x ) & 0x000000ff ) << 24) \
+                                                 )
+  #define SWAP_2B( x )                                                             \
+                                                 ( ( ( ( x ) & 0xff00     ) >>  8) \
+                                                 | ( ( ( x ) & 0x00ff     ) <<  8) \
+                                                 )
+#if defined(ENABLE_BIG_ENDIAN)
+  #define SWAP_2B_BEDN( x )                      ( x )
+  #define SWAP_2B_LEDN( x )                      ( SWAP_2B ( x ) )
+  #define SWAP_4B_BEDN( x )                      ( x )
+  #define SWAP_4B_LEDN( x )                      ( SWAP_4B ( x ) )
+#else
+  #define SWAP_2B_BEDN( x )                      ( SWAP_2B ( x ) )
+  #define SWAP_2B_LEDN( x )                      ( x )
+  #define SWAP_4B_BEDN( x )                      ( SWAP_4B ( x ) )
+  #define SWAP_4B_LEDN( x )                      ( x )
+#endif
+
+#if defined(ENABLE_BIG_ENDIAN)
+  #define SWAP_4B_BEDN_NCSI( x )                 ( x )
+  #define SWAP_4B_LEDN_NCSI( x )                 ( SWAP_4B( x ) )
+#else
+  #define SWAP_4B_BEDN_NCSI( x )                 ( SWAP_4B( x ) )
+  #define SWAP_4B_LEDN_NCSI( x )                 ( x )
+#endif
+
+#if defined(ENABLE_BIG_ENDIAN_MEM)
+  #define SWAP_4B_LEDN_MEM( x )                  ( SWAP_4B( x ) )
+#else
+  #define SWAP_4B_LEDN_MEM( x )                  ( x )
+#endif
+#if defined(ENABLE_BIG_ENDIAN_REG)
+  #define SWAP_4B_LEDN_REG( x )                  ( SWAP_4B( x ) )
+#else
+  #define SWAP_4B_LEDN_REG( x )                  ( x )
+#endif
+
+#if defined(DOS_ALONE) || defined(SLT_NEW_ARCH) || defined(LinuxAP)
+//    #define DELAY( x )                           delay( x )         // For DOS, the unit of delay() is m
+    #define DELAY( x )                           delay_hwtimer( x )   // For hardware timer #2, the unit of delay() is ms.s.
+    #define GET_CAHR                             getchar
+#elif defined(SLT_UBOOT)
+    #define DELAY( x )                           udelay( ( x ) * 1000 ) // For Uboot, the unit of udelay() is us.
+    #define GET_CAHR                             getc
+#else
+    #define DELAY(val)                           usleep( val * 1000 )
+    #define GET_CAHR                             getchar
+#endif
+
+//---------------------------------------------------------
+// Default argument
+//---------------------------------------------------------
+#define  DEF_GUSER_DEF_PACKET_VAL                0x66666666     //0xff00ff00, 0xf0f0f0f0, 0xcccccccc, 0x55aa55aa, 0x5a5a5a5a, 0x66666666
+#define  DEF_GIOTIMINGBUND                       5              //0/1/3/5/7
+#define  DEF_GPHY_ADR                            0
+#define  DEF_GTESTMODE                           0              //[0]0: no burst mode, 1: 0xff, 2: 0x55, 3: random, 4: ARP, 5: ARP, 6: IO timing, 7: IO timing+IO Strength
+#define  DEF_GLOOP_MAX                           1
+#define  DEF_MAC_LOOP_BACK                       0              //GCtrl bit6
+#define  DEF_SKIP_CHECK_PHY                      0              //GCtrl bit5
+#define  DEF_INTERNAL_LOOP_PHY                   0              //GCtrl bit4
+#define  DEF_INIT_PHY                            1              //GCtrl bit3
+#define  DEF_DIS_RECOVPHY                        0              //GCtrl bit2
+#define  DEF_GCTRL                               (( DEF_MAC_LOOP_BACK << 6 ) | ( DEF_SKIP_CHECK_PHY << 5 ) | ( DEF_INTERNAL_LOOP_PHY << 4 ) | ( DEF_INIT_PHY << 3 ) | ( DEF_DIS_RECOVPHY << 2 ))
+
+#define  SET_1GBPS                               0              // 1G bps
+#define  SET_100MBPS                             1              // 100M bps
+#define  SET_10MBPS                              2              // 10M bps
+#define  SET_1G_100M_10MBPS                      3              // 1G and 100M and 10M bps
+#define  SET_100M_10MBPS                         4              // 100M and 10M bps
+#ifdef Enable_MAC_ExtLoop
+#define  DEF_GSPEED                              SET_1GBPS
+#else
+#define  DEF_GSPEED                              SET_1G_100M_10MBPS
+#endif
+#define  DEF_GARPNUMCNT                          0
+
+//---------------------------------------------------------
+// MAC information
+//---------------------------------------------------------
+#if ( AST1010_IOMAP == 1 )
+  // AST1010 only has a MAC
+  #define MAC_BASE1                              AST_MAC1_BASE
+  #define MAC_BASE2                              AST_MAC1_BASE
+  #define MAC_BASE3                              AST_MAC1_BASE
+  #define MAC_BASE4                              AST_MAC1_BASE
+#endif
+
+#if ( AST1010_IOMAP == 2 )
+  // AST1010 only has a MAC
+  #define MAC_BASE1                              0x00830000
+  #define MAC_BASE2                              0x00830000
+  #define MAC_BASE3                              0x00830000
+  #define MAC_BASE4                              0x00830000
+#endif
+
+#ifndef AST1010_IOMAP
+  #define MAC_BASE1                              0x1e660000
+  #define MAC_BASE2                              0x1e680000
+  #define MAC_BASE3                              0x1e670000
+  #define MAC_BASE4                              0x1e690000
+#endif
+
+#define MDC_Thres                                0x3f
+#define MAC_PHYWr                                0x08000000
+#define MAC_PHYRd                                0x04000000
+
+#define MAC_PHYWr_New                            0x00009400
+#define MAC_PHYRd_New                            0x00009800
+#define MAC_PHYBusy_New                          0x00008000
+
+//#define MAC_030_def                              0x00001010
+//#define MAC_034_def                              0x00000000
+//#define MAC_038_def                              0x00d22f00 //default 0x22f00
+//#define MAC_038_def                              0x00022f00 //default 0x22f00
+//#define MAC_038_def                              0x00022400 //default 0x22500 (AST2500)
+//#define MAC_040_def                              0x00000000
+
+#ifdef Enable_BufMerge
+    #define MAC_048_def                          0x007702F1 //default 0xf1
+#else
+  #ifdef AST2500_IOMAP
+    #define MAC_048_def                          0x000002F1 //default 0xf1
+  #elif defined(AST1010_IOMAP)
+    #define MAC_048_def                          0x000002F1 //default 0xf1
+  #else
+    #define MAC_048_def                          0x000002F1 //default 0xf1
+  #endif
+#endif
+//#define MAC_058_def                              0x00000040 //0x000001c0
+
+//---------------------------------------------------------
+// Data information
+//---------------------------------------------------------
+#if defined(Enable_MAC_ExtLoop) || defined(Enable_MAC_ExtLoop_PakcegMode) || defined(SelectSimpleBoundary) || defined(PHY_SPECIAL)
+      #define ZeroCopy_OFFSET                    0
+#else
+      #define ZeroCopy_OFFSET                    (( eng->run.TM_Burst ) ? 0 : 2)
+#endif
+
+//      --------------------------------- DRAM_MapAdr            = TDES_BASE
+//              | TX descriptor ring    |
+//              ------------------------- DRAM_MapAdr + 0x040000 = RDES_BASE
+//              | RX descriptor ring    |
+//              -------------------------
+//              | Reserved              |
+//              -------------------------
+//              | Reserved              |
+//      --------------------------------- DRAM_MapAdr + 0x100000 = DMA_BASE    -------------------------
+//              |   #1                  |  \                                   |     #1     Tx         |
+//  DMA buffer  |                       |   DMA_BufSize                        |      LOOP = 0         |
+// ( Tx/Rx )    -------------------------  /                                   --------------------------------------------------
+//              |   #2                  |                                      |     #2     Rx         |  #2     Tx             |
+//              |                       |                                      |      LOOP = 0         |   LOOP = 1             |
+//              -------------------------                                      --------------------------------------------------
+//              |   #3                  |                                                              |  #3     Rx             |
+//              |                       |                                                              |   LOOP = 1             |
+//              -------------------------                                                              -------------------------
+//              |   #4                  |                                                                                     ..........
+//              |                       |
+//              -------------------------
+//              |   #5                  |
+//              |                       |
+//              -------------------------
+//              |   #6                  |
+//              |                       |
+//              -------------------------
+//                           .
+//                           .
+//              -------------------------
+//              |   #n, n = DMA_BufNum  |
+//              |                       |
+//      ---------------------------------
+
+#if ( AST1010_IOMAP == 1 )
+    #define DRAM_OFS_BUF                         0x01a00000
+    #define DRAM_OFS_WINDOW                      0x00800000
+//    #define DRAM_OFS_REMAP                      (CONFIG_DRAM_SWAP_BASE & 0x7fffffff)
+    #define DRAM_OFS_REMAP                       0x01800000
+#endif
+#if ( AST1010_IOMAP == 2 )
+    #define DRAM_OFS_BUF                         0x00a00000
+    #define DRAM_OFS_WINDOW                      0x01000000
+    #define DRAM_OFS_REMAP                       0x00000000
+#endif
+#ifndef AST1010_IOMAP
+  #ifdef Enable_MAC_ExtLoop
+    #define DRAM_OFS_BUF                         0x00000000
+  #else
+    #define DRAM_OFS_BUF                         0x04000000
+    // #define DRAM_OFS_BUF                         0x06000000 // in-house setting
+  #endif
+  #ifdef AST2500_IOMAP
+    #define DRAM_OFS_WINDOW                      0x80000000
+  #else
+    #define DRAM_OFS_WINDOW                      0x40000000
+  #endif
+    #define DRAM_OFS_REMAP                       0x00000000
+#endif
+
+  #define TDES_BASE1                             ( 0x00000000 + DRAM_OFS_BUF - DRAM_OFS_REMAP + DRAM_OFS_WINDOW )
+#ifdef Enable_MAC_ExtLoop
+  #define RDES_BASE1                             ( 0x00080000 + DRAM_OFS_BUF - DRAM_OFS_REMAP + DRAM_OFS_WINDOW )
+#else
+  #define RDES_BASE1                             ( 0x00040000 + DRAM_OFS_BUF - DRAM_OFS_REMAP + DRAM_OFS_WINDOW )
+#endif
+  #define DMA_BASE                               ( 0x00100000 + DRAM_OFS_BUF - DRAM_OFS_REMAP + DRAM_OFS_WINDOW )
+
+  #define TDES_IniVal                            ( 0xb0000000 + eng->dat.FRAME_LEN_Cur )
+  #define RDES_IniVal                            ( 0x00000fff )
+  #define EOR_IniVal                             ( 0x40008000 )
+  #define HWOwnTx(dat)                           ( dat & 0x80000000      )
+  #define HWOwnRx(dat)                           ((dat & 0x80000000) == 0)
+  #define HWEOR(dat)                             ( dat & 0x40000000      )
+
+  #define AT_MEMRW_BUF( x )                      ( ( x ) + DRAM_OFS_REMAP - DRAM_OFS_WINDOW )
+  #define AT_BUF_MEMRW( x )                      ( ( x ) - DRAM_OFS_REMAP + DRAM_OFS_WINDOW )
+
+//---------------------------------------------------------
+// Error Flag Bits
+//---------------------------------------------------------
+#define Err_Flag_MACMode                              ( 1 <<  0 )   // MAC interface mode mismatch
+#define Err_Flag_PHY_Type                             ( 1 <<  1 )   // Unidentifiable PHY
+#define Err_Flag_MALLOC_FrmSize                       ( 1 <<  2 )   // Malloc fail at frame size buffer
+#define Err_Flag_MALLOC_LastWP                        ( 1 <<  3 )   // Malloc fail at last WP buffer
+#define Err_Flag_Check_Buf_Data                       ( 1 <<  4 )   // Received data mismatch
+#define Err_Flag_Check_Des                            ( 1 <<  5 )   // Descriptor error
+#define Err_Flag_NCSI_LinkFail                        ( 1 <<  6 )   // NCSI packet retry number over flows
+#define Err_Flag_NCSI_Check_TxOwnTimeOut              ( 1 <<  7 )   // Time out of checking Tx owner bit in NCSI packet
+#define Err_Flag_NCSI_Check_RxOwnTimeOut              ( 1 <<  8 )   // Time out of checking Rx owner bit in NCSI packet
+#define Err_Flag_NCSI_Check_ARPOwnTimeOut             ( 1 <<  9 )   // Time out of checking ARP owner bit in NCSI packet
+#define Err_Flag_NCSI_No_PHY                          ( 1 << 10 )   // Can not find NCSI PHY
+#define Err_Flag_NCSI_Channel_Num                     ( 1 << 11 )   // NCSI Channel Number Mismatch
+#define Err_Flag_NCSI_Package_Num                     ( 1 << 12 )   // NCSI Package Number Mismatch
+#define Err_Flag_PHY_TimeOut_RW                       ( 1 << 13 )   // Time out of read/write PHY register
+#define Err_Flag_PHY_TimeOut_Rst                      ( 1 << 14 )   // Time out of reset PHY register
+#define Err_Flag_RXBUF_UNAVA                          ( 1 << 15 )   // MAC00h[2]:Receiving buffer unavailable
+#define Err_Flag_RPKT_LOST                            ( 1 << 16 )   // MAC00h[3]:Received packet lost due to RX FIFO full
+#define Err_Flag_NPTXBUF_UNAVA                        ( 1 << 17 )   // MAC00h[6]:Normal priority transmit buffer unavailable
+#define Err_Flag_TPKT_LOST                            ( 1 << 18 )   // MAC00h[7]:Packets transmitted to Ethernet lost
+#define Err_Flag_DMABufNum                            ( 1 << 19 )   // DMA Buffer is not enough
+#define Err_Flag_IOMargin                             ( 1 << 20 )   // IO timing margin is not enough
+#define Err_Flag_IOMarginOUF                          ( 1 << 21 )   // IO timing testing out of boundary
+#define Err_Flag_MHCLK_Ratio                          ( 1 << 22 )   // Error setting of MAC AHB bus clock (SCU08[18:16])
+
+#define Wrn_Flag_IOMarginOUF                          ( 1 <<  0 )   // IO timing testing out of boundary
+#define Wrn_Flag_RxErFloatting                        ( 1 <<  1 )   // NCSI RXER pin may be floatting to the MAC
+//#define Wrn_Flag_RMIICK_IOMode                        ( 1 <<  2 )   // The PHY's RMII refreence clock input/output mode
+
+#define PHY_Flag_RMIICK_IOMode_RTL8201E               ( 1 <<  0 )
+#define PHY_Flag_RMIICK_IOMode_RTL8201F               ( 1 <<  1 )
+
+#define Des_Flag_TxOwnTimeOut                         ( 1 <<  0 )   // Time out of checking Tx owner bit
+#define Des_Flag_RxOwnTimeOut                         ( 1 <<  1 )   // Time out of checking Rx owner bit
+#define Des_Flag_FrameLen                             ( 1 <<  2 )   // Frame length mismatch
+#define Des_Flag_RxErr                                ( 1 <<  3 )   // Input signal RxErr
+#define Des_Flag_CRC                                  ( 1 <<  4 )   // CRC error of frame
+#define Des_Flag_FTL                                  ( 1 <<  5 )   // Frame too long
+#define Des_Flag_Runt                                 ( 1 <<  6 )   // Runt packet
+#define Des_Flag_OddNibble                            ( 1 <<  7 )   // Nibble bit happen
+#define Des_Flag_RxFIFOFull                           ( 1 <<  8 )   // Rx FIFO full
+
+#define NCSI_Flag_Get_Version_ID                      ( 1 <<  0 )   // Time out when Get Version ID
+#define NCSI_Flag_Get_Capabilities                    ( 1 <<  1 )   // Time out when Get Capabilities
+#define NCSI_Flag_Select_Active_Package               ( 1 <<  2 )   // Time out when Select Active Package
+#define NCSI_Flag_Enable_Set_MAC_Address              ( 1 <<  3 )   // Time out when Enable Set MAC Address
+#define NCSI_Flag_Enable_Broadcast_Filter             ( 1 <<  4 )   // Time out when Enable Broadcast Filter
+#define NCSI_Flag_Enable_Network_TX                   ( 1 <<  5 )   // Time out when Enable Network TX
+#define NCSI_Flag_Enable_Channel                      ( 1 <<  6 )   // Time out when Enable Channel
+#define NCSI_Flag_Disable_Network_TX                  ( 1 <<  7 )   // Time out when Disable Network TX
+#define NCSI_Flag_Disable_Channel                     ( 1 <<  8 )   // Time out when Disable Channel
+#define NCSI_Flag_Select_Package                      ( 1 <<  9 )   // Time out when Select Package
+#define NCSI_Flag_Deselect_Package                    ( 1 << 10 )   // Time out when Deselect Package
+#define NCSI_Flag_Set_Link                            ( 1 << 11 )   // Time out when Set Link
+#define NCSI_Flag_Get_Controller_Packet_Statistics    ( 1 << 12 )   // Time out when Get Controller Packet Statistics
+
+
+//---------------------------------------------------------
+// SCU information
+//---------------------------------------------------------
+#if ( AST1010_IOMAP == 1 )
+  #define SMB_BASE                               0x00300000
+//  #define SCU_BASE                               AST_SCU_BASE
+  #define SCU_BASE                               0x00241000
+  #define SDR_BASE                               0x00240000
+  #define WDT_BASE                               0x00243000
+  #define TIMER_BASE                             0x00242000
+  #define GPIO_BASE                              0x0024b000
+#endif
+#if ( AST1010_IOMAP == 2 )
+  #define SMB_BASE                               0x00900000
+  #define SCU_BASE                               0x00841000
+  #define SDR_BASE                               0x00840000
+  #define WDT_BASE                               0x00843000
+  #define TIMER_BASE                             0x00842000
+  #define GPIO_BASE                              0x0084b000
+#endif
+#ifndef AST1010_IOMAP
+  #define SMB_BASE                               0x1e720000
+  #define SCU_BASE                               0x1e6e2000
+  #define SDR_BASE                               0x1e6e0000
+  #define WDT_BASE                               0x1e785000
+  #define TIMER_BASE                             0x1e782000
+  #define GPIO_BASE                              0x1e780000
+#endif
+
+#define SCU_48h_AST1010                          0x00000200
+#define SCU_48h_AST2300                          0x00222255
+#define SCU_48h_AST2500                          0x00082208
+#define SCU_B8h_AST2500                          0x00082208
+#define SCU_BCh_AST2500                          0x00082208
+
+//#define SCU_80h                                  0x0000000f     //AST2300[3:0]MAC1~4 PHYLINK
+//#define SCU_88h                                  0xc0000000     //AST2300[31]MAC1 MDIO, [30]MAC1 MDC
+//#define SCU_90h                                  0x00000004     //AST2300[2 ]MAC2 MDC/MDIO
+//#define SCU_74h                                  0x06300000     //AST3000[20]MAC2 MDC/MDIO, [21]MAC2 MII, [25]MAC1 PHYLINK, [26]MAC2 PHYLINK
+
+//---------------------------------------------------------
+// DMA Buffer information
+//---------------------------------------------------------
+#ifdef FPGA
+        #define DRAM_KByteSize                   ( 56 * 1024 )
+#else
+  #ifdef AST1010_IOMAP
+        #define DRAM_KByteSize                   ( 3 * 1024 )      // DATA buffer only use 0xB00000 to 0xE00000
+  #else
+//      #define DRAM_KByteSize                     ( 18 * 1024 )
+//      #define DRAM_KByteSize                     ( 15 * 1024 )
+      #define DRAM_KByteSize                     ( 16500 )         //16.11328125 K
+  #endif
+#endif
+
+
+#ifdef Enable_Jumbo
+  #define DMA_PakSize                            ( 10 * 1024 )
+#else
+  #define DMA_PakSize                            ( 2 * 1024 ) // The size of one LAN packet
+#endif
+
+#ifdef SelectSimpleBoundary
+  #define DMA_BufSize                            (     ( ( ( ( eng->dat.Des_Num + 15 ) * DMA_PakSize ) >> 2 ) << 2 ) ) //vary by Des_Num
+#else
+  #define DMA_BufSize                            ( 4 + ( ( ( ( eng->dat.Des_Num + 15 ) * DMA_PakSize ) >> 2 ) << 2 ) ) //vary by Des_Num
+#endif
+
+#define DMA_BufNum                               ( ( DRAM_KByteSize * 1024 ) / ( eng->dat.DMABuf_Size ) )                //vary by eng->dat.Des_Num
+#define GET_DMA_BASE_SETUP                       ( DMA_BASE )
+//#define GET_DMA_BASE(x)                          ( DMA_BASE + ( ( ( ( x ) % eng->dat.DMABuf_Num ) + 1 ) * eng->dat.DMABuf_Size ) )//vary by eng->dat.Des_Num
+#define GET_DMA_BASE(x)                          ( DMA_BASE + ( ( ( ( x ) % eng->dat.DMABuf_Num ) + 1 ) * eng->dat.DMABuf_Size ) + ( ( ( x ) % 7 ) * DMA_PakSize ) )//vary by eng->dat.Des_Num
+
+#define SEED_START                               8
+#define DATA_SEED(seed)                          ( ( seed ) | (( seed + 1 ) << 16 ) )
+#define DATA_IncVal                              0x00020001
+//#define DATA_IncVal                              0x01000001     //fail
+//#define DATA_IncVal                              0x10000001     //fail
+//#define DATA_IncVal                              0x10000000     //fail
+//#define DATA_IncVal                              0x80000000     //fail
+//#define DATA_IncVal                              0x00000001     //ok
+//#define DATA_IncVal                              0x01000100     //ok
+//#define DATA_IncVal                              0x01010000     //ok
+//#define DATA_IncVal                              0x01010101     //ok
+//#define DATA_IncVal                              0x00000101     //ok
+//#define DATA_IncVal                              0x00001111     //fail
+//#define DATA_IncVal                              0x00000011     //fail
+//#define DATA_IncVal                              0x10100101     //fail
+//#define DATA_IncVal                              0xfeff0201
+//#define DATA_IncVal                              0x00010001
+#define PktByteSize                              ( ( ( ( ZeroCopy_OFFSET + eng->dat.FRAME_LEN_Cur - 1 ) >> 2 ) + 1) << 2 )
+
+//---------------------------------------------------------
+// Delay (ms)
+//---------------------------------------------------------
+//#define Delay_DesGap                             1    //off
+//#define Delay_CntMax                             40
+//#define Delay_CntMax                             1000
+//#define Delay_CntMax                             8465
+//#define Delay_CntMaxIncVal                       50000
+#define Delay_CntMaxIncVal                       47500
+
+
+//#define Delay_ChkRxOwn                           1
+//#define Delay_ChkTxOwn                           1
+
+#define Delay_PHYRst                             100
+//#define Delay_PHYRd                              5
+#define Delay_PHYRd                              1         //20150423
+
+//#define Delay_SCU                                11
+#define Delay_SCU                                1         //20150423
+#define Delay_MACRst                             1
+#define Delay_MACDump                            1
+
+//#define Delay_DES                                1
+#ifdef Enable_MAC_ExtLoop_PakcegMode
+  #define Delay_CheckData                        100
+  #define Delay_CheckData_LoopNum                100
+#else
+//  #define Delay_CheckData                        100
+//  #define Delay_CheckData_LoopNum                100
+#endif
+
+//---------------------------------------------------------
+// Time Out
+//---------------------------------------------------------
+#ifdef Enable_MAC_ExtLoop_PakcegMode
+//    #define TIME_OUT_Des_1G                      40000
+//    #define TIME_OUT_Des_100M                    400000
+//    #define TIME_OUT_Des_10M                     2000000
+    #define TIME_OUT_Des_1G                      0
+    #define TIME_OUT_Des_100M                    0
+    #define TIME_OUT_Des_10M                     0
+    #define TIME_OUT_NCSI                        0
+    #define TIME_OUT_PHY_RW                      10000
+    #define TIME_OUT_PHY_Rst                     10000
+#else
+  #if defined(USE_LPC)
+    #define TIME_OUT_Des_1G                      4         //20150529 (16x slower then DOS_PCIE)
+    #define TIME_OUT_Des_100M                    40
+    #define TIME_OUT_Des_10M                     200
+    #define TIME_OUT_NCSI                        400
+    #define TIME_OUT_PHY_RW                      1000
+    #define TIME_OUT_PHY_Rst                     10
+  #elif defined(USE_P2A)
+    #define TIME_OUT_Des_1G                      1000       //20150423
+    #define TIME_OUT_Des_100M                    2000
+    #define TIME_OUT_Des_10M                     5000
+    #define TIME_OUT_NCSI                        10000
+    #define TIME_OUT_PHY_RW                      200000
+    #define TIME_OUT_PHY_Rst                     2000
+  #elif ( AST1010_IOMAP == 1 ) /* Coldfire and uboot */
+    #define TIME_OUT_Des_1G                      40
+    #define TIME_OUT_Des_100M                    400
+    #define TIME_OUT_Des_10M                     2000
+    #define TIME_OUT_NCSI                        4000
+    #define TIME_OUT_PHY_RW                      10000
+    #define TIME_OUT_PHY_Rst                     100
+  #else
+    #define TIME_OUT_Des_1G                      10000     //400
+    #define TIME_OUT_Des_100M                    20000     //4000
+    #define TIME_OUT_Des_10M                     50000     //20000
+    #define TIME_OUT_NCSI                        100000    //40000
+    #define TIME_OUT_PHY_RW                      2000000   //100000
+    #define TIME_OUT_PHY_Rst                     20000     //1000
+  #endif
+#endif
+//#define TIME_OUT_PHY_RW                          10000
+//#define TIME_OUT_PHY_Rst                         10000
+
+////#define TIME_OUT_NCSI                            300000
+//#define TIME_OUT_NCSI                            30000     //20150423
+
+//---------------------------------------------------------
+// Others
+//---------------------------------------------------------
+#define Loop_OverFlow                            0x7fffffff
+
+//---------------------------------------------------------
+// Chip memory MAP
+//---------------------------------------------------------
+#define LITTLE_ENDIAN_ADDRESS                    0
+#define BIG_ENDIAN_ADDRESS                       1
+
+typedef struct {
+    ULONG StartAddr;
+    ULONG EndAddr;
+}  LittleEndian_Area;
+
+#if ( AST1010_IOMAP == 1 )
+  static const LittleEndian_Area LittleEndianArea[] = {
+            { AST_IO_BASE, (AST_IO_BASE + 0x000FFFFF) },
+            { 0xFFFFFFFF, 0xFFFFFFFF } // End
+            };
+#else
+  static const LittleEndian_Area LittleEndianArea[] = {
+            { 0xFFFFFFFF, 0xFFFFFFFF } // End
+            };
+#endif
+
+// ========================================================
+// For ncsi.c
+
+#define DEF_GPACKAGE2NUM                         1         // Default value
+#define DEF_GCHANNEL2NUM                         2         // Default value
+
+//---------------------------------------------------------
+// Variable
+//---------------------------------------------------------
+//NC-SI Command Packet
+typedef struct {
+//Ethernet Header
+	unsigned char        DA[6];                        // Destination Address
+	unsigned char        SA[6];                        // Source Address
+	unsigned short       EtherType;                    // DMTF NC-SI, it should be 0x88F8
+//NC-SI Control Packet
+	unsigned char        MC_ID;                        // Management Controller should set this field to 0x00
+	unsigned char        Header_Revision;              // For NC-SI 1.0 spec, this field has to set 0x01
+	unsigned char        Reserved_1;                   // Reserved has to set to 0x00
+	unsigned char        IID;                          // Instance ID
+	unsigned char        Command;
+//	unsigned char        Channel_ID;
+	unsigned char        ChID;
+	unsigned short       Payload_Length;               // Payload Length = 12 bits, 4 bits are reserved
+	unsigned long        Reserved_2;
+	unsigned long        Reserved_3;
+
+	unsigned short       Reserved_4;
+	unsigned short       Reserved_5;
+	unsigned short       Response_Code;
+	unsigned short       Reason_Code;
+	unsigned char        Payload_Data[64];
+#if !defined(SLT_UBOOT)
+}  NCSI_Command_Packet;
+#else
+}  __attribute__ ((__packed__)) NCSI_Command_Packet;
+#endif
+
+//NC-SI Response Packet
+typedef struct {
+	unsigned char        DA[6];
+	unsigned char        SA[6];
+	unsigned short       EtherType;                    //DMTF NC-SI
+//NC-SI Control Packet
+	unsigned char        MC_ID;                        //Management Controller should set this field to 0x00
+	unsigned char        Header_Revision;              //For NC-SI 1.0 spec, this field has to set 0x01
+	unsigned char        Reserved_1;                   //Reserved has to set to 0x00
+	unsigned char        IID;                          //Instance ID
+	unsigned char        Command;
+//	unsigned char        Channel_ID;
+	unsigned char        ChID;
+	unsigned short       Payload_Length;               //Payload Length = 12 bits, 4 bits are reserved
+	unsigned short       Reserved_2;
+	unsigned short       Reserved_3;
+	unsigned short       Reserved_4;
+	unsigned short       Reserved_5;
+
+	unsigned short       Response_Code;
+	unsigned short       Reason_Code;
+	unsigned char        Payload_Data[64];
+#if !defined(SLT_UBOOT)
+}  NCSI_Response_Packet;
+#else
+}  __attribute__ ((__packed__)) NCSI_Response_Packet;
+#endif
+
+typedef struct {
+	unsigned char        All_ID                                   ;//__attribute__ ((aligned (4)));
+	unsigned char        Package_ID                               ;//__attribute__ ((aligned (4)));
+	unsigned char        Channel_ID                               ;//__attribute__ ((aligned (4)));
+	unsigned long        Capabilities_Flags                       ;//__attribute__ ((aligned (4)));
+	unsigned long        Broadcast_Packet_Filter_Capabilities     ;//__attribute__ ((aligned (4)));
+	unsigned long        Multicast_Packet_Filter_Capabilities     ;//__attribute__ ((aligned (4)));
+	unsigned long        Buffering_Capabilities                   ;//__attribute__ ((aligned (4)));
+	unsigned long        AEN_Control_Support                      ;//__attribute__ ((aligned (4)));
+	unsigned char        VLAN_Filter_Count                        ;//__attribute__ ((aligned (4)));
+	unsigned char        Mixed_Filter_Count                       ;//__attribute__ ((aligned (4)));
+	unsigned char        Multicast_Filter_Count                   ;//__attribute__ ((aligned (4)));
+	unsigned char        Unicast_Filter_Count                     ;//__attribute__ ((aligned (4)));
+	unsigned char        VLAN_Mode_Support                        ;//__attribute__ ((aligned (4)));
+	unsigned char        Channel_Count                            ;//__attribute__ ((aligned (4)));
+	unsigned long        PCI_DID_VID                              ;//__attribute__ ((aligned (4)));
+	unsigned long        ManufacturerID                           ;//__attribute__ ((aligned (4)));
+} NCSI_Capability;
+typedef struct {
+	ULONG                MAC_000                       ;//__attribute__ ((aligned (4)));
+	ULONG                MAC_008                       ;//__attribute__ ((aligned (4)));
+	ULONG                MAC_00c                       ;//__attribute__ ((aligned (4)));
+	ULONG                MAC_040                       ;//__attribute__ ((aligned (4)));
+	ULONG                MAC_040_new                   ;//__attribute__ ((aligned (4)));
+	ULONG                MAC_050                       ;//__attribute__ ((aligned (4)));
+	ULONG                MAC_050_Speed                 ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_004                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_004_mix                   ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_004_rstbit                ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_004_dis                   ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_004_en                    ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_008                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_00c                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_00c_mix                   ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_00c_clkbit                ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_00c_dis                   ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_00c_en                    ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_048                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_048_mix                   ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_048_default               ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_048_check                 ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_070                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_074                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_074_mix                   ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_07c                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_080                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_088                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_090                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_09c                       ;//__attribute__ ((aligned (4)));
+#ifdef AST1010_CHIP
+	ULONG                SCU_0ac                       ;//__attribute__ ((aligned (4)));
+#endif
+#ifdef AST2500_IOMAP
+	ULONG                SCU_0b8                       ;//__attribute__ ((aligned (4)));
+	ULONG                SCU_0bc                       ;//__attribute__ ((aligned (4)));
+#endif
+	ULONG                SCU_0f0                       ;//__attribute__ ((aligned (4)));
+	ULONG                WDT_00c                       ;//__attribute__ ((aligned (4)));
+	ULONG                WDT_02c                       ;//__attribute__ ((aligned (4)));
+#ifdef AST2500_IOMAP
+	ULONG                WDT_04c                       ;//__attribute__ ((aligned (4)));
+#endif
+
+	CHAR                 SCU_oldvld                    ;//__attribute__ ((aligned (4)));
+} MAC_Register;
+typedef struct {
+	CHAR                 ASTChipType                   ;//__attribute__ ((aligned (4)));
+	CHAR                 ASTChipName[64]               ;//__attribute__ ((aligned (4)));
+	CHAR                 AST1100                       ;//__attribute__ ((aligned (4)));//Different in phy & dram initiation & dram size & RMII
+	CHAR                 AST2300                       ;//__attribute__ ((aligned (4)));
+	CHAR                 AST2400                       ;//__attribute__ ((aligned (4)));
+	CHAR                 AST1010                       ;//__attribute__ ((aligned (4)));
+	CHAR                 AST2500                       ;//__attribute__ ((aligned (4)));
+	CHAR                 AST2500A1                     ;//__attribute__ ((aligned (4)));
+
+	CHAR                 MAC_Mode                      ;//__attribute__ ((aligned (4)));
+	CHAR                 MAC1_1Gvld                    ;//__attribute__ ((aligned (4)));
+	CHAR                 MAC2_1Gvld                    ;//__attribute__ ((aligned (4)));
+	CHAR                 MAC1_RMII                     ;//__attribute__ ((aligned (4)));
+	CHAR                 MAC2_RMII                     ;//__attribute__ ((aligned (4)));
+	CHAR                 MAC_atlast_1Gvld              ;//__attribute__ ((aligned (4)));
+	CHAR                 MAC2_vld                      ;//__attribute__ ((aligned (4)));
+	CHAR                 MAC34_vld                     ;//__attribute__ ((aligned (4)));
+
+	CHAR                 MAC_1Gvld                     ;//__attribute__ ((aligned (4)));
+	CHAR                 MAC_RMII                      ;//__attribute__ ((aligned (4)));
+
+	CHAR                 MHCLK_Ratio                   ;//__attribute__ ((aligned (4)));
+
+	ULONG                VGAMode                       ;//__attribute__ ((aligned (4)));
+	char                 VGAModeVld                    ;//__attribute__ ((aligned (4)));
+} MAC_Environment;
+typedef struct {
+	ULONG                GARPNumCnt                    ;//__attribute__ ((aligned (4)));//argv     [6]
+	ULONG                GUserDVal                     ;//__attribute__ ((aligned (4)));//argv[8]
+	BYTE                 GChk_TimingBund               ;//__attribute__ ((aligned (4)));//argv[7]  [5]
+	CHAR                 GPHYADR                       ;//__attribute__ ((aligned (4)));//argv[6]
+	BYTE                 GTestMode                     ;//__attribute__ ((aligned (4)));//argv[5]  [4]
+	CHAR                 GLOOP_Str[32]                 ;//__attribute__ ((aligned (4)));//argv[4]
+	BYTE                 GCtrl                         ;//__attribute__ ((aligned (4)));//argv[3]  [7]
+	BYTE                 GSpeed                        ;//__attribute__ ((aligned (4)));//argv[2]
+	BYTE                 GChannelTolNum                ;//__attribute__ ((aligned (4)));//argv     [3]
+	BYTE                 GPackageTolNum                ;//__attribute__ ((aligned (4)));//argv     [2]
+	BYTE                 GRun_Mode                     ;//__attribute__ ((aligned (4)));//argv[1]  [1]
+
+	CHAR                 GIEEE_sel                     ;//__attribute__ ((aligned (4)));//argv[7]
+	CHAR                 GLOOP_INFINI                  ;//__attribute__ ((aligned (4)));//argv[4]
+	ULONG                GLOOP_MAX                     ;//__attribute__ ((aligned (4)));//argv[4]
+
+	CHAR                 GEn_SkipRxEr                  ;//__attribute__ ((aligned (4)));//GCtrl    [1]
+	CHAR                 GEn_PrintNCSI                 ;//__attribute__ ((aligned (4)));//GCtrl    [0]
+	CHAR                 GEn_RMIIPHY_IN                ;//__attribute__ ((aligned (4)));//GCtrl[9]
+	CHAR                 GEn_RMII_50MOut               ;//__attribute__ ((aligned (4)));//GCtrl[8] [8]
+	CHAR                 GEn_MACLoopback               ;//__attribute__ ((aligned (4)));//GCtrl[7] [7]
+	CHAR                 GEn_FullRange                 ;//__attribute__ ((aligned (4)));//GCtrl[6] [6]
+	CHAR                 GEn_SkipChkPHY                ;//__attribute__ ((aligned (4)));//GCtrl[5]
+	CHAR                 GEn_IntLoopPHY                ;//__attribute__ ((aligned (4)));//GCtrl[4]
+	CHAR                 GEn_InitPHY                   ;//__attribute__ ((aligned (4)));//GCtrl[3]
+	CHAR                 GDis_RecovPHY                 ;//__attribute__ ((aligned (4)));//GCtrl[2]
+	CHAR                 GEn_PHYAdrInv                 ;//__attribute__ ((aligned (4)));//GCtrl[1]
+	CHAR                 GEn_SinglePacket              ;//__attribute__ ((aligned (4)));//GCtrl[0]
+} MAC_Argument;
+typedef struct {
+	CHAR                 MAC_idx                       ;//__attribute__ ((aligned (4)));//GRun_Mode
+	CHAR                 MAC_idx_PHY                   ;//__attribute__ ((aligned (4)));//GRun_Mode
+	ULONG                MAC_BASE                      ;//__attribute__ ((aligned (4)));//GRun_Mode
+
+	CHAR                 Speed_1G                      ;//__attribute__ ((aligned (4)));//GSpeed
+	CHAR                 Speed_org[3]                  ;//__attribute__ ((aligned (4)));//GSpeed
+	CHAR                 Speed_sel[3]                  ;//__attribute__ ((aligned (4)));
+	CHAR                 Speed_idx                     ;//__attribute__ ((aligned (4)));
+
+	CHAR                 TM_Burst                      ;//__attribute__ ((aligned (4)));//GTestMode
+	CHAR                 TM_IEEE                       ;//__attribute__ ((aligned (4)));//GTestMode
+	CHAR                 TM_IOTiming                   ;//__attribute__ ((aligned (4)));//GTestMode
+	CHAR                 TM_IOStrength                 ;//__attribute__ ((aligned (4)));//GTestMode
+	CHAR                 TM_TxDataEn                   ;//__attribute__ ((aligned (4)));//GTestMode
+	CHAR                 TM_RxDataEn                   ;//__attribute__ ((aligned (4)));//GTestMode
+	CHAR                 TM_WaitStart                  ;//__attribute__ ((aligned (4)));//GTestMode
+	CHAR                 TM_DefaultPHY                 ;//__attribute__ ((aligned (4)));//GTestMode
+	CHAR                 TM_NCSI_DiSChannel            ;//__attribute__ ((aligned (4)));//GTestMode
+
+	BYTE                 IO_Bund                       ;//__attribute__ ((aligned (4)));
+	CHAR                 IO_MrgChk                     ;//__attribute__ ((aligned (4)));
+
+	ULONG                TDES_BASE                     ;//__attribute__ ((aligned (4)));
+	ULONG                RDES_BASE                     ;//__attribute__ ((aligned (4)));
+
+	ULONG                NCSI_TxDesBase                ;//__attribute__ ((aligned (4)));
+	ULONG                NCSI_RxDesBase                ;//__attribute__ ((aligned (4)));
+	int                  NCSI_RxTimeOutScale           ;//__attribute__ ((aligned (4)));
+
+	int                  LOOP_MAX                      ;//__attribute__ ((aligned (4)));
+	ULONG                LOOP_CheckNum                 ;//__attribute__ ((aligned (4)));
+	ULONG                CheckBuf_MBSize               ;//__attribute__ ((aligned (4)));
+	ULONG                TIME_OUT_Des                  ;//__attribute__ ((aligned (4)));
+	ULONG                TIME_OUT_Des_PHYRatio         ;//__attribute__ ((aligned (4)));
+
+	int                  Loop_ofcnt                    ;//__attribute__ ((aligned (4)));
+	int                  Loop                          ;//__attribute__ ((aligned (4)));
+	int                  Loop_rl[3]                    ;//__attribute__ ((aligned (4)));
+} MAC_Running;
+typedef struct {
+	CHAR                 SA[6]                         ;//__attribute__ ((aligned (4)));
+	CHAR                 NewMDIO                       ;//__attribute__ ((aligned (4))); //start from AST2300
+} MAC_Information;
+typedef struct {
+	ULONG                PHY_BASE                      ;//__attribute__ ((aligned (4)));
+	int                  loop_phy                      ;//__attribute__ ((aligned (4)));
+	CHAR                 default_phy                   ;//__attribute__ ((aligned (4)));
+	CHAR                 Adr                           ;//__attribute__ ((aligned (4)));
+
+	CHAR                 PHYName[64]                   ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_ID3                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_ID2                       ;//__attribute__ ((aligned (4)));
+
+	ULONG                PHY_00h                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_06h                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_09h                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_0eh                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_10h                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_11h                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_12h                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_14h                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_15h                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_18h                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_19h                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_1ch                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_1eh                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_1fh                       ;//__attribute__ ((aligned (4)));
+	ULONG                PHY_06hA[7]                   ;//__attribute__ ((aligned (4)));
+	BOOLEAN              PHYAdrValid                   ;//__attribute__ ((aligned (4)));
+
+	ULONG                RMIICK_IOMode                 ;//__attribute__ ((aligned (4)));
+} MAC_PHY;
+typedef struct {
+	CHAR                 init_done                     ;//__attribute__ ((aligned (4)));
+
+	BYTE                 Dly_MrgEn                     ;//__attribute__ ((aligned (4)));
+	CHAR                 Dly_3Regiser                  ;//__attribute__ ((aligned (4)));
+
+	ULONG                Str_reg_idx                   ;//__attribute__ ((aligned (4)));
+	BYTE                 Str_reg_Lbit                  ;//__attribute__ ((aligned (4)));
+	BYTE                 Str_reg_Hbit                  ;//__attribute__ ((aligned (4)));
+	ULONG                Str_reg_value                 ;//__attribute__ ((aligned (4)));
+	ULONG                Str_reg_mask                  ;//__attribute__ ((aligned (4)));
+	BYTE                 Str_max                       ;//__attribute__ ((aligned (4)));
+	BYTE                 Str_shf                       ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_stagebit                  ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_stage                     ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_stage_in                  ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_stage_out                 ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_step                      ;//__attribute__ ((aligned (4)));
+	ULONG                Dly_mask_bit_in               ;//__attribute__ ((aligned (4)));
+	ULONG                Dly_mask_bit_out              ;//__attribute__ ((aligned (4)));
+	ULONG                Dly_mask_pos                  ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_in_shf                    ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_out_shf                   ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_in_shf_regH               ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_out_shf_regH              ;//__attribute__ ((aligned (4)));
+	BYTE                 value_ary[64]                 ;//__attribute__ ((aligned (4)));
+#ifdef AST2500_IOMAP
+	BYTE                 Dly_stage_shf_i               ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_stage_shf_o               ;//__attribute__ ((aligned (4)));
+#endif
+
+	ULONG                Dly_reg_idx                   ;//__attribute__ ((aligned (4)));
+	ULONG                Dly_reg_value                 ;//__attribute__ ((aligned (4)));
+	char                 Dly_reg_name_tx[32]           ;//__attribute__ ((aligned (4)));
+	char                 Dly_reg_name_rx[32]           ;//__attribute__ ((aligned (4)));
+	char                 Dly_reg_name_tx_new[32]       ;//__attribute__ ((aligned (4)));
+	char                 Dly_reg_name_rx_new[32]       ;//__attribute__ ((aligned (4)));
+	ULONG                Dly_in_reg                    ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_in_reg_idx                ;//__attribute__ ((aligned (4)));
+	SCHAR                Dly_in_min                    ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_in_max                    ;//__attribute__ ((aligned (4)));
+	ULONG                Dly_out_reg                   ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_out_reg_idx               ;//__attribute__ ((aligned (4)));
+	SCHAR                Dly_out_min                   ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_out_max                   ;//__attribute__ ((aligned (4)));
+	ULONG                Dly_in_cval                   ;//__attribute__ ((aligned (4)));
+	SCHAR                Dly_in_str                    ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_in_end                    ;//__attribute__ ((aligned (4)));
+	ULONG                Dly_out_cval                  ;//__attribute__ ((aligned (4)));
+	SCHAR                Dly_out_str                   ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_out_end                   ;//__attribute__ ((aligned (4)));
+
+	BYTE                 Str_i                         ;//__attribute__ ((aligned (4)));
+	ULONG                Str_val                       ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_in                        ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_in_selval                 ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_out                       ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_out_selval                ;//__attribute__ ((aligned (4)));
+	ULONG                Dly_val                       ;//__attribute__ ((aligned (4)));
+	BYTE                 Dly_out_reg_hit               ;//__attribute__ ((aligned (4)));
+	CHAR                 Dly_result                    ;//__attribute__ ((aligned (4)));
+	CHAR                 dlymap[64][64]                ;//__attribute__ ((aligned (4)));
+} MAC_IO;
+typedef struct {
+#ifdef Enable_ShowBW
+	double               Total_frame_len               ;//__attribute__ ((aligned (8)));
+#endif
+	ULONG                Des_Num                       ;//__attribute__ ((aligned (4)));
+	ULONG                Des_Num_Org                   ;//__attribute__ ((aligned (4)));
+	ULONG                DMABuf_Size                   ;//__attribute__ ((aligned (4)));
+	ULONG                DMABuf_Num                    ;//__attribute__ ((aligned (4)));
+
+	ULONG                *FRAME_LEN                    ;//__attribute__ ((aligned (4)));
+	ULONG                FRAME_LEN_Cur                 ;//__attribute__ ((aligned (4)));
+	ULONG                *wp_lst                       ;//__attribute__ ((aligned (4)));
+	ULONG                wp_fir                        ;//__attribute__ ((aligned (4)));
+
+	ULONG                DMA_Base_Setup                 ;//__attribute__ ((aligned (4)));
+	ULONG                DMA_Base_Tx                  ;//__attribute__ ((aligned (4)));
+	ULONG                DMA_Base_Rx                   ;//__attribute__ ((aligned (4)));
+
+	ULONG                ARP_data[16]                  ;//__attribute__ ((aligned (4)));
+	ULONG                TxDes0DW                      ;//__attribute__ ((aligned (4)));
+	ULONG                RxDes0DW                      ;//__attribute__ ((aligned (4)));
+	ULONG                RxDes3DW                      ;//__attribute__ ((aligned (4)));
+
+	BYTE                 number_chl                    ;//__attribute__ ((aligned (4)));
+	BYTE                 number_pak                    ;//__attribute__ ((aligned (4)));
+	char                 NCSI_RxEr                     ;//__attribute__ ((aligned (4)));
+	ULONG                NCSI_TxDWBUF[512]             ;//__attribute__ ((aligned (4)));
+	ULONG                NCSI_RxDWBUF[512]             ;//__attribute__ ((aligned (4)));
+	char                 NCSI_CommandStr[512]          ;//__attribute__ ((aligned (4)));
+	unsigned char        *NCSI_TxByteBUF               ;//__attribute__ ((aligned (4)));
+	unsigned char        *NCSI_RxByteBUF               ;//__attribute__ ((aligned (4)));
+	unsigned char        NCSI_Payload_Data[16]         ;//__attribute__ ((aligned (4)));
+	unsigned long        Payload_Checksum_NCSI         ;//__attribute__ ((aligned (4)));
+} MAC_Data;
+typedef struct {
+	ULONG                Wrn_Flag                      ;//__attribute__ ((aligned (4)));
+	ULONG                Err_Flag                      ;//__attribute__ ((aligned (4)));
+	ULONG                Des_Flag                      ;//__attribute__ ((aligned (4)));
+	ULONG                NCSI_Flag                     ;//__attribute__ ((aligned (4)));
+	ULONG                Bak_Err_Flag                  ;//__attribute__ ((aligned (4)));
+	ULONG                Bak_NCSI_Flag                 ;//__attribute__ ((aligned (4)));
+	CHAR                 Flag_PrintEn                  ;//__attribute__ ((aligned (4)));
+	ULONG                CheckDesFail_DesNum           ;//__attribute__ ((aligned (4)));
+	CHAR                 AllFail                       ;//__attribute__ ((aligned (4)));
+} MAC_Flag;
+typedef struct {
+	MAC_Register         reg;
+	MAC_Environment      env;
+	MAC_Argument         arg;
+	MAC_Running          run;
+	MAC_Information      inf;
+	MAC_PHY              phy;
+	MAC_IO               io;
+	MAC_Data             dat;
+	MAC_Flag             flg;
+	NCSI_Command_Packet  ncsi_req;
+	NCSI_Response_Packet ncsi_rsp;
+	NCSI_Capability      ncsi_cap;
+
+	PHY_GPIOstr          GPIO;
+	PHY_BCMIMP           BCMIMP;
+#if defined(ENABLE_LOG_FILE)
+	FILE                 *fp_log;
+	FILE                 *fp_io;
+#endif
+	CHAR                 ModeSwitch;
+
+#ifdef Enable_ShowBW
+  #ifdef SLT_UBOOT
+  #else
+	double               timeused;//__attribute__ ((aligned (8)));
+  #endif
+#endif
+#ifdef SLT_UBOOT
+#elif defined(LinuxAP)
+#else
+	time_t               timestart;
+#endif
+} MAC_ENGINE;
+typedef void (* PHY_SETTING) (MAC_ENGINE *);
+typedef struct {
+	PHY_SETTING          fp_set;
+	PHY_SETTING          fp_clr;
+} PHY_ENGINE;
+
+#undef GLOBAL
+#ifdef NCSI_C
+#define GLOBAL
+#else
+#define GLOBAL    extern
+#endif
+
+GLOBAL  char phy_ncsi (MAC_ENGINE *eng);
+
+// ========================================================
+// For mactest
+
+#undef GLOBAL
+#ifdef MACTEST_C
+#define GLOBAL
+#else
+#define GLOBAL    extern
+#endif
+
+#define MODE_DEDICATED                           0x01
+#define MODE_NSCI                                0x02
+
+#ifdef SPI_BUS
+    GLOBAL ULONG             mmiobase;
+#else
+    // ( USE_P2A | USE_LPC )
+    GLOBAL  UCHAR            *mmiobase;
+    GLOBAL  ULONG            ulPCIBaseAddress;
+    GLOBAL  ULONG            ulMMIOBaseAddress;
+#endif
+
+#ifdef SLT_UBOOT
+    GLOBAL  BYTE             display_lantest_log_msg;
+#endif
+
+// ========================================================
+// For mac.c
+#undef GLOBAL
+#ifdef MAC_C
+#define GLOBAL
+#else
+#define GLOBAL    extern
+#endif
+
+#if defined(MAC_C)
+static  const  char version_name[] = VER_NAME;
+static  const  BYTE IOValue_Array_A0[16] = {8,1, 10,3, 12,5, 14,7, 0,9, 2,11, 4,13, 6,15}; // AST2300-A0
+#endif
+
+GLOBAL void    Debug_delay (void);
+GLOBAL ULONG   Read_Mem_Dat_NCSI_DD (ULONG addr);
+GLOBAL ULONG   Read_Mem_Des_NCSI_DD (ULONG addr);
+GLOBAL ULONG   Read_Mem_Dat_DD (ULONG addr);
+GLOBAL ULONG   Read_Mem_Des_DD (ULONG addr);
+GLOBAL ULONG   Read_Reg_MAC_DD (MAC_ENGINE *eng, ULONG addr);
+GLOBAL ULONG   Read_Reg_PHY_DD (MAC_ENGINE *eng, ULONG addr);
+GLOBAL ULONG   Read_Reg_SCU_DD (ULONG addr);
+GLOBAL ULONG   Read_Reg_WDT_DD (ULONG addr);
+GLOBAL ULONG   Read_Reg_SDR_DD (ULONG addr);
+GLOBAL ULONG   Read_Reg_SMB_DD (ULONG addr);
+GLOBAL ULONG   Read_Reg_TIMER_DD (ULONG addr);
+GLOBAL ULONG   Read_Reg_GPIO_DD (ULONG addr);
+GLOBAL void    Write_Mem_Dat_NCSI_DD (ULONG addr, ULONG data);
+GLOBAL void    Write_Mem_Des_NCSI_DD (ULONG addr, ULONG data);
+GLOBAL void    Write_Mem_Dat_DD (ULONG addr, ULONG data);
+GLOBAL void    Write_Mem_Des_DD (ULONG addr, ULONG data);
+GLOBAL void    Write_Reg_MAC_DD (MAC_ENGINE *eng, ULONG addr, ULONG data);
+GLOBAL void    Write_Reg_PHY_DD (MAC_ENGINE *eng, ULONG addr, ULONG data);
+GLOBAL void    Write_Reg_SCU_DD (ULONG addr, ULONG data);
+GLOBAL void    Write_Reg_WDT_DD (ULONG addr, ULONG data);
+GLOBAL void    Write_Reg_TIMER_DD (ULONG addr, ULONG data);
+GLOBAL void    Write_Reg_GPIO_DD (ULONG addr, ULONG data);
+GLOBAL void    init_iodelay (MAC_ENGINE *eng);
+GLOBAL int     get_iodelay (MAC_ENGINE *eng);
+GLOBAL void    read_scu (MAC_ENGINE *eng);
+GLOBAL void    Setting_scu (MAC_ENGINE *eng);
+GLOBAL void    PrintMode (MAC_ENGINE *eng);
+GLOBAL void    PrintPakNUm (MAC_ENGINE *eng);
+GLOBAL void    PrintChlNUm (MAC_ENGINE *eng);
+GLOBAL void    PrintTest (MAC_ENGINE *eng);
+GLOBAL void    PrintIOTimingBund (MAC_ENGINE *eng);
+GLOBAL void    PrintSpeed (MAC_ENGINE *eng);
+GLOBAL void    PrintCtrl (MAC_ENGINE *eng);
+GLOBAL void    PrintLoop (MAC_ENGINE *eng);
+GLOBAL void    PrintPHYAdr (MAC_ENGINE *eng);
+GLOBAL void    Finish_Close (MAC_ENGINE *eng);
+GLOBAL void    Calculate_LOOP_CheckNum (MAC_ENGINE *eng);
+GLOBAL char    Finish_Check (MAC_ENGINE *eng, int value);
+GLOBAL void    init_scu1 (MAC_ENGINE *eng);
+GLOBAL void    init_scu_macio (MAC_ENGINE *eng);
+GLOBAL void    init_scu_macrst (MAC_ENGINE *eng);
+GLOBAL void    init_scu_macdis (MAC_ENGINE *eng);
+GLOBAL void    init_scu_macen (MAC_ENGINE *eng);
+GLOBAL void    setup_arp (MAC_ENGINE *eng);
+GLOBAL void    TestingSetup (MAC_ENGINE *eng);
+GLOBAL void    init_scu2 (MAC_ENGINE *eng);
+GLOBAL void    init_scu3 (MAC_ENGINE *eng);
+GLOBAL void    get_mac_info (MAC_ENGINE *eng);
+GLOBAL void    init_mac (MAC_ENGINE *eng);
+GLOBAL char    TestingLoop (MAC_ENGINE *eng, ULONG loop_checknum);
+GLOBAL void    PrintIO_Line_LOG (MAC_ENGINE *eng);
+GLOBAL void    init_phy (MAC_ENGINE *eng, PHY_ENGINE *phyeng);
+GLOBAL BOOLEAN find_phyadr (MAC_ENGINE *eng);
+GLOBAL void    phy_write (MAC_ENGINE *eng, int adr, ULONG data);
+GLOBAL ULONG   phy_read (MAC_ENGINE *eng, int adr);
+GLOBAL void    phy_sel (MAC_ENGINE *eng, PHY_ENGINE *phyeng);
+GLOBAL void    recov_phy (MAC_ENGINE *eng, PHY_ENGINE *phyeng);
+GLOBAL int     FindErr (MAC_ENGINE *eng, int value);
+GLOBAL int     FindErr_Des (MAC_ENGINE *eng, int value);
+GLOBAL void    PrintIO_Header (MAC_ENGINE *eng, BYTE option);
+GLOBAL void    Print_Header (MAC_ENGINE *eng, BYTE option);
+GLOBAL void    PrintIO_LineS (MAC_ENGINE *eng, BYTE option);
+GLOBAL void    PrintIO_Line (MAC_ENGINE *eng, BYTE option);
+GLOBAL void    FPri_ErrFlag (MAC_ENGINE *eng, BYTE option);
+
+GLOBAL void init_hwtimer( void );
+GLOBAL void delay_hwtimer( USHORT msec );
+
+#ifdef SUPPORT_PHY_LAN9303
+// ========================================================
+// For LAN9303.c
+#undef GLOBAL
+#ifdef LAN9303_C
+#define GLOBAL
+#else
+#define GLOBAL    extern
+#endif
+
+GLOBAL void LAN9303(int num, int phy_adr, int speed, int int_loopback);
+#endif // SUPPORT_PHY_LAN9303
+
+// ========================================================
+// For PHYGPIO.c
+#undef GLOBAL
+#ifdef PHYGPIO_C
+#define GLOBAL
+#else
+#define GLOBAL    extern
+#endif
+
+#if defined(PHY_GPIO)
+GLOBAL void    phy_gpio_init( MAC_ENGINE *eng );
+GLOBAL void    phy_gpio_write( MAC_ENGINE *eng, int regadr, int wrdata );
+GLOBAL ULONG   phy_gpio_read( MAC_ENGINE *eng, int regadr );
+#endif
+
+// ========================================================
+// For PHYSPECIAL.c
+#undef GLOBAL
+#ifdef PHYMISC_C
+#define GLOBAL
+#else
+#define GLOBAL    extern
+#endif
+
+#ifdef PHY_SPECIAL
+GLOBAL void    special_PHY_init (MAC_ENGINE *eng);
+GLOBAL void    special_PHY_MDIO_init (MAC_ENGINE *eng);
+GLOBAL void    special_PHY_buf_init (MAC_ENGINE *eng);
+GLOBAL void    special_PHY_recov (MAC_ENGINE *eng);
+GLOBAL void    special_PHY_reg_init (MAC_ENGINE *eng);
+GLOBAL void    special_PHY_debug (MAC_ENGINE *eng);
+GLOBAL ULONG   special_PHY_FRAME_LEN (MAC_ENGINE *eng);
+GLOBAL ULONG  *special_PHY_txpkt_ptr (MAC_ENGINE *eng);
+GLOBAL ULONG  *special_PHY_rxpkt_ptr (MAC_ENGINE *eng);
+#endif
+
+#endif // End COMMINF_H
--- uboot_old/oem/ami/standalone/nettest/DEF_SPI.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/DEF_SPI.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,36 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef DEF_SPI_H
+#define DEF_SPI_H
+
+#include "TYPEDEF.H"
+#include "SWFUNC.H"
+
+typedef struct _DEVICE_PCI_INFO
+{
+    USHORT    usVendorID;
+    USHORT    usDeviceID;
+    ULONG     ulPCIConfigurationBaseAddress;
+    ULONG     ulPhysicalBaseAddress;
+    ULONG     ulMMIOBaseAddress;
+    USHORT    usRelocateIO;
+} DEVICE_PCI_INFO;
+
+//VIDEO Engine Info
+typedef struct _VIDEO_ENGINE_INFO {
+    USHORT             iEngVersion;
+    DEVICE_PCI_INFO    VGAPCIInfo;
+} VIDEO_ENGINE_INFO;
+
+BOOLEAN  GetDevicePCIInfo (VIDEO_ENGINE_INFO *VideoEngineInfo);
+
+#endif // DEF_SPI_H
--- uboot_old/oem/ami/standalone/nettest/DRAM_SPI.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/DRAM_SPI.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,79 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define DRAM_SPI_C
+static const char ThisFile[] = "DRAM_SPI.c";
+
+#include "SWFUNC.H"
+
+#ifdef SPI_BUS
+#include <stdio.h>
+#include "DEF_SPI.H"
+#include "LIB_SPI.H"
+
+VOID Set_MMIO_Base(ULONG PCI_BASE, ULONG addr)
+{
+  static ULONG MMIO_BASE = -1;
+
+  if(MMIO_BASE != (addr & 0xffff0000)){
+    if(MMIO_BASE == -1){
+      *(ULONG *)(PCI_BASE + 0xF000) = 1;
+    }
+    *(ULONG *)(PCI_BASE + 0xF004) = addr;
+    MMIO_BASE = addr & 0xffff0000;
+  }
+}
+
+VOID  MOutbm(ULONG PCI_BASE, ULONG Offset, BYTE Data)
+{
+  Set_MMIO_Base(PCI_BASE, Offset);
+  *(BYTE *)(PCI_BASE + 0x10000 + (Offset & 0xffff)) = Data;
+}
+
+VOID  MOutwm(ULONG PCI_BASE, ULONG Offset, USHORT Data)
+{
+  Set_MMIO_Base(PCI_BASE, Offset);
+  *(USHORT *)(PCI_BASE + 0x10000 + (Offset & 0xffff)) = Data;
+}
+
+VOID  MOutdwm(ULONG PCI_BASE, ULONG Offset, ULONG Data)
+{
+  Set_MMIO_Base(PCI_BASE, Offset);
+  *(ULONG *)(PCI_BASE + 0x10000 + (Offset & 0xffff)) = Data;
+}
+
+BYTE  MInbm(ULONG PCI_BASE, ULONG Offset)
+{
+  BYTE jData;
+
+  Set_MMIO_Base(PCI_BASE, Offset);
+  jData = *(BYTE *)(PCI_BASE + 0x10000 + (Offset & 0xffff));
+  return(jData);
+}
+
+USHORT  MInwm(ULONG PCI_BASE, ULONG Offset)
+{
+  USHORT usData;
+
+  Set_MMIO_Base(PCI_BASE, Offset);
+  usData = *(USHORT *)(PCI_BASE + 0x10000 + (Offset & 0xffff));
+  return(usData);
+}
+
+ULONG  MIndwm(ULONG PCI_BASE, ULONG Offset)
+{
+  ULONG ulData;
+
+  Set_MMIO_Base(PCI_BASE, Offset);
+  ulData = *(ULONG *)(PCI_BASE + 0x10000 + (Offset & 0xffff));
+  return(ulData);
+}
+#endif // End SPI_BUS
--- uboot_old/oem/ami/standalone/nettest/entry.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/entry.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,29 @@
+#include <common.h>
+#include <exports.h>
+#include "stdalone.h"
+
+/* standalone application entry point */
+
+extern cmd_map_t cmd_map[CMD_MAX];
+
+int entry(int argc, char * const argv[])
+{
+	int idx;
+
+	app_startup(argv);
+
+	if (argc < 2){
+		/* ex: go 83000000 mactest 1 1 24 1 6 0 3 */
+		printf("usage: go %x <cmdname>\n", CONFIG_STANDALONE_LOAD_ADDR);
+		return 0;
+	}
+
+	for(idx = 0; idx < CMD_MAX; idx++){
+		if((strcmp(argv[1], cmd_map[idx].cmdname) == 0)) {
+			return cmd_map[idx].cmdfunc(argc - 1, &argv[1]);
+		}
+	}
+
+	printf("cmdname not found\n");
+	return 0;
+}
--- uboot_old/oem/ami/standalone/nettest/IO.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/IO.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,381 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define IO_C
+static const char ThisFile[] = "IO.c";
+
+#include "SWFUNC.H"
+
+#if defined(LinuxAP)
+  #include <stdio.h>
+  #include <string.h>
+  #include <stdlib.h>
+  #include <stdarg.h>
+  #include <unistd.h>
+  #include <string.h>
+  #include <fcntl.h>
+  #include <pthread.h>
+  #include <sys/mman.h>
+  #if	defined(__i386__) || defined(__amd64__) 
+    #include <sys/io.h>
+  #endif	
+  #include "COMMINF.H"
+#endif
+#if defined(SLT_UBOOT)
+  #include <common.h>
+  #include <command.h>
+  #include <post.h>
+  #include <malloc.h>
+  #include <net.h>
+  #include "COMMINF.H"
+#endif
+#if defined(DOS_ALONE)
+  #include <stdlib.h>
+  #include <stdio.h>
+  #include <time.h>
+  #include <conio.h>
+  #include <dos.h>
+  #include <mem.h>
+  #include "TYPEDEF.H"
+  #include "LIB.H"
+  #include "COMMINF.H"
+#endif
+#if defined(SLT_NEW_ARCH)
+  #include <stdlib.h>
+  #include <conio.h>
+  #include "TYPEDEF.H"
+  #include "LIB.H"
+  #include "COMMINF.H"
+#endif
+
+#include "TYPEDEF.H"
+#include "IO.H"
+#include "LIB_SPI.H"
+
+#ifdef SPI_BUS
+#endif
+#if defined(USE_LPC)
+    USHORT	usLPCPort;
+#endif
+#if defined(USE_P2A)
+#endif
+
+#if defined(USE_LPC)
+//------------------------------------------------------------
+// LPC access
+//------------------------------------------------------------
+void open_aspeed_sio_password(void)
+{
+    ob (usLPCPort, 0xaa);
+
+    ob (usLPCPort, 0xa5);
+    ob (usLPCPort, 0xa5);
+}
+
+//------------------------------------------------------------
+void close_aspeed_sio_password(void)
+{
+    ob (usLPCPort, 0xaa);
+}
+
+//------------------------------------------------------------
+void enable_aspeed_LDU(BYTE jldu_number)
+{
+    ob (usLPCPort, 0x07);
+    ob ((usLPCPort + 1), jldu_number);
+    ob (usLPCPort, 0x30);
+    ob ((usLPCPort + 1), 0x01);
+}
+
+//------------------------------------------------------------
+void disable_aspeed_LDU(BYTE jldu_number)
+{
+    ob (usLPCPort, 0x07);
+    ob ((usLPCPort + 1), jldu_number);
+    ob (usLPCPort, 0x30);
+    ob ((usLPCPort + 1), 0x00);
+}
+
+//------------------------------------------------------------
+/*
+ulAddress = AHB address
+jmode = 0: byte mode
+        1: word mode
+        2: dword mode
+*/
+static ULONG lpc_read (ULONG ulAddress, BYTE jmode)
+{
+    ULONG    uldata = 0;
+    ULONG    ultemp = 0;
+    BYTE     jtemp;
+
+    //Write Address
+    ob (  usLPCPort,        0xf0);
+    ob ( (usLPCPort + 1  ), ((ulAddress & 0xff000000) >> 24));
+    ob (  usLPCPort,        0xf1);
+    ob ( (usLPCPort + 1)  , ((ulAddress & 0x00ff0000) >> 16));
+    ob (  usLPCPort,        0xf2);
+    ob ( (usLPCPort + 1),   ((ulAddress & 0x0000ff00) >> 8));
+    ob (  usLPCPort,        0xf3);
+    ob ( (usLPCPort + 1),   ulAddress & 0xff);
+
+    //Write Mode
+    ob (usLPCPort, 0xf8);
+    jtemp = ib ((usLPCPort + 1));
+    ob ((usLPCPort + 1), ((jtemp & 0xfc) | jmode));
+
+    //Fire
+    ob (usLPCPort, 0xfe);
+    jtemp = ib ((usLPCPort + 1));
+
+    //Get Data
+    switch ( jmode )
+    {
+        case 0:
+            ob (usLPCPort, 0xf7);
+            ultemp  = ib ((usLPCPort + 1));
+            uldata |= (ultemp);
+            break;
+
+        case 1:
+            ob (usLPCPort, 0xf6);
+            ultemp  = ib ((usLPCPort + 1));
+            uldata |= (ultemp << 8);
+            ob (usLPCPort, 0xf7);
+            ultemp  = ib ((usLPCPort + 1));
+            uldata |= (ultemp << 0);
+            break;
+
+        case 2:
+            ob (usLPCPort, 0xf4);
+            ultemp  = ib ((usLPCPort + 1));
+            uldata |= (ultemp << 24);
+            ob (usLPCPort, 0xf5);
+            ultemp  = ib ((usLPCPort + 1));
+            uldata |= (ultemp << 16);
+            ob (usLPCPort, 0xf6);
+            ultemp  = ib ((usLPCPort + 1));
+            uldata |= (ultemp << 8);
+            ob (usLPCPort, 0xf7);
+            ultemp  = ib ((usLPCPort + 1));
+            uldata |= ultemp;
+            break;
+    } // End switch ( jmode )
+
+    return uldata;
+} // End static ULONG lpc_read (ULONG ulAddress, BYTE jmode)
+
+//------------------------------------------------------------
+static void lpc_write (ULONG ulAddress, ULONG uldata, BYTE jmode)
+{
+    BYTE     jtemp;
+
+    //Write Address
+    ob ( usLPCPort,      0xf0);
+    ob ((usLPCPort + 1), ((ulAddress & 0xff000000) >> 24));
+    ob ( usLPCPort,      0xf1);
+    ob ((usLPCPort + 1), ((ulAddress & 0x00ff0000) >> 16));
+    ob ( usLPCPort,      0xf2);
+    ob ((usLPCPort + 1), ((ulAddress & 0x0000ff00) >> 8));
+    ob ( usLPCPort,      0xf3);
+    ob ((usLPCPort + 1), ulAddress & 0xff);
+
+    //Write Data
+    switch ( jmode )
+    {
+        case 0:
+            ob ( usLPCPort,      0xf7);
+            ob ((usLPCPort + 1), (uldata & 0xff));
+            break;
+        case 1:
+            ob ( usLPCPort,      0xf6);
+            ob ((usLPCPort + 1), ((uldata & 0xff00) >> 8));
+            ob ( usLPCPort,      0xf7);
+            ob ((usLPCPort + 1), (uldata & 0x00ff));
+            break;
+        case 2:
+            ob ( usLPCPort,      0xf4);
+            ob ((usLPCPort + 1), ((uldata & 0xff000000) >> 24));
+            ob ( usLPCPort,      0xf5);
+            ob ((usLPCPort + 1), ((uldata & 0x00ff0000) >> 16));
+            ob ( usLPCPort,      0xf6);
+            ob ((usLPCPort + 1), ((uldata & 0x0000ff00) >> 8));
+            ob ( usLPCPort,      0xf7);
+            ob ((usLPCPort + 1), uldata & 0xff);
+            break;
+    } // End switch ( jmode )
+
+    //Write Mode
+    ob (usLPCPort, 0xf8);
+    jtemp = ib ((usLPCPort + 1));
+    ob ((usLPCPort + 1), ((jtemp & 0xfc) | jmode));
+
+    //Fire
+    ob (usLPCPort, 0xfe);
+    ob ((usLPCPort + 1), 0xcf);
+
+} // End static void lpc_write (ULONG ulAddress, ULONG uldata, BYTE jmode)
+
+//------------------------------------------------------------
+static USHORT usLPCPortList[] = {0x2e, 0x4e, 0xff};
+int findlpcport(BYTE jldu_number)
+{
+    USHORT  *jLPCPortPtr;
+    ULONG   ulData;
+
+    jLPCPortPtr = usLPCPortList;
+    while (*(USHORT *)(jLPCPortPtr) != 0xff )
+    {
+        usLPCPort = *(USHORT *)(jLPCPortPtr++);
+
+        open_aspeed_sio_password();
+        enable_aspeed_LDU(0x0d);
+
+        ulData  = lpc_read( (SCU_BASE + 0x7C), 2);
+
+        if ( (ulData != 0x00000000)	&&
+             (ulData != 0xFFFFFFFF)   )
+        {
+            printf("Find LPC IO port at 0x%2x \n", usLPCPort );
+            return 1;
+        }
+
+        disable_aspeed_LDU(0x0d);
+        close_aspeed_sio_password();
+    }
+
+    //printf("[Error] Fail to find proper LPC IO Port \n");
+    return 0;
+}
+#endif // End #if defined(USE_LPC)
+
+#if defined(USE_P2A)
+//------------------------------------------------------------
+// A2P Access
+//------------------------------------------------------------
+void mm_write (ULONG addr, ULONG data, BYTE jmode)
+{
+    *((volatile ULONG *) (mmiobase + 0xF004)) = (ULONG) ((addr) & 0xFFFF0000);
+#if defined(__powerpc64__)        
+    asm("eieio");
+#endif    
+    *((volatile ULONG *) (mmiobase + 0xF000)) = (ULONG) 0x00000001;
+
+    switch ( jmode )
+    {
+        case 0:
+            *((volatile BYTE *) (mmiobase + 0x10000 + ((addr) & 0x0000FFFF))) = (BYTE) data;
+            break;
+        case 1:
+            *((volatile USHORT *) (mmiobase + 0x10000 + ((addr) & 0x0000FFFF))) = (USHORT) data;
+            break;
+        case 2:
+        default:
+            *((volatile ULONG *) (mmiobase + 0x10000 + ((addr) & 0x0000FFFF))) = data;
+            break;
+    } //switch
+}
+
+//------------------------------------------------------------
+ULONG mm_read (ULONG addr, BYTE jmode)
+{
+    *((volatile ULONG *) (mmiobase + 0xF004)) = (ULONG) ((addr) & 0xFFFF0000);
+#if defined(__powerpc64__)    
+    asm("eieio");
+#endif    
+    *((volatile ULONG *) (mmiobase + 0xF000)) = (ULONG) 0x00000001;
+    switch ( jmode )
+    {
+    case 0:
+        return ( *((volatile BYTE *) (mmiobase + 0x10000 + ((addr) & 0x0000FFFF))) );
+        break;
+    case 1:
+        return ( *((volatile USHORT *) (mmiobase + 0x10000 + ((addr) & 0x0000FFFF))) );
+        break;
+    default:
+    case 2:
+        return ( *((volatile ULONG *) (mmiobase + 0x10000 + ((addr) & 0x0000FFFF))) );
+        break;
+    } //switch
+
+    return 0;
+}
+#endif // End #if defined(USE_P2A)
+
+//------------------------------------------------------------
+// General Access API
+//------------------------------------------------------------
+#if ( defined(USE_LPC) && ( defined(LinuxAP) || defined(SLT_NEW_ARCH) ) )
+void SetLPCport( UCHAR port )
+{
+    usLPCPort = (USHORT )port;
+    printf("LPC port: [%X]\n", usLPCPort );
+}
+#endif // End #if ( defined(USE_LPC) && ( defined(LinuxAP) || defined(SLT_NEW_ARCH) ) )
+
+#ifdef SLT_UBOOT
+BYTE Check_BEorLN ( ULONG chkaddr )
+{
+    BYTE ret = BIG_ENDIAN_ADDRESS;
+    BYTE i   = 0;
+
+    do {
+        if ( LittleEndianArea[i].StartAddr == LittleEndianArea[i].EndAddr )
+            break;
+
+        if ( ( LittleEndianArea[i].StartAddr <= chkaddr ) &&
+             ( LittleEndianArea[i].EndAddr   >= chkaddr )    ) {
+            ret = LITTLE_ENDIAN_ADDRESS;
+            break;
+        }
+        i++;
+    } while ( 1 );
+
+    return ret;
+}
+#endif // End #ifdef SLT_UBOOT
+
+void WriteSOC_DD(ULONG addr, ULONG data)
+{
+#ifdef SLT_UBOOT
+    if ( Check_BEorLN( addr ) == BIG_ENDIAN_ADDRESS )
+        *(volatile unsigned long *)(addr) = cpu_to_le32(data);
+    else
+        *(volatile unsigned long *)(addr) = data;
+#else
+  #if defined(USE_LPC)
+        lpc_write(addr, data, 2);
+  #endif
+  #if defined(USE_P2A)
+        mm_write(addr, data, 2);
+  #endif
+#endif
+}
+
+//------------------------------------------------------------
+ULONG ReadSOC_DD(ULONG addr)
+{
+#if defined(SLT_UBOOT)
+    if ( Check_BEorLN( addr ) == BIG_ENDIAN_ADDRESS )
+        return le32_to_cpu(*(volatile unsigned long *) (addr));
+    else
+        return (*(volatile unsigned long *) (addr));
+#else
+  #if defined(USE_LPC)
+        return (lpc_read(addr, 2));
+  #endif
+  #if defined(USE_P2A)
+        return (mm_read(addr, 2));
+  #endif
+#endif
+    return 0;
+}
+
--- uboot_old/oem/ami/standalone/nettest/IO.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/IO.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,70 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef IO_H
+#define IO_H
+
+#include "SWFUNC.H"
+
+//
+// Macro
+//
+#if 	defined(LinuxAP)
+    #define ob(p,d) outb(d,p)
+    #define ib(p) inb(p)
+#else
+    #define ob(p,d)	outp(p,d)
+    #define ib(p) inp(p)
+#endif
+
+#if defined(LinuxAP)
+typedef enum _ACCESS_MODE {
+    P2A_MODE,
+    LPC_MODE
+} ACCESS_MODE;
+
+typedef enum _MAC_INTERFACE {
+	MDC_INTERFACE,
+	NCSI_INTERFACE
+} MAC_INTERFACE;
+
+typedef struct _MACINFO {
+	USHORT  MACInterface __attribute__ ((aligned (4)));
+	USHORT  AccessMode  __attribute__ ((aligned (4)));
+
+    // PCIe interface
+	ULONG   ulMMIOBaseAddress __attribute__ ((aligned (4)));
+	UCHAR   *ulMMIOLinear __attribute__ ((aligned (4)));
+
+	char    *argv[10] __attribute__ ((aligned (4)));
+    int     argc __attribute__ ((aligned (4)));
+
+    // LPC interface
+    UCHAR   LPC_port __attribute__ ((aligned (4)));
+} _MACInfo __attribute__ ((aligned (4)));
+
+int  MACTest(_MACInfo  *MACInfo);
+#if defined(USE_LPC)
+  void SetLPCport( UCHAR port ); // LPC infterface
+#endif
+#endif // End defined(LinuxAP)
+
+#ifdef USE_LPC
+void open_aspeed_sio_password(void);
+void enable_aspeed_LDU(BYTE jldu_number);
+int findlpcport(BYTE jldu_number);
+#endif
+
+#ifndef WINSLT
+void WriteSOC_DD(ULONG addr, ULONG data);
+ULONG ReadSOC_DD(ULONG addr);
+#endif
+#endif
--- uboot_old/oem/ami/standalone/nettest/LAN9303.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/LAN9303.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,532 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define LAN9303_C
+static const char ThisFile[] = "LAN9303.c";
+
+#include "SWFUNC.H"
+#ifdef SLT_UBOOT
+  #include "COMMINF.H"
+  #include "MAC.H"
+  #include "IO.H"
+#endif
+
+#if defined(LinuxAP) || defined(DOS_ALONE) || defined(SLT_NEW_ARCH)
+  #include "COMMINF.H"
+  #include <stdlib.h>
+  #include "IO.H"
+#endif
+
+#ifdef SUPPORT_PHY_LAN9303
+//#define LAN9303M
+#define I2C_Debug           0
+#define Print_DWRW          0
+#define Print_PHYRW         0
+#define I2C_TIMEOUT         10000000
+
+
+typedef struct {
+	ULONG      devbase;
+	ULONG      busnum;
+	ULONG      byte;
+}  LAN9303_Data;
+
+//------------------------------------------------------------
+// Basic
+//------------------------------------------------------------
+void actime(ULONG ac1, ULONG ac2, ULONG *fact, ULONG *ckh, ULONG *ckl)
+{
+        static int      divcnt;
+
+        ac1 = ac1 * 50 + 1;
+        ac2 = ac2 * 50 + 1;
+
+        divcnt = 0;
+        while (ac1 > 8 || ac2 > 8) {
+                divcnt++;
+                ac1 >>= 1;
+                ac2 >>= 1;
+        }
+
+        if (ac1 < 2  ) ac1  = 2;
+        if (ac2 < 2  ) ac2  = 2;
+        if (ac1 > ac2) ac2  = 1;
+        else           ac1 += 1;
+
+#ifdef PRINT_MSG
+        printf("Divcnt = %d, ckdiv = %d, ckh = %d, ckl = %d\n",(1<<divcnt)*(ac1+ac2),divcnt,ac1-1,ac2-1);
+        printf("CKH = %d us, CKL = %d us\n",(1<<divcnt)*ac1/50,(1<<divcnt)*ac2/50);
+#endif
+
+        *fact = divcnt;
+        *ckh  = ac1 - 1;
+        *ckl  = ac2 - 1;
+}
+
+//------------------------------------------------------------
+ULONG PollStatus(LAN9303_Data *eng)
+{
+        static ULONG    status;
+        static ULONG    cnt = 0;
+
+        do {
+                status = ReadSOC_DD( eng->devbase + 0x14 ) & 0xff;
+
+                if ( ++cnt > I2C_TIMEOUT ) {
+                        printf("\nWait1 Timeout at bus %d!\n", eng->busnum);
+                        printf("Status 14 = %08x\n", ReadSOC_DD( eng->devbase + 0x14 ));
+                        exit(0);
+                }
+        } while (status != 0);
+
+        cnt = 0;
+        do {
+                status = ReadSOC_DD( eng->devbase + 0x10 );
+                if ( ++cnt > I2C_TIMEOUT ) {
+                        printf("\nWait2 Timeout at bus %d!\n", eng->busnum);
+                        printf("Status 14 = %08x\n", ReadSOC_DD( eng->devbase + 0x14 ));
+                        exit(0);
+                }
+        } while (status == 0);
+
+        WriteSOC_DD( eng->devbase + 0x10, status );
+
+        return(status);
+}
+
+
+//------------------------------------------------------------
+ULONG writeb(LAN9303_Data *eng, ULONG start, ULONG data, ULONG stop)
+{
+        WriteSOC_DD( eng->devbase + 0x20, data);
+        WriteSOC_DD( eng->devbase + 0x14, 0x02 | start | (stop << 5) );
+        return( PollStatus( eng ) );
+}
+
+//------------------------------------------------------------
+ULONG readb(LAN9303_Data *eng, ULONG last, ULONG stop)
+{
+        static ULONG    data;
+
+        WriteSOC_DD( eng->devbase + 0x14, 0x08 | (last << 4) | (stop << 5) );
+        data = PollStatus( eng );
+
+        if (data & 0x4) {
+                data = ReadSOC_DD( eng->devbase + 0x20 );
+                return(data >> 8);
+        }
+        else {
+                return(-1);
+        }
+}
+
+//------------------------------------------------------------
+void Initial(LAN9303_Data *eng, ULONG base, ULONG ckh, ULONG ckl)
+{
+        static ULONG    ackh;
+        static ULONG    ackl;
+        static ULONG    divx;
+
+        actime(ckh, ckl, &divx, &ackh, &ackl);
+        WriteSOC_DD(base + 0x00, 0x1);
+        if (ReadSOC_DD(base + 0x00) != 0x1) {
+                printf("Controller initial fail : %x\n",base);
+                exit(0);
+        }
+        WriteSOC_DD(base + 0x04, 0x77700360 | (ackh << 16) | (ackl << 12) | divx);
+        WriteSOC_DD(base + 0x08, 0x0);
+        WriteSOC_DD(base + 0x0c, 0x0);
+        WriteSOC_DD(base + 0x10, 0xffffffff);
+        WriteSOC_DD(base + 0x14, 0x00);
+        WriteSOC_DD(base + 0x1C, 0xff0000);
+        WriteSOC_DD(base + 0x20, 0x00);
+}
+
+//------------------------------------------------------------
+void print_status(ULONG status)
+{
+        if ( status & 0x02 ) printf("Device NAK\n"      );
+        if ( status & 0x08 ) printf("Arbitration Loss\n");
+        if ( status & 0x10 ) printf("STOP\n"            );
+        if ( status & 0x20 ) printf("Abnormal STOP\n"   );
+        if ( status & 0x40 ) printf("SCL Low timeout\n" );
+}
+
+//------------------------------------------------------------
+void readme()
+{
+#ifdef LAN9303M
+        printf("LAN9303M [bus] [vir_PHY_adr] [speed] [func]\n");
+#else
+        printf("LAN9303 [bus] [vir_PHY_adr] [speed] [func]\n" );
+#endif
+        printf("[bus]         | 1~14: I2C bus number\n"       );
+        printf("[vir_PHY_adr] | 0~1: virtual PHY address\n"   );
+        printf("[speed]       | 1: 100M\n"                    );
+        printf("              | 2: 10 M\n"                    );
+        printf("[func]        | 0: external loopback\n"       );
+        printf("              | 1: internal loopback\n"       );
+}
+
+//------------------------------------------------------------
+void quit(LAN9303_Data *eng)
+{
+        WriteSOC_DD( eng->devbase + 0x14, 0x20 );
+        PollStatus( eng );
+        readme();
+}
+
+//------------------------------------------------------------
+// Double-Word Read/Write
+//------------------------------------------------------------
+ULONG I2C_DWRead(LAN9303_Data *eng, ULONG adr)
+{
+	static ULONG         status;
+	int                  i;
+        static ULONG         data_rd;
+        static ULONG         byte;
+
+        Initial( eng, eng->devbase, 10, 10 );
+
+        if ( Print_DWRW )
+            printf("RAdr %02x: ", adr);
+
+        status = writeb( eng, 1, LAN9303_I2C_ADR, 0 );
+        if ( I2C_Debug )
+            printf("R1W[%02x]%02x ", status, LAN9303_I2C_ADR);
+
+        if ( status != 0x1 ) {
+                print_status(status);
+                quit( eng );
+                exit(0);
+        }
+
+        status = writeb( eng, 0, adr, 0 );
+        if ( I2C_Debug )
+            printf("R2W[%02x]%02x ", status, adr);
+        if ( !(status & 0x1) ) {
+                print_status(status);
+                quit( eng );
+                exit(0);
+        }
+
+        status = writeb( eng, 1, LAN9303_I2C_ADR | 0x1, 0 );
+        if ( I2C_Debug )
+            printf("R3W[%02x]%02x ", status, LAN9303_I2C_ADR | 0x1);
+        if ( status != 0x1 ) {
+                print_status(status);
+                quit( eng );
+                exit(0);
+        }
+
+        if ( I2C_Debug )
+            printf("R4");
+
+        data_rd = 0;
+        for (i = 24; i >= 0; i-=8) {
+                if (i == 0) eng->byte = readb( eng, 1, 1 );
+                else        eng->byte = readb( eng, 0, 0 );
+
+                if ( I2C_Debug )
+                    printf("%02x ", eng->byte);
+                data_rd = data_rd | (eng->byte << i);
+        }
+
+        if ( Print_DWRW )
+            printf("%08x\n", data_rd);
+
+        return (data_rd);
+} // End ULONG I2C_DWRead(LAN9303_Data *eng, ULONG adr)
+
+//------------------------------------------------------------
+void I2C_DWWrite(LAN9303_Data *eng, ULONG adr, ULONG dwdata)
+{
+        static ULONG    status;
+        int             i;
+        ULONG           endx;
+
+        Initial( eng, eng->devbase, 10, 10 );
+        if ( Print_DWRW )
+            printf("WAdr %02x: ", adr);
+
+        status = writeb( eng, 1, LAN9303_I2C_ADR, 0 );
+        if ( I2C_Debug )
+            printf("W1[%02x]%02x ", status, LAN9303_I2C_ADR);
+        if ( status != 0x1 ) {
+                print_status(status);
+                quit( eng );
+                exit(0);
+        }
+        status = writeb( eng, 0, adr, 0 );
+        if ( I2C_Debug )
+            printf("W2[%02x]%02x ", status, adr);
+        if ( !(status & 0x1) ) {
+                print_status(status);
+                quit( eng );
+                exit(0);
+        }
+
+        if (I2C_Debug)
+            printf("W3");
+        endx = 0;
+        for (i = 24; i >= 0; i-=8) {
+                if (i == 0)
+                    endx = 1;
+                eng->byte   = (dwdata >> i) & 0xff;
+                status = writeb( eng, 0, eng->byte, endx);
+
+            if (I2C_Debug)
+                printf("[%02x]%02x ", status, eng->byte);
+                if (!(status & 0x1)) {
+                        print_status(status);
+                        quit( eng );
+                        exit(0);
+                }
+        }
+
+        if ( Print_DWRW ) printf("%08x\n", dwdata);
+} // End void I2C_DWWrite(LAN9303_Data *eng, ULONG adr, ULONG dwdata)
+
+//------------------------------------------------------------
+// PHY Read/Write
+//------------------------------------------------------------
+ULONG LAN9303_PHY_Read(LAN9303_Data *eng, ULONG phy_adr, ULONG reg_adr)
+{
+        static ULONG         data_rd;
+
+        I2C_DWWrite( eng, 0x2a, ((phy_adr & 0x1f) << 11) | ((reg_adr & 0x1f) << 6));//[0A8h]PMI_ACCESS
+        do {
+            data_rd = I2C_DWRead( eng, 0x2a );
+        } while(data_rd & 0x00000001);//[0A8h]PMI_ACCESS
+
+        data_rd = I2C_DWRead( eng, 0x29 );//[0A4h]PMI_DATA
+        if ( Print_PHYRW )
+            printf("PHY:%2d, Reg:%2d, Data:%08x\n", phy_adr, reg_adr, data_rd);
+
+        return(data_rd);
+}
+
+//------------------------------------------------------------
+void LAN9303_PHY_Write(LAN9303_Data *eng, ULONG phy_adr, ULONG reg_adr, ULONG data_wr)
+{
+        static ULONG         data_rd;
+
+        I2C_DWWrite( eng, 0x29, data_wr );//[0A4h]PMI_DATA
+
+        I2C_DWWrite( eng, 0x2a, ((phy_adr & 0x1f) << 11) | ((reg_adr & 0x1f) << 6) | 0x2 );//[0A8h]PMI_ACCESS
+        do {
+            data_rd = I2C_DWRead( eng, 0x2a );
+        } while( data_rd & 0x00000001 );//[0A8h]PMI_ACCESS
+}
+
+//------------------------------------------------------------
+ULONG LAN9303_PHY_Read_WD(LAN9303_Data *eng, ULONG data_ctl)
+{
+        static ULONG         data_rd;
+
+        I2C_DWWrite( eng, 0x2a, data_ctl );//[0A8h]PMI_ACCESS
+        do {
+            data_rd = I2C_DWRead( eng, 0x2a );
+        } while(data_rd & 0x00000001);//[0A8h]PMI_ACCESS
+
+        data_rd = I2C_DWRead( eng, 0x29 );//[0A4h]PMI_DATA
+        if ( Print_PHYRW )
+            printf("WD Data:%08x\n", data_ctl);
+
+        return(data_rd);
+}
+
+//------------------------------------------------------------
+void LAN9303_PHY_Write_WD(LAN9303_Data *eng, ULONG data_ctl, ULONG data_wr)
+{
+        static ULONG         data_rd;
+
+        I2C_DWWrite( eng,  0x29, data_wr  ); //[0A4h]PMI_DATA
+        I2C_DWWrite( eng,  0x2a, data_ctl ); //[0A8h]PMI_ACCESS
+        do {
+            data_rd = I2C_DWRead( eng, 0x2a );
+        } while(data_rd & 0x00000001); //[0A8h]PMI_ACCESS
+}
+
+//------------------------------------------------------------
+// Virtual PHY Read/Write
+//------------------------------------------------------------
+ULONG LAN9303_VirPHY_Read(LAN9303_Data *eng, ULONG reg_adr)
+{
+        static ULONG         data_rd;
+
+        data_rd = I2C_DWRead( eng, 0x70 + reg_adr );//[1C0h]
+        if ( Print_PHYRW )
+            printf("VirPHY Reg:%2d, Data:%08x\n", reg_adr, data_rd);
+
+        return(data_rd);
+}
+
+//------------------------------------------------------------
+void LAN9303_VirPHY_Write(LAN9303_Data *eng, ULONG reg_adr, ULONG data_wr)
+{
+        I2C_DWWrite( eng, 0x70+reg_adr, data_wr );//[1C0h]
+}
+
+//------------------------------------------------------------
+void LAN9303_VirPHY_RW(LAN9303_Data *eng, ULONG reg_adr, ULONG data_clr, ULONG data_set)
+{
+        I2C_DWWrite( eng, 0x70+reg_adr, (LAN9303_VirPHY_Read( eng, reg_adr ) & (~data_clr)) | data_set );//[1C0h]
+}
+
+//------------------------------------------------------------
+// PHY Read/Write
+//------------------------------------------------------------
+ULONG LAN9303_Read(LAN9303_Data *eng, ULONG adr)
+{
+        static ULONG         data_rd;
+
+        I2C_DWWrite( eng, 0x6c, 0xc00f0000 | adr & 0xffff );//[1B0h]SWITCH_CSR_CMD
+        do {
+            data_rd = I2C_DWRead( eng, 0x6c );
+        } while( data_rd & 0x80000000 );//[1B0h]SWITCH_CSR_CMD
+
+        return( I2C_DWRead( eng, 0x6b ) );//[1ACh]SWITCH_CSR_DATA
+}
+
+//------------------------------------------------------------
+void LAN9303_Write(LAN9303_Data *eng, ULONG adr, ULONG data)
+{
+        static ULONG         data_rd;
+
+        I2C_DWWrite( eng, 0x6b, data );//[1ACh]SWITCH_CSR_DATA
+        I2C_DWWrite( eng, 0x6c, 0x800f0000 | adr & 0xffff );//[1B0h]SWITCH_CSR_CMD
+
+        do {
+            data_rd = I2C_DWRead( eng, 0x6c );
+        } while( data_rd & 0x80000000 );//[1B0h]SWITCH_CSR_CMD
+}
+
+//------------------------------------------------------------
+void LAN9303(int num, int phy_adr, int speed, int int_loopback)
+{
+	LAN9303_Data         LAN9303_ENG;
+	LAN9303_Data         *eng;
+	static ULONG         data_rd;
+
+	eng = &LAN9303_ENG;
+    //------------------------------------------------------------
+    // I2C Initial
+    //------------------------------------------------------------
+        eng->busnum = num;
+        if (eng->busnum <= 7) eng->devbase = 0x1E78A000 + ( eng->busnum    * 0x40);
+        else                  eng->devbase = 0x1E78A300 + ((eng->busnum-8) * 0x40);
+        Initial( eng, eng->devbase, 10, 10 );
+
+    //------------------------------------------------------------
+    // LAN9303 Register Setting
+    //------------------------------------------------------------
+    printf("----> Start\n");
+        if (int_loopback == 0) {
+            //Force Speed & external loopback
+                if (speed == 1) { //100M
+                        LAN9303_VirPHY_RW( eng,  0, 0xffff, 0x2300 );      //adr clr set //VPHY_BASIC_CTRL
+                        LAN9303_VirPHY_RW( eng, 11, 0xffff, 0x2300 );      //adr clr set //P1_MII_BASIC_CONTROL
+                        LAN9303_PHY_Write( eng, phy_adr + 1, 0, 0x2300 );
+                        LAN9303_PHY_Write( eng, phy_adr + 2, 0, 0x2300 );
+                }
+                else {
+                        LAN9303_VirPHY_RW( eng,  0, 0xffff, 0x0100 );      //adr clr set //VPHY_BASIC_CTRL
+                        LAN9303_VirPHY_RW( eng, 11, 0xffff, 0x0100 );      //adr clr set //P1_MII_BASIC_CONTROL
+                        LAN9303_PHY_Write( eng, phy_adr + 1, 0, 0x0100 );
+                        LAN9303_PHY_Write( eng, phy_adr + 2, 0, 0x0100 );
+                }
+
+                LAN9303_Write( eng, 0x180c, 0x00000001 ); // SWE_VLAN_WR_DATA
+                LAN9303_Write( eng, 0x180b, 0x00000010 ); // SWE_VLAN_CMD
+                do {data_rd = LAN9303_Read( eng, 0x1810 );} while(data_rd & 0x1);
+
+                LAN9303_Write( eng, 0x180c, 0x00000002 ); // SWE_VLAN_WR_DATA
+                LAN9303_Write( eng, 0x180b, 0x00000011 ); // SWE_VLAN_CMD
+                do {data_rd = LAN9303_Read( eng, 0x1810 );} while(data_rd & 0x1);
+
+                LAN9303_Write( eng, 0x180c, 0x00000003 ); // SWE_VLAN_WR_DATA
+                LAN9303_Write( eng, 0x180b, 0x00000012 ); // SWE_VLAN_CMD
+                do {data_rd = LAN9303_Read( eng, 0x1810 );} while(data_rd & 0x1);
+
+#ifdef LAN9303M
+                LAN9303_Write( eng, 0x180c, 0x00022001 ); // SWE_VLAN_WR_DATA
+                LAN9303_Write( eng, 0x180b, 0x00000000 ); // SWE_VLAN_CMD
+                do {data_rd = LAN9303_Read( eng, 0x1810 );} while(data_rd & 0x1);
+
+                LAN9303_Write( eng, 0x180c, 0x00024002 ); // SWE_VLAN_WR_DATA
+                LAN9303_Write( eng, 0x180b, 0x00000001 ); // SWE_VLAN_CMD
+                do {data_rd = LAN9303_Read( eng, 0x1810 );} while(data_rd & 0x1);
+
+                LAN9303_Write( eng, 0x180c, 0x0002a003 ); // SWE_VLAN_WR_DATA
+                LAN9303_Write( eng, 0x180b, 0x00000002 ); // SWE_VLAN_CMD
+                do {data_rd = LAN9303_Read( eng, 0x1810 );} while(data_rd & 0x1);
+#else
+                LAN9303_Write( eng, 0x180c, 0x0002a001 ); // SWE_VLAN_WR_DATA
+                LAN9303_Write( eng, 0x180b, 0x00000000 ); // SWE_VLAN_CMD
+                do {data_rd = LAN9303_Read( eng, 0x1810 );} while(data_rd & 0x1);
+
+                LAN9303_Write( eng, 0x180c, 0x0000a002 ); // SWE_VLAN_WR_DATA
+                LAN9303_Write( eng, 0x180b, 0x00000001 ); // SWE_VLAN_CMD
+                do {data_rd = LAN9303_Read( eng, 0x1810 );} while(data_rd & 0x1);
+
+                LAN9303_Write( eng, 0x180c, 0x00022003 ); // SWE_VLAN_WR_DATA
+                LAN9303_Write( eng, 0x180b, 0x00000002 ); // SWE_VLAN_CMD
+                do {data_rd = LAN9303_Read( eng, 0x1810 );} while(data_rd & 0x1);
+#endif
+                LAN9303_Write( eng, 0x1840, 0x00000007);
+        }
+        else if ( int_loopback == 1 ) {
+            //Force Speed & internal loopback
+                if ( speed == 1 ) {
+                    //100M
+                        LAN9303_VirPHY_RW( eng,  0, 0xffff, 0x6300 ); // adr clr set //VPHY_BASIC_CTRL
+                        LAN9303_VirPHY_RW( eng, 11, 0xffff, 0x6300 ); // adr clr set //P1_MII_BASIC_CONTROL
+                        LAN9303_PHY_Write( eng, phy_adr + 1, 0, 0x6300 );
+                        LAN9303_PHY_Write( eng, phy_adr + 2, 0, 0x6300 );
+                }
+                else {
+                        LAN9303_VirPHY_RW( eng,  0, 0xffff, 0x4100 ); // adr clr set //VPHY_BASIC_CTRL
+                        LAN9303_VirPHY_RW( eng, 11, 0xffff, 0x4100 ); // adr clr set //P1_MII_BASIC_CONTROL
+                        LAN9303_PHY_Write( eng, phy_adr + 1, 0, 0x4100 );
+                        LAN9303_PHY_Write( eng, phy_adr + 2, 0, 0x4100 );
+                }
+        }
+        else {
+            //Force Speed
+                if (speed == 1) {
+                    //100M
+                        LAN9303_VirPHY_RW( eng,  0, 0xffff, 0x2300 ); // adr clr set //VPHY_BASIC_CTRL
+                        LAN9303_VirPHY_RW( eng, 11, 0xffff, 0x2300 ); // adr clr set //P1_MII_BASIC_CONTROL
+                        LAN9303_PHY_Write( eng, phy_adr + 1, 0, 0x2300 );
+                        LAN9303_PHY_Write( eng, phy_adr + 2, 0, 0x2300 );
+                }
+                else {
+                        LAN9303_VirPHY_RW( eng,  0, 0xffff, 0x0100 ); // adr clr set //VPHY_BASIC_CTRL
+                        LAN9303_VirPHY_RW( eng, 11, 0xffff, 0x0100 ); // adr clr set //P1_MII_BASIC_CONTROL
+                        LAN9303_PHY_Write( eng, phy_adr + 1, 0, 0x0100 );
+                        LAN9303_PHY_Write( eng, phy_adr + 2, 0, 0x0100 );
+                }
+#ifdef LAN9303M
+#else
+                if (int_loopback == 3) {
+                    //[LAN9303]IEEE measurement
+                        data_rd = LAN9303_PHY_Read( eng, phy_adr+1, 27 );//PHY_SPECIAL_CONTROL_STAT_IND_x
+                        LAN9303_PHY_Write( eng,phy_adr+1, 27, (data_rd & 0x9fff) | 0x8000 );//PHY_SPECIAL_CONTROL_STAT_IND_x
+
+                        data_rd = LAN9303_PHY_Read( eng, phy_adr+2, 27 );//PHY_SPECIAL_CONTROL_STAT_IND_x
+                        LAN9303_PHY_Write( eng,phy_adr+2, 27, (data_rd & 0x9fff) | 0x8000 );//PHY_SPECIAL_CONTROL_STAT_IND_x
+                }
+#endif
+        } // End if (int_loopback == 0)
+} // End void LAN9303(int num, int phy_adr, int speed, int int_loopback)
+#endif // SUPPORT_PHY_LAN9303
+
--- uboot_old/oem/ami/standalone/nettest/LIB.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/LIB.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,189 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define LIB_C
+static const char ThisFile[] = "LIB.c";
+
+#include "SWFUNC.H"
+
+#ifdef SLT_UBOOT
+  #include <common.h>
+  #include <command.h>
+#endif
+#ifdef DOS_ALONE
+  #include <stdlib.h>
+  #include <stdio.h>
+  #include <time.h>
+  #include <conio.h>
+  #include <dos.h>
+  #include <mem.h>
+#endif
+#if defined(LinuxAP)
+  #include <stdio.h>
+#endif
+
+#include "LIB.H"
+#include "TYPEDEF.H"
+
+#ifdef USE_P2A
+//------------------------------------------------------------
+// PCI
+//------------------------------------------------------------
+ULONG ReadPCIReg (ULONG ulPCIConfigAddress, BYTE jOffest, ULONG ulMask)
+{
+#ifndef Windows
+    OUTDWPORT(0xcf8, ulPCIConfigAddress + jOffest);
+
+    return (((ULONG)INDWPORT(0xcfc)) & ulMask);
+#else
+    WRITE_PORT_ULONG((PULONG)0xcf8, ulPCIConfigAddress + jOffest);
+
+    return (READ_PORT_ULONG((PULONG)0xcfc) & ulMask);
+#endif
+}
+
+//------------------------------------------------------------
+VOID WritePCIReg (ULONG ulPCIConfigAddress, BYTE jOffest, ULONG ulMask, ULONG ulData)
+{
+#ifndef Windows
+    OUTDWPORT(0xcf8, ulPCIConfigAddress + jOffest);
+    OUTDWPORT(0xcfc, (INDWPORT(0xcfc) & ulMask | ulData));
+#else
+    WRITE_PORT_ULONG((PULONG)0xcf8, ulPCIConfigAddress + jOffest);
+    WRITE_PORT_ULONG((PULONG)0xcfc, (READ_PORT_ULONG((PULONG)0xcfc) & ulMask | ulData));
+#endif
+}
+
+//------------------------------------------------------------
+ULONG FindPCIDevice (USHORT usVendorID, USHORT usDeviceID, USHORT usBusType)
+{
+//Return: ulPCIConfigAddress
+//usBusType: ACTIVE/PCI/AGP/PCI-E
+
+    ULONG   Base[256];
+    ULONG   ebx;
+    USHORT  i;
+    USHORT  j;
+
+    for (i = 0; i < 256; i++) {
+        Base[i] = 0x80000000 + 0x10000 * i;
+    }
+
+    if (usBusType == PCI)
+    {
+      ebx = 0x80000000;
+    }
+    else if (usBusType == PCIE)
+    {
+      ebx = 0x80020000;
+    }
+    else     // AGP and ACTIVE
+    {
+      ebx = 0x80010000;
+    }
+
+    if ( usBusType != ACTIVE )    //AGP, PCI, PCIE
+    {
+      for (i = 0; i < 32; i++)
+      {
+        ebx = ebx + (0x800);
+        if (((USHORT)ReadPCIReg(ebx, 0, 0xffff) == usVendorID) && ((USHORT)(ReadPCIReg(ebx, 0, 0xffff0000) >> 16) == usDeviceID))
+        {
+          return ebx;
+        }
+      }
+      return 0;
+    }
+    else     //ACTIVE
+    {
+      for (j = 0; j < 256; j++)
+      {
+        ebx = Base[j];
+        for (i = 0; i < 32; i++)
+        {
+          ebx = ebx + (0x800);
+          if (((USHORT)ReadPCIReg(ebx, 0, 0xffff) == usVendorID) && ((USHORT)(ReadPCIReg(ebx, 0, 0xffff0000) >> 16) == usDeviceID))
+          {
+            return ebx;
+          }
+        }
+      }
+      return 0;
+    }
+} // End ULONG FindPCIDevice (USHORT usVendorID, USHORT usDeviceID, USHORT usBusType)
+#endif
+//------------------------------------------------------------
+// Allocate Resource
+//------------------------------------------------------------
+#ifdef DOS_ALONE
+ULONG InitDOS32()
+{
+  union REGS regs ;
+
+  regs.w.ax = 0xee00;
+  INTFUNC(0x31, &regs, &regs) ;
+
+  if(regs.w.ax >= 0x301)    // DOS32 version >= 3.01 ?
+    return 1;
+  else
+    return 0;
+}
+
+//------------------------------------------------------------
+USHORT CheckDOS()
+{
+    union REGS  regs;
+
+    regs.w.ax = 0xeeff;
+    int386(0x31, &regs, &regs);
+    if (regs.x.eax == 0x504d4457)
+    {
+        return 0;
+    } else {
+        printf("PMODEW Init. fail\n");
+        return 1;
+    }
+}
+
+//------------------------------------------------------------
+ULONG MapPhysicalToLinear (ULONG ulBaseAddress, ULONG ulSize)
+{
+  union REGS regs;
+
+  regs.w.ax = 0x0800;                        // map physcial memory
+  regs.w.bx = ulBaseAddress >> 16;           // bx:cx = physical address
+  regs.w.cx = ulBaseAddress;
+  regs.w.si = ulSize >> 16;                  // si:di = mapped memory block size
+  regs.w.di = ulSize;
+  INTFUNC(0x31, &regs, &regs);               // int386(0x31, &regs, &regs);
+  if (regs.w.cflag == 0)
+    return (ULONG) (regs.w.bx << 16 + regs.w.cx);  // Linear Addr = bx:cx
+  else
+    return 0;
+}
+
+//------------------------------------------------------------
+USHORT FreePhysicalMapping(ULONG udwLinAddress)
+{
+    union REGS regs;
+
+    regs.w.ax = 0x0801;
+    regs.w.bx = udwLinAddress >> 16;
+    regs.w.cx = udwLinAddress & 0xFFFF;
+    int386(0x31, &regs, &regs);
+
+    if (regs.x.cflag)
+        return ((USHORT) 0);
+    else return ((USHORT) 1);
+}
+#endif
+
+
--- uboot_old/oem/ami/standalone/nettest/LIB.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/LIB.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,42 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef LIB_H
+#define LIB_H
+
+#include "TYPEDEF.H"
+#if defined(DOS_ALONE) || defined(SLT_NEW_ARCH) || defined(LinuxAP)
+  #include "COMMINF.H"
+#endif
+//
+// Macro
+//
+#define   INTFUNC              int386
+
+#define   OUTDWPORT         	outpd
+#define   INDWPORT          	inpd
+#define   OUTPUT            	outp
+#define   INPUT             	inp
+
+//
+// PCI
+//
+ULONG ReadPCIReg (ULONG ulPCIConfigAddress, BYTE jOffest, ULONG ulMask);
+ULONG FindPCIDevice (USHORT usVendorID, USHORT usDeviceID, USHORT usBusType);
+VOID  WritePCIReg (ULONG ulPCIConfigAddress, BYTE jOffest, ULONG ulMask, ULONG ulData);
+
+//
+// Map Resource
+//
+ULONG    MapPhysicalToLinear (ULONG ulBaseAddress, ULONG ulSize);
+
+
+#endif
--- uboot_old/oem/ami/standalone/nettest/LIB_SPI.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/LIB_SPI.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,24 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef LIB_SPI_H
+#define LIB_SPI_H
+
+#ifdef SPI_BUS
+    // MMIO Functions
+    VOID       MOutwm (ULONG, ULONG, USHORT);
+    VOID       MOutdwm (ULONG, ULONG, ULONG);
+    ULONG      MIndwm (ULONG, ULONG);
+
+    void spim_init(int cs);
+#endif
+
+#endif // LIB_SPI_H
--- uboot_old/oem/ami/standalone/nettest/MAC.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/MAC.c	2018-06-25 17:35:55.530288662 +0800
@@ -0,0 +1,2879 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define MAC_C
+static const char ThisFile[] = "MAC.c";
+
+#include "SWFUNC.H"
+
+#if defined(SLT_UBOOT)
+  #include <malloc.h>
+  #include <common.h>
+  #include <command.h>
+  #include "STDUBOOT.H"
+  #include "COMMINF.H"
+#endif
+#if defined(DOS_ALONE)
+  #include <stdio.h>
+  #include <stdlib.h>
+  #include <conio.h>
+  #include <string.h>
+  #include "COMMINF.H"
+#endif
+#if defined(LinuxAP)
+  #include <stdio.h>
+  #include <string.h>
+  #include "COMMINF.H"
+#endif
+#if defined(SLT_NEW_ARCH)
+  #include <stdlib.h>
+  #include "COMMINF.H"
+#endif
+
+// -------------------------------------------------------------
+const  ULONG   ARP_org_data[16] = {
+	0xffffffff,
+	0x0000ffff, // SA:00-00-
+	0x12345678, // SA:78-56-34-12
+	0x01000608, // ARP(0x0806)
+	0x04060008,
+	0x00000100, // sender MAC Address: 00 00
+	0x12345678, // sender MAC Address: 12 34 56 78
+	0xeb00a8c0, // sender IP Address:  192.168.0.235 (C0.A8.0.EB)
+	0x00000000, // target MAC Address: 00 00 00 00
+	0xa8c00000, // target MAC Address: 00 00, target IP Address:192.168
+	0x00005c00, // target IP Address:  0.92 (C0.A8.0.5C)
+//	0x00000100, // target IP Address:  0.1 (C0.A8.0.1)
+//	0x0000de00, // target IP Address:  0.222 (C0.A8.0.DE)
+	0x00000000,
+	0x00000000,
+	0x00000000,
+	0x00000000,
+	0xc68e2bd5
+};
+
+//------------------------------------------------------------
+// Read Memory
+//------------------------------------------------------------
+ULONG Read_Mem_Dat_NCSI_DD (ULONG addr) {
+	return ( SWAP_4B_LEDN_MEM( ReadSOC_DD(addr) ) );
+}
+ULONG Read_Mem_Des_NCSI_DD (ULONG addr) {
+	return ( SWAP_4B_LEDN_MEM( ReadSOC_DD(addr) ) );
+}
+ULONG Read_Mem_Dat_DD (ULONG addr) {
+	return ( SWAP_4B_LEDN_MEM( ReadSOC_DD(addr) ) );
+}
+ULONG Read_Mem_Des_DD (ULONG addr) {
+	return ( SWAP_4B_LEDN_MEM( ReadSOC_DD(addr) ) );
+}
+
+//------------------------------------------------------------
+// Read Register
+//------------------------------------------------------------
+ULONG Read_Reg_MAC_DD (MAC_ENGINE *eng, ULONG addr) {
+//printf("[RegRd-MAC] %08lx\n", eng->run.MAC_BASE + addr);
+	return ( SWAP_4B_LEDN_REG( ReadSOC_DD( eng->run.MAC_BASE + addr ) ) );
+}
+ULONG Read_Reg_PHY_DD (MAC_ENGINE *eng, ULONG addr) {
+//	ULONG	temp;
+//	temp = SWAP_4B_LEDN_REG( ReadSOC_DD( eng->phy.PHY_BASE + addr ) );
+//printf("[RegRd-PHY] %08lx = %08lx\n", eng->phy.PHY_BASE + addr, temp );
+//	return ( temp );
+	return ( SWAP_4B_LEDN_REG( ReadSOC_DD( eng->phy.PHY_BASE + addr ) ) );
+}
+ULONG Read_Reg_SCU_DD (ULONG addr) {
+	return ( SWAP_4B_LEDN_REG( ReadSOC_DD( SCU_BASE + addr ) ) );
+}
+ULONG Read_Reg_WDT_DD (ULONG addr) {
+	return ( SWAP_4B_LEDN_REG( ReadSOC_DD( WDT_BASE + addr ) ) );
+}
+ULONG Read_Reg_SDR_DD (ULONG addr) {
+	return ( SWAP_4B_LEDN_REG( ReadSOC_DD( SDR_BASE + addr ) ) );
+}
+ULONG Read_Reg_SMB_DD (ULONG addr) {
+	return ( SWAP_4B_LEDN_REG( ReadSOC_DD( SMB_BASE + addr ) ) );
+}
+ULONG Read_Reg_TIMER_DD (ULONG addr) {
+	return ( SWAP_4B_LEDN_REG( ReadSOC_DD( TIMER_BASE + addr ) ) );
+}
+ULONG Read_Reg_GPIO_DD (ULONG addr) {
+	return ( SWAP_4B_LEDN_REG( ReadSOC_DD( GPIO_BASE + addr ) ) );
+}
+
+//------------------------------------------------------------
+// Write Memory
+//------------------------------------------------------------
+void Write_Mem_Dat_NCSI_DD (ULONG addr, ULONG data) {
+	WriteSOC_DD( addr, SWAP_4B_LEDN_MEM( data ) );
+}
+void Write_Mem_Des_NCSI_DD (ULONG addr, ULONG data) {
+	WriteSOC_DD( addr, SWAP_4B_LEDN_MEM( data ) );
+}
+void Write_Mem_Dat_DD (ULONG addr, ULONG data) {
+	WriteSOC_DD( addr, SWAP_4B_LEDN_MEM( data ) );
+}
+void Write_Mem_Des_DD (ULONG addr, ULONG data) {
+	WriteSOC_DD( addr, SWAP_4B_LEDN_MEM( data ) );
+}
+
+//------------------------------------------------------------
+// Write Register
+//------------------------------------------------------------
+void Write_Reg_MAC_DD (MAC_ENGINE *eng, ULONG addr, ULONG data) {
+//printf("[RegWr-MAC] %08lx = %08lx\n", eng->run.MAC_BASE + addr, SWAP_4B_LEDN_REG( data ));
+	WriteSOC_DD( eng->run.MAC_BASE + addr, SWAP_4B_LEDN_REG( data ) );
+}
+void Write_Reg_PHY_DD (MAC_ENGINE *eng, ULONG addr, ULONG data) {
+//printf("[RegWr-PHY] %08lx = %08lx\n", eng->phy.PHY_BASE + addr, SWAP_4B_LEDN_REG( data ));
+	WriteSOC_DD( eng->phy.PHY_BASE + addr, SWAP_4B_LEDN_REG( data ) );
+}
+void Write_Reg_SCU_DD (ULONG addr, ULONG data) {
+//printf("[RegWr-SCU] %08lx = %08lx\n", SCU_BASE + addr, SWAP_4B_LEDN_REG( data ));
+	WriteSOC_DD( SCU_BASE + addr, SWAP_4B_LEDN_REG( data ) );
+}
+void Write_Reg_WDT_DD (ULONG addr, ULONG data) {
+	WriteSOC_DD( WDT_BASE + addr, SWAP_4B_LEDN_REG( data ) );
+}
+void Write_Reg_TIMER_DD (ULONG addr, ULONG data) {
+	WriteSOC_DD( TIMER_BASE + addr, SWAP_4B_LEDN_REG( data ) );
+}
+void Write_Reg_GPIO_DD (ULONG addr, ULONG data) {
+//printf("[RegWr-GPIO]%08lx: %08lx\n", GPIO_BASE + addr, SWAP_4B_LEDN_REG( data ));
+	WriteSOC_DD( GPIO_BASE + addr, SWAP_4B_LEDN_REG( data ) );
+}
+
+//------------------------------------------------------------
+// Others
+//------------------------------------------------------------
+void Debug_delay (void) {
+#ifdef DbgPrn_Enable_Debug_delay
+	GET_CAHR();
+#endif
+}
+
+//------------------------------------------------------------
+void dump_mac_ROreg (MAC_ENGINE *eng) {
+#ifdef Delay_MACDump
+	DELAY( Delay_MACDump );
+#endif
+	printf("\n");
+	printf("[MAC-H] ROReg A0h~ACh: %08lx %08lx %08lx %08lx\n", Read_Reg_MAC_DD( eng, 0xA0 ), Read_Reg_MAC_DD( eng, 0xA4 ), Read_Reg_MAC_DD( eng, 0xA8 ), Read_Reg_MAC_DD( eng, 0xAC ));
+	printf("[MAC-H] ROReg B0h~BCh: %08lx %08lx %08lx %08lx\n", Read_Reg_MAC_DD( eng, 0xB0 ), Read_Reg_MAC_DD( eng, 0xB4 ), Read_Reg_MAC_DD( eng, 0xB8 ), Read_Reg_MAC_DD( eng, 0xBC ));
+	printf("[MAC-H] ROReg C0h~C8h: %08lx %08lx %08lx      \n", Read_Reg_MAC_DD( eng, 0xC0 ), Read_Reg_MAC_DD( eng, 0xC4 ), Read_Reg_MAC_DD( eng, 0xC8 ));
+}
+
+//------------------------------------------------------------
+// IO delay
+//------------------------------------------------------------
+void init_iodelay (MAC_ENGINE *eng) {
+	int        index;
+
+#ifdef  DbgPrn_FuncHeader
+	printf("init_iodelay\n");
+	Debug_delay();
+#endif
+
+#ifdef AST2500_IOMAP
+	if ( eng->env.AST2500A1 && ( !eng->env.MAC_RMII ) )
+		eng->io.Dly_3Regiser = 1;
+	else
+		eng->io.Dly_3Regiser = 0;
+#else
+	eng->io.Dly_3Regiser = 0;
+#endif
+
+	//------------------------------
+	// IO Strength Max.
+	//------------------------------
+	//------------------------------
+	// [IO]setup Str_reg_idx
+	// [IO]setup Str_reg_value
+	// [IO]setup Str_reg_mask
+	// [IO]setup Str_max
+	// [IO]setup Str_shf
+	//------------------------------
+	// Get bit (shift) of IO driving strength register
+#ifdef AST1010_CHIP
+	//GPIOL4 ~ GPIOL6
+	eng->io.Str_reg_idx   = 0xac;
+	eng->io.Str_reg_Lbit  = 14;
+	eng->io.Str_reg_Hbit  = 15;
+	eng->io.Str_reg_value = ( eng->reg.SCU_0ac >> eng->io.Str_reg_Lbit ) & 0x3;
+	eng->io.Str_reg_mask  = ( eng->reg.SCU_0ac & 0xffff3fff );
+	eng->io.Str_max       = 1;//0~1
+	eng->io.Str_shf       = 14;
+#elif defined(AST2500_IOMAP)
+	eng->io.Str_reg_idx   = 0x90;
+	eng->io.Str_reg_Lbit  =  8;
+	eng->io.Str_reg_Hbit  = 11;
+	eng->io.Str_reg_value = ( eng->reg.SCU_090 >> eng->io.Str_reg_Lbit ) & 0xf;
+	eng->io.Str_reg_mask  = ( eng->reg.SCU_090 & 0xfffff0ff );
+	eng->io.Str_max       = 1;//0~1
+	if ( eng->env.MAC_RMII ) {
+		switch ( eng->run.MAC_idx ) {
+			case 0  : eng->io.Str_shf =  9; break;
+			case 1  : eng->io.Str_shf = 11; break;
+		}
+	}
+	else {//AST2500 RGMII only support high drive RGMIITXCK
+		if ( eng->run.TM_IOStrength )
+			printf("The RGMII driving strength testing v1.0\n");
+		switch ( eng->run.MAC_idx ) {
+			case 0  : eng->io.Str_shf =  8; break;
+			case 1  : eng->io.Str_shf = 10; break;
+		}
+	}
+#else
+	if ( eng->env.AST2400 ) {
+		eng->io.Str_reg_idx   = 0x90;
+		eng->io.Str_reg_Lbit  =  8;
+		eng->io.Str_reg_Hbit  = 11;
+		eng->io.Str_reg_value = ( eng->reg.SCU_090 >> eng->io.Str_reg_Lbit ) & 0xf;
+		eng->io.Str_reg_mask  = ( eng->reg.SCU_090 & 0xfffff0ff );
+		eng->io.Str_max       = 1;//0~1
+		switch ( eng->run.MAC_idx ) {
+			case 0  : eng->io.Str_shf =  9; break;
+			case 1  : eng->io.Str_shf = 11; break;
+		}
+	}
+	else {
+		eng->io.Str_reg_idx   = 0x90;
+		eng->io.Str_reg_Lbit  =  8;
+		eng->io.Str_reg_Hbit  = 15;
+		eng->io.Str_reg_value = ( eng->reg.SCU_090 >> eng->io.Str_reg_Lbit ) & 0xff;
+		eng->io.Str_reg_mask  = ( eng->reg.SCU_090 & 0xffff00ff );
+		eng->io.Str_max       = 3;//0~3
+		switch ( eng->run.MAC_idx ) {
+			case 0  : eng->io.Str_shf =  8; break;
+			case 1  : eng->io.Str_shf = 10; break;
+			case 2  : eng->io.Str_shf = 12; break;
+			case 3  : eng->io.Str_shf = 14; break;
+		}
+	}
+#endif
+	if ( !eng->run.TM_IOStrength )
+		eng->io.Str_max = 0;
+
+	//------------------------------
+	// IO Delay Stage/Step
+	//------------------------------
+	//------------------------------
+	// [IO]setup Dly_stagebit
+	// [IO]setup Dly_stage_in
+	// [IO]setup Dly_stage_out
+	// [IO]setup Dly_step
+	// [IO]setup Dly_mask
+	//------------------------------
+#ifdef AST2500_IOMAP
+	eng->io.Dly_stage_shf_i = (eng->arg.GEn_FullRange) ? 0 : AST2500_IOStageShiftBit_In ;
+	eng->io.Dly_stage_shf_o = (eng->arg.GEn_FullRange) ? 0 : AST2500_IOStageShiftBit_Out;
+#endif
+
+#ifdef AST2500_IOMAP
+	eng->io.Dly_stagebit  = 6;
+	eng->io.Dly_stage     =   ( 1 << eng->io.Dly_stagebit );
+	eng->io.Dly_stage_in  = ( eng->io.Dly_stage >> eng->io.Dly_stage_shf_i );
+	eng->io.Dly_stage_out = ( eng->io.Dly_stage >> eng->io.Dly_stage_shf_o );
+	eng->io.Dly_step      = AST2500_IOStageStep;
+#else
+	eng->io.Dly_stagebit  = 4;
+	eng->io.Dly_stage     = ( 1 << eng->io.Dly_stagebit );
+	eng->io.Dly_stage_in  = eng->io.Dly_stage;
+	eng->io.Dly_stage_out = eng->io.Dly_stage;
+	eng->io.Dly_step      = 1;
+#endif
+	eng->io.Dly_mask_bit_in = eng->io.Dly_stage - 1;
+	if ( eng->env.MAC_RMII )
+		eng->io.Dly_mask_bit_out = 1;
+	else
+		eng->io.Dly_mask_bit_out = eng->io.Dly_mask_bit_in;
+
+	//------------------------------
+	// IO-Delay Register Bit Position
+	//------------------------------
+	//------------------------------
+	// [IO]setup Dly_out_shf
+	// [IO]setup Dly_in_shf
+	// [IO]setup Dly_in_shf_regH
+	// [IO]setup Dly_out_shf_regH
+	//------------------------------
+#ifdef AST2500_IOMAP
+	if ( eng->env.MAC_RMII ) {
+		switch ( eng->run.MAC_idx ) {
+			case 0  : eng->io.Dly_out_shf = 24; eng->io.Dly_in_shf = 12; break;
+			case 1  : eng->io.Dly_out_shf = 25; eng->io.Dly_in_shf = 18; break;
+		}
+	}
+	else {
+		switch ( eng->run.MAC_idx ) {
+			case 0  : eng->io.Dly_out_shf =  0; eng->io.Dly_in_shf  = 12; break;
+			case 1  : eng->io.Dly_out_shf =  6; eng->io.Dly_in_shf  = 18; break;
+		}
+	} // End if ( eng->env.MAC_RMII )
+#else
+	if ( eng->env.MAC_RMII ) {
+		switch ( eng->run.MAC_idx ) {
+			case 0  : eng->io.Dly_out_shf = 24; eng->io.Dly_in_shf =  8; break;
+			case 1  : eng->io.Dly_out_shf = 25; eng->io.Dly_in_shf = 12; break;
+			case 2  : eng->io.Dly_out_shf = 26; eng->io.Dly_in_shf = 16; break;
+			case 3  : eng->io.Dly_out_shf = 27; eng->io.Dly_in_shf = 20; break;
+		}
+	}
+	else {
+		switch ( eng->run.MAC_idx ) {
+			case 0  : eng->io.Dly_out_shf =  0; eng->io.Dly_in_shf  =  8; break;
+			case 1  : eng->io.Dly_out_shf =  4; eng->io.Dly_in_shf  = 12; break;
+		}
+	} // End if ( eng->env.MAC_RMII )
+#endif
+	eng->io.Dly_in_shf_regH  = eng->io.Dly_in_shf  + eng->io.Dly_stagebit - 1;
+	eng->io.Dly_out_shf_regH = eng->io.Dly_out_shf + eng->io.Dly_stagebit - 1;
+
+	//------------------------------
+	// [IO]setup Dly_mask_pos
+	// [Reg]setup SCU_048_mix
+	//------------------------------
+	eng->io.Dly_mask_pos = ( eng->io.Dly_mask_bit_in  << eng->io.Dly_in_shf  )
+	                     | ( eng->io.Dly_mask_bit_out << eng->io.Dly_out_shf );
+	eng->reg.SCU_048_mix = eng->reg.SCU_048_mix | ( eng->reg.SCU_048_check & ( ~eng->io.Dly_mask_pos ) );
+
+	//------------------------------
+	// [IO]setup value_ary
+	//------------------------------
+	if ( eng->env.AST2300 && (eng->reg.SCU_07c == 0x01000003) ) {
+		//AST2300-A0
+		for (index = 0; index < 16; index++) {
+			eng->io.value_ary[ index ] = IOValue_Array_A0[ index ];
+		}
+	}
+	else {
+		for (index = 0; index < eng->io.Dly_stage; index++)
+			eng->io.value_ary[ index ] = index;
+	}
+
+	eng->io.init_done = 1;
+}
+
+//------------------------------------------------------------
+int get_iodelay (MAC_ENGINE *eng) {
+	int        index;
+#ifdef AST2500_IOMAP
+	int        index_max;
+#endif
+
+#ifdef  DbgPrn_FuncHeader
+	printf("get_iodelay\n");
+	Debug_delay();
+#endif
+
+	//------------------------------
+	// IO Delay Register Setting
+	//------------------------------
+	//------------------------------
+	// [IO]setup Dly_reg_idx
+	// [IO]setup Dly_reg_value
+	//------------------------------
+#ifdef AST2500_IOMAP
+	switch ( eng->run.Speed_idx ) {
+		case 0        : eng->io.Dly_reg_idx = 0x48; eng->io.Dly_reg_value = eng->reg.SCU_048; break;
+		case 1        : eng->io.Dly_reg_idx = 0xb8; eng->io.Dly_reg_value = eng->reg.SCU_0b8; break;
+		case 2        : eng->io.Dly_reg_idx = 0xbc; eng->io.Dly_reg_value = eng->reg.SCU_0bc; break;
+	}
+#else
+	eng->io.Dly_reg_idx   = 0x48;
+	eng->io.Dly_reg_value = eng->reg.SCU_048;
+#endif
+
+	//------------------------------
+	// [IO]setup Dly_reg_name_tx
+	// [IO]setup Dly_reg_name_rx
+	// [IO]setup Dly_reg_name_tx_new
+	// [IO]setup Dly_reg_name_rx_new
+	//------------------------------
+	if ( eng->env.MAC_RMII )
+		sprintf( eng->io.Dly_reg_name_tx, "Tx:SCU%2lX[   %2d]=",  eng->io.Dly_reg_idx,                           eng->io.Dly_out_shf );
+	else
+		sprintf( eng->io.Dly_reg_name_tx, "Tx:SCU%2lX[%2d:%2d]=", eng->io.Dly_reg_idx, eng->io.Dly_out_shf_regH, eng->io.Dly_out_shf );
+	sprintf( eng->io.Dly_reg_name_rx, "Rx:SCU%2lX[%2d:%2d]=", eng->io.Dly_reg_idx, eng->io.Dly_in_shf_regH,  eng->io.Dly_in_shf );
+#ifdef AST2500_IOMAP
+	if ( eng->env.MAC_RMII )
+		sprintf( eng->io.Dly_reg_name_tx_new, "Tx[   %2d]=",                            eng->io.Dly_out_shf );
+	else
+		sprintf( eng->io.Dly_reg_name_tx_new, "Tx[%2d:%2d]=", eng->io.Dly_out_shf_regH, eng->io.Dly_out_shf );
+	sprintf( eng->io.Dly_reg_name_rx_new, "Rx[%2d:%2d]=", eng->io.Dly_in_shf_regH,  eng->io.Dly_in_shf );
+#endif
+
+	//------------------------------
+	// [IO]setup Dly_in_reg
+	// [IO]setup Dly_out_reg
+	//------------------------------
+	// Get current clock delay value of TX(out) and RX(in) in the SCU48 register
+	// and setting test range
+	eng->io.Dly_in_reg  = ( eng->io.Dly_reg_value >> eng->io.Dly_in_shf  ) & eng->io.Dly_mask_bit_in;
+	eng->io.Dly_out_reg = ( eng->io.Dly_reg_value >> eng->io.Dly_out_shf ) & eng->io.Dly_mask_bit_out;
+
+	//------------------------------
+	// [IO]setup Dly_in_reg_idx
+	// [IO]setup Dly_in_min
+	// [IO]setup Dly_in_max
+	// [IO]setup Dly_out_reg_idx
+	// [IO]setup Dly_out_min
+	// [IO]setup Dly_out_max
+	//------------------------------
+	// Find the coordinate in X-Y axis
+#ifdef AST2500_IOMAP
+	index_max = ( eng->io.Dly_stage_in << eng->io.Dly_stage_shf_i );
+	for ( index = 0; index < index_max; index++ )
+#else
+	for ( index = 0; index < eng->io.Dly_stage_in; index++ )
+#endif
+		if ( eng->io.Dly_in_reg == eng->io.value_ary[ index ] ) {
+			eng->io.Dly_in_reg_idx = index;
+			eng->io.Dly_in_min     = index - ( eng->run.IO_Bund >> 1 );
+			eng->io.Dly_in_max     = index + ( eng->run.IO_Bund >> 1 );
+			break;
+		}
+#ifdef AST2500_IOMAP
+	index_max = ( eng->io.Dly_stage_out << eng->io.Dly_stage_shf_o );
+	for ( index = 0; index < index_max; index++ )
+#else
+	for ( index = 0; index < eng->io.Dly_stage_out; index++ )
+#endif
+		if ( eng->io.Dly_out_reg == eng->io.value_ary[ index ] ) {
+			eng->io.Dly_out_reg_idx = index;
+			if ( eng->env.MAC_RMII ) {
+				eng->io.Dly_out_min = index;
+				eng->io.Dly_out_max = index;
+			}
+			else {
+				eng->io.Dly_out_min = index - ( eng->run.IO_Bund >> 1 );
+				eng->io.Dly_out_max = index + ( eng->run.IO_Bund >> 1 );
+			}
+			break;
+		}
+
+	if ( eng->run.IO_MrgChk ) {
+		if ( eng->io.Dly_in_reg_idx >= eng->io.Dly_stage_in )
+			return( ( eng->flg.Err_Flag = eng->flg.Err_Flag | Err_Flag_IOMarginOUF ) );
+		if ( eng->io.Dly_out_reg_idx >= eng->io.Dly_stage_out )
+			return( ( eng->flg.Err_Flag = eng->flg.Err_Flag | Err_Flag_IOMarginOUF ) );
+#ifdef Enable_No_IOBoundary
+	if ( eng->io.Dly_in_min  <  0                     ) { eng->flg.Wrn_Flag = eng->flg.Wrn_Flag | Wrn_Flag_IOMarginOUF; eng->io.Dly_in_min  = 0                     ;}
+	if ( eng->io.Dly_in_max  >= eng->io.Dly_stage_in  ) { eng->flg.Wrn_Flag = eng->flg.Wrn_Flag | Wrn_Flag_IOMarginOUF; eng->io.Dly_in_max  = eng->io.Dly_stage_in-1;}
+
+	if ( eng->io.Dly_out_min <  0                     ) { eng->flg.Wrn_Flag = eng->flg.Wrn_Flag | Wrn_Flag_IOMarginOUF; eng->io.Dly_out_min = 0                      ;}
+	if ( eng->io.Dly_out_max >= eng->io.Dly_stage_out ) { eng->flg.Wrn_Flag = eng->flg.Wrn_Flag | Wrn_Flag_IOMarginOUF; eng->io.Dly_out_max = eng->io.Dly_stage_out-1;}
+#else
+		if ( ( eng->io.Dly_in_min < 0 ) || ( eng->io.Dly_in_max >= eng->io.Dly_stage_in ) )
+			return( ( eng->flg.Err_Flag = eng->flg.Err_Flag | Err_Flag_IOMarginOUF ) );
+
+		if ( ( eng->io.Dly_out_min < 0 ) || ( eng->io.Dly_out_max >= eng->io.Dly_stage_out ) )
+			return( ( eng->flg.Err_Flag = eng->flg.Err_Flag | Err_Flag_IOMarginOUF ) );
+#endif
+	}
+
+	//------------------------------
+	// IO Delay Testing Boundary
+	//------------------------------
+	//------------------------------
+	// [IO]setup Dly_in_cval
+	// [IO]setup Dly_out_cval
+	// [IO]setup Dly_in_str
+	// [IO]setup Dly_in_end
+	// [IO]setup Dly_out_str
+	// [IO]setup Dly_out_end
+	//------------------------------
+	// Get the range for testmargin block
+	if ( eng->run.TM_IOTiming ) {
+		eng->io.Dly_in_cval  = eng->io.Dly_step;
+		eng->io.Dly_out_cval = eng->io.Dly_step;
+		eng->io.Dly_in_str   = 0;
+		eng->io.Dly_in_end   = eng->io.Dly_stage_in-1;
+		eng->io.Dly_out_str  = 0;
+		if ( eng->env.MAC_RMII )
+			eng->io.Dly_out_end  = 1;
+		else
+			eng->io.Dly_out_end  = eng->io.Dly_stage_out-1;
+	}
+	else if ( eng->run.IO_Bund ) {
+		eng->io.Dly_in_cval  = eng->io.Dly_step;
+		eng->io.Dly_out_cval = eng->io.Dly_step;
+		eng->io.Dly_in_str   = eng->io.Dly_in_min;
+		eng->io.Dly_in_end   = eng->io.Dly_in_max;
+		eng->io.Dly_out_str  = eng->io.Dly_out_min;
+		eng->io.Dly_out_end  = eng->io.Dly_out_max;
+	}
+	else {
+		eng->io.Dly_in_cval  = 1;
+		eng->io.Dly_out_cval = 1;
+		eng->io.Dly_in_str   = 0;
+		eng->io.Dly_in_end   = 0;
+		eng->io.Dly_out_str  = 0;
+		eng->io.Dly_out_end  = 0;
+	} // End if ( eng->run.TM_IOTiming )
+
+	return(0);
+}
+
+//------------------------------------------------------------
+// SCU
+//------------------------------------------------------------
+void recov_scu (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("recov_scu\n");
+	Debug_delay();
+#endif
+
+	//MAC
+//	Write_Reg_MAC_DD( eng, 0x08, eng->reg.MAC_008 );
+//	Write_Reg_MAC_DD( eng, 0x0c, eng->reg.MAC_00c );
+//	Write_Reg_MAC_DD( eng, 0x40, eng->reg.MAC_040 );
+
+	//SCU
+//	Write_Reg_SCU_DD( 0x004, eng->reg.SCU_004 );
+	Write_Reg_SCU_DD( 0x008, eng->reg.SCU_008 );
+//	Write_Reg_SCU_DD( 0x00c, eng->reg.SCU_00c );
+	Write_Reg_SCU_DD( 0x048, eng->reg.SCU_048 );
+//#if defined(SLT_UBOOT) || defined(Enable_MAC_ExtLoop)
+//#elif ( AST1010_IOMAP == 1 )
+//#else
+//  #ifdef AST2500_IOMAP
+//	Write_Reg_SCU_DD( 0x07c, (~eng->reg.SCU_070) );
+//  #endif
+//	Write_Reg_SCU_DD( 0x070, eng->reg.SCU_070  );
+//#endif
+	Write_Reg_SCU_DD( 0x074, eng->reg.SCU_074 );
+	Write_Reg_SCU_DD( 0x080, eng->reg.SCU_080 );
+	Write_Reg_SCU_DD( 0x088, eng->reg.SCU_088 );
+	Write_Reg_SCU_DD( 0x090, eng->reg.SCU_090 );
+//	Write_Reg_SCU_DD( 0x09c, eng->reg.SCU_09c );
+#ifdef AST1010_CHIP
+	Write_Reg_SCU_DD( 0x0ac, eng->reg.SCU_0ac );
+#endif
+#ifdef AST2500_IOMAP
+	Write_Reg_SCU_DD( 0x0b8, eng->reg.SCU_0b8 );
+	Write_Reg_SCU_DD( 0x0bc, eng->reg.SCU_0bc );
+#endif
+
+	//WDT
+//	Write_Reg_WDT_DD( 0x00c, eng->reg.WDT_00c );
+//	Write_Reg_WDT_DD( 0x02c, eng->reg.WDT_02c );
+#ifdef AST2500_IOMAP
+//	Write_Reg_WDT_DD( 0x04c, eng->reg.WDT_04c );
+#endif
+
+} // End void recov_scu (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void read_scu (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("read_scu\n");
+	Debug_delay();
+#endif
+
+	if ( !eng->reg.SCU_oldvld ) {
+		//SCU
+		eng->reg.SCU_004 = Read_Reg_SCU_DD( 0x004 );
+		eng->reg.SCU_008 = Read_Reg_SCU_DD( 0x008 );
+		eng->reg.SCU_00c = Read_Reg_SCU_DD( 0x00c );
+		eng->reg.SCU_048 = Read_Reg_SCU_DD( 0x048 );
+		eng->reg.SCU_070 = Read_Reg_SCU_DD( 0x070 );
+		eng->reg.SCU_074 = Read_Reg_SCU_DD( 0x074 );
+		eng->reg.SCU_07c = Read_Reg_SCU_DD( 0x07c );
+		eng->reg.SCU_080 = Read_Reg_SCU_DD( 0x080 );
+		eng->reg.SCU_088 = Read_Reg_SCU_DD( 0x088 );
+		eng->reg.SCU_090 = Read_Reg_SCU_DD( 0x090 );
+		eng->reg.SCU_09c = Read_Reg_SCU_DD( 0x09c );
+#ifdef AST1010_CHIP
+		eng->reg.SCU_0ac = Read_Reg_SCU_DD( 0x0ac );
+#endif
+#ifdef AST2500_IOMAP
+		eng->reg.SCU_0b8 = Read_Reg_SCU_DD( 0x0b8 );
+		eng->reg.SCU_0bc = Read_Reg_SCU_DD( 0x0bc );
+#endif
+		eng->reg.SCU_0f0 = Read_Reg_SCU_DD( 0x0f0 );
+
+		//WDT
+		eng->reg.WDT_00c = Read_Reg_WDT_DD( 0x00c );
+		eng->reg.WDT_02c = Read_Reg_WDT_DD( 0x02c );
+#ifdef AST2500_IOMAP
+		eng->reg.WDT_04c = Read_Reg_WDT_DD( 0x04c );
+#endif
+
+		eng->reg.SCU_oldvld = 1;
+	} // End if ( !eng->reg.SCU_oldvld )
+} // End read_scu(MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void Setting_scu (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("Setting_scu\n");
+	Debug_delay();
+#endif
+
+	//------------------------------
+	// [SCU]Disable CPU
+	//------------------------------
+#if defined(SLT_UBOOT) || defined(Enable_MAC_ExtLoop)
+#else
+  #ifdef AST1010_CHIP
+	do {
+		Write_Reg_SCU_DD( 0x00 , 0x1688a8a8 );
+		Write_Reg_SCU_DD( 0x70 , eng->reg.SCU_070 & 0xfffffffe );  // Disable CPU
+	} while ( Read_Reg_SCU_DD( 0x00 ) != 0x1 );
+  #elif defined(AST2500_IOMAP)
+	do {
+		Write_Reg_SCU_DD( 0x00, 0x1688a8a8 );
+		Write_Reg_SCU_DD( 0x70, 0x3 ); // Disable CPU
+	} while ( Read_Reg_SCU_DD( 0x00 ) != 0x1 );
+  #else
+	do {
+		Write_Reg_SCU_DD( 0x00, 0x1688a8a8 );
+		Write_Reg_SCU_DD( 0x70, eng->reg.SCU_070 | 0x3 ); // Disable CPU
+	} while ( Read_Reg_SCU_DD( 0x00 ) != 0x1 );
+  #endif
+#endif
+
+	//------------------------------
+	// [WDT]Disable Timer
+	//------------------------------
+#ifdef AST1010_CHIP
+	Write_Reg_SCU_DD( 0x9c, eng->reg.SCU_09c & 0xffffffdf ); //[5] Watchdog Reset for MAC
+#else
+	if ( eng->env.AST2400 ) {
+		Write_Reg_SCU_DD( 0x9c, eng->reg.SCU_09c & 0xffffff9f ); //[5:6]Watchdog Reset for MAC
+	}
+#endif
+
+	Write_Reg_WDT_DD( 0x00c, eng->reg.WDT_00c & 0xfffffffc );
+	Write_Reg_WDT_DD( 0x02c, eng->reg.WDT_02c & 0xfffffffc );
+#ifdef AST2500_IOMAP
+	Write_Reg_WDT_DD( 0x04c, eng->reg.WDT_04c & 0xfffffffc );
+
+	Write_Reg_WDT_DD( 0x01c, Read_Reg_WDT_DD( 0x01c ) & 0xffffff9f );
+	Write_Reg_WDT_DD( 0x03c, Read_Reg_WDT_DD( 0x03c ) & 0xffffff9f );
+	Write_Reg_WDT_DD( 0x05c, Read_Reg_WDT_DD( 0x05c ) & 0xffffff9f );
+#endif
+
+	//------------------------------
+	// [SCU]Disable Cache
+	//------------------------------
+#ifdef AST1010_CHIP
+  #if( AST1010_IOMAP == 1 )
+	Write_Reg_SCU_DD( 0x11C, 0x00000000 ); // Disable Cache functionn
+  #endif
+#endif
+}
+
+//------------------------------------------------------------
+void init_scu1 (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("init_scu1\n");
+	Debug_delay();
+#endif
+
+	if ( eng->ModeSwitch == MODE_DEDICATED )
+		init_scu_macio ( eng );
+
+#ifndef AST1010_CHIP
+	if ( eng->env.AST2300 ) {
+  #ifdef Enable_BufMerge
+		Write_Reg_SCU_DD( 0xf0, 0x66559959 );//MAC buffer merge
+  #endif
+  #ifdef Enable_Int125MHz
+  #endif
+	}
+	else {
+	} // End if ( eng->env.AST2300 )
+#endif /* End AST1010_CHIP */
+} // End void init_scu1 (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void init_scu_macio (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("init_scu_macio\n");
+	Debug_delay();
+#endif
+//------------------------------
+// MDC/MDIO, LINK
+//------------------------------
+#ifdef AST1010_CHIP
+  #if defined(PHY_GPIO)
+	Write_Reg_SCU_DD( 0x88, ((eng->reg.SCU_088 & 0x003fffff ) | 0xff000000) );//Multi-function Pin Control //[22]MDC, [23]MDIO
+  #else
+	Write_Reg_SCU_DD( 0x88, ((eng->reg.SCU_088 & 0x003fffff ) | 0xffc00000) );//Multi-function Pin Control //[22]MDC, [23]MDIO
+  #endif
+#else
+	if ( eng->env.AST2300 ) {
+		switch ( eng->run.MAC_idx_PHY ) {
+  #if defined(PHY_GPIO)
+			case 0  : Write_Reg_SCU_DD( 0x88, (eng->reg.SCU_088 & 0x3fffffff)              ); break;//[31]MAC1 MDIO, [30]MAC1 MDC
+			case 1  : Write_Reg_SCU_DD( 0x90, (eng->reg.SCU_090 & 0xfffffffb)              ); break;//[2 ]MAC2 MDC/MDIO
+  #else
+			case 0  : Write_Reg_SCU_DD( 0x88, (eng->reg.SCU_088 & 0x3fffffff) | 0xc0000000 ); break;//[31]MAC1 MDIO, [30]MAC1 MDC
+			case 1  : Write_Reg_SCU_DD( 0x90, (eng->reg.SCU_090 & 0xfffffffb) | 0x00000004 ); break;//[2 ]MAC2 MDC/MDIO
+  #endif
+			default : break;
+		}
+//		Write_Reg_SCU_DD( 0x80, (eng->reg.SCU_080 & 0xfffffff0) | 0x0000000f );//MAC1LINK/MAC2LINK
+	}
+	else {
+		switch ( eng->run.MAC_idx_PHY ) {
+//			case 0  :
+//				eng->reg.SCU_074_mix = (eng->reg.SCU_074_mix & 0xfdffffff) | 0x02000000;//[25]MAC1 PHYLINK
+//				break;
+			case 1  :
+  #if defined(PHY_GPIO)
+//				eng->reg.SCU_074_mix = (eng->reg.SCU_074_mix & 0xfbefffff) | 0x04000000;//[26]MAC2 PHYLINK, [20]MAC2 MDC/MDIO
+				eng->reg.SCU_074_mix = (eng->reg.SCU_074_mix & 0xffefffff)             ;//[26]MAC2 PHYLINK, [20]MAC2 MDC/MDIO
+  #else
+//				eng->reg.SCU_074_mix = (eng->reg.SCU_074_mix & 0xfbefffff) | 0x04100000;//[26]MAC2 PHYLINK, [20]MAC2 MDC/MDIO
+				eng->reg.SCU_074_mix = (eng->reg.SCU_074_mix & 0xffefffff) | 0x00100000;//[26]MAC2 PHYLINK, [20]MAC2 MDC/MDIO
+  #endif
+				break;
+			default : 
+				break;
+		} // End switch ( eng->run.MAC_idx_PHY )
+		//------------------------------
+		// MAC2 MII Interface
+		//------------------------------
+		switch ( eng->run.MAC_idx ) {
+			case 1  :
+				if ( eng->env.MAC2_RMII )
+					eng->reg.SCU_074_mix = (eng->reg.SCU_074_mix & 0xffdfffff)             ;//[21]MAC2 MII
+				else
+					eng->reg.SCU_074_mix = (eng->reg.SCU_074_mix & 0xffdfffff) | 0x00200000;//[21]MAC2 MII
+			default :
+				break;
+		} // End switch ( eng->run.MAC_idx )
+		Write_Reg_SCU_DD( 0x74, eng->reg.SCU_074_mix);
+	} // End if ( eng->env.AST2300 )
+#endif
+} // End void init_scu_macio (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void init_scu_macrst (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("init_scu_macrst\n");
+	Debug_delay();
+#endif
+
+	Write_Reg_SCU_DD( 0x04, eng->reg.SCU_004_dis );//Rst
+#ifdef Delay_SCU
+	DELAY( Delay_SCU );
+#endif
+	Write_Reg_SCU_DD( 0x04, eng->reg.SCU_004_en );//Enable Engine
+
+#ifndef AST2500_IOMAP //MAC40h is SCU reset before AST2500
+  #ifdef MAC_040_def
+	Write_Reg_MAC_DD( eng, 0x40, eng->reg.MAC_040_new | MAC_040_def );
+  #else
+	Write_Reg_MAC_DD( eng, 0x40, eng->reg.MAC_040_new );
+  #endif
+#endif
+} // End void init_scu_macrst (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void init_scu_macdis (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("init_scu_macdis\n");
+	Debug_delay();
+#endif
+
+	Write_Reg_SCU_DD( 0x04, eng->reg.SCU_004_dis );//Rst
+	Read_Reg_SCU_DD( 0x04 );//delay
+
+	Write_Reg_SCU_DD( 0x0c, eng->reg.SCU_00c_dis );//Clock
+	Read_Reg_SCU_DD( 0x0c );//delay
+} // End void init_scu_macdis (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void init_scu_macen (MAC_ENGINE *eng) {
+
+#ifdef  DbgPrn_FuncHeader
+	printf("init_scu_macen\n");
+	Debug_delay();
+#endif
+
+	Read_Reg_SCU_DD( 0x0c );//delay
+	Write_Reg_SCU_DD( 0x0c, eng->reg.SCU_00c_en );//Clock
+	Read_Reg_SCU_DD( 0x0c );//delay
+
+	Read_Reg_SCU_DD( 0x04 );//delay
+	Write_Reg_SCU_DD( 0x04, eng->reg.SCU_004_en );//Enable Engine
+	Read_Reg_SCU_DD( 0x04 );//delay
+
+#ifndef AST2500_IOMAP
+  #ifdef MAC_040_def
+	Write_Reg_MAC_DD( eng, 0x40, eng->reg.MAC_040_new | MAC_040_def );
+  #else
+	Write_Reg_MAC_DD( eng, 0x40, eng->reg.MAC_040_new );
+  #endif
+#endif
+} // End void init_scu_macrst (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void init_scu2 (MAC_ENGINE *eng) {
+#ifdef SCU_74h
+  #ifdef  DbgPrn_FuncHeader
+	printf("init_scu2\n");
+	Debug_delay();
+  #endif
+
+	Write_Reg_SCU_DD( 0x74, eng->reg.SCU_074 | SCU_74h );//PinMux
+  #ifdef Delay_SCU
+	DELAY( Delay_SCU );
+  #endif
+#endif
+} // End void init_scu2 (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void init_scu3 (MAC_ENGINE *eng) {
+#ifdef SCU_74h
+  #ifdef  DbgPrn_FuncHeader
+	printf("init_scu3\n");
+	Debug_delay();
+  #endif
+
+	Write_Reg_SCU_DD( 0x74, eng->reg.SCU_074 | (SCU_74h & 0xffefffff) );//PinMux
+  #ifdef Delay_SCU
+	DELAY( Delay_SCU );
+  #endif
+#endif
+} // End void init_scu3 (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+// MAC
+//------------------------------------------------------------
+void get_mac_info (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("get_mac_info\n");
+	Debug_delay();
+#endif
+
+	//------------------------------
+	// [Inf]setup SA
+	//------------------------------
+	eng->reg.MAC_008 = Read_Reg_MAC_DD( eng, 0x08 );
+	eng->reg.MAC_00c = Read_Reg_MAC_DD( eng, 0x0c );
+	if (  (( eng->reg.MAC_008 == 0x0000 ) && ( eng->reg.MAC_00c == 0x00000000 ))
+	   || (( eng->reg.MAC_008 == 0xffff ) && ( eng->reg.MAC_00c == 0xffffffff ))
+	   )
+	{
+//		eng->reg.MAC_008 = 0x00000057;//MSB(0x00)
+//		eng->reg.MAC_00c = 0x89568838;//LSB(0x38)
+		eng->reg.MAC_008 = 0x0000000a;//MSB(0x00)
+		eng->reg.MAC_00c = 0xf7837dd4;//LSB(0xd4)
+	}
+
+#if defined( MELLANOX_CONNECTX_4 )
+	eng->reg.MAC_008 = 0x00000000;//MSB(0x00)  20170523
+	eng->reg.MAC_00c = 0x00000000;//LSB(0xd4)	20170523
+
+	Write_Reg_MAC_DD( eng, 0x08, eng->reg.MAC_008 ); // 20170523
+	Write_Reg_MAC_DD( eng, 0x0c, eng->reg.MAC_00c ); // 20170523
+#endif
+
+	eng->inf.SA[ 0 ] = ( eng->reg.MAC_008 >>  8 ) & 0xff;//MSB
+	eng->inf.SA[ 1 ] = ( eng->reg.MAC_008       ) & 0xff;
+	eng->inf.SA[ 2 ] = ( eng->reg.MAC_00c >> 24 ) & 0xff;
+	eng->inf.SA[ 3 ] = ( eng->reg.MAC_00c >> 16 ) & 0xff;
+	eng->inf.SA[ 4 ] = ( eng->reg.MAC_00c >>  8 ) & 0xff;
+	eng->inf.SA[ 5 ] = ( eng->reg.MAC_00c       ) & 0xff;//LSB
+
+	//------------------------------
+	// [Reg]setup MAC_040_new
+	//------------------------------
+	eng->reg.MAC_040 = Read_Reg_MAC_DD( eng, 0x40 );
+	if ( eng->arg.GEn_MACLoopback )
+		eng->reg.MAC_040_new = eng->reg.MAC_040 | 0x40000000;
+	else
+		eng->reg.MAC_040_new = eng->reg.MAC_040;
+
+#ifdef AST2500_IOMAP
+  #ifdef MAC_040_def
+	Write_Reg_MAC_DD( eng, 0x40, eng->reg.MAC_040_new | MAC_040_def );
+  #else
+	Write_Reg_MAC_DD( eng, 0x40, eng->reg.MAC_040_new );
+  #endif
+#endif
+}
+
+//------------------------------------------------------------
+void init_mac (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("init_mac\n");
+	Debug_delay();
+#endif
+
+#ifdef Enable_MAC_SWRst
+	Write_Reg_MAC_DD( eng, 0x50, 0x80000000 | eng->reg.MAC_050_Speed );
+
+	while (0x80000000 & Read_Reg_MAC_DD( eng, 0x50 )) {
+//printf(".");
+  #ifdef Delay_MACRst
+		DELAY( Delay_MACRst );
+  #endif
+	}
+  #ifdef Delay_MACRst
+	DELAY( Delay_MACRst );
+  #endif
+#endif
+
+//	Write_Reg_MAC_DD( eng, 0x20, ( eng->run.TDES_BASE + CPU_BUS_ADDR_SDRAM_OFFSET ) ); // 20130730
+//	Write_Reg_MAC_DD( eng, 0x24, ( eng->run.RDES_BASE + CPU_BUS_ADDR_SDRAM_OFFSET ) ); // 20130730
+	Write_Reg_MAC_DD( eng, 0x20, AT_MEMRW_BUF( eng->run.TDES_BASE ) ); // 20130730
+	Write_Reg_MAC_DD( eng, 0x24, AT_MEMRW_BUF( eng->run.RDES_BASE ) ); // 20130730
+
+//#ifdef AST2500_IOMAP
+//#else
+	Write_Reg_MAC_DD( eng, 0x08, eng->reg.MAC_008 );
+	Write_Reg_MAC_DD( eng, 0x0c, eng->reg.MAC_00c );
+//#endif
+#ifdef MAC_030_def
+	Write_Reg_MAC_DD( eng, 0x30, MAC_030_def );//Int Thr/Cnt
+#endif
+#ifdef MAC_034_def
+	Write_Reg_MAC_DD( eng, 0x34, MAC_034_def );//Poll Cnt
+#endif
+#ifdef MAC_038_def
+	Write_Reg_MAC_DD( eng, 0x38, MAC_038_def );
+#endif
+#ifdef MAC_048_def
+	Write_Reg_MAC_DD( eng, 0x48, MAC_048_def );
+#endif
+#ifdef MAC_058_def
+  #ifdef AST2500_IOMAP
+	Write_Reg_MAC_DD( eng, 0x58, MAC_058_def );
+  #endif
+#endif
+
+	if ( eng->ModeSwitch == MODE_NSCI )
+		Write_Reg_MAC_DD( eng, 0x4c, NCSI_RxDMA_PakSize );
+	else
+		Write_Reg_MAC_DD( eng, 0x4c, DMA_PakSize );
+
+	Write_Reg_MAC_DD( eng, 0x50, eng->reg.MAC_050_Speed );
+#ifdef Delay_MACRst
+#endif
+	DELAY( Delay_MACRst );
+} // End void init_mac (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+// Basic
+//------------------------------------------------------------
+void FPri_RegValue (MAC_ENGINE *eng, BYTE option) {
+#ifdef SLT_UBOOT
+#else
+	time_t     timecur;
+#endif
+#ifdef  DbgPrn_FuncHeader
+	printf("FPri_RegValue\n");
+	Debug_delay();
+#endif
+
+#ifdef AST2500_IOMAP
+	PRINTF( option, "[SDR] Date:%08lx\n", Read_Reg_SDR_DD( 0x88 ) );
+	PRINTF( option, "[SDR]  80:%08lx %08lx %08lx %08lx\n", Read_Reg_SDR_DD( 0x80 ), Read_Reg_SDR_DD( 0x84 ), Read_Reg_SDR_DD( 0x88 ), Read_Reg_SDR_DD( 0x8c ) );
+#else
+	if ( eng->env.AST2300 ) {
+		PRINTF( option, "[SMB] Date:%08lx\n", Read_Reg_SMB_DD( 0xa8 ) );
+		PRINTF( option, "[SMB]  00:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0x00 ), Read_Reg_SMB_DD( 0x04 ), Read_Reg_SMB_DD( 0x08 ), Read_Reg_SMB_DD( 0x0c ) );
+		PRINTF( option, "[SMB]  10:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0x10 ), Read_Reg_SMB_DD( 0x14 ), Read_Reg_SMB_DD( 0x18 ), Read_Reg_SMB_DD( 0x1c ) );
+		PRINTF( option, "[SMB]  20:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0x20 ), Read_Reg_SMB_DD( 0x24 ), Read_Reg_SMB_DD( 0x28 ), Read_Reg_SMB_DD( 0x2c ) );
+		PRINTF( option, "[SMB]  30:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0x30 ), Read_Reg_SMB_DD( 0x34 ), Read_Reg_SMB_DD( 0x38 ), Read_Reg_SMB_DD( 0x3c ) );
+		PRINTF( option, "[SMB]  40:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0x40 ), Read_Reg_SMB_DD( 0x44 ), Read_Reg_SMB_DD( 0x48 ), Read_Reg_SMB_DD( 0x4c ) );
+		PRINTF( option, "[SMB]  50:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0x50 ), Read_Reg_SMB_DD( 0x54 ), Read_Reg_SMB_DD( 0x58 ), Read_Reg_SMB_DD( 0x5c ) );
+		PRINTF( option, "[SMB]  60:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0x60 ), Read_Reg_SMB_DD( 0x64 ), Read_Reg_SMB_DD( 0x68 ), Read_Reg_SMB_DD( 0x6c ) );
+		PRINTF( option, "[SMB]  70:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0x70 ), Read_Reg_SMB_DD( 0x74 ), Read_Reg_SMB_DD( 0x78 ), Read_Reg_SMB_DD( 0x7c ) );
+		PRINTF( option, "[SMB]  80:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0x80 ), Read_Reg_SMB_DD( 0x84 ), Read_Reg_SMB_DD( 0x88 ), Read_Reg_SMB_DD( 0x8c ) );
+		PRINTF( option, "[SMB]  90:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0x90 ), Read_Reg_SMB_DD( 0x94 ), Read_Reg_SMB_DD( 0x98 ), Read_Reg_SMB_DD( 0x9c ) );
+		PRINTF( option, "[SMB]  A0:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0xa0 ), Read_Reg_SMB_DD( 0xa4 ), Read_Reg_SMB_DD( 0xa8 ), Read_Reg_SMB_DD( 0xac ) );
+		PRINTF( option, "[SMB]  B0:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0xb0 ), Read_Reg_SMB_DD( 0xb4 ), Read_Reg_SMB_DD( 0xb8 ), Read_Reg_SMB_DD( 0xbc ) );
+		PRINTF( option, "[SMB]  C0:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0xc0 ), Read_Reg_SMB_DD( 0xc4 ), Read_Reg_SMB_DD( 0xc8 ), Read_Reg_SMB_DD( 0xcc ) );
+		PRINTF( option, "[SMB]  D0:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0xd0 ), Read_Reg_SMB_DD( 0xd4 ), Read_Reg_SMB_DD( 0xd8 ), Read_Reg_SMB_DD( 0xdc ) );
+		PRINTF( option, "[SMB]  E0:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0xe0 ), Read_Reg_SMB_DD( 0xe4 ), Read_Reg_SMB_DD( 0xe8 ), Read_Reg_SMB_DD( 0xec ) );
+		PRINTF( option, "[SMB]  F0:%08lx %08lx %08lx %08lx\n", Read_Reg_SMB_DD( 0xf0 ), Read_Reg_SMB_DD( 0xf4 ), Read_Reg_SMB_DD( 0xf8 ), Read_Reg_SMB_DD( 0xfc ) );
+	}
+#endif
+
+	PRINTF( option, "[SCU]  04:%08lx  08:%08lx  0c:%08lx\n",           eng->reg.SCU_004, eng->reg.SCU_008, eng->reg.SCU_00c );
+	PRINTF( option, "[SCU]  1c:%08lx  2c:%08lx  48:%08lx  4c:%08lx\n", Read_Reg_SCU_DD( 0x01c ), Read_Reg_SCU_DD( 0x02c ), eng->reg.SCU_048, Read_Reg_SCU_DD( 0x04c ) );
+	PRINTF( option, "[SCU]  70:%08lx  74:%08lx  7c:%08lx  f0:%08lx\n", eng->reg.SCU_070, eng->reg.SCU_074, eng->reg.SCU_07c, eng->reg.SCU_0f0 );
+	PRINTF( option, "[SCU]  80:%08lx  88:%08lx  90:%08lx  9c:%08lx\n", eng->reg.SCU_080, eng->reg.SCU_088, eng->reg.SCU_090, eng->reg.SCU_09c );
+#ifdef AST1010_CHIP
+	PRINTF( option, "[SCU]  a4:%08lx  ac:%08lx\n",                     Read_Reg_SCU_DD( 0x0a4 ), eng->reg.SCU_0ac );
+#elif defined(AST2500_IOMAP)
+	PRINTF( option, "[SCU]  a0:%08lx  a4:%08lx  b8:%08lx  bc:%08lx\n", Read_Reg_SCU_DD( 0x0a0 ), Read_Reg_SCU_DD( 0x0a4 ), eng->reg.SCU_0b8, eng->reg.SCU_0bc );
+#else
+	PRINTF( option, "[SCU]  a4:%08lx\n",                               Read_Reg_SCU_DD( 0x0a4 ) );
+#endif
+#ifdef AST2500_IOMAP
+	PRINTF( option, "[SCU] 13c:%08lx 140:%08lx 144:%08lx 1dc:%08lx\n", Read_Reg_SCU_DD( 0x13c ), Read_Reg_SCU_DD( 0x140 ), Read_Reg_SCU_DD( 0x144 ), Read_Reg_SCU_DD( 0x1dc ) );
+	PRINTF( option, "[WDT]  0c:%08lx  2c:%08lx  4c:%08lx\n", eng->reg.WDT_00c, eng->reg.WDT_02c, eng->reg.WDT_04c );
+#else
+	PRINTF( option, "[WDT]  0c:%08lx  2c:%08lx\n", eng->reg.WDT_00c, eng->reg.WDT_02c );
+#endif
+	PRINTF( option, "[MAC]  08:%08lx  0c:%08lx\n", eng->reg.MAC_008, eng->reg.MAC_00c );
+	PRINTF( option, "[MAC]  A0|%08lx %08lx %08lx %08lx\n", Read_Reg_MAC_DD( eng, 0xa0 ), Read_Reg_MAC_DD( eng, 0xa4 ), Read_Reg_MAC_DD( eng, 0xa8 ), Read_Reg_MAC_DD( eng, 0xac ) );
+	PRINTF( option, "[MAC]  B0|%08lx %08lx %08lx %08lx\n", Read_Reg_MAC_DD( eng, 0xb0 ), Read_Reg_MAC_DD( eng, 0xb4 ), Read_Reg_MAC_DD( eng, 0xb8 ), Read_Reg_MAC_DD( eng, 0xbc ) );
+	PRINTF( option, "[MAC]  C0|%08lx %08lx %08lx\n",       Read_Reg_MAC_DD( eng, 0xc0 ), Read_Reg_MAC_DD( eng, 0xc4 ), Read_Reg_MAC_DD( eng, 0xc8 ) );
+
+#ifdef SLT_UBOOT
+#elif defined(LinuxAP)
+#else
+	PRINTF( option, "Time: %s", ctime( &(eng->timestart) ));
+	time( &timecur );
+	PRINTF( option, "----> %s", ctime( &timecur ) );
+#endif
+} // End void FPri_RegValue (MAC_ENGINE *eng, BYTE *fp)
+
+//------------------------------------------------------------
+void FPri_End (MAC_ENGINE *eng, BYTE option) {
+#ifdef  DbgPrn_FuncHeader
+	printf("FPri_End\n");
+	Debug_delay();
+#endif
+	if ( eng->env.MAC_RMII && ( eng->phy.RMIICK_IOMode != 0 ) && eng->run.IO_MrgChk && eng->flg.AllFail ) {
+		if ( eng->arg.GEn_RMIIPHY_IN == 0 ) {
+			PRINTF( option, "\n\n\n\n\n\n[Info] The PHY's RMII reference clock pin is setting to the OUTPUT mode now.\n" );
+			PRINTF( option, "       Maybe you can run the INPUT mode command \"mactest  %d %d %d %ld %d %d %d\".\n\n\n\n", eng->arg.GRun_Mode, eng->arg.GSpeed, (eng->arg.GCtrl | 0x80), eng->arg.GLOOP_MAX, eng->arg.GTestMode, eng->arg.GPHYADR, eng->arg.GChk_TimingBund );
+		}
+		else {
+			PRINTF( option, "\n\n\n\n\n\n[Info] The PHY's RMII reference clock pin is setting to the INPUT mode now.\n" );
+			PRINTF( option, "       Maybe you can run the OUTPUT mode command \"mactest  %d %d %d %ld %d %d %d\".\n\n\n\n", eng->arg.GRun_Mode, eng->arg.GSpeed, (eng->arg.GCtrl & 0x7f), eng->arg.GLOOP_MAX, eng->arg.GTestMode, eng->arg.GPHYADR, eng->arg.GChk_TimingBund );
+		}
+	} // End if ( eng->env.MAC_RMII && ( eng->phy.RMIICK_IOMode != 0 ) && eng->run.IO_MrgChk && eng->flg.AllFail )
+
+	if ( !eng->run.TM_RxDataEn ) {
+	}
+	else if ( eng->flg.Err_Flag ) {
+		PRINTF( option, "                    \n----> fail !!!\n" );
+	}
+	else {
+		PRINTF( option, "                    \n----> All Pass !!!\n" );
+	}
+
+	//------------------------------
+	//[Warning] PHY Address
+	//------------------------------
+	if ( eng->ModeSwitch == MODE_DEDICATED ) {
+		if ( eng->arg.GPHYADR != eng->phy.Adr )
+			PRINTF( option, "\n[Warning] PHY Address change from %d to %d !!!\n", eng->arg.GPHYADR, eng->phy.Adr );
+	}
+
+	if ( eng->env.AST2300 ) {
+		//------------------------------
+		//[Warning] IO Strength
+		//------------------------------
+		if ( eng->io.init_done && eng->io.Str_reg_value ) {
+			PRINTF( option, "\n[Warning] SCU%02lX[%2d:%2d] == 0x%02lx is not the suggestion value 0.\n", eng->io.Str_reg_idx, eng->io.Str_reg_Hbit, eng->io.Str_reg_Lbit, eng->io.Str_reg_value );
+			PRINTF( option, "          This change at this platform must been proven again by the ASPEED.\n" );
+		}
+
+		//------------------------------
+		//[Warning] IO Timing
+ 		//------------------------------
+		if ( ( eng->reg.SCU_048_check != eng->reg.SCU_048_default ) ) {
+			PRINTF( option, "\n[Warning] SCU48 == 0x%08lx is not the suggestion value 0x%08lx.\n", eng->reg.SCU_048, eng->reg.SCU_048_default );
+			PRINTF( option, "          This change at this platform must been proven again by the ASPEED.\n" );
+		}
+  #ifdef AST2500_IOMAP
+		if ( eng->env.AST2500A1 ) {
+			if ( ( eng->reg.SCU_0b8 != SCU_B8h_AST2500 ) ) {
+				PRINTF( option, "\n[Warning] SCUB8 == 0x%08lx is not the suggestion value 0x%08x.\n", eng->reg.SCU_0b8, SCU_B8h_AST2500 );
+				PRINTF( option, "          This change at this platform must been proven again by the ASPEED.\n" );
+			}
+			if ( ( eng->reg.SCU_0bc != SCU_BCh_AST2500 ) ) {
+				PRINTF( option, "\n[Warning] SCUBC == 0x%08lx is not the suggestion value 0x%08x.\n", eng->reg.SCU_0bc, SCU_BCh_AST2500 );
+				PRINTF( option, "          This change at this platform must been proven again by the ASPEED.\n" );
+			}
+		}
+  #endif
+	} // End if ( eng->env.AST2300 )
+
+	if ( eng->ModeSwitch == MODE_NSCI ) {
+		PRINTF( option, "\n[Arg] %d %d %d %d %d %d %ld (%s){%d}\n", eng->arg.GRun_Mode, eng->arg.GPackageTolNum, eng->arg.GChannelTolNum, eng->arg.GTestMode, eng->arg.GChk_TimingBund, eng->arg.GCtrl, eng->arg.GARPNumCnt, eng->env.ASTChipName, TIME_OUT_NCSI );
+
+		switch ( eng->ncsi_cap.PCI_DID_VID ) {
+			case PCI_DID_VID_Intel_82574L             : { PRINTF( option, "[NC]%08lx %08lx: Intel 82574L       \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82575_10d6         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82575        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82575_10a7         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82575        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82575_10a9         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82575        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82576_10c9         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82576        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82576_10e6         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82576        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82576_10e7         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82576        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82576_10e8         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82576        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82576_1518         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82576        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82576_1526         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82576        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82576_150a         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82576        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82576_150d         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82576        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82599_10fb         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82599        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_82599_1557         : { PRINTF( option, "[NC]%08lx %08lx: Intel 82599        \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_I210_1533          : { PRINTF( option, "[NC]%08lx %08lx: Intel I210         \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_I210_1537          : { PRINTF( option, "[NC]%08lx %08lx: Intel I210         \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_I350_1521          : { PRINTF( option, "[NC]%08lx %08lx: Intel I350         \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_I350_1523          : { PRINTF( option, "[NC]%08lx %08lx: Intel I350         \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_X540               : { PRINTF( option, "[NC]%08lx %08lx: Intel X540         \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_X550               : { PRINTF( option, "[NC]%08lx %08lx: Intel X550         \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_Broadwell_DE       : { PRINTF( option, "[NC]%08lx %08lx: Intel Broadwell-DE \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Intel_X722_37d0          : { PRINTF( option, "[NC]%08lx %08lx: Intel X722         \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Broadcom_BCM5718         : { PRINTF( option, "[NC]%08lx %08lx: Broadcom BCM5718   \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Broadcom_BCM5719         : { PRINTF( option, "[NC]%08lx %08lx: Broadcom BCM5719   \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Broadcom_BCM5720         : { PRINTF( option, "[NC]%08lx %08lx: Broadcom BCM5720   \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Broadcom_BCM5725         : { PRINTF( option, "[NC]%08lx %08lx: Broadcom BCM5725   \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Broadcom_BCM57810S       : { PRINTF( option, "[NC]%08lx %08lx: Broadcom BCM57810S \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Broadcom_Cumulus         : { PRINTF( option, "[NC]%08lx %08lx: Broadcom Cumulus   \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Broadcom_BCM57302        : { PRINTF( option, "[NC]%08lx %08lx: Broadcom BCM57302  \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Broadcom_BCM957452       : { PRINTF( option, "[NC]%08lx %08lx: Broadcom BCM957452 \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Mellanox_ConnectX_3_1003 : { PRINTF( option, "[NC]%08lx %08lx: Mellanox ConnectX-3\n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Mellanox_ConnectX_3_1007 : { PRINTF( option, "[NC]%08lx %08lx: Mellanox ConnectX-3\n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			case PCI_DID_VID_Mellanox_ConnectX_4      : { PRINTF( option, "[NC]%08lx %08lx: Mellanox ConnectX-4\n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			default:
+			switch ( eng->ncsi_cap.ManufacturerID ) {
+				case ManufacturerID_Intel    : { PRINTF( option, "[NC]%08lx %08lx: Intel              \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+				case ManufacturerID_Broadcom : { PRINTF( option, "[NC]%08lx %08lx: Broadcom           \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+				case ManufacturerID_Mellanox : { PRINTF( option, "[NC]%08lx %08lx: Mellanox           \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+				case ManufacturerID_Mellanox1: { PRINTF( option, "[NC]%08lx %08lx: Mellanox           \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+				case ManufacturerID_Emulex   : { PRINTF( option, "[NC]%08lx %08lx: Emulex             \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+				default                      : { PRINTF( option, "[NC]%08lx %08lx                     \n", eng->ncsi_cap.ManufacturerID, eng->ncsi_cap.PCI_DID_VID ); break; }
+			} // End switch ( eng->ncsi_cap.ManufacturerID )
+		} // End switch ( eng->ncsi_cap.PCI_DID_VID )
+	}
+	else {
+		if (eng->arg.GLOOP_INFINI) {
+			PRINTF( option, "\n[Arg] %d %d %d # %d %d %d %lx (%s){%ld x:%d %d %d}[%d %d %d] %ld\n"  , eng->arg.GRun_Mode, eng->arg.GSpeed, eng->arg.GCtrl,                     eng->arg.GTestMode, eng->arg.GPHYADR, eng->arg.GChk_TimingBund, eng->arg.GUserDVal, eng->env.ASTChipName, eng->run.TIME_OUT_Des_PHYRatio, TIME_OUT_Des_1G, TIME_OUT_Des_100M, TIME_OUT_Des_10M, eng->run.Loop_rl[0], eng->run.Loop_rl[1], eng->run.Loop_rl[2], eng->dat.Des_Num );
+		}
+		else {
+			PRINTF( option, "\n[Arg] %d %d %d %ld %d %d %d %lx (%s){%ld x:%d %d %d}[%d %d %d] %ld\n", eng->arg.GRun_Mode, eng->arg.GSpeed, eng->arg.GCtrl, eng->arg.GLOOP_MAX, eng->arg.GTestMode, eng->arg.GPHYADR, eng->arg.GChk_TimingBund, eng->arg.GUserDVal, eng->env.ASTChipName, eng->run.TIME_OUT_Des_PHYRatio, TIME_OUT_Des_1G, TIME_OUT_Des_100M, TIME_OUT_Des_10M, eng->run.Loop_rl[0], eng->run.Loop_rl[1], eng->run.Loop_rl[2], eng->dat.Des_Num );
+		}
+
+		PRINTF( option, "[PHY] Adr:%d ID2:%04lx ID3:%04lx (%s)\n", eng->phy.Adr, eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName );
+	} // End if ( eng->ModeSwitch == MODE_NSCI )
+
+#ifdef SUPPORT_PHY_LAN9303
+	PRINTF ( option, "[Ver II] %s (for LAN9303 with I2C%d)\n", version_name, LAN9303_I2C_BUSNUM );
+#else
+	PRINTF( option, "[Ver II] %s\n", version_name );
+#endif
+} // End void FPri_End (MAC_ENGINE *eng, BYTE option)
+
+//------------------------------------------------------------
+void FPri_ErrFlag (MAC_ENGINE *eng, BYTE option) {
+#ifdef  DbgPrn_FuncHeader
+	printf("FPri_ErrFlag\n");
+	Debug_delay();
+#endif
+	if ( eng->flg.Flag_PrintEn ) {
+		if ( eng->flg.Wrn_Flag ) {
+			if ( eng->flg.Wrn_Flag & Wrn_Flag_IOMarginOUF ) {
+				PRINTF( option, "[Warning] IO timing testing range out of boundary\n" );
+				if ( eng->env.MAC_RMII ) {
+					PRINTF( option, "      (reg:%d,%d) %dx1(%d~%d,%d)\n", eng->io.Dly_in_reg_idx,
+											      eng->io.Dly_out_reg_idx,
+											      eng->run.IO_Bund,
+											      eng->io.Dly_in_min,
+											      eng->io.Dly_in_max,
+											      eng->io.Dly_out_min );
+				}
+				else {
+					PRINTF( option, "      (reg:%d,%d) %dx%d(%d~%d,%d~%d)\n", eng->io.Dly_in_reg_idx,
+												  eng->io.Dly_out_reg_idx,
+												  eng->run.IO_Bund,
+												  eng->run.IO_Bund,
+												  eng->io.Dly_in_min,
+												  eng->io.Dly_in_max,
+												  eng->io.Dly_out_min,
+												  eng->io.Dly_out_max );
+				}
+			} // End if ( eng->flg.Wrn_Flag & Wrn_Flag_IOMarginOUF )
+			if ( eng->flg.Wrn_Flag & Wrn_Flag_RxErFloatting ) {
+				PRINTF( option, "[Warning] NCSI RXER pin may be floatting to the MAC !!!\n" );
+				PRINTF( option, "          Please contact with the ASPEED Inc. for more help.\n" );
+			} // End if ( eng->flg.Wrn_Flag & Wrn_Flag_RxErFloatting )
+		} // End if ( eng->flg.Wrn_Flag )
+
+		if ( eng->flg.Err_Flag ) {
+			PRINTF( option, "\n\n" );
+//PRINTF( option, "Err_Flag: %x\n\n", eng->flg.Err_Flag );
+
+			if ( eng->flg.Err_Flag & Err_Flag_PHY_Type                ) { PRINTF( option, "[Err] Unidentifiable PHY                                     \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_MALLOC_FrmSize          ) { PRINTF( option, "[Err] Malloc fail at frame size buffer                       \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_MALLOC_LastWP           ) { PRINTF( option, "[Err] Malloc fail at last WP buffer                          \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_Check_Buf_Data          ) { PRINTF( option, "[Err] Received data mismatch                                 \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_NCSI_Check_TxOwnTimeOut ) { PRINTF( option, "[Err] Time out of checking Tx owner bit in NCSI packet       \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_NCSI_Check_RxOwnTimeOut ) { PRINTF( option, "[Err] Time out of checking Rx owner bit in NCSI packet       \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_NCSI_Check_ARPOwnTimeOut) { PRINTF( option, "[Err] Time out of checking ARP owner bit in NCSI packet      \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_NCSI_No_PHY             ) { PRINTF( option, "[Err] Can not find NCSI PHY                                  \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_NCSI_Channel_Num        ) { PRINTF( option, "[Err] NCSI Channel Number Mismatch                           \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_NCSI_Package_Num        ) { PRINTF( option, "[Err] NCSI Package Number Mismatch                           \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_PHY_TimeOut_RW          ) { PRINTF( option, "[Err] Time out of read/write PHY register                    \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_PHY_TimeOut_Rst         ) { PRINTF( option, "[Err] Time out of reset PHY register                         \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_RXBUF_UNAVA             ) { PRINTF( option, "[Err] MAC00h[2]:Receiving buffer unavailable                 \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_RPKT_LOST               ) { PRINTF( option, "[Err] MAC00h[3]:Received packet lost due to RX FIFO full     \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_NPTXBUF_UNAVA           ) { PRINTF( option, "[Err] MAC00h[6]:Normal priority transmit buffer unavailable  \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_TPKT_LOST               ) { PRINTF( option, "[Err] MAC00h[7]:Packets transmitted to Ethernet lost         \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_DMABufNum               ) { PRINTF( option, "[Err] DMA Buffer is not enough                               \n" ); }
+			if ( eng->flg.Err_Flag & Err_Flag_IOMargin                ) { PRINTF( option, "[Err] IO timing margin is not enough                         \n" ); }
+
+			if ( eng->flg.Err_Flag & Err_Flag_MHCLK_Ratio             ) {
+#ifdef AST1010_CHIP
+				PRINTF( option, "[Err] Error setting of MAC AHB bus clock (SCU08[13:12])      \n" );
+				PRINTF( option, "      SCU08[13:12] == 0x%01x is not the suggestion value 3.\n", eng->env.MHCLK_Ratio );
+#elif defined(AST2500_IOMAP)
+				PRINTF( option, "[Err] Error setting of MAC AHB bus clock (SCU08[18:16])      \n" );
+				if ( eng->env.MAC_atlast_1Gvld )
+					{ PRINTF( option, "      SCU08[18:16] == 0x%01x is not the suggestion value 2.\n", eng->env.MHCLK_Ratio ); }
+				else
+					{ PRINTF( option, "      SCU08[18:16] == 0x%01x is not the suggestion value 4.\n", eng->env.MHCLK_Ratio ); }
+#else
+				PRINTF( option, "[Err] Error setting of MAC AHB bus clock (SCU08[18:16])      \n" );
+				if ( eng->env.MAC_atlast_1Gvld )
+					{ PRINTF( option, "      SCU08[18:16] == 0x%01x is not the suggestion value 2.\n", eng->env.MHCLK_Ratio ); }
+				else
+					{ PRINTF( option, "      SCU08[18:16] == 0x%01x is not the suggestion value 4.\n", eng->env.MHCLK_Ratio ); }
+#endif
+			} // End if ( eng->flg.Err_Flag & Err_Flag_MHCLK_Ratio             )
+
+			if ( eng->flg.Err_Flag & Err_Flag_IOMarginOUF ) {
+				PRINTF( option, "[Err] IO timing testing range out of boundary\n");
+				if ( eng->env.MAC_RMII ) {
+				PRINTF( option, "      (%d,%d): %dx1 [%d:%d]x[%d]\n", eng->io.Dly_in_reg_idx,
+											      eng->io.Dly_out_reg_idx,
+											      eng->run.IO_Bund,
+											      eng->io.Dly_in_min,
+											      eng->io.Dly_in_max,
+											      eng->io.Dly_out_min );
+				}
+				else {
+				PRINTF( option, "      (%d,%d): %dx%d [%d:%d]x[%d:%d]\n", eng->io.Dly_in_reg_idx,
+												  eng->io.Dly_out_reg_idx,
+												  eng->run.IO_Bund,
+												  eng->run.IO_Bund,
+												  eng->io.Dly_in_min,
+												  eng->io.Dly_in_max,
+												  eng->io.Dly_out_min,
+												  eng->io.Dly_out_max );
+				}
+			} // End if ( eng->flg.Err_Flag & Err_Flag_IOMarginOUF )
+
+			if ( eng->flg.Err_Flag & Err_Flag_Check_Des ) {
+				PRINTF( option, "[Err] Descriptor error\n");
+				if ( eng->flg.Des_Flag & Des_Flag_TxOwnTimeOut ) { PRINTF( option, "[Des] Time out of checking Tx owner bit\n" ); }
+				if ( eng->flg.Des_Flag & Des_Flag_RxOwnTimeOut ) { PRINTF( option, "[Des] Time out of checking Rx owner bit\n" ); }
+				if ( eng->flg.Des_Flag & Des_Flag_FrameLen     ) { PRINTF( option, "[Des] Frame length mismatch            \n" ); }
+				if ( eng->flg.Des_Flag & Des_Flag_RxErr        ) { PRINTF( option, "[Des] Input signal RxErr               \n" ); }
+				if ( eng->flg.Des_Flag & Des_Flag_CRC          ) { PRINTF( option, "[Des] CRC error of frame               \n" ); }
+				if ( eng->flg.Des_Flag & Des_Flag_FTL          ) { PRINTF( option, "[Des] Frame too long                   \n" ); }
+				if ( eng->flg.Des_Flag & Des_Flag_Runt         ) { PRINTF( option, "[Des] Runt packet                      \n" ); }
+				if ( eng->flg.Des_Flag & Des_Flag_OddNibble    ) { PRINTF( option, "[Des] Nibble bit happen                \n" ); }
+				if ( eng->flg.Des_Flag & Des_Flag_RxFIFOFull   ) { PRINTF( option, "[Des] Rx FIFO full                     \n" ); }
+			} // End if ( eng->flg.Err_Flag & Err_Flag_Check_Des )
+
+			if ( eng->flg.Err_Flag & Err_Flag_MACMode ) {
+				PRINTF( option, "[Err] MAC interface mode mismatch\n" );
+#ifndef AST1010_CHIP
+				if ( eng->env.AST2300 ) {
+					switch ( eng->env.MAC_Mode ) {
+						case 0 : { PRINTF( option, "      SCU70h[7:6] == 0: [MAC#1] RMII   [MAC#2] RMII \n" ); break; }
+						case 1 : { PRINTF( option, "      SCU70h[7:6] == 1: [MAC#1] RGMII  [MAC#2] RMII \n" ); break; }
+						case 2 : { PRINTF( option, "      SCU70h[7:6] == 2: [MAC#1] RMII   [MAC#2] RGMII\n" ); break; }
+						case 3 : { PRINTF( option, "      SCU70h[7:6] == 3: [MAC#1] RGMII  [MAC#2] RGMII\n" ); break; }
+					}
+				}
+				else {
+					switch ( eng->env.MAC_Mode ) {
+						case 0 : { PRINTF( option, "      SCU70h[8:6] == 000: [MAC#1] GMII               \n" ); break; }
+						case 1 : { PRINTF( option, "      SCU70h[8:6] == 001: [MAC#1] MII    [MAC#2] MII \n" ); break; }
+						case 2 : { PRINTF( option, "      SCU70h[8:6] == 010: [MAC#1] RMII   [MAC#2] MII \n" ); break; }
+						case 3 : { PRINTF( option, "      SCU70h[8:6] == 011: [MAC#1] MII                \n" ); break; }
+						case 4 : { PRINTF( option, "      SCU70h[8:6] == 100: [MAC#1] RMII               \n" ); break; }
+						case 5 : { PRINTF( option, "      SCU70h[8:6] == 101: Reserved                   \n" ); break; }
+						case 6 : { PRINTF( option, "      SCU70h[8:6] == 110: [MAC#1] RMII   [MAC#2] RMII\n" ); break; }
+						case 7 : { PRINTF( option, "      SCU70h[8:6] == 111: Disable MAC                \n" ); break; }
+					}
+				} // End if ( eng->env.AST2300 )
+#endif
+			} // End if ( eng->flg.Err_Flag & Err_Flag_MACMode )
+
+			if ( eng->ModeSwitch == MODE_NSCI ) {
+				if ( eng->flg.Err_Flag & Err_Flag_NCSI_LinkFail ) {
+					PRINTF( option, "[Err] NCSI packet retry number over flows when find channel\n" );
+
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Get_Version_ID                  ) { PRINTF( option, "[NCSI] Time out when Get Version ID                  \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Get_Capabilities                ) { PRINTF( option, "[NCSI] Time out when Get Capabilities                \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Select_Active_Package           ) { PRINTF( option, "[NCSI] Time out when Select Active Package           \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Enable_Set_MAC_Address          ) { PRINTF( option, "[NCSI] Time out when Enable Set MAC Address          \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Enable_Broadcast_Filter         ) { PRINTF( option, "[NCSI] Time out when Enable Broadcast Filter         \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Enable_Network_TX               ) { PRINTF( option, "[NCSI] Time out when Enable Network TX               \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Enable_Channel                  ) { PRINTF( option, "[NCSI] Time out when Enable Channel                  \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Disable_Network_TX              ) { PRINTF( option, "[NCSI] Time out when Disable Network TX              \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Disable_Channel                 ) { PRINTF( option, "[NCSI] Time out when Disable Channel                 \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Select_Package                  ) { PRINTF( option, "[NCSI] Time out when Select Package                  \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Deselect_Package                ) { PRINTF( option, "[NCSI] Time out when Deselect Package                \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Set_Link                        ) { PRINTF( option, "[NCSI] Time out when Set Link                        \n" ); }
+					if ( eng->flg.NCSI_Flag & NCSI_Flag_Get_Controller_Packet_Statistics) { PRINTF( option, "[NCSI] Time out when Get Controller Packet Statistics\n" ); }
+				}
+
+				if ( eng->flg.Err_Flag & Err_Flag_NCSI_Channel_Num ) { PRINTF( option, "[NCSI] Channel number expected: %d, real: %d\n", eng->arg.GChannelTolNum, eng->dat.number_chl ); }
+				if ( eng->flg.Err_Flag & Err_Flag_NCSI_Package_Num ) { PRINTF( option, "[NCSI] Peckage number expected: %d, real: %d\n", eng->arg.GPackageTolNum, eng->dat.number_pak ); }
+			} // End if ( eng->ModeSwitch == MODE_NSCI )
+		} // End if ( eng->flg.Err_Flag )
+	} // End if ( eng->flg.Flag_PrintEn )
+} // End void FPri_ErrFlag (MAC_ENGINE *eng, BYTE option)
+
+//------------------------------------------------------------
+void Finish_Close (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("Finish_Close\n");
+	Debug_delay();
+#endif
+#ifdef Enable_RecovSCU
+	if ( eng->reg.SCU_oldvld )
+		recov_scu( eng );
+#endif
+
+#if defined(ENABLE_LOG_FILE)
+	if ( eng->fp_io && eng->run.TM_IOTiming )
+		fclose( eng->fp_io );
+
+	if ( eng->fp_log )
+		fclose( eng->fp_log );
+#endif
+} // End void Finish_Close (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+char Finish_Check (MAC_ENGINE *eng, int value) {
+#if defined( SLT_UBOOT ) && defined( CONFIG_AST2500 )
+	ULONG   reg;
+	BYTE    shift_value = 0;
+#endif
+#ifdef Disable_VGA
+	if ( eng->env.VGAModeVld ) {
+		outp(0x3d4, 0x17);
+		outp(0x3d5, eng->env.VGAMode);
+	}
+#endif
+#ifdef  DbgPrn_FuncHeader
+	printf("Finish_Check\n");
+	Debug_delay();
+#endif
+
+	if ( eng->ModeSwitch ==  MODE_DEDICATED ) {
+		if ( eng->dat.FRAME_LEN )
+			free( eng->dat.FRAME_LEN );
+
+		if ( eng->dat.wp_lst )
+			free( eng->dat.wp_lst );
+	}
+
+	eng->flg.Err_Flag = eng->flg.Err_Flag | value;
+
+	if ( DbgPrn_ErrFlg )
+		printf("\nErr_Flag: [%08lx]\n", eng->flg.Err_Flag);
+
+	if ( !eng->run.TM_Burst )
+		FPri_ErrFlag( eng, FP_LOG );
+
+	if ( eng->run.TM_IOTiming )
+		FPri_ErrFlag( eng, FP_IO );
+
+	FPri_ErrFlag( eng, STD_OUT );
+
+	if ( !eng->run.TM_Burst )
+		FPri_End( eng, FP_LOG );
+
+	if ( eng->run.TM_IOTiming )
+		FPri_End( eng, FP_IO );
+
+	FPri_End( eng, STD_OUT );
+
+
+	if ( !eng->run.TM_Burst )
+		FPri_RegValue( eng, FP_LOG );
+	if ( eng->run.TM_IOTiming )
+		FPri_RegValue( eng, FP_IO  );
+#ifdef PHY_SPECIAL
+	if ( !eng->run.TM_Burst )
+		special_PHY_debug( eng );
+#endif
+
+	Finish_Close( eng );
+
+#if defined( SLT_UBOOT ) && defined( CONFIG_AST2500 )
+	reg = Read_Reg_SCU_DD( 0x40 );
+	if ( eng->ModeSwitch == MODE_DEDICATED )
+		shift_value = 18 + eng->run.MAC_idx;
+	else
+		shift_value = 16 + eng->run.MAC_idx;
+#endif
+
+	if ( eng->flg.Err_Flag )
+	{
+		// Fail
+#if defined( SLT_UBOOT ) && defined( CONFIG_AST2500 )
+		reg = reg & ~( 1 << shift_value );
+		Write_Reg_SCU_DD( 0x40, reg );
+#endif
+		return( 1 );
+	}
+	else
+	{
+		// PASS
+#if defined( SLT_UBOOT ) && defined( CONFIG_AST2500 )
+		reg |= ( 1 << shift_value );
+		Write_Reg_SCU_DD( 0x40, reg );
+#endif
+		return( 0 );
+	}
+} // End char Finish_Check (MAC_ENGINE *eng, int value)
+
+//------------------------------------------------------------
+int FindErr (MAC_ENGINE *eng, int value) {
+	eng->flg.Err_Flag = eng->flg.Err_Flag | value;
+
+	if ( DbgPrn_ErrFlg )
+		printf("\nErr_Flag: [%08lx]\n", eng->flg.Err_Flag);
+
+	return(1);
+}
+
+//------------------------------------------------------------
+int FindErr_Des (MAC_ENGINE *eng, int value) {
+	eng->flg.Err_Flag = eng->flg.Err_Flag | Err_Flag_Check_Des;
+	eng->flg.Des_Flag = eng->flg.Des_Flag | value;
+	if ( DbgPrn_ErrFlg )
+		printf("\nErr_Flag: [%08lx] Des_Flag: [%08lx]\n", eng->flg.Err_Flag, eng->flg.Des_Flag);
+
+	return(1);
+}
+
+//------------------------------------------------------------
+// Get and Check status of Interrupt
+//------------------------------------------------------------
+int check_int (MAC_ENGINE *eng, char *type ) {
+#ifdef  DbgPrn_FuncHeader
+	printf("check_int  : [%d]%d\n", eng->run.Loop_ofcnt, eng->run.Loop);
+	Debug_delay();
+#endif
+
+	eng->reg.MAC_000 = Read_Reg_MAC_DD( eng, 0x00 );//Interrupt Status
+#ifdef CheckRxbufUNAVA
+	if ( eng->reg.MAC_000 & 0x00000004 ) {
+		PRINTF( FP_LOG, "[%sIntStatus] Receiving buffer unavailable               : %08lx [loop[%d]:%d]\n", type, eng->reg.MAC_000, eng->run.Loop_ofcnt, eng->run.Loop );
+		FindErr( eng, Err_Flag_RXBUF_UNAVA );
+	}
+#endif
+
+#ifdef CheckRPktLost
+	if ( eng->reg.MAC_000 & 0x00000008 ) {
+		PRINTF( FP_LOG, "[%sIntStatus] Received packet lost due to RX FIFO full   : %08lx [loop[%d]:%d]\n", type, eng->reg.MAC_000, eng->run.Loop_ofcnt, eng->run.Loop );
+		FindErr( eng, Err_Flag_RPKT_LOST );
+	}
+#endif
+
+#ifdef CheckNPTxbufUNAVA
+	if ( eng->reg.MAC_000 & 0x00000040 ) {
+		PRINTF( FP_LOG, "[%sIntStatus] Normal priority transmit buffer unavailable: %08lx [loop[%d]:%d]\n", type, eng->reg.MAC_000, eng->run.Loop_ofcnt, eng->run.Loop );
+		FindErr( eng, Err_Flag_NPTXBUF_UNAVA );
+	}
+#endif
+
+#ifdef CheckTPktLost
+	if ( eng->reg.MAC_000 & 0x00000080 ) {
+		PRINTF( FP_LOG, "[%sIntStatus] Packets transmitted to Ethernet lost       : %08lx [loop[%d]:%d]\n", type, eng->reg.MAC_000, eng->run.Loop_ofcnt, eng->run.Loop );
+		FindErr( eng, Err_Flag_TPKT_LOST );
+	}
+#endif
+
+	if ( eng->flg.Err_Flag )
+		return(1);
+	else
+		return(0);
+} // End int check_int (MAC_ENGINE *eng, char *type)
+
+
+//------------------------------------------------------------
+// Buffer
+//------------------------------------------------------------
+void setup_framesize (MAC_ENGINE *eng) {
+	LONG       des_num;
+
+#ifdef  DbgPrn_FuncHeader
+	printf("setup_framesize\n");
+	Debug_delay();
+#endif
+
+	//------------------------------
+	// Fill Frame Size out descriptor area
+	//------------------------------
+#ifdef SLT_UBOOT
+	if (0)
+#else
+	if ( ENABLE_RAND_SIZE )
+#endif
+	{
+		for ( des_num = 0; des_num < eng->dat.Des_Num; des_num++ ) {
+			if ( RAND_SIZE_SIMPLE )
+				switch( rand() % 5 ) {
+					case 0 : eng->dat.FRAME_LEN[ des_num ] = 0x4e ; break;
+					case 1 : eng->dat.FRAME_LEN[ des_num ] = 0x4ba; break;
+					default: eng->dat.FRAME_LEN[ des_num ] = 0x5ea; break;
+				}
+			else
+//				eng->dat.FRAME_LEN[ des_num ] = ( rand() + RAND_SIZE_MIN ) % ( RAND_SIZE_MAX + 1 );
+				eng->dat.FRAME_LEN[ des_num ] = RAND_SIZE_MIN + ( rand() % ( RAND_SIZE_MAX - RAND_SIZE_MIN + 1 ) );
+
+			if ( DbgPrn_FRAME_LEN )
+				PRINTF( FP_LOG, "[setup_framesize] FRAME_LEN_Cur:%08lx[Des:%ld][loop[%d]:%d]\n", eng->dat.FRAME_LEN[ des_num ], des_num, eng->run.Loop_ofcnt, eng->run.Loop );
+		}
+	}
+	else {
+		for ( des_num = 0; des_num < eng->dat.Des_Num; des_num++ ) {
+#ifdef SelectSimpleLength
+			if ( des_num % FRAME_SELH_PERD )
+				eng->dat.FRAME_LEN[ des_num ] = FRAME_LENH;
+			else
+				eng->dat.FRAME_LEN[ des_num ] = FRAME_LENL;
+#elif defined(PHY_SPECIAL)
+			eng->dat.FRAME_LEN[ des_num ] = special_PHY_FRAME_LEN( eng );
+#else
+			if ( eng->run.TM_Burst ) {
+				if ( eng->run.TM_IEEE )
+					eng->dat.FRAME_LEN[ des_num ] = 1514;
+				else
+  #ifdef ENABLE_ARP_2_WOL
+					eng->dat.FRAME_LEN[ des_num ] = 164;
+  #else
+					eng->dat.FRAME_LEN[ des_num ] = 60;
+  #endif
+			}
+			else {
+  #ifdef SelectLengthInc
+				eng->dat.FRAME_LEN[ des_num ] = 1514 - ( des_num % 1455 );
+  #else
+				if ( des_num % FRAME_SELH_PERD )
+					eng->dat.FRAME_LEN[ des_num ] = FRAME_LENH;
+				else
+					eng->dat.FRAME_LEN[ des_num ] = FRAME_LENL;
+  #endif
+			} // End if ( eng->run.TM_Burst )
+#endif
+			if ( DbgPrn_FRAME_LEN )
+				PRINTF( FP_LOG, "[setup_framesize] FRAME_LEN_Cur:%08lx[Des:%ld][loop[%d]:%d]\n", eng->dat.FRAME_LEN[ des_num ], des_num, eng->run.Loop_ofcnt, eng->run.Loop );
+
+		} // End for (des_num = 0; des_num < eng->dat.Des_Num; des_num++)
+	} // End if ( ENABLE_RAND_SIZE )
+
+	// Calculate average of frame size
+#ifdef Enable_ShowBW
+	eng->dat.Total_frame_len = 0;
+
+	for ( des_num = 0; des_num < eng->dat.Des_Num; des_num++ )
+		eng->dat.Total_frame_len += eng->dat.FRAME_LEN[ des_num ];
+#endif
+
+	//------------------------------
+	// Write Plane
+	//------------------------------
+	switch( ZeroCopy_OFFSET & 0x3 ) {
+		case 0: eng->dat.wp_fir = 0xffffffff; break;
+		case 1: eng->dat.wp_fir = 0xffffff00; break;
+		case 2: eng->dat.wp_fir = 0xffff0000; break;
+		case 3: eng->dat.wp_fir = 0xff000000; break;
+	}
+
+	for ( des_num = 0; des_num < eng->dat.Des_Num; des_num++ )
+		switch( ( ZeroCopy_OFFSET + eng->dat.FRAME_LEN[ des_num ] - 1 ) & 0x3 ) {
+			case 0: eng->dat.wp_lst[ des_num ] = 0x000000ff; break;
+			case 1: eng->dat.wp_lst[ des_num ] = 0x0000ffff; break;
+			case 2: eng->dat.wp_lst[ des_num ] = 0x00ffffff; break;
+			case 3: eng->dat.wp_lst[ des_num ] = 0xffffffff; break;
+		}
+} // End void setup_framesize (void)
+
+//------------------------------------------------------------
+void setup_arp (MAC_ENGINE *eng) {
+	int        i;
+
+#ifdef  DbgPrn_FuncHeader
+	printf("setup_arp\n");
+	Debug_delay();
+#endif
+	for (i = 0; i < 16; i++ )
+		eng->dat.ARP_data[ i ] = ARP_org_data[ i ];
+
+	eng->dat.ARP_data[ 1 ] = 0x0000ffff
+	                       | ( eng->inf.SA[ 0 ] << 16 )//MSB
+	                       | ( eng->inf.SA[ 1 ] << 24 );
+
+	eng->dat.ARP_data[ 2 ] = ( eng->inf.SA[ 2 ]       )
+	                       | ( eng->inf.SA[ 3 ] <<  8 )
+	                       | ( eng->inf.SA[ 4 ] << 16 )
+	                       | ( eng->inf.SA[ 5 ] << 24 );//LSB
+
+	eng->dat.ARP_data[ 5 ] = 0x00000100
+	                       | ( eng->inf.SA[ 0 ] << 16 )//MSB
+	                       | ( eng->inf.SA[ 1 ] << 24 );
+
+	eng->dat.ARP_data[ 6 ] = ( eng->inf.SA[ 2 ]       )
+	                       | ( eng->inf.SA[ 3 ] <<  8 )
+	                       | ( eng->inf.SA[ 4 ] << 16 )
+	                       | ( eng->inf.SA[ 5 ] << 24 );//LSB
+} // End void setup_arp (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void setup_buf (MAC_ENGINE *eng) {
+	LONG       des_num_max;
+	LONG       des_num;
+	int        i;
+	ULONG      adr;
+	ULONG      adr_srt;
+	ULONG      adr_end;
+	ULONG      Current_framelen;
+	ULONG      gdata = 0;
+#ifdef PHY_SPECIAL
+	ULONG      *gdata_ptr;
+#endif	
+#ifdef SelectSimpleDA
+	int        cnt = 0;
+	ULONG      len;
+#endif
+
+#ifdef ENABLE_ARP_2_WOL
+	int        DA[3];
+
+	DA[ 0 ] =  ( ( SelectWOLDA_DatH >>  8 ) & 0x00ff ) |
+	           ( ( SelectWOLDA_DatH <<  8 ) & 0xff00 );
+
+	DA[ 1 ] =  ( ( SelectWOLDA_DatL >> 24 ) & 0x00ff ) |
+	           ( ( SelectWOLDA_DatL >>  8 ) & 0xff00 );
+
+	DA[ 2 ] =  ( ( SelectWOLDA_DatL >>  8 ) & 0x00ff ) |
+	           ( ( SelectWOLDA_DatL <<  8 ) & 0xff00 );
+#endif
+
+#ifdef  DbgPrn_FuncHeader
+	printf("setup_buf  : [%d]%d\n", eng->run.Loop_ofcnt, eng->run.Loop);
+	Debug_delay();
+#endif
+
+	// It need be multiple of 4
+	eng->dat.DMA_Base_Setup = GET_DMA_BASE_SETUP & 0xfffffffc;
+	adr_srt = eng->dat.DMA_Base_Setup;//base for read/write
+
+	if ( eng->run.TM_Burst ) {
+		if ( eng->run.TM_IEEE ) {
+			for ( des_num = 0; des_num < eng->dat.Des_Num; des_num++ ) {
+				if ( DbgPrn_BufAdr )
+					printf("[loop[%d]:%4d][des:%4ld][setup_buf  ] %08lx\n", eng->run.Loop_ofcnt, eng->run.Loop, des_num, adr_srt);
+#ifdef ENABLE_DASA
+				Write_Mem_Dat_DD( adr_srt    , 0xffffffff           );
+				Write_Mem_Dat_DD( adr_srt + 4, eng->dat.ARP_data[1] );
+				Write_Mem_Dat_DD( adr_srt + 8, eng->dat.ARP_data[2] );
+
+				for ( adr = (adr_srt + 12); adr < (adr_srt + DMA_PakSize); adr += 4 )
+#else
+				for ( adr =  adr_srt;       adr < (adr_srt + DMA_PakSize); adr += 4 )
+#endif
+				{
+					switch( eng->arg.GTestMode ) {
+						case 1: gdata = 0xffffffff;              break;
+						case 2: gdata = 0x55555555;              break;
+						case 3: gdata = rand() | (rand() << 16); break;
+						case 5: gdata = eng->arg.GUserDVal;      break;
+					}
+					Write_Mem_Dat_DD( adr, gdata );
+				} // End for()
+				adr_srt += DMA_PakSize;
+			} // End for (des_num = 0; des_num < eng->dat.Des_Num; des_num++)
+		}
+		else {
+			printf("----->[ARP] 60 bytes\n");
+			for (i = 0; i < 16; i++)
+				printf("      [Tx%02d] %08lx %08lx\n", i, eng->dat.ARP_data[i], SWAP_4B( eng->dat.ARP_data[i] ) );
+
+			for ( des_num = 0; des_num < eng->dat.Des_Num; des_num++ ) {
+				if ( DbgPrn_BufAdr )
+					printf("[loop[%d]:%4d][des:%4ld][setup_buf  ] %08lx\n", eng->run.Loop_ofcnt, eng->run.Loop, des_num, adr_srt);
+
+				for (i = 0; i < 16; i++)
+					Write_Mem_Dat_DD( adr_srt + ( i << 2 ), eng->dat.ARP_data[i] );
+
+#ifdef ENABLE_ARP_2_WOL
+				for (i = 16; i < 40; i += 3) {
+					Write_Mem_Dat_DD( adr_srt + ( i << 2 ),     ( DA[ 1 ] << 16 ) |  DA[ 0 ] );
+					Write_Mem_Dat_DD( adr_srt + ( i << 2 ) + 4, ( DA[ 0 ] << 16 ) |  DA[ 2 ] );
+					Write_Mem_Dat_DD( adr_srt + ( i << 2 ) + 8, ( DA[ 2 ] << 16 ) |  DA[ 1 ] );
+				}
+#endif
+				adr_srt += DMA_PakSize;
+			} // End for (des_num = 0; des_num < eng->dat.Des_Num; des_num++)
+		} // End if ( eng->run.TM_IEEE )
+	}
+	else {
+		if ( eng->arg.GEn_SinglePacket )
+			des_num_max = 1;
+		else
+			des_num_max = eng->dat.Des_Num;
+#ifdef PHY_SPECIAL
+		for ( des_num = 0; des_num < des_num_max; des_num++ ) {
+			Current_framelen = eng->dat.FRAME_LEN[ des_num ];
+			gdata_ptr = special_PHY_txpkt_ptr( eng );
+
+			adr_end = adr_srt + ( ( ( Current_framelen + 3 ) >> 2 ) << 2 );
+			for ( adr = adr_srt; adr < adr_end; adr += 4 ) {
+				Write_Mem_Dat_DD( adr, *gdata_ptr );
+				gdata_ptr++;
+			}
+			adr_srt += DMA_PakSize;
+		} // End for (des_num = 0; des_num < eng->dat.Des_Num; des_num++)
+#else
+		for ( des_num = 0; des_num < des_num_max; des_num++ ) {
+			if ( DbgPrn_BufAdr )
+				printf("[loop[%d]:%4d][des:%4ld][setup_buf  ] %08lx\n", eng->run.Loop_ofcnt, eng->run.Loop, des_num, adr_srt);
+  #ifdef SelectSimpleData
+    #ifdef SimpleData_Fix
+			switch( des_num % SimpleData_FixNum ) {
+				case  0 : gdata = SimpleData_FixVal00; break;
+				case  1 : gdata = SimpleData_FixVal01; break;
+				case  2 : gdata = SimpleData_FixVal02; break;
+				case  3 : gdata = SimpleData_FixVal03; break;
+				case  4 : gdata = SimpleData_FixVal04; break;
+				case  5 : gdata = SimpleData_FixVal05; break;
+				case  6 : gdata = SimpleData_FixVal06; break;
+				case  7 : gdata = SimpleData_FixVal07; break;
+				case  8 : gdata = SimpleData_FixVal08; break;
+				case  9 : gdata = SimpleData_FixVal09; break;
+				case 10 : gdata = SimpleData_FixVal10; break;
+				default : gdata = SimpleData_FixVal11; break;
+			}
+    #else
+			gdata   = 0x11111111 * ((des_num + SEED_START) % 256);
+    #endif
+  #else
+			gdata   = DATA_SEED( des_num + SEED_START );
+  #endif
+			Current_framelen = eng->dat.FRAME_LEN[ des_num ];
+
+			if ( DbgPrn_FRAME_LEN )
+				PRINTF( FP_LOG, "[setup_buf      ] Current_framelen:%08lx[Des:%ld][loop[%d]:%d]\n", Current_framelen, des_num, eng->run.Loop_ofcnt, eng->run.Loop );
+
+//			cnt     = 0;
+#ifdef SelectSimpleDA
+			len     = ( ( ( Current_framelen - 14 ) & 0xff ) << 8) |
+			            ( ( Current_framelen - 14 ) >> 8 );
+#endif				    
+			adr_end = adr_srt + DMA_PakSize;
+			for ( adr = adr_srt; adr < adr_end; adr += 4 ) {
+  #ifdef SelectSimpleDA
+				cnt++;
+				if      ( cnt == 1 ) Write_Mem_Dat_DD( adr, SelectSimpleDA_Dat0 );
+				else if ( cnt == 2 ) Write_Mem_Dat_DD( adr, SelectSimpleDA_Dat1 );
+				else if ( cnt == 3 ) Write_Mem_Dat_DD( adr, SelectSimpleDA_Dat2 );
+				else if ( cnt == 4 ) Write_Mem_Dat_DD( adr, len | (len << 16)   );
+				else
+  #endif
+				                     Write_Mem_Dat_DD( adr, gdata );
+  #ifdef SelectSimpleData
+				gdata = gdata ^ SimpleData_XORVal;
+  #else
+				gdata += DATA_IncVal;
+  #endif
+			}
+			adr_srt += DMA_PakSize;
+		} // End for (des_num = 0; des_num < eng->dat.Des_Num; des_num++)
+#endif
+	} // End if ( eng->run.TM_Burst )
+} // End void setup_buf (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+// Check data of one packet
+//------------------------------------------------------------
+char check_Data (MAC_ENGINE *eng, ULONG datbase, LONG number) {
+	LONG       number_dat;
+	int        index;
+	ULONG      rdata;
+	ULONG      wp_lst_cur;
+	ULONG      adr_las;
+	ULONG      adr;
+	ULONG      adr_srt;
+	ULONG      adr_end;
+#ifdef SelectSimpleDA
+	int        cnt;
+	ULONG      len;
+	ULONG      gdata_bak;
+#endif
+	ULONG      gdata;
+#ifdef PHY_SPECIAL
+	ULONG      *gdata_ptr;
+#endif
+	ULONG      wp;
+
+#ifdef  DbgPrn_FuncHeader
+	printf("check_Data : [%d]%d\n", eng->run.Loop_ofcnt, eng->run.Loop);
+	Debug_delay();
+#endif
+
+	if ( eng->arg.GEn_SinglePacket )
+		number_dat = 0;
+	else
+		number_dat = number;
+
+	wp_lst_cur             = eng->dat.wp_lst[ number ];
+	eng->dat.FRAME_LEN_Cur = eng->dat.FRAME_LEN[ number_dat ];
+
+	if ( DbgPrn_FRAME_LEN )
+		PRINTF( FP_LOG, "[check_Data     ] FRAME_LEN_Cur:%08lx[Des:%ld][loop[%d]:%d]\n", eng->dat.FRAME_LEN_Cur, number, eng->run.Loop_ofcnt, eng->run.Loop );
+
+	adr_srt = datbase;
+	adr_end = adr_srt + PktByteSize;
+
+#ifdef PHY_SPECIAL
+	gdata_ptr = special_PHY_rxpkt_ptr( eng );
+	gdata = *gdata_ptr;
+#elif defined(SelectSimpleData)
+    #ifdef SimpleData_Fix
+	switch( number_dat % SimpleData_FixNum ) {
+		case  0 : gdata = SimpleData_FixVal00; break;
+		case  1 : gdata = SimpleData_FixVal01; break;
+		case  2 : gdata = SimpleData_FixVal02; break;
+		case  3 : gdata = SimpleData_FixVal03; break;
+		case  4 : gdata = SimpleData_FixVal04; break;
+		case  5 : gdata = SimpleData_FixVal05; break;
+		case  6 : gdata = SimpleData_FixVal06; break;
+		case  7 : gdata = SimpleData_FixVal07; break;
+		case  8 : gdata = SimpleData_FixVal08; break;
+		case  9 : gdata = SimpleData_FixVal09; break;
+		case 10 : gdata = SimpleData_FixVal10; break;
+		default : gdata = SimpleData_FixVal11; break;
+	}
+    #else
+	gdata   = 0x11111111 * (( number_dat + SEED_START ) % 256 );
+    #endif
+#else
+	gdata   = DATA_SEED( number_dat + SEED_START );
+#endif
+
+//printf("check_buf: %08lx - %08lx [%08lx]\n", adr_srt, adr_end, datbase);
+	wp      = eng->dat.wp_fir;
+	adr_las = adr_end - 4;
+#ifdef SelectSimpleDA
+	cnt     = 0;
+	len     = ((( eng->dat.FRAME_LEN_Cur-14 ) & 0xff ) << 8 ) |
+	          ( ( eng->dat.FRAME_LEN_Cur-14 )          >> 8 );
+#endif
+
+	if ( DbgPrn_Bufdat )
+		PRINTF( FP_LOG, " Inf:%08lx ~ %08lx(%08lx) %08lx [Des:%ld][loop[%d]:%d]\n", adr_srt, adr_end, adr_las, gdata, number, eng->run.Loop_ofcnt, eng->run.Loop );
+
+	for ( adr = adr_srt; adr < adr_end; adr+=4 ) {
+#ifdef SelectSimpleDA
+		cnt++;
+		if      ( cnt == 1 ) { gdata_bak = gdata; gdata = SelectSimpleDA_Dat0; }
+		else if ( cnt == 2 ) { gdata_bak = gdata; gdata = SelectSimpleDA_Dat1; }
+		else if ( cnt == 3 ) { gdata_bak = gdata; gdata = SelectSimpleDA_Dat2; }
+		else if ( cnt == 4 ) { gdata_bak = gdata; gdata = len | (len << 16);   }
+#endif
+		rdata = Read_Mem_Dat_DD( adr );
+		if ( adr == adr_las )
+			wp = wp & wp_lst_cur;
+
+		if ( ( rdata & wp ) != ( gdata & wp ) ) {
+			PRINTF( FP_LOG, "\nError: Adr:%08lx[%3ld] (%08lx) (%08lx:%08lx) [Des:%ld][loop[%d]:%d]\n", adr, ( adr - adr_srt ) / 4, rdata, gdata, wp, number, eng->run.Loop_ofcnt, eng->run.Loop );
+			for ( index = 0; index < 6; index++ )
+				PRINTF( FP_LOG, "Rep  : Adr:%08lx      (%08lx) (%08lx:%08lx) [Des:%ld][loop[%d]:%d]\n", adr, Read_Mem_Dat_DD( adr ), gdata, wp, number, eng->run.Loop_ofcnt, eng->run.Loop );
+
+			if ( DbgPrn_DumpMACCnt )
+				dump_mac_ROreg( eng );
+
+			return( FindErr( eng, Err_Flag_Check_Buf_Data ) );
+		} // End if ( (rdata & wp) != (gdata & wp) )
+		if ( DbgPrn_BufdatDetail )
+			PRINTF( FP_LOG, " Adr:%08lx[%3ld] (%08lx) (%08lx:%08lx) [Des:%ld][loop[%d]:%d]\n", adr, ( adr - adr_srt ) / 4, rdata, gdata, wp, number, eng->run.Loop_ofcnt, eng->run.Loop );
+
+#ifdef SelectSimpleDA
+		if ( cnt <= 4 )
+			gdata = gdata_bak;
+#endif
+
+#ifdef PHY_SPECIAL
+		gdata_ptr++;
+		gdata = *gdata_ptr;
+#elif defined(SelectSimpleData)
+		gdata = gdata ^ SimpleData_XORVal;
+#else
+		gdata += DATA_IncVal;
+#endif
+
+		wp     = 0xffffffff;
+	}
+	return(0);
+} // End char check_Data (MAC_ENGINE *eng, ULONG datbase, LONG number)
+
+//------------------------------------------------------------
+char check_buf (MAC_ENGINE *eng, int loopcnt) {
+	LONG       des_num;
+	ULONG      desadr;
+	ULONG      datbase;
+
+#ifdef  DbgPrn_FuncHeader
+	printf("check_buf  : [%d]%d\n", eng->run.Loop_ofcnt, eng->run.Loop);
+	Debug_delay();
+#endif
+
+	desadr = eng->run.RDES_BASE + ( 16 * eng->dat.Des_Num ) - 4;//base for read/write
+	for ( des_num = eng->dat.Des_Num - 1; des_num >= 0; des_num-- ) {
+		datbase = AT_BUF_MEMRW( Read_Mem_Des_DD( desadr ) & 0xfffffffc);//base for read/write
+		if ( check_Data( eng, datbase, des_num ) ) {
+			check_int ( eng, "" );
+			return(1);
+		}
+		desadr-= 16;
+	}
+	if ( check_int ( eng, "" ) )
+		return(1);
+
+#if defined(Delay_CheckData_LoopNum) && defined(Delay_CheckData)
+	if ( ( loopcnt % Delay_CheckData_LoopNum ) == 0 ) DELAY( Delay_CheckData );
+#endif
+	return(0);
+} // End char check_buf (MAC_ENGINE *eng, int loopcnt)
+
+//------------------------------------------------------------
+// Descriptor
+//------------------------------------------------------------
+void setup_txdes (MAC_ENGINE *eng, ULONG desadr, ULONG bufbase) {
+	ULONG      bufadr;
+	ULONG      bufadrgap;
+	ULONG      desval = 0;
+	LONG       des_num;
+
+#ifdef  DbgPrn_FuncHeader
+	printf("setup_txdes: [%d]%d\n", eng->run.Loop_ofcnt, eng->run.Loop);
+	Debug_delay();
+#endif
+
+	bufadr = bufbase;
+	if ( eng->arg.GEn_SinglePacket )
+		bufadrgap = 0;
+	else
+		bufadrgap = DMA_PakSize;
+
+	if ( eng->run.TM_TxDataEn ) {
+		for ( des_num = 0; des_num < eng->dat.Des_Num; des_num++ ) {
+			eng->dat.FRAME_LEN_Cur = eng->dat.FRAME_LEN[ des_num ];
+			desval                 = TDES_IniVal;
+			Write_Mem_Des_DD( desadr + 0x04, 0      );
+			Write_Mem_Des_DD( desadr + 0x08, 0      );
+			Write_Mem_Des_DD( desadr + 0x0C, bufadr );
+			Write_Mem_Des_DD( desadr       , desval );
+
+			if ( DbgPrn_FRAME_LEN )
+				PRINTF( FP_LOG, "[setup_txdes    ] FRAME_LEN_Cur:%08lx[Des:%ld][loop[%d]:%d]\n", eng->dat.FRAME_LEN_Cur, des_num, eng->run.Loop_ofcnt, eng->run.Loop );
+
+			if ( DbgPrn_BufAdr )
+				printf("[loop[%d]:%4d][des:%4ld][setup_txdes] %08lx [%08lx]\n", eng->run.Loop_ofcnt, eng->run.Loop, des_num, desadr, bufadr);
+
+			desadr += 16;
+			bufadr += bufadrgap;
+		}
+		Write_Mem_Des_DD( desadr - 0x10, desval | EOR_IniVal );
+	}
+	else {
+		Write_Mem_Des_DD( desadr, 0);
+	} // End if ( eng->run.TM_TxDataEn )
+} // End void setup_txdes (ULONG desadr, ULONG bufbase)
+
+//------------------------------------------------------------
+void setup_rxdes (MAC_ENGINE *eng, ULONG desadr, ULONG bufbase) {
+	ULONG      bufadr;
+	ULONG      desval;
+	LONG       des_num;
+
+#ifdef  DbgPrn_FuncHeader
+	printf("setup_rxdes: [%d]%d\n", eng->run.Loop_ofcnt, eng->run.Loop);
+	Debug_delay();
+#endif
+
+	bufadr = bufbase;
+	desval = RDES_IniVal;
+	if ( eng->run.TM_RxDataEn ) {
+		for ( des_num = 0; des_num < eng->dat.Des_Num; des_num++ ) {
+			Write_Mem_Des_DD( desadr + 0x04, 0      );
+			Write_Mem_Des_DD( desadr + 0x08, 0      );
+			Write_Mem_Des_DD( desadr + 0x0C, bufadr );
+			Write_Mem_Des_DD( desadr       , desval );
+
+			if ( DbgPrn_BufAdr )
+				printf("[loop[%d]:%4d][des:%4ld][setup_rxdes] %08lx [%08lx]\n", eng->run.Loop_ofcnt, eng->run.Loop, des_num, desadr, bufadr);
+
+			desadr += 16;
+			bufadr += DMA_PakSize;
+		}
+		Write_Mem_Des_DD( desadr - 0x10, desval | EOR_IniVal );
+	}
+	else {
+		Write_Mem_Des_DD( desadr, 0x80000000 );
+	} // End if ( eng->run.TM_RxDataEn )
+} // End void setup_rxdes (ULONG desadr, ULONG bufbase)
+
+//------------------------------------------------------------
+// First setting TX and RX information
+//------------------------------------------------------------
+void setup_des (MAC_ENGINE *eng, ULONG bufnum) {
+#ifdef  DbgPrn_FuncHeader
+	printf("setup_des: %ld\n", bufnum);
+	Debug_delay();
+#else
+	if ( DbgPrn_BufAdr ) {
+		printf("setup_des: %ld\n", bufnum);
+		Debug_delay();
+	}
+#endif
+
+//	eng->dat.DMA_Base_Tx = CPU_BUS_ADDR_SDRAM_OFFSET + ZeroCopy_OFFSET + eng->dat.DMA_Base_Setup; // 20130730
+//	eng->dat.DMA_Base_Rx = CPU_BUS_ADDR_SDRAM_OFFSET + ZeroCopy_OFFSET + GET_DMA_BASE(0); // 20130730
+	eng->dat.DMA_Base_Tx = ZeroCopy_OFFSET + eng->dat.DMA_Base_Setup; // 20130730
+	eng->dat.DMA_Base_Rx = ZeroCopy_OFFSET + GET_DMA_BASE(0); // 20130730
+#ifndef Enable_MAC_ExtLoop
+	setup_txdes( eng, eng->run.TDES_BASE, AT_MEMRW_BUF( eng->dat.DMA_Base_Tx ) );//base for read/write //base of the descriptor
+#endif
+	setup_rxdes( eng, eng->run.RDES_BASE, AT_MEMRW_BUF( eng->dat.DMA_Base_Rx ) );//base for read/write //base of the descriptor
+} // End void setup_des (ULONG bufnum)
+
+//------------------------------------------------------------
+// Move buffer point of TX and RX descriptor to next DMA buffer
+//------------------------------------------------------------
+void setup_des_loop (MAC_ENGINE *eng, ULONG bufnum) {
+	LONG       des_num;
+	ULONG      H_rx_desadr;
+	ULONG      H_tx_desadr;
+	ULONG      H_tx_bufadr;
+	ULONG      H_rx_bufadr;
+
+#ifdef  DbgPrn_FuncHeader
+	printf("setup_rxdes_loop: %ld\n", bufnum);
+	Debug_delay();
+#else
+	if ( DbgPrn_BufAdr ) {
+		printf("setup_des_loop: %ld\n", bufnum);
+		Debug_delay();
+	}
+#endif
+
+	if ( eng->run.TM_RxDataEn ) {
+		H_rx_bufadr = AT_MEMRW_BUF( eng->dat.DMA_Base_Rx );//base of the descriptor
+		H_rx_desadr = eng->run.RDES_BASE;//base for read/write
+		for ( des_num = 0; des_num < eng->dat.Des_Num - 1; des_num++ ) {
+			Write_Mem_Des_DD( H_rx_desadr + 0x0C, H_rx_bufadr );
+			Write_Mem_Des_DD( H_rx_desadr       , RDES_IniVal );
+			if ( DbgPrn_BufAdr )
+				printf("[loop[%d]:%4d][des:%4ld][setup_rxdes] %08lx [%08lx]\n", eng->run.Loop_ofcnt, eng->run.Loop, des_num, H_rx_desadr, H_rx_bufadr);
+
+			H_rx_bufadr += DMA_PakSize;
+			H_rx_desadr += 16;
+		}
+		Write_Mem_Des_DD( H_rx_desadr + 0x0C, H_rx_bufadr );
+		Write_Mem_Des_DD( H_rx_desadr       , RDES_IniVal | EOR_IniVal );
+		if ( DbgPrn_BufAdr )
+			printf("[loop[%d]:%4d][des:%4ld][setup_rxdes] %08lx [%08lx]\n", eng->run.Loop_ofcnt, eng->run.Loop, des_num, H_rx_desadr, H_rx_bufadr);
+	}
+//	Write_Reg_MAC_DD( eng, 0x1c, 0x00000000 ); // Rx Poll
+
+	if ( eng->run.TM_TxDataEn ) {
+		H_tx_bufadr = AT_MEMRW_BUF( eng->dat.DMA_Base_Tx );//base of the descriptor
+		H_tx_desadr = eng->run.TDES_BASE;//base for read/write
+		for ( des_num = 0; des_num < eng->dat.Des_Num - 1; des_num++ ) {
+			eng->dat.FRAME_LEN_Cur = eng->dat.FRAME_LEN[ des_num ];
+			Write_Mem_Des_DD( H_tx_desadr + 0x0C, H_tx_bufadr );
+			Write_Mem_Des_DD( H_tx_desadr       , TDES_IniVal );
+			if ( DbgPrn_BufAdr )
+				printf("[loop[%d]:%4d][des:%4ld][setup_txdes] %08lx [%08lx]\n", eng->run.Loop_ofcnt, eng->run.Loop, des_num, H_tx_desadr, H_tx_bufadr);
+
+			H_tx_bufadr += DMA_PakSize;
+			H_tx_desadr += 16;
+		}
+		eng->dat.FRAME_LEN_Cur = eng->dat.FRAME_LEN[ des_num ];
+		Write_Mem_Des_DD( H_tx_desadr + 0x0C, H_tx_bufadr );
+		Write_Mem_Des_DD( H_tx_desadr       , TDES_IniVal | EOR_IniVal );
+		if ( DbgPrn_BufAdr )
+			printf("[loop[%d]:%4d][des:%4ld][setup_txdes] %08lx [%08lx]\n", eng->run.Loop_ofcnt, eng->run.Loop, des_num, H_tx_desadr, H_tx_bufadr);
+	}
+//	Write_Reg_MAC_DD( eng, 0x18, 0x00000000 ); // Tx Poll
+} // End void setup_des_loop (ULONG bufnum)
+
+//------------------------------------------------------------
+char check_des_header_Tx (MAC_ENGINE *eng, char *type, ULONG adr, LONG desnum) {
+	int        timeout = 0;
+
+	eng->dat.TxDes0DW = Read_Mem_Des_DD( adr );
+
+	while ( HWOwnTx( eng->dat.TxDes0DW ) ) {
+		// we will run again, if transfer has not been completed.
+		if ( ( eng->run.TM_Burst || eng->run.TM_RxDataEn ) && ( ++timeout > eng->run.TIME_OUT_Des ) ) {
+			PRINTF( FP_LOG, "[%sTxDesOwn] Address %08lx = %08lx [Des:%ld][loop[%d]:%d]\n", type, adr, eng->dat.TxDes0DW, desnum, eng->run.Loop_ofcnt, eng->run.Loop );
+			return( FindErr_Des( eng, Des_Flag_TxOwnTimeOut ) );
+		}
+//		Write_Reg_MAC_DD( eng, 0x1c, 0x00000000 );//Rx Poll
+//		Write_Reg_MAC_DD( eng, 0x18, 0x00000000 );//Tx Poll
+
+#ifdef Delay_ChkTxOwn
+		DELAY( Delay_ChkTxOwn );
+#endif
+		eng->dat.TxDes0DW = Read_Mem_Des_DD( adr );
+	}
+
+	return(0);
+} // End char check_des_header_Tx (MAC_ENGINE *eng, char *type, ULONG adr, LONG desnum)
+
+//------------------------------------------------------------
+char check_des_header_Rx (MAC_ENGINE *eng, char *type, ULONG adr, LONG desnum) {
+#ifdef CheckRxOwn
+	int        timeout = 0;
+
+	eng->dat.RxDes0DW = Read_Mem_Des_DD( adr );
+
+	while ( HWOwnRx( eng->dat.RxDes0DW ) ) {
+		// we will run again, if transfer has not been completed.
+		if ( eng->run.TM_TxDataEn && ( ++timeout > eng->run.TIME_OUT_Des ) ) {
+			PRINTF( FP_LOG, "[%sRxDesOwn] Address %08lx = %08lx [Des:%ld][loop[%d]:%d]\n", type, adr, eng->dat.RxDes0DW, desnum, eng->run.Loop_ofcnt, eng->run.Loop );
+			return( FindErr_Des( eng, Des_Flag_RxOwnTimeOut ) );
+		}
+//		Write_Reg_MAC_DD( eng, 0x1c, 0x00000000 );//Rx Poll
+//		Write_Reg_MAC_DD( eng, 0x18, 0x00000000 );//Tx Poll
+
+  #ifdef Delay_ChkRxOwn
+		DELAY( Delay_ChkRxOwn );
+  #endif
+		eng->dat.RxDes0DW = Read_Mem_Des_DD( adr );
+	};
+
+
+  #ifdef CheckRxLen
+	if ( DbgPrn_FRAME_LEN )
+		PRINTF( FP_LOG, "[%sRxDes          ] FRAME_LEN_Cur:%08lx[Des:%ld][loop[%d]:%d]\n", type, ( eng->dat.FRAME_LEN_Cur + 4 ), desnum, eng->run.Loop_ofcnt, eng->run.Loop );
+
+	if ( ( eng->dat.RxDes0DW & 0x3fff ) != ( eng->dat.FRAME_LEN_Cur + 4 ) ) {
+		eng->dat.RxDes3DW = Read_Mem_Des_DD( adr + 12 );
+		PRINTF( FP_LOG, "[%sRxDes] Error Frame Length %08lx:%08lx %08lx(%4ld/%4ld) [Des:%ld][loop[%d]:%d]\n",   type, adr, eng->dat.RxDes0DW, eng->dat.RxDes3DW, ( eng->dat.RxDes0DW & 0x3fff ), ( eng->dat.FRAME_LEN_Cur + 4 ), desnum, eng->run.Loop_ofcnt, eng->run.Loop );
+		FindErr_Des( eng, Des_Flag_FrameLen );
+	}
+  #endif // End CheckRxLen
+
+	if ( eng->dat.RxDes0DW & Check_ErrMask_ALL ) {
+		eng->dat.RxDes3DW = Read_Mem_Des_DD( adr + 12 );
+  #ifdef CheckRxErr
+		if ( eng->dat.RxDes0DW & Check_ErrMask_RxErr ) {
+			PRINTF( FP_LOG, "[%sRxDes] Error RxErr        %08lx:%08lx %08lx            [Des:%ld][loop[%d]:%d]\n", type, adr, eng->dat.RxDes0DW, eng->dat.RxDes3DW, desnum, eng->run.Loop_ofcnt, eng->run.Loop );
+			FindErr_Des( eng, Des_Flag_RxErr );
+		}
+  #endif // End CheckRxErr
+
+  #ifdef CheckCRC
+		if ( eng->dat.RxDes0DW & Check_ErrMask_CRC ) {
+			PRINTF( FP_LOG, "[%sRxDes] Error CRC          %08lx:%08lx %08lx            [Des:%ld][loop[%d]:%d]\n", type, adr, eng->dat.RxDes0DW, eng->dat.RxDes3DW, desnum, eng->run.Loop_ofcnt, eng->run.Loop );
+			FindErr_Des( eng, Des_Flag_CRC );
+		}
+  #endif // End CheckCRC
+
+  #ifdef CheckFTL
+		if ( eng->dat.RxDes0DW & Check_ErrMask_FTL ) {
+			PRINTF( FP_LOG, "[%sRxDes] Error FTL          %08lx:%08lx %08lx            [Des:%ld][loop[%d]:%d]\n", type, adr, eng->dat.RxDes0DW, eng->dat.RxDes3DW, desnum, eng->run.Loop_ofcnt, eng->run.Loop );
+			FindErr_Des( eng, Des_Flag_FTL );
+		}
+  #endif // End CheckFTL
+
+  #ifdef CheckRunt
+		if ( eng->dat.RxDes0DW & Check_ErrMask_Runt) {
+			PRINTF( FP_LOG, "[%sRxDes] Error Runt         %08lx:%08lx %08lx            [Des:%ld][loop[%d]:%d]\n", type, adr, eng->dat.RxDes0DW, eng->dat.RxDes3DW, desnum, eng->run.Loop_ofcnt, eng->run.Loop );
+			FindErr_Des( eng, Des_Flag_Runt );
+		}
+  #endif // End CheckRunt
+
+  #ifdef CheckOddNibble
+		if ( eng->dat.RxDes0DW & Check_ErrMask_OddNibble ) {
+			PRINTF( FP_LOG, "[%sRxDes] Odd Nibble         %08lx:%08lx %08lx            [Des:%ld][loop[%d]:%d]\n", type, adr, eng->dat.RxDes0DW, eng->dat.RxDes3DW, desnum, eng->run.Loop_ofcnt, eng->run.Loop );
+			FindErr_Des( eng, Des_Flag_OddNibble );
+		}
+  #endif // End CheckOddNibble
+
+  #ifdef CheckRxFIFOFull
+		if ( eng->dat.RxDes0DW & Check_ErrMask_RxFIFOFull ) {
+			PRINTF( FP_LOG, "[%sRxDes] Error Rx FIFO Full %08lx:%08lx %08lx            [Des:%ld][loop[%d]:%d]\n", type, adr, eng->dat.RxDes0DW, eng->dat.RxDes3DW, desnum, eng->run.Loop_ofcnt, eng->run.Loop );
+			FindErr_Des( eng, Des_Flag_RxFIFOFull );
+		}
+  #endif // End CheckRxFIFOFull
+	}
+
+#endif // End CheckRxOwn
+
+	if ( eng->flg.Err_Flag )
+		return(1);
+	else
+		return(0);
+} // End char check_des_header_Rx (MAC_ENGINE *eng, char *type, ULONG adr, LONG desnum)
+
+//------------------------------------------------------------
+char check_des (MAC_ENGINE *eng, ULONG bufnum, int checkpoint) {
+	LONG       desnum;
+	CHAR       desnum_last;
+	ULONG      H_rx_desadr;
+	ULONG      H_tx_desadr;
+	ULONG      H_tx_bufadr;
+	ULONG      H_rx_bufadr;
+#ifdef Delay_DesGap
+	ULONG      dly_cnt = 0;
+	ULONG      dly_max = Delay_CntMaxIncVal;
+#endif
+
+#ifdef  DbgPrn_FuncHeader
+	printf("check_des  : [%d]%d(%d)\n", eng->run.Loop_ofcnt, eng->run.Loop, checkpoint);
+	Debug_delay();
+#endif
+
+	// Fire the engine to send and recvice
+	Write_Reg_MAC_DD( eng, 0x1c, 0x00000000 );//Rx Poll
+	Write_Reg_MAC_DD( eng, 0x18, 0x00000000 );//Tx Poll
+
+#ifndef SelectSimpleDes
+	H_tx_bufadr = AT_MEMRW_BUF( eng->dat.DMA_Base_Tx );//base of the descriptor
+	H_rx_bufadr = AT_MEMRW_BUF( eng->dat.DMA_Base_Rx );//base of the descriptor
+#endif
+	H_rx_desadr = eng->run.RDES_BASE;//base for read/write
+	H_tx_desadr = eng->run.TDES_BASE;//base for read/write
+
+#ifdef Delay_DES
+	DELAY( Delay_DES );
+#endif
+
+	for ( desnum = 0; desnum < eng->dat.Des_Num; desnum++ ) {
+		desnum_last = ( desnum == ( eng->dat.Des_Num - 1 ) ) ? 1 : 0;
+		if ( DbgPrn_BufAdr ) {
+			if ( checkpoint )
+				printf("[loop[%d]:%4d][des:%4ld][check_des  ] %08lx %08lx [%08lx %08lx]\n", eng->run.Loop_ofcnt, eng->run.Loop, desnum, ( H_tx_desadr ), ( H_rx_desadr ), Read_Mem_Des_DD( H_tx_desadr + 12 ), Read_Mem_Des_DD( H_rx_desadr + 12 ) );
+			else
+				printf("[loop[%d]:%4d][des:%4ld][check_des  ] %08lx %08lx [%08lx %08lx]->[%08lx %08lx]\n", eng->run.Loop_ofcnt, eng->run.Loop, desnum, ( H_tx_desadr ), ( H_rx_desadr ), Read_Mem_Des_DD( H_tx_desadr + 12 ), Read_Mem_Des_DD( H_rx_desadr + 12 ), H_tx_bufadr, H_rx_bufadr );
+		}
+
+		//[Delay]--------------------
+#ifdef Delay_DesGap
+//		if ( dly_cnt++ > 3 ) {
+		if ( dly_cnt > Delay_CntMax ) {
+//			switch ( rand() % 12 ) {
+//				case 1 : dly_max = 00000; break;
+//				case 3 : dly_max = 20000; break;
+//				case 5 : dly_max = 40000; break;
+//				case 7 : dly_max = 60000; break;
+//				defaule: dly_max = 70000; break;
+//			}
+//
+//			dly_max += ( rand() % 4 ) * 14321;
+//
+//			while (dly_cnt < dly_max) {
+//				dly_cnt++;
+//			}
+			DELAY( Delay_DesGap );
+			dly_cnt = 0;
+		}
+		else {
+			dly_cnt++;
+//			timeout = 0;
+//			while (timeout < 50000) {timeout++;};
+		}
+#endif // End Delay_DesGap
+
+		//[Check Owner Bit]--------------------
+		eng->dat.FRAME_LEN_Cur = eng->dat.FRAME_LEN[ desnum ];
+		if ( DbgPrn_FRAME_LEN )
+			PRINTF( FP_LOG, "[check_des      ] FRAME_LEN_Cur:%08lx[Des:%ld][loop[%d]:%d]%d\n", eng->dat.FRAME_LEN_Cur, desnum, eng->run.Loop_ofcnt, eng->run.Loop, checkpoint );
+
+		// Check the description of Tx and Rx
+		if ( eng->run.TM_TxDataEn && check_des_header_Tx( eng, "", H_tx_desadr, desnum ) ) {
+			eng->flg.CheckDesFail_DesNum = desnum;
+
+			return(1);
+		}
+		if ( eng->run.TM_RxDataEn && check_des_header_Rx( eng, "", H_rx_desadr, desnum ) ) {
+			eng->flg.CheckDesFail_DesNum = desnum;
+
+			return(1);
+		}
+
+#ifndef SelectSimpleDes
+		if ( !checkpoint ) {
+			// Setting buffer address to description of Tx and Rx on next stage
+			if ( eng->run.TM_RxDataEn ) {
+				Write_Mem_Des_DD( H_rx_desadr + 0x0C, H_rx_bufadr );
+				if ( desnum_last )
+					Write_Mem_Des_DD( H_rx_desadr, RDES_IniVal | EOR_IniVal );
+				else
+					Write_Mem_Des_DD( H_rx_desadr, RDES_IniVal );
+				Write_Reg_MAC_DD( eng, 0x1c, 0x00000000 ); //Rx Poll
+				H_rx_bufadr += DMA_PakSize;
+			}
+			if ( eng->run.TM_TxDataEn ) {
+				Write_Mem_Des_DD( H_tx_desadr + 0x0C, H_tx_bufadr );
+				if ( desnum_last )
+					Write_Mem_Des_DD( H_tx_desadr, TDES_IniVal | EOR_IniVal );
+				else
+					Write_Mem_Des_DD( H_tx_desadr, TDES_IniVal );
+				Write_Reg_MAC_DD( eng, 0x18, 0x00000000 ); //Tx Poll
+				H_tx_bufadr += DMA_PakSize;
+			}
+		}
+#endif // End SelectSimpleDes
+
+		H_rx_desadr += 16;
+		H_tx_desadr += 16;
+	} // End for (desnum = 0; desnum < eng->dat.Des_Num; desnum++)
+
+	return(0);
+} // End char check_des (MAC_ENGINE *eng, ULONG bufnum, int checkpoint)
+//#endif
+
+//------------------------------------------------------------
+// Print
+//------------------------------------------------------------
+void PrintMode (MAC_ENGINE *eng) {
+	if (eng->env.MAC34_vld) printf("run_mode[dec]    | 0->MAC1 1->MAC2 2->MAC3 3->MAC4\n");
+	else                    printf("run_mode[dec]    | 0->MAC1 1->MAC2\n");
+}
+
+//------------------------------------------------------------
+void PrintSpeed (MAC_ENGINE *eng) {
+#ifdef Enable_MAC_ExtLoop
+	printf("speed[dec]       | 0->1G   1->100M 2->10M (default:%3d)\n", DEF_GSPEED);
+#else
+	printf("speed[dec]       | 0->1G   1->100M 2->10M  3->all speed (default:%3d)\n", DEF_GSPEED);
+#endif
+}
+
+//------------------------------------------------------------
+void PrintCtrl (MAC_ENGINE *eng) {
+	printf("ctrl[dec]        | bit0  : Reserved\n");
+	printf("(default:%3d)    | bit1  : Enable to use the other MAC's MDC/MDIO\n", DEF_GCTRL);
+	printf("                 | bit2  : Disable recovery PHY's status\n");
+	printf("                 | bit3  : 1->Enable PHY init       0->Disable PHY init\n");
+	printf("                 | bit4  : 1->PHY internal loopback 0->PHY external loopback\n");
+	printf("                 | bit5  : 1->Ignore PHY ID         0->Check PHY ID\n");
+	if ( eng->env.AST2500 ) {
+	printf("                 | bit6  : 1->Enable MAC int-loop   0->Disable MAC int-loop\n");
+	}
+	else if ( eng->env.AST2400 ) {
+	printf("                 | bit7  : 1->Enable MAC int-loop   0->Disable MAC int-loop\n");
+	}
+}
+
+//------------------------------------------------------------
+void PrintLoop (MAC_ENGINE *eng) {
+	printf("loop_max[dec]    | 1G  :  (default:%3d)\n", DEF_GLOOP_MAX * 20);
+	printf("                 | 100M:  (default:%3d)\n", DEF_GLOOP_MAX * 2);
+	printf("                 | 10M :  (default:%3d)\n", DEF_GLOOP_MAX);
+}
+
+//------------------------------------------------------------
+void PrintTest (MAC_ENGINE *eng) {
+	if ( eng->ModeSwitch == MODE_NSCI ) {
+		printf("test_mode[dec]   | 0: NCSI configuration with    Disable_Channel request\n");
+		printf("(default:%3d)    | 1: NCSI configuration without Disable_Channel request\n", DEF_GTESTMODE);
+	}
+	else {
+		printf("test_mode[dec]   | 0: Tx/Rx frame checking\n");
+		printf("(default:%3d)    | 1: Tx output 0xff frame\n", DEF_GTESTMODE);
+		printf("                 | 2: Tx output 0x55 frame\n");
+		printf("                 | 3: Tx output random frame\n");
+		printf("                 | 4: Tx output ARP frame\n");
+		printf("                 | 5: Tx output user defined value frame (default:0x%8x)\n", DEF_GUSER_DEF_PACKET_VAL);
+	} // End if ( eng->ModeSwitch == MODE_NSCI )
+
+	if ( eng->env.AST2300 ) {
+		printf("                 | 6: IO timing testing\n");
+		printf("                 | 7: IO timing/strength testing\n");
+	}
+}
+
+//------------------------------------------------------------
+void PrintPHYAdr (MAC_ENGINE *eng) {
+	printf("phy_adr[dec]     | 0~31: PHY Address (default:%d)\n", DEF_GPHY_ADR);
+}
+
+//------------------------------------------------------------
+void PrintIOTimingBund (MAC_ENGINE *eng) {
+	if ( eng->env.AST2300 )
+		printf("IO margin[dec]   | 0/1/3/5/7/... (default:%d)\n", DEF_GIOTIMINGBUND);
+}
+
+//------------------------------------------------------------
+void PrintPakNUm (MAC_ENGINE *eng) {
+	printf("package_num[dec] | 1~ 8: Total Number of NCSI Package (default:%d)\n", DEF_GPACKAGE2NUM);
+}
+
+//------------------------------------------------------------
+void PrintChlNUm (MAC_ENGINE *eng) {
+	printf("channel_num[dec] | 1~32: Total Number of NCSI Channel (default:%d)\n", DEF_GCHANNEL2NUM);
+}
+
+//------------------------------------------------------------
+void Print_Header (MAC_ENGINE *eng, BYTE option) {
+
+	if      ( eng->run.Speed_sel[ 0 ] ) { PRINTF( option, " 1G   " ); }
+	else if ( eng->run.Speed_sel[ 1 ] ) { PRINTF( option, " 100M " ); }
+	else                                { PRINTF( option, " 10M  " ); }
+
+#ifdef Enable_MAC_ExtLoop
+	PRINTF( option, "Tx/Rx loop\n" );
+#else
+	switch ( eng->arg.GTestMode ) {
+		case 0 : { PRINTF( option, "Tx/Rx frame checking       \n" ); break;                     }
+		case 1 : { PRINTF( option, "Tx output 0xff frame       \n" ); break;                     }
+		case 2 : { PRINTF( option, "Tx output 0x55 frame       \n" ); break;                     }
+		case 3 : { PRINTF( option, "Tx output random frame     \n" ); break;                     }
+		case 4 : { PRINTF( option, "Tx output ARP frame        \n" ); break;                     }
+		case 5 : { PRINTF( option, "Tx output 0x%08lx frame    \n", eng->arg.GUserDVal ); break; }
+		case 6 : { PRINTF( option, "IO delay testing           \n" ); break;                     }
+		case 7 : { PRINTF( option, "IO delay testing(Strength) \n" ); break;                     }
+		case 8 : { PRINTF( option, "Tx frame                   \n" ); break;                     }
+		case 9 : { PRINTF( option, "Rx frame checking          \n" ); break;                     }
+	}
+#endif
+}
+
+//------------------------------------------------------------
+void PrintIO_Header (MAC_ENGINE *eng, BYTE option) {
+
+	if ( eng->run.TM_IOStrength ) {
+		if ( eng->io.Str_max > 1 )
+			{ PRINTF( option, "<IO Strength: SCU%02lx[%2d:%2d]=%2d>", eng->io.Str_reg_idx, eng->io.Str_shf + 1, eng->io.Str_shf, eng->io.Str_i ); }
+		else
+			{ PRINTF( option, "<IO Strength: SCU%02lx[%2d]=%2d>", eng->io.Str_reg_idx, eng->io.Str_shf, eng->io.Str_i ); }
+	}
+
+	if      ( eng->run.Speed_sel[ 0 ] ) { PRINTF( option, "[1G  ]========================================>\n" ); }
+	else if ( eng->run.Speed_sel[ 1 ] ) { PRINTF( option, "[100M]========================================>\n" ); }
+	else                                { PRINTF( option, "[10M ]========================================>\n" ); }
+
+	if ( !(option == FP_LOG) ) {
+#ifdef AST2500_IOMAP
+		PRINTF( option, "   SCU%2lX      ", eng->io.Dly_reg_idx );
+
+		for ( eng->io.Dly_in = eng->io.Dly_in_str; eng->io.Dly_in <= eng->io.Dly_in_end; eng->io.Dly_in+=eng->io.Dly_in_cval ) {
+			eng->io.Dly_in_selval = eng->io.value_ary[ eng->io.Dly_in ];
+			PRINTF( option, "%1x", ( eng->io.Dly_in_selval >> 4 ) );
+		}
+
+		PRINTF( option, "\n%s    ", eng->io.Dly_reg_name_rx_new );
+		for ( eng->io.Dly_in = eng->io.Dly_in_str; eng->io.Dly_in <= eng->io.Dly_in_end; eng->io.Dly_in+=eng->io.Dly_in_cval ) {
+			eng->io.Dly_in_selval = eng->io.value_ary[ eng->io.Dly_in ];
+			PRINTF( option, "%1x", eng->io.Dly_in_selval & 0xf );
+		}
+
+		PRINTF( option, "\n              " );
+		for ( eng->io.Dly_in = eng->io.Dly_in_str; eng->io.Dly_in <= eng->io.Dly_in_end; eng->io.Dly_in+=eng->io.Dly_in_cval ) {
+			if ( eng->io.Dly_in_reg_idx == eng->io.Dly_in ) { PRINTF( option, "|" ); }
+			else                                            { PRINTF( option, " " ); }
+		}
+#else
+		PRINTF( option, "%s   ", eng->io.Dly_reg_name_rx );
+
+		for ( eng->io.Dly_in = eng->io.Dly_in_str; eng->io.Dly_in <= eng->io.Dly_in_end; eng->io.Dly_in+=eng->io.Dly_in_cval ) {
+			eng->io.Dly_in_selval = eng->io.value_ary[ eng->io.Dly_in ];
+			PRINTF( option, "%2x", eng->io.Dly_in_selval & 0xf );
+		}
+
+		PRINTF( option, "\n                   " );
+		for ( eng->io.Dly_in = eng->io.Dly_in_str; eng->io.Dly_in <= eng->io.Dly_in_end; eng->io.Dly_in+=eng->io.Dly_in_cval ) {
+			if ( eng->io.Dly_in_reg_idx == eng->io.Dly_in ) { PRINTF( option, " |" ); }
+			else                                            { PRINTF( option, "  " ); }
+		}
+#endif
+		PRINTF( option, "\n");
+	} // End if ( !(option == FP_LOG) )
+} // End void PrintIO_Header (MAC_ENGINE *eng, BYTE option)
+
+//------------------------------------------------------------
+void PrintIO_LineS (MAC_ENGINE *eng, BYTE option) {
+
+#ifdef AST2500_IOMAP
+	if ( eng->io.Dly_out_reg_hit )
+		{ PRINTF( option, "%s%02x:-", eng->io.Dly_reg_name_tx_new, eng->io.Dly_out_selval ); }
+	else
+		{ PRINTF( option, "%s%02x: ", eng->io.Dly_reg_name_tx_new, eng->io.Dly_out_selval ); }
+#else
+	if ( eng->io.Dly_out_reg_hit )
+		{ PRINTF( option, "%s%01x:-", eng->io.Dly_reg_name_tx, eng->io.Dly_out_selval ); }
+	else
+		{ PRINTF( option, "%s%01x: ", eng->io.Dly_reg_name_tx, eng->io.Dly_out_selval ); }
+#endif
+} // End void PrintIO_LineS (MAC_ENGINE *eng, BYTE option)
+
+//------------------------------------------------------------
+void PrintIO_Line (MAC_ENGINE *eng, BYTE option) {
+
+#ifdef AST2500_IOMAP
+	if ( ( eng->io.Dly_in_reg == eng->io.Dly_in_selval ) && eng->io.Dly_out_reg_hit ) {
+		if ( eng->io.Dly_result ) { PRINTF( option, "X" ); }
+		else                      { PRINTF( option, "O" ); }
+	}
+	else {
+		if ( eng->io.Dly_result ) { PRINTF( option, "x" ); }
+		else                      { PRINTF( option, "o" ); }
+	}
+#else
+	if ( ( eng->io.Dly_in_reg == eng->io.Dly_in_selval ) && eng->io.Dly_out_reg_hit ) {
+		if ( eng->io.Dly_result ) { PRINTF( option, " X" ); }
+		else                      { PRINTF( option, " O" ); }
+	}
+	else {
+		if ( eng->io.Dly_result ) { PRINTF( option, " x" ); }
+		else                      { PRINTF( option, " o" ); }
+	}
+#endif
+} // End void PrintIO_Line (MAC_ENGINE *eng, BYTE option)
+
+//------------------------------------------------------------
+void PrintIO_Line_LOG (MAC_ENGINE *eng) {
+#ifdef PHY_SPECIAL
+	special_PHY_debug( eng );
+#endif
+	if ( eng->io.Dly_result ) {
+		PRINTF( FP_LOG, "\n=====>[Check]%s%2x, %s%2x:  X\n", eng->io.Dly_reg_name_rx, eng->io.Dly_in_selval, eng->io.Dly_reg_name_tx, eng->io.Dly_out_selval );
+	}
+	else {
+		PRINTF( FP_LOG, "\n=====>[Check]%s%2x, %s%2x:  O\n", eng->io.Dly_reg_name_rx, eng->io.Dly_in_selval, eng->io.Dly_reg_name_tx, eng->io.Dly_out_selval );
+	}
+}
+
+//------------------------------------------------------------
+// main
+//------------------------------------------------------------
+void Calculate_LOOP_CheckNum (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("Calculate_LOOP_CheckNum\n");
+	Debug_delay();
+#endif
+
+#define ONE_MBYTE    1048576
+
+#ifdef CheckDataEveryTime
+	eng->run.LOOP_CheckNum = 1;
+#else
+	if ( eng->run.IO_MrgChk || ( eng->arg.GSpeed == SET_1G_100M_10MBPS ) || ( eng->arg.GSpeed == SET_100M_10MBPS ) ) {
+		eng->run.LOOP_CheckNum = eng->run.LOOP_MAX;
+	}
+	else {
+		switch ( eng->arg.GSpeed ) {
+			case SET_1GBPS    : eng->run.CheckBuf_MBSize =  MOVE_DATA_MB_SEC      ; break; // 1G
+			case SET_100MBPS  : eng->run.CheckBuf_MBSize = (MOVE_DATA_MB_SEC >> 3); break; // 100M ~ 1G / 8
+			case SET_10MBPS   : eng->run.CheckBuf_MBSize = (MOVE_DATA_MB_SEC >> 6); break; // 10M  ~ 1G / 64
+		}
+		eng->run.LOOP_CheckNum = ( eng->run.CheckBuf_MBSize / ( ((eng->dat.Des_Num * DMA_PakSize) / ONE_MBYTE ) + 1) );
+	}
+#endif
+}
+
+//------------------------------------------------------------
+void TestingSetup (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("TestingSetup\n");
+	Debug_delay();
+#endif
+
+#ifdef SLT_UBOOT
+#elif defined(LinuxAP)
+#else
+  #ifdef RAND_SIZE_SED
+	srand((unsigned) RAND_SIZE_SED );
+  #else
+	srand((unsigned) eng->timestart );
+  #endif
+#endif
+
+	//[Disable VGA]--------------------
+#ifdef Disable_VGA
+//	if ( eng->arg.GLOOP_INFINI & ~( eng->run.TM_Burst || eng->run.TM_IOTiming ) ) {
+		eng->env.VGAModeVld = 1;
+		outp(0x3d4, 0x17);
+		eng->env.VGAMode = inp(0x3d5);
+		outp(0x3d4, 0x17);
+		outp(0x3d5, 0);
+//	}
+#endif
+
+	//[Setup]--------------------
+	setup_framesize( eng );
+	setup_buf( eng );
+}
+
+//------------------------------------------------------------
+// Return 1 ==> fail
+// Return 0 ==> PASS
+//------------------------------------------------------------
+char TestingLoop (MAC_ENGINE *eng, ULONG loop_checknum) {
+	char       checkprd;
+	char       looplast;
+	char       checken;
+#ifdef Enable_MAC_ExtLoop
+  #ifdef Enable_MAC_ExtLoop_PakcegMode
+	ULONG      desadr;
+  #endif
+#endif
+
+#ifdef Enable_ShowBW
+  #ifdef SLT_UBOOT
+  #else
+	clock_t    timeold;
+  #endif
+#endif
+
+#ifdef  DbgPrn_FuncHeader
+	printf("TestingLoop: [%d]%d\n", eng->run.Loop_ofcnt, eng->run.Loop);
+	Debug_delay();
+#endif
+
+	if ( DbgPrn_DumpMACCnt )
+		dump_mac_ROreg( eng );
+
+	//[Setup]--------------------
+	eng->run.Loop = 0;
+	checkprd = 0;
+	checken  = 0;
+	looplast = 0;
+
+
+	setup_des( eng, 0 );
+
+#ifdef Enable_ShowBW
+  #ifdef SLT_UBOOT
+  #else
+	timeold = clock();
+  #endif
+#endif
+	if ( eng->run.TM_WaitStart ) {
+		printf("Press any key to start...\n");
+		GET_CAHR();
+	}
+
+#ifdef Enable_MAC_ExtLoop
+	while ( 0 ) {
+#else
+	while ( ( eng->run.Loop < eng->run.LOOP_MAX ) || eng->arg.GLOOP_INFINI ) {
+#endif
+		looplast = !eng->arg.GLOOP_INFINI && ( eng->run.Loop == eng->run.LOOP_MAX - 1 );
+
+#ifdef CheckRxBuf
+		if ( !eng->run.TM_Burst )
+			checkprd = ( ( eng->run.Loop % loop_checknum ) == ( loop_checknum - 1 ) );
+		checken = looplast | checkprd;
+#endif
+
+
+
+#ifdef  DbgPrn_FuncHeader
+		printf("for start ======> [%d]%d/%d(%d) looplast:%d checkprd:%d checken:%d\n", eng->run.Loop_ofcnt, eng->run.Loop, eng->run.LOOP_MAX, eng->arg.GLOOP_INFINI, looplast, checkprd, checken);
+		Debug_delay();
+#else
+		if ( DbgPrn_BufAdr ) {
+			printf("for start ======> [%d]%d/%d(%d) looplast:%d checkprd:%d checken:%d\n", eng->run.Loop_ofcnt, eng->run.Loop, eng->run.LOOP_MAX, eng->arg.GLOOP_INFINI, looplast, checkprd, checken);
+			Debug_delay();
+		}
+#endif
+
+#ifndef PHY_SPECIAL
+		if ( eng->run.TM_RxDataEn )
+			eng->dat.DMA_Base_Tx = eng->dat.DMA_Base_Rx;
+#endif
+//		eng->dat.DMA_Base_Rx = CPU_BUS_ADDR_SDRAM_OFFSET + ZeroCopy_OFFSET + GET_DMA_BASE( eng->run.Loop + 1 ); // 20130730
+		eng->dat.DMA_Base_Rx = ZeroCopy_OFFSET + GET_DMA_BASE( eng->run.Loop + 1 ); // 20130730
+		//[Check DES]--------------------
+		if ( check_des( eng, eng->run.Loop, checken ) ) {
+			//descriptor error
+#ifdef CheckRxBuf
+			eng->dat.Des_Num = eng->flg.CheckDesFail_DesNum + 1;
+			if ( checkprd )
+				check_buf( eng, loop_checknum );
+			else
+				check_buf( eng, ( eng->run.LOOP_MAX % loop_checknum ) );
+			eng->dat.Des_Num = eng->dat.Des_Num_Org;
+#endif
+
+			if ( DbgPrn_DumpMACCnt )
+				dump_mac_ROreg ( eng );
+
+			return(1);
+		}
+
+		//[Check Buf]--------------------
+		if ( eng->run.TM_RxDataEn && checken ) {
+#ifdef Enable_ShowBW
+  #ifdef SLT_UBOOT
+  #elif defined(LinuxAP)
+  #else
+			eng->timeused = ( clock() - timeold ) / (double) CLK_TCK;
+  #endif
+#endif
+
+			if ( checkprd ) {
+#ifdef Enable_ShowBW
+  #if defined(LinuxAP)
+  #else
+				printf("[run loop:%3d] BandWidth: %7.2f Mbps, %6.2f sec\n", loop_checknum, ((double)loop_checknum * (double)eng->dat.Total_frame_len * 8.0) / ((double)eng->timeused * 1000000.0), eng->timeused);
+				PRINTF( FP_LOG, "[run loop:%3d] BandWidth: %7.2f Mbps, %6.2f sec\n", loop_checknum, ((double)loop_checknum * (double)eng->dat.Total_frame_len * 8.0) / ((double)eng->timeused * 1000000.0), eng->timeused );
+  #endif
+#endif
+
+#ifdef CheckRxBuf
+				if ( check_buf( eng, loop_checknum ) )
+					return(1);
+#endif
+			}
+			else {
+#ifdef Enable_ShowBW
+  #if defined(LinuxAP)
+  #else
+				printf("[run loop:%3d] BandWidth: %7.2f Mbps, %6.2f sec\n", (eng->run.LOOP_MAX % loop_checknum), ((double)(eng->run.LOOP_MAX % loop_checknum) * (double)eng->dat.Total_frame_len * 8.0) / ((double)eng->timeused * 1000000.0), eng->timeused);
+				PRINTF( FP_LOG, "[run loop:%3d] BandWidth: %7.2f Mbps, %6.2f sec\n", (eng->run.LOOP_MAX % loop_checknum), ((double)(eng->run.LOOP_MAX % loop_checknum) * (double)eng->dat.Total_frame_len * 8.0) / ((double)eng->timeused * 1000000.0), eng->timeused );
+  #endif
+#endif
+
+#ifdef CheckRxBuf
+				if ( check_buf( eng, ( eng->run.LOOP_MAX % loop_checknum ) ) )
+					return(1);
+#endif
+			} // End if ( checkprd )
+
+#ifndef SelectSimpleDes
+			if ( !looplast )
+				setup_des_loop( eng, eng->run.Loop );
+#endif
+
+#ifdef Enable_ShowBW
+  #if defined(LinuxAP)
+  #else
+			timeold = clock();
+  #endif
+#endif
+		} // End if ( eng->run.TM_RxDataEn && checken )
+
+#ifdef SelectSimpleDes
+		if ( !looplast )
+			setup_des_loop( eng, eng->run.Loop );
+#endif
+
+		if ( eng->arg.GLOOP_INFINI )
+			printf("===============> Loop[%d]: %d  \r", eng->run.Loop_ofcnt, eng->run.Loop);
+		else if ( eng->arg.GTestMode == 0 ) {
+			if ( !( DbgPrn_BufAdr || eng->run.IO_Bund ) )
+				printf(" [%d]%d                        \r", eng->run.Loop_ofcnt, eng->run.Loop);
+//			switch ( eng->run.Loop % 4 ) {
+//				case 0x00: printf("| [%d]%d                        \r", eng->run.Loop_ofcnt, eng->run.Loop); break;
+//				case 0x01: printf("/ [%d]%d                        \r", eng->run.Loop_ofcnt, eng->run.Loop); break;
+//				case 0x02: printf("- [%d]%d                        \r", eng->run.Loop_ofcnt, eng->run.Loop); break;
+//				default  : printf("\ [%d]%d                        \r", eng->run.Loop_ofcnt, eng->run.Loop); break;
+//			}
+		}
+
+#ifdef  DbgPrn_FuncHeader
+		printf("for end   ======> [%d]%d/%d(%d)\n", eng->run.Loop_ofcnt, eng->run.Loop, eng->run.LOOP_MAX, eng->arg.GLOOP_INFINI);
+		Debug_delay();
+#else
+		if ( DbgPrn_BufAdr ) {
+			printf("for end   ======> [%d]%d/%d(%d)\n", eng->run.Loop_ofcnt, eng->run.Loop, eng->run.LOOP_MAX, eng->arg.GLOOP_INFINI);
+			Debug_delay();
+		}
+#endif
+
+		if ( eng->run.Loop >= Loop_OverFlow ) {
+			printf("Over-flow\n");
+			eng->run.Loop = 0;
+			eng->run.Loop_ofcnt++;
+		}
+		else
+			eng->run.Loop++;
+	} // End while ( ( eng->run.Loop < eng->run.LOOP_MAX ) || eng->arg.GLOOP_INFINI )
+	eng->run.Loop_rl[ (int)eng->run.Speed_idx ] = eng->run.Loop;
+
+
+#ifdef Enable_MAC_ExtLoop
+  #ifdef Enable_MAC_ExtLoop_PakcegMode
+	desadr = eng->run.RDES_BASE + ( eng->dat.Des_Num - 1 ) * 16;//base for read/write
+	Write_Reg_MAC_DD( eng, 0x1c, 0x00000000 );//Rx Poll
+	while ( 1 ) {
+		while ( !HWOwnTx( Read_Mem_Des_DD( desadr ) ) ) {
+		}
+		Write_Reg_MAC_DD( eng, 0x18, 0x00000000 );//Tx Poll
+
+		while ( !HWOwnRx( Read_Mem_Des_DD( eng->run.RDES_BASE ) ) ) {
+		}
+		Write_Reg_MAC_DD( eng, 0x1c, 0x00000000 );//Rx Poll
+		switch ( eng->run.Loop % 4 ) {
+			case 0x00: printf("| [%d]%d                        \r", eng->run.Loop_ofcnt, eng->run.Loop); break;
+			case 0x01: printf("/ [%d]%d                        \r", eng->run.Loop_ofcnt, eng->run.Loop); break;
+			case 0x02: printf("- [%d]%d                        \r", eng->run.Loop_ofcnt, eng->run.Loop); break;
+			default  : printf("\ [%d]%d                        \r", eng->run.Loop_ofcnt, eng->run.Loop); break;
+		}
+//		printf("===============> Loop[%d]: %d  \r", eng->run.Loop_ofcnt, eng->run.Loop);
+		eng->run.Loop++;
+		Write_Reg_MAC_DD( eng, 0x1c, 0x00000000 );//Rx Poll
+	}
+  #else
+	while ( !kbhit() ) {
+		Write_Reg_MAC_DD( eng, 0x1c, 0x00000000 );//Rx Poll
+		Write_Reg_MAC_DD( eng, 0x18, 0x00000000 );//Tx Poll
+
+		if ( Read_Reg_MAC_DD( eng, 0xb0 ) == 0xffffffff ) {
+			Write_Reg_MAC_DD( eng, 0xa0, 0x00000000 );
+			eng->run.Loop_ofcnt++;
+		}
+
+		printf("[%d]Tx:%08lx(%08lx), Rx:%08lx %08lx\r", eng->run.Loop_ofcnt, Read_Reg_MAC_DD( eng, 0xa0 ), Read_Reg_MAC_DD( eng, 0x90 ), Read_Reg_MAC_DD( eng, 0xb0 ), Read_Reg_MAC_DD( eng, 0xb4 ));
+	}
+  #endif
+#endif
+
+	eng->flg.AllFail = 0;
+	return(0);
+} // End char TestingLoop (MAC_ENGINE *eng, ULONG loop_checknum)
+
+#if defined(DOS_ALONE) || defined(SLT_NEW_ARCH) || defined(LinuxAP)
+VOID init_hwtimer( VOID )
+{
+    ULONG uldata;
+
+    // Use timer #2
+    Write_Reg_TIMER_DD( 0x30, ( Read_Reg_TIMER_DD( 0x30 ) & 0xFFFFFF0F ) ); // Clear timer #2
+    Write_Reg_TIMER_DD( 0x10, 0xFFFFFFFF );
+    Write_Reg_TIMER_DD( 0x14, 0xFFFFFFFF );
+    uldata = Read_Reg_TIMER_DD( 0x30 );
+    uldata |= 0x30;
+    Write_Reg_TIMER_DD( 0x30, uldata );		/* enable timer2 */
+}
+
+VOID delay_hwtimer( USHORT msec )
+{
+    ULONG start = Read_Reg_TIMER_DD( 0x10 );
+    ULONG now;
+    ULONG diff;
+
+    do {
+        now = Read_Reg_TIMER_DD( 0x10 );
+        if ( start < now )
+            diff = 0xFFFFFFFF - now + start;
+        else
+            diff = start - now;
+        if ( diff > (msec * 1000) )
+            break;
+    } while ( 1 );
+
+    return;
+}
+#endif
--- uboot_old/oem/ami/standalone/nettest/MAC.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/MAC.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,154 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef MAC_H
+#define MAC_H
+
+#ifdef SPI_BUS
+  #include <stdio.h>
+  #include <stdlib.h>
+  #include <time.h>
+  #define SPI_CS            1
+#endif
+// ( USE_P2A | USE_LPC )
+
+#if defined(LinuxAP)
+  #include <stdio.h>
+  #include <stdlib.h>
+  #include <string.h>
+  #include <stdarg.h>
+  #include <unistd.h>
+  #include <string.h>
+  #include <fcntl.h>
+  #include <pthread.h>
+  #include <sys/mman.h>
+  #if	defined(__i386__) || defined(__amd64__) 
+    #include <sys/io.h>
+  #endif
+#endif
+#ifdef SLT_UBOOT
+  #include <common.h>
+  #include <command.h>
+#endif
+#ifdef DOS_ALONE
+  #include <stdio.h>
+  #include <stdlib.h>
+  #include <time.h>
+  #include <conio.h>
+  #include <dos.h>
+  #include <mem.h>
+#endif
+
+#include "NCSI.H"
+#include "IO.H"
+
+// --------------------------------------------------------------
+// Define
+// --------------------------------------------------------------
+//#define Force_Enable_NewMDIO           //[off][SLT:off] (Force enable new MDC/MDIO)
+#define Enable_RecovSCU                //[ON]
+//#define Enable_Fast_SCU                //[off]
+#define ENABLE_DASA                    //[ON]
+//#define Enable_Int125MHz               //[off] only for AST2300/AST2500
+//#define ENABLE_ARP_2_WOL               //[off]
+//#define Enable_MAC_SWRst               //[off]
+//#define Enable_MAC_ExtLoop             //[off]
+//#define   Enable_MAC_ExtLoop_PakcegMode//[off]
+#define   Enable_No_IOBoundary         //[ON]
+//#define Enable_LOOP_INFINI             //[off]
+//#define Enable_Dual_Mode               //[off]
+//#define Enable_CLK_Stable              //[off]
+
+//#define Enable_Runt
+//#define Enable_Jumbo
+//#define Enable_BufMerge
+//#define Disable_VGA
+//#define Enable_ShowBW
+
+#ifdef AST2500_IOMAP
+  #define AST2500_IOStageStep                    1
+  #define AST2500_IOStageShiftBit_In             2 //0~2
+  #define AST2500_IOStageShiftBit_Out            2 //0~2
+#endif
+
+//#define SelectSimpleBoundary                                    //[off] Using in debug
+//#define SelectSimpleData                                        //[off] Using in debug
+//#define SelectSimpleLength                       1514           //[off] 60(0x3c) ~ 1514(0x5ea); 1512(0x5e8)
+//#define SelectDesNumber                          8              //[off] 1 ~
+//#define SelectSimpleDA                                          //[off] Using in debug
+//#define SelectSimpleDes                                         //[off]
+//#define SelectLengthInc                                         //[off] Using in debug
+
+#define   SimpleData_Fix                                        //[ON] Using in debug
+#define     SimpleData_FixNum                    12
+#define     SimpleData_FixVal00                  0x00000000     //[0]no SelectSimpleDA: (60: 0412 8908)(1512: e20d e9da)
+#define     SimpleData_FixVal01                  0xffffffff     //[0]no SelectSimpleDA: (60: f48c f14d)(1512: af05 260c)
+#define     SimpleData_FixVal02                  0x55555555     //[0]no SelectSimpleDA: (60: 5467 5ecb)(1512: d90a 5368)
+#define     SimpleData_FixVal03                  0xaaaaaaaa     //[0]no SelectSimpleDA: (60: a4f9 268e)(1512: 9402 9cbe)
+#define     SimpleData_FixVal04                  0x5a5a5a5a     //[1]no SelectSimpleDA: (60: 7f01 e22d)(1512: 4fd3 8012)
+#define     SimpleData_FixVal05                  0xc3c3c3c3     //[1]no SelectSimpleDA: (60: 5916 02d5)(1512: 99f1 6127)
+#define     SimpleData_FixVal06                  0x96969696     //[1]no SelectSimpleDA: (60: 0963 d516)(1512: a2f6 db95)
+#define     SimpleData_FixVal07                  0xf0f0f0f0     //[1]no SelectSimpleDA: (60: dfea 4dab)(1512: 39dc f576)
+#define     SimpleData_FixVal08                  0x5555aaaa     //[2]no SelectSimpleDA: (60: b61b 5777)(1512: 4652 ddb0)
+#define     SimpleData_FixVal09                  0xffff0000     //[2]no SelectSimpleDA: (60: 16f0 f8f1)(1512: 305d a8d4)
+#define     SimpleData_FixVal10                  0x5a5aa5a5     //[2]no SelectSimpleDA: (60: 9d7d eb91)(1512: d08b 0eca)
+#define     SimpleData_FixVal11                  0xc3c33c3c     //[2]no SelectSimpleDA: (60: bb6a 0b69)(1512: 06a9 efff)
+
+#define   SimpleData_XORVal                      0x00000000
+//#define   SimpleData_XORVal                    0xffffffff
+
+#define   SelectSimpleDA_Dat0                    0x67052301
+#define   SelectSimpleDA_Dat1                    0xe0cda089
+#define   SelectSimpleDA_Dat2                    0x98badcfe
+
+#define   SelectWOLDA_DatH                       0x206a
+#define   SelectWOLDA_DatL                       0x8a374d9b
+
+#ifdef Enable_MAC_ExtLoop_PakcegMode
+  #define MOVE_DATA_MB_SEC                       80             // MByte per second to move data
+#else
+  #define MOVE_DATA_MB_SEC                       800            // MByte per second to move data
+#endif
+
+//---------------------------------------------------------
+// Frame size
+//---------------------------------------------------------
+#define ENABLE_RAND_SIZE                         0
+#define   RAND_SIZE_SED                          0xffccd
+#define   RAND_SIZE_SIMPLE                       0
+#define   RAND_SIZE_MIN                          60
+#define   RAND_SIZE_MAX                          1514
+
+#define FRAME_SELH_PERD                          7
+#ifdef Enable_Jumbo
+    #define FRAME_LENH                           9592           //max:9592
+    #define FRAME_LENL                           9592           //max:9592
+//    #define FRAME_LENH                           9212           //max:9212
+//    #define FRAME_LENL                           9211           //max:9212
+//    #define FRAME_LENH                           9212           //max:9212
+//    #define FRAME_LENL                           9212           //max:9212
+//    #define FRAME_LENH                           2048
+//    #define FRAME_LENL                           2048
+#else
+  #ifdef SelectSimpleLength
+//    #define FRAME_LENH                           ( SelectSimpleLength + 1 )
+//    #define FRAME_LENL                           ( SelectSimpleLength     )
+    #define FRAME_LENH                           SelectSimpleLength
+    #define FRAME_LENL                           SelectSimpleLength
+  #else
+//    #define FRAME_LENH                           1514           //max:1514
+//    #define FRAME_LENL                           1513           //max:1514
+    #define FRAME_LENH                           1514           //max:1514
+    #define FRAME_LENL                           1514           //max:1514
+  #endif
+#endif
+
+#endif // MAC_H
--- uboot_old/oem/ami/standalone/nettest/MACTEST.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/MACTEST.c	2018-06-22 17:51:23.921169873 +0800
@@ -0,0 +1,1448 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define MACTEST_C
+static const char ThisFile[] = "MACTEST.c";
+
+#include "SWFUNC.H"
+#include "stdalone.h"
+
+#if defined(SLT_UBOOT)
+  #include <common.h>
+  #include <command.h>
+  #include <post.h>
+  #include <malloc.h>
+  #include <net.h>
+  #include "COMMINF.H"
+  #include "STDUBOOT.H"
+  #include "IO.H"
+#else
+  #include <stdlib.h>
+  #include <string.h>
+  #include "LIB.H"
+  #include "COMMINF.H"
+  #include "IO.H"
+#endif
+
+#if	defined(SLT_NEW_ARCH)
+  #include "mactest.h"
+#endif
+
+#if	defined(LinuxAP) || defined(SLT_NEW_ARCH)
+int MACTest(_MACInfo *MACInfo)
+#endif
+#if defined(SLT_UBOOT)
+int main_function(int argc, char * const argv[], char mode)
+#endif
+#if defined(DOS_ALONE)
+int main(int argc, char *argv[])
+#endif
+{
+#if defined(SPI_BUS)
+	VIDEO_ENGINE_INFO    VideoEngineInfo;
+#endif
+#if defined(USE_P2A)
+	UCHAR                *ulMMIOLinearBaseAddress;
+#endif
+
+#if defined(SLT_UBOOT)
+#elif defined(LinuxAP) || defined(SLT_NEW_ARCH)
+	int                  argc;
+	char                 **argv;
+#else
+#endif
+	MAC_ENGINE           MACENG;
+	MAC_ENGINE           *eng;
+	PHY_ENGINE           PHYENG;
+	PHY_ENGINE           *phyeng;
+#if defined(ENABLE_LOG_FILE)
+	CHAR                 FileNameMain[256];
+	CHAR                 FileName[256];
+#endif
+	int                  DES_LowNumber;
+
+	CHAR                 *stop_at;
+	ULONG                Wrn_Flag_allapeed;
+	ULONG                Err_Flag_allapeed;
+	ULONG                Des_Flag_allapeed;
+	ULONG                NCSI_Flag_allapeed;
+
+	int                  i;
+	int                  j;
+	ULONG                temp;
+
+//------------------------------------------------------------
+// main Start
+//------------------------------------------------------------
+	eng    = &MACENG;
+	phyeng = &PHYENG;
+	phyeng->fp_set = 0;
+	phyeng->fp_clr = 0;
+	
+#if defined(PHY_SPECIAL)
+	special_PHY_init( eng );
+#endif
+
+#if defined(SLT_UBOOT)
+	eng->ModeSwitch = mode;
+#endif
+
+#if defined(DOS_ALONE)
+  #if defined(PHY_NCSI)
+	// For DOS compiler OPEN WATCOM
+	eng->ModeSwitch = MODE_NSCI;
+  #else
+	eng->ModeSwitch = MODE_DEDICATED;
+  #endif
+#endif
+
+#if defined(DOS_ALONE) || defined(SLT_NEW_ARCH)
+	// DOS system
+	time( &(eng->timestart) );
+#endif
+
+//------------------------------------------------------------
+// Bus Initial
+//------------------------------------------------------------
+
+#if defined(LinuxAP) || defined(SLT_NEW_ARCH)
+	if ( MACInfo->MACInterface == MDC_INTERFACE )
+		eng->ModeSwitch = MODE_DEDICATED;
+	else
+		eng->ModeSwitch = MODE_NSCI;
+
+  #if defined(USE_P2A)
+	ulMMIOBaseAddress = MACInfo->ulMMIOBaseAddress;
+	ulMMIOLinearBaseAddress = (UCHAR *) (MACInfo->ulMMIOLinear);
+  #endif
+  #if defined(USE_LPC)
+	// LPC interface
+	SetLPCport( MACInfo->LPC_port );
+  #endif
+
+	argc = (int)MACInfo->argc;
+	argv = MACInfo->argv;
+#endif // End ( defined(LinuxAP) || defined(SLT_NEW_ARCH) )
+
+#ifdef DOS_ALONE
+  //DOS system
+  #ifdef SPI_BUS
+  #endif
+  #ifdef USE_LPC
+	if ( findlpcport( 0x0d ) == 0) {
+		printf("Failed to find proper LPC port \n");
+
+		return(1);
+	}
+	open_aspeed_sio_password();
+	enable_aspeed_LDU( 0x0d );
+  #endif
+  #ifdef USE_P2A
+	// PCI bus
+    #ifdef DOS_PMODEW
+	if (CheckDOS()) return (1);
+    #endif
+
+    #ifdef  DbgPrn_FuncHeader
+	printf("Initial-MMIO\n");
+	Debug_delay();
+    #endif
+	ulPCIBaseAddress = FindPCIDevice ( 0x1A03, 0x2000, ACTIVE );
+	if ( ulPCIBaseAddress == 0 )
+		ulPCIBaseAddress = FindPCIDevice ( 0x1688, 0x2000, ACTIVE );
+	if ( ulPCIBaseAddress == 0 )
+		ulPCIBaseAddress = FindPCIDevice ( 0x1A03, 0x0200, ACTIVE );
+	if ( ulPCIBaseAddress == 0 )
+		ulPCIBaseAddress = FindPCIDevice ( 0x1A03, 0x3000, ACTIVE );
+	if ( ulPCIBaseAddress == 0 )
+		ulPCIBaseAddress = FindPCIDevice ( 0x1A03, 0x2010, ACTIVE );
+	if ( ulPCIBaseAddress == 0 ) {
+		printf("Can't find device\n");
+
+		return(1);
+	}
+
+	WritePCIReg (ulPCIBaseAddress, 0x04, 0xFFFFFFFc, 0x3);
+	ulMMIOBaseAddress       = ReadPCIReg ( ulPCIBaseAddress, 0x14, 0xFFFF0000 );
+	ulMMIOLinearBaseAddress = (UCHAR *)MapPhysicalToLinear ( ulMMIOBaseAddress, 64 * 1024 * 1024 );
+  #endif // #ifdef USE_P2A
+#endif // End DOS_ALONE
+
+#ifdef SPI_BUS
+	GetDevicePCIInfo ( &VideoEngineInfo );
+	mmiobase = VideoEngineInfo.VGAPCIInfo.ulMMIOBaseAddress;
+	spim_init( SPI_CS );
+#endif
+#if defined(USE_P2A)
+	mmiobase = ulMMIOLinearBaseAddress;
+#endif
+
+#if defined(DOS_ALONE) || defined(SLT_NEW_ARCH) || defined(LinuxAP)
+	init_hwtimer();
+#endif
+
+//------------------------------------------------------------
+// Get Chip Feature
+//------------------------------------------------------------
+	Wrn_Flag_allapeed  = 0;
+	Err_Flag_allapeed  = 0;
+	Des_Flag_allapeed  = 0;
+	NCSI_Flag_allapeed = 0;
+	eng->flg.Wrn_Flag              = 0;
+	eng->flg.Err_Flag              = 0;
+	eng->flg.Des_Flag              = 0;
+	eng->flg.NCSI_Flag             = 0;
+	eng->flg.Flag_PrintEn          = 1;
+	eng->flg.AllFail               = 0;
+	eng->run.TIME_OUT_Des_PHYRatio = 1;
+	eng->run.Loop_ofcnt            = 0;
+	eng->run.Loop                  = 0;
+	eng->run.Loop_rl[0]            = 0;
+	eng->run.Loop_rl[1]            = 0;
+	eng->run.Loop_rl[2]            = 0;
+	eng->dat.FRAME_LEN             = 0;
+	eng->dat.wp_lst                = 0;
+	eng->io.init_done              = 0;
+	eng->env.VGAModeVld            = 0;
+	eng->reg.SCU_oldvld            = 0;
+	eng->phy.Adr                   = 0;
+	eng->phy.loop_phy              = 0;
+	eng->phy.default_phy           = 0;
+	eng->phy.PHY_ID2               = 0;
+	eng->phy.PHY_ID3               = 0;
+	eng->phy.PHYName[0]            = 0;
+	eng->phy.RMIICK_IOMode         = 0;
+	eng->ncsi_cap.PCI_DID_VID      = 0;
+	eng->ncsi_cap.ManufacturerID   = 0;
+	read_scu( eng );
+
+	if ( RUN_STEP >= 1 ) {
+		//------------------------------
+		// [Reg]check SCU_07c
+		// [Env]setup ASTChipName
+		// [Env]setup ASTChipType
+		//------------------------------
+		switch ( eng->reg.SCU_07c ) {
+#ifdef AST2500_IOMAP
+#elif defined(AST1010_CHIP)
+			case 0x03010003 : sprintf( eng->env.ASTChipName, "[*]AST1010-A2/AST1010-A1"         ); eng->env.ASTChipType = 6; break;//AST1010-A2/A1
+			case 0x03000003 : sprintf( eng->env.ASTChipName, "[ ]AST1010-A0"                    ); eng->env.ASTChipType = 6; break;//AST1010-A0
+#else
+			case 0x02010303 : sprintf( eng->env.ASTChipName, "[*]AST2400-A1"                    ); eng->env.ASTChipType = 5; break;//AST2400-A1
+			case 0x02000303 : sprintf( eng->env.ASTChipName, "[ ]AST2400-A0"                    ); eng->env.ASTChipType = 5; break;//AST2400-A0
+			case 0x02010103 : sprintf( eng->env.ASTChipName, "[*]AST1400-A1"                    ); eng->env.ASTChipType = 5; break;//AST1400-A1
+			case 0x02000003 : sprintf( eng->env.ASTChipName, "[ ]AST1400-A0"                    ); eng->env.ASTChipType = 5; break;//AST1400-A0
+
+			case 0x01010303 : sprintf( eng->env.ASTChipName, "[*]AST2300-A1"                    ); eng->env.ASTChipType = 4; break;//AST2300-A1
+			case 0x01010203 : sprintf( eng->env.ASTChipName, "[*]AST1050-A1"                    ); eng->env.ASTChipType = 4; break;//AST1050-A1
+			case 0x01010003 : sprintf( eng->env.ASTChipName, "[*]AST1300-A1"                    ); eng->env.ASTChipType = 4; break;//AST1300-A1
+			case 0x01000003 : sprintf( eng->env.ASTChipName, "[ ]AST2300-A0"                    ); eng->env.ASTChipType = 4; break;//AST2300-A0
+
+			case 0x00000102 : sprintf( eng->env.ASTChipName, "[*]AST2200-A1"                    ); eng->env.ASTChipType = 3; break;//AST2200-A1/A0
+
+			case 0x00000302 : sprintf( eng->env.ASTChipName, "[*]AST2100-A3"                    ); eng->env.ASTChipType = 2; break;//AST2100-A3/A2
+			case 0x00000301 : sprintf( eng->env.ASTChipName, "[ ]AST2100-A1"                    ); eng->env.ASTChipType = 2; break;//AST2100-A1
+			case 0x00000300 : sprintf( eng->env.ASTChipName, "[ ]AST2100-A0"                    ); eng->env.ASTChipType = 2; break;//AST2100-A0
+			case 0x00000202 : sprintf( eng->env.ASTChipName, "[*]AST2050/AST1100-A3, AST2150-A1"); eng->env.ASTChipType = 1; break;//AST2050/AST1100-A3/A2 AST2150-A1/A0
+			case 0x00000201 : sprintf( eng->env.ASTChipName, "[ ]AST2050/AST1100-A1"            ); eng->env.ASTChipType = 1; break;//AST2050/AST1100-A1
+			case 0x00000200 : sprintf( eng->env.ASTChipName, "[ ]AST2050/AST1100-A0"            ); eng->env.ASTChipType = 1; break;//AST2050/AST1100-A0
+#endif
+			default:
+				sprintf( eng->env.ASTChipName, "[ ]" );
+				temp = ( eng->reg.SCU_07c ) & 0xff000000;
+				switch ( temp ) {
+#ifdef AST2500_IOMAP
+					case 0x04000000 : eng->env.ASTChipType = 8; goto PASS_CHIP_ID;
+#elif defined(AST1010_CHIP)
+					case 0x03000000 : eng->env.ASTChipType = 6; goto PASS_CHIP_ID;
+#else
+					case 0x02000000 : eng->env.ASTChipType = 5; goto PASS_CHIP_ID;
+					case 0x01000000 : eng->env.ASTChipType = 4; goto PASS_CHIP_ID;
+#endif
+					default:
+						printf("Error Silicon Revision ID(SCU7C) %08lx [%08lx]!!!\n", eng->reg.SCU_07c, temp);
+#if defined(AST1010_CHIP)
+						printf("Only support AST1010\n");
+#endif
+				}
+				return(1);
+		} // End switch ( eng->reg.SCU_07c )
+PASS_CHIP_ID:
+		//------------------------------
+		// [Env]check ASTChipType
+		// [Env]setup AST1100
+		// [Env]setup AST2300
+		// [Env]setup AST2400
+		// [Env]setup AST1010
+		// [Env]setup AST2500
+		// [Env]setup AST2500A1
+		//------------------------------
+		eng->env.AST1100   = 0;
+		eng->env.AST2300   = 0;
+		eng->env.AST2400   = 0;
+		eng->env.AST1010   = 0;
+		eng->env.AST2500   = 0;
+		eng->env.AST2500A1 = 0;
+		switch ( eng->env.ASTChipType ) {
+			case 8  : eng->env.AST2500A1 = 1;
+			case 7  : eng->env.AST2500   = 1;
+			case 5  : eng->env.AST2400   = 1;
+			case 4  : eng->env.AST2300   = 1; break;
+
+			case 6  : eng->env.AST2300 = 1; eng->env.AST2400 = 1; eng->env.AST1010 = 1; break;
+			case 1  : eng->env.AST1100 = 1; break;
+			default : break;
+		} // End switch ( eng->env.ASTChipType )
+
+		//------------------------------
+		// [Env]check ASTChipType
+		// [Reg]check SCU_0f0
+		// [Env]setup MAC34_vld
+		//------------------------------
+		if ( ( eng->env.ASTChipType == 4 ) && ( eng->reg.SCU_0f0 & 0x00000001 ) )//only AST2300
+			eng->env.MAC34_vld = 1;
+		else
+			eng->env.MAC34_vld = 0;
+
+//------------------------------------------------------------
+// Get Argument Input
+//------------------------------------------------------------
+		//------------------------------
+		// Load default value
+		//------------------------------
+		if ( eng->ModeSwitch == MODE_NSCI ) {
+			eng->arg.GARPNumCnt     = DEF_GARPNUMCNT;
+			eng->arg.GChannelTolNum = DEF_GCHANNEL2NUM;
+			eng->arg.GPackageTolNum = DEF_GPACKAGE2NUM;
+			eng->arg.GCtrl          = 0;
+			eng->arg.GSpeed         = SET_100MBPS;        // In NCSI mode, we set to 100M bps
+		}
+		else {
+			eng->arg.GUserDVal    = DEF_GUSER_DEF_PACKET_VAL;
+			eng->arg.GPHYADR      = DEF_GPHY_ADR;
+			eng->arg.GLOOP_INFINI = 0;
+			eng->arg.GLOOP_MAX    = 0;
+			eng->arg.GCtrl        = DEF_GCTRL;
+			eng->arg.GSpeed       = DEF_GSPEED;
+		}
+		eng->arg.GChk_TimingBund = DEF_GIOTIMINGBUND;
+		eng->arg.GTestMode       = DEF_GTESTMODE;
+
+		//------------------------------
+		// Get setting information by user
+		//------------------------------
+		eng->arg.GRun_Mode = (BYTE)atoi(argv[1]);
+		if (argc > 1) {
+			if ( eng->ModeSwitch == MODE_NSCI ) {
+				switch ( argc ) {
+					case 8: eng->arg.GARPNumCnt      = (ULONG)atoi(argv[7]);
+					case 7: eng->arg.GCtrl           = (BYTE)atoi(argv[6]);
+					case 6: eng->arg.GChk_TimingBund = (BYTE)atoi(argv[5]);
+					case 5: eng->arg.GTestMode       = (BYTE)atoi(argv[4]);
+					case 4: eng->arg.GChannelTolNum  = (BYTE)atoi(argv[3]);
+					case 3: eng->arg.GPackageTolNum  = (BYTE)atoi(argv[2]);
+					default: break;
+				}
+			}
+			else {
+				eng->arg.GARPNumCnt = 0;
+				switch ( argc ) {
+					case 9: eng->arg.GUserDVal       = strtoul (argv[8], &stop_at, 16);
+					case 8: eng->arg.GChk_TimingBund = (BYTE)atoi(argv[7]);
+					case 7: eng->arg.GPHYADR         = (BYTE)atoi(argv[6]);
+					case 6: eng->arg.GTestMode       = (BYTE)atoi(argv[5]);
+					case 5: strcpy( eng->arg.GLOOP_Str, argv[4] );
+						if ( !strcmp( eng->arg.GLOOP_Str, "#" ) )
+							eng->arg.GLOOP_INFINI = 1;
+						else
+							eng->arg.GLOOP_MAX    = (ULONG)atoi( eng->arg.GLOOP_Str );
+					case 4: eng->arg.GCtrl           = (BYTE)atoi(argv[3]);
+					case 3: eng->arg.GSpeed          = (BYTE)atoi(argv[2]);
+					default: break;
+				}
+			} // End if ( eng->ModeSwitch == MODE_NSCI )
+		}
+		else {
+			// Wrong parameter
+			if ( eng->ModeSwitch == MODE_NSCI ) {
+				if ( eng->env.AST2300 )
+					printf("\nNCSITEST.exe  run_mode  <package_num>  <channel_num>  <test_mode>  <IO margin>\n\n");
+				else
+					printf("\nNCSITEST.exe  run_mode  <package_num>  <channel_num>  <test_mode>\n\n");
+				PrintMode         ( eng );
+				PrintPakNUm       ( eng );
+				PrintChlNUm       ( eng );
+				PrintTest         ( eng );
+				PrintIOTimingBund ( eng );
+			}
+			else {
+#ifdef Enable_MAC_ExtLoop
+				printf("\nMACLOOP.exe  run_mode  <speed>\n\n");
+#else
+				if ( eng->env.AST2300 )
+					printf("\nMACTEST.exe  run_mode  <speed>  <ctrl>  <loop_max>  <test_mode>  <phy_adr>  <IO margin>\n\n");
+				else
+					printf("\nMACTEST.exe  run_mode  <speed>  <ctrl>  <loop_max>  <test_mode>  <phy_adr>\n\n");
+#endif
+				PrintMode         ( eng );
+				PrintSpeed        ( eng );
+#ifndef Enable_MAC_ExtLoop
+				PrintCtrl         ( eng );
+				PrintLoop         ( eng );
+				PrintTest         ( eng );
+				PrintPHYAdr       ( eng );
+				PrintIOTimingBund ( eng );
+#endif
+			} // End if ( eng->ModeSwitch == MODE_NSCI )
+			Finish_Close( eng );
+
+			return(1);
+		} // End if (argc > 1)
+
+#ifdef Enable_MAC_ExtLoop
+		eng->arg.GChk_TimingBund = 0;
+		eng->arg.GTestMode       = 0;
+		eng->arg.GLOOP_INFINI    = 1;
+//		eng->arg.GCtrl           = 0;
+		eng->arg.GCtrl           = eng->arg.GCtrl & 0x08;
+#endif
+
+//------------------------------------------------------------
+// Check Argument & Setup
+//------------------------------------------------------------
+		//------------------------------
+		// [Env]check MAC34_vld
+		// [Arg]check GRun_Mode
+		// [Run]setup MAC_idx
+		// [Run]setup MAC_BASE
+		//------------------------------
+		switch ( eng->arg.GRun_Mode ) {
+			case 0:                            printf("\n[MAC1]\n"); eng->run.MAC_idx = 0; eng->run.MAC_BASE = MAC_BASE1; break;
+			case 1:                            printf("\n[MAC2]\n"); eng->run.MAC_idx = 1; eng->run.MAC_BASE = MAC_BASE2; break;
+			case 2: if ( eng->env.MAC34_vld ) {printf("\n[MAC3]\n"); eng->run.MAC_idx = 2; eng->run.MAC_BASE = MAC_BASE3; break;}
+			        else
+			            goto Error_GRun_Mode;
+			case 3: if ( eng->env.MAC34_vld ) {printf("\n[MAC4]\n"); eng->run.MAC_idx = 3; eng->run.MAC_BASE = MAC_BASE4; break;}
+			        else
+			            goto Error_GRun_Mode;
+			default:
+Error_GRun_Mode:
+				printf("Error run_mode!!!\n");
+				PrintMode ( eng );
+				return(1);
+		} // End switch ( eng->arg.GRun_Mode )
+
+		//------------------------------
+		// [Arg]check GSpeed
+		// [Run]setup Speed_1G
+		// [Run]setup Speed_org
+		//------------------------------
+		switch ( eng->arg.GSpeed ) {
+			case SET_1GBPS          : eng->run.Speed_1G = 1; eng->run.Speed_org[ 0 ] = 1; eng->run.Speed_org[ 1 ] = 0; eng->run.Speed_org[ 2 ] = 0; break;
+			case SET_100MBPS        : eng->run.Speed_1G = 0; eng->run.Speed_org[ 0 ] = 0; eng->run.Speed_org[ 1 ] = 1; eng->run.Speed_org[ 2 ] = 0; break;
+			case SET_10MBPS         : eng->run.Speed_1G = 0; eng->run.Speed_org[ 0 ] = 0; eng->run.Speed_org[ 1 ] = 0; eng->run.Speed_org[ 2 ] = 1; break;
+#ifndef Enable_MAC_ExtLoop
+			case SET_1G_100M_10MBPS : eng->run.Speed_1G = 0; eng->run.Speed_org[ 0 ] = 1; eng->run.Speed_org[ 1 ] = 1; eng->run.Speed_org[ 2 ] = 1; break;
+			case SET_100M_10MBPS    : eng->run.Speed_1G = 0; eng->run.Speed_org[ 0 ] = 0; eng->run.Speed_org[ 1 ] = 1; eng->run.Speed_org[ 2 ] = 1; break;
+#endif
+			default:
+				printf("Error speed!!!\n");
+				PrintSpeed ( eng );
+				return(1);
+		} // End switch ( eng->arg.GSpeed )
+
+
+		if ( eng->ModeSwitch == MODE_NSCI ) {
+			//------------------------------
+			// [Arg]check GPackageTolNum
+			// [Arg]check GChannelTolNum
+			//------------------------------
+			if (( eng->arg.GPackageTolNum < 1 ) || ( eng->arg.GPackageTolNum >  8 )) {
+				PrintPakNUm ( eng );
+				return(1);
+			}
+			if (( eng->arg.GChannelTolNum < 1 ) || ( eng->arg.GChannelTolNum > 32 )) {
+				PrintChlNUm ( eng );
+				return(1);
+			}
+			//------------------------------
+			// [Arg]check GCtrl
+			// [Arg]setup GEn_RMII_50MOut
+			// [Arg]setup GEn_MACLoopback
+			// [Arg]setup GEn_FullRange
+			// [Arg]setup GEn_SkipRxEr
+			// [Arg]setup GEn_PrintNCSI
+			//------------------------------
+			eng->arg.GEn_RMII_50MOut = ( eng->arg.GCtrl >> 8 ) & 0x1;
+			eng->arg.GEn_MACLoopback = ( eng->arg.GCtrl >> 7 ) & 0x1;
+			eng->arg.GEn_FullRange   = ( eng->arg.GCtrl >> 6 ) & 0x1;
+
+			eng->arg.GEn_SkipRxEr    = ( eng->arg.GCtrl >> 1 ) & 0x1;
+			eng->arg.GEn_PrintNCSI   = ( eng->arg.GCtrl      ) & 0x1;
+		}
+		else {
+			//------------------------------
+			// [Arg]check GCtrl
+			// [Arg]setup GEn_RMIIPHY_IN
+			// [Arg]setup GEn_RMII_50MOut
+			// [Arg]setup GEn_MACLoopback
+			// [Arg]setup GEn_FullRange
+			// [Arg]setup GEn_SkipChkPHY
+			// [Arg]setup GEn_IntLoopPHY
+			// [Arg]setup GEn_InitPHY
+			// [Arg]setup GDis_RecovPHY
+			// [Arg]setup GEn_PHYAdrInv
+			// [Arg]setup GEn_SinglePacket
+			//------------------------------
+			if ( eng->arg.GCtrl & 0xfffffe00 ) {
+				printf("Error ctrl!!!\n");
+				PrintCtrl ( eng );
+				return(1);
+			}
+			else {
+				eng->arg.GEn_RMIIPHY_IN   = ( eng->arg.GCtrl >> 9 ) & 0x1;
+				eng->arg.GEn_RMII_50MOut  = ( eng->arg.GCtrl >> 8 ) & 0x1;
+				eng->arg.GEn_MACLoopback  = ( eng->arg.GCtrl >> 7 ) & 0x1;
+				eng->arg.GEn_FullRange    = ( eng->arg.GCtrl >> 6 ) & 0x1;
+				eng->arg.GEn_SkipChkPHY   = ( eng->arg.GCtrl >> 5 ) & 0x1;
+				eng->arg.GEn_IntLoopPHY   = ( eng->arg.GCtrl >> 4 ) & 0x1;
+				eng->arg.GEn_InitPHY      = ( eng->arg.GCtrl >> 3 ) & 0x1;
+				eng->arg.GDis_RecovPHY    = ( eng->arg.GCtrl >> 2 ) & 0x1;
+				eng->arg.GEn_PHYAdrInv    = ( eng->arg.GCtrl >> 1 ) & 0x1;
+				eng->arg.GEn_SinglePacket = ( eng->arg.GCtrl      ) & 0x1;
+				if ( !eng->env.AST2400 && eng->arg.GEn_MACLoopback ) {
+					printf("Error ctrl!!!\n");
+					PrintCtrl ( eng );
+					return(1);
+				}
+			} // End if ( eng->arg.GCtrl & 0xffffff83 )
+
+			//------------------------------
+			// [Arg]check GPHYADR
+			//------------------------------
+			if ( eng->arg.GPHYADR > 31 ) {
+				printf("Error phy_adr!!!\n");
+				PrintPHYAdr ( eng );
+				return(1);
+			} // End if ( eng->arg.GPHYADR > 31)
+			//------------------------------
+			// [Arg]check GLOOP_MAX
+			// [Arg]check GSpeed
+			// [Arg]setup GLOOP_MAX
+			//------------------------------
+			if ( !eng->arg.GLOOP_MAX )
+				switch ( eng->arg.GSpeed ) {
+					case SET_1GBPS         : eng->arg.GLOOP_MAX = DEF_GLOOP_MAX * 20; break;
+					case SET_100MBPS       : eng->arg.GLOOP_MAX = DEF_GLOOP_MAX * 2 ; break;
+					case SET_10MBPS        : eng->arg.GLOOP_MAX = DEF_GLOOP_MAX     ; break;
+					case SET_1G_100M_10MBPS: eng->arg.GLOOP_MAX = DEF_GLOOP_MAX * 20; break;
+					case SET_100M_10MBPS   : eng->arg.GLOOP_MAX = DEF_GLOOP_MAX * 2 ; break;
+				}
+		} // End if ( eng->ModeSwitch == MODE_NSCI )
+
+//------------------------------------------------------------
+// Check Argument & Setup Running Parameter
+//------------------------------------------------------------
+		//------------------------------
+		// [Env]check AST2300
+		// [Arg]check GTestMode
+		// [Run]setup TM_IOTiming
+		// [Run]setup TM_IOStrength
+		// [Run]setup TM_TxDataEn
+		// [Run]setup TM_RxDataEn
+		// [Run]setup TM_NCSI_DiSChannel
+		// [Run]setup TM_Burst
+		// [Run]setup TM_IEEE
+		// [Run]setup TM_WaitStart
+		//------------------------------
+		eng->run.TM_IOTiming        = 0;
+		eng->run.TM_IOStrength      = 0;
+		eng->run.TM_TxDataEn        = 1;
+		eng->run.TM_RxDataEn        = 1;
+		eng->run.TM_NCSI_DiSChannel = 1; // For ncsitest function
+		eng->run.TM_Burst           = 0; // For mactest function
+		eng->run.TM_IEEE            = 0; // For mactest function
+		eng->run.TM_WaitStart       = 0; // For mactest function
+		eng->run.TM_DefaultPHY      = 0; // For mactest function
+		if ( eng->ModeSwitch == MODE_NSCI ) {
+			switch ( eng->arg.GTestMode ) {
+				case 0 :     break;
+				case 1 :     eng->run.TM_NCSI_DiSChannel = 0; break;
+				case 6 : if ( eng->env.AST2300 ) {
+					     eng->run.TM_IOTiming = 1; break;}
+					 else
+					     goto Error_GTestMode_NCSI;
+				case 7 : if ( eng->env.AST2300 ) {
+					     eng->run.TM_IOTiming = 1; eng->run.TM_IOStrength = 1; break;}
+					 else
+					     goto Error_GTestMode_NCSI;
+				default:
+Error_GTestMode_NCSI:
+					printf("Error test_mode!!!\n");
+					PrintTest ( eng );
+					return(1);
+			} // End switch ( eng->arg.GTestMode )
+		}
+		else {
+			switch ( eng->arg.GTestMode ) {
+				case  0 :     break;
+				case  1 :     eng->run.TM_RxDataEn = 0; eng->run.TM_Burst = 1; eng->run.TM_IEEE = 1; break;
+				case  2 :     eng->run.TM_RxDataEn = 0; eng->run.TM_Burst = 1; eng->run.TM_IEEE = 1; break;
+				case  3 :     eng->run.TM_RxDataEn = 0; eng->run.TM_Burst = 1; eng->run.TM_IEEE = 1; break;
+				case  4 :     eng->run.TM_RxDataEn = 0; eng->run.TM_Burst = 1; eng->run.TM_IEEE = 0; break;
+				case  5 :     eng->run.TM_RxDataEn = 0; eng->run.TM_Burst = 1; eng->run.TM_IEEE = 1; break;
+				case  6 : if ( eng->env.AST2300 ) {
+				              eng->run.TM_IOTiming = 1; break;}
+				          else
+				              goto Error_GTestMode;
+				case  7 : if ( eng->env.AST2300 ) {
+				              eng->run.TM_IOTiming = 1; eng->run.TM_IOStrength = 1; break;}
+				          else
+				              goto Error_GTestMode;
+				case  8 :     eng->run.TM_RxDataEn = 0; eng->run.TM_DefaultPHY = 1; break;
+				case  9 :     eng->run.TM_TxDataEn = 0; eng->run.TM_DefaultPHY = 1; break;
+				case 10 :     eng->run.TM_WaitStart = 1; break;
+				default:
+Error_GTestMode:
+					printf("Error test_mode!!!\n");
+					PrintTest ( eng );
+					return(1);
+			} // End switch ( eng->arg.GTestMode )
+#ifdef Enable_MAC_ExtLoop
+			eng->run.TM_DefaultPHY = 1;
+#endif
+		} // End if ( eng->ModeSwitch == MODE_NSCI )
+
+		//------------------------------
+		// [Env]check AST2300
+		// [Arg]check GChk_TimingBund
+		// [Run]check TM_Burst
+		// [Arg]setup GIEEE_sel
+		// [Run]setup IO_Bund
+		//------------------------------
+		if ( eng->run.TM_Burst ) {
+			eng->arg.GIEEE_sel = eng->arg.GChk_TimingBund;
+			eng->run.IO_Bund = 0;
+		}
+		else {
+			eng->arg.GIEEE_sel = 0;
+			if ( eng->env.AST2300 ) {
+				eng->run.IO_Bund = eng->arg.GChk_TimingBund;
+
+//				if ( !( ( ( 7 >= eng->run.IO_Bund ) && ( eng->run.IO_Bund & 0x1 ) ) ||
+//				        ( eng->run.IO_Bund == 0                                   )
+//				       ) ) {
+//					printf("Error IO margin!!!\n");
+//					PrintIOTimingBund ( eng );
+//					return(1);
+//				}
+				if ( !( ( eng->run.IO_Bund & 0x1 ) ||
+				        ( eng->run.IO_Bund == 0  )
+				       ) ) {
+					printf("Error IO margin!!!\n");
+					PrintIOTimingBund ( eng );
+					return(1);
+				}
+			}
+			else {
+				eng->run.IO_Bund = 0;
+			}
+		} // End if ( eng->run.TM_Burst )
+
+//------------------------------------------------------------
+// File Name
+//------------------------------------------------------------
+#if defined(ENABLE_LOG_FILE)
+		//------------------------------
+		// [Arg]check GEn_IntLoopPHY
+		// [Run]check TM_Burst
+		// [Run]check TM_IOTiming
+		// [Run]check TM_IOStrength
+		//------------------------------
+		if ( !eng->run.TM_Burst ) {
+			// Define Output file name
+			if ( eng->ModeSwitch == MODE_NSCI )
+				sprintf( FileNameMain, "%d",  eng->run.MAC_idx + 1 );
+			else {
+				if ( eng->arg.GEn_IntLoopPHY )
+					sprintf( FileNameMain, "%dI",  eng->run.MAC_idx + 1 );
+				else
+					sprintf( FileNameMain, "%dE",  eng->run.MAC_idx + 1 );
+			}
+
+			if ( eng->run.TM_IOTiming ) {
+				if ( eng->run.TM_IOStrength )
+					sprintf( FileName, "MIOD%sS.log", FileNameMain );
+				else
+					sprintf( FileName, "MIOD%s.log", FileNameMain );
+
+				eng->fp_log = fopen( FileName,"w" );
+
+				if ( eng->run.TM_IOStrength )
+					sprintf( FileName, "MIO%sS.log", FileNameMain );
+				else
+					sprintf( FileName, "MIO%s.log", FileNameMain );
+
+				eng->fp_io  = fopen( FileName,"w" );
+			}
+			else {
+				sprintf( FileName, "MAC%s.log", FileNameMain );
+
+				eng->fp_log = fopen( FileName,"w" );
+			}
+		} // End if ( !eng->run.TM_Burst )
+#endif // End defined(ENABLE_LOG_FILE)
+
+//------------------------------------------------------------
+// Setup Environment
+//------------------------------------------------------------
+		//------------------------------
+		// [Env]check AST1010
+		// [Env]check AST2300
+		// [Reg]check SCU_070
+		// [Env]setup MAC_Mode
+		// [Env]setup MAC1_1Gvld
+		// [Env]setup MAC2_1Gvld
+		// [Env]setup MAC1_RMII
+		// [Env]setup MAC2_RMII
+		// [Env]setup MAC2_vld
+		//------------------------------
+#ifdef AST1010_CHIP
+		eng->env.MAC_Mode   = 0;
+		eng->env.MAC1_1Gvld = 0;
+		eng->env.MAC2_1Gvld = 0;
+
+		eng->env.MAC1_RMII  = 1;
+		eng->env.MAC2_RMII  = 0;
+		eng->env.MAC2_vld   = 0;
+#else
+		if ( eng->env.AST2300 ) {
+			eng->env.MAC_Mode   = ( eng->reg.SCU_070 >> 6 ) & 0x3;
+			eng->env.MAC1_1Gvld = ( eng->env.MAC_Mode & 0x1 ) ? 1 : 0;//1:RGMII, 0:RMII
+			eng->env.MAC2_1Gvld = ( eng->env.MAC_Mode & 0x2 ) ? 1 : 0;//1:RGMII, 0:RMII
+
+			eng->env.MAC1_RMII  = !eng->env.MAC1_1Gvld;
+			eng->env.MAC2_RMII  = !eng->env.MAC2_1Gvld;
+			eng->env.MAC2_vld   = 1;
+		}
+		else {
+			eng->env.MAC_Mode   = ( eng->reg.SCU_070 >> 6 ) & 0x7;
+			eng->env.MAC1_1Gvld = ( eng->env.MAC_Mode == 0x0 ) ? 1 : 0;
+			eng->env.MAC2_1Gvld = 0;
+
+			switch ( eng->env.MAC_Mode ) {
+				case 0 : eng->env.MAC1_RMII = 0; eng->env.MAC2_RMII = 0; eng->env.MAC2_vld = 0; break; //000: Select GMII(MAC#1) only
+				case 1 : eng->env.MAC1_RMII = 0; eng->env.MAC2_RMII = 0; eng->env.MAC2_vld = 1; break; //001: Select MII (MAC#1) and MII(MAC#2)
+				case 2 : eng->env.MAC1_RMII = 1; eng->env.MAC2_RMII = 0; eng->env.MAC2_vld = 1; break; //010: Select RMII(MAC#1) and MII(MAC#2)
+				case 3 : eng->env.MAC1_RMII = 0; eng->env.MAC2_RMII = 0; eng->env.MAC2_vld = 0; break; //011: Select MII (MAC#1) only
+				case 4 : eng->env.MAC1_RMII = 1; eng->env.MAC2_RMII = 0; eng->env.MAC2_vld = 0; break; //100: Select RMII(MAC#1) only
+//				case 5 : eng->env.MAC1_RMII = 0; eng->env.MAC2_RMII = 0; eng->env.MAC2_vld = 0; break; //101: Reserved
+				case 6 : eng->env.MAC1_RMII = 1; eng->env.MAC2_RMII = 1; eng->env.MAC2_vld = 1; break; //110: Select RMII(MAC#1) and RMII(MAC#2)
+//				case 7 : eng->env.MAC1_RMII = 0; eng->env.MAC2_RMII = 0; eng->env.MAC2_vld = 0; break; //111: Disable dual MAC
+				default:
+					return( Finish_Check( eng, Err_Flag_MACMode ) );
+			}
+		} // End if ( eng->env.AST2300 )
+#endif
+
+		eng->env.MAC_atlast_1Gvld = eng->env.MAC1_1Gvld | eng->env.MAC2_1Gvld;
+
+//------------------------------------------------------------
+// Check & Setup Environment
+//------------------------------------------------------------
+		//------------------------------
+		// [Phy]setup PHY_BASE
+		// [Env]setup MAC_1Gvld
+		// [Env]setup MAC_RMII
+		//------------------------------
+		if ( eng->run.MAC_idx == 0 ) {
+			if ( eng->arg.GEn_PHYAdrInv ) {
+				eng->phy.PHY_BASE    = MAC_BASE2;
+				eng->run.MAC_idx_PHY = 1;
+			} else {
+				eng->phy.PHY_BASE    = MAC_BASE1;
+				eng->run.MAC_idx_PHY = 0;
+			}
+			eng->env.MAC_1Gvld = eng->env.MAC1_1Gvld;
+			eng->env.MAC_RMII  = eng->env.MAC1_RMII;
+
+			if ( eng->run.Speed_1G & !eng->env.MAC1_1Gvld ) {
+				printf("\nMAC1 don't support 1Gbps !!!\n");
+				return( Finish_Check( eng, Err_Flag_MACMode ) );
+			}
+		}
+		else if ( eng->run.MAC_idx == 1 ) {
+			if ( eng->arg.GEn_PHYAdrInv ) {
+				eng->phy.PHY_BASE    = MAC_BASE1;
+				eng->run.MAC_idx_PHY = 0;
+			} else {
+				eng->phy.PHY_BASE    = MAC_BASE2;
+				eng->run.MAC_idx_PHY = 1;
+			}
+			eng->env.MAC_1Gvld = eng->env.MAC2_1Gvld;
+			eng->env.MAC_RMII  = eng->env.MAC2_RMII;
+
+			if ( eng->run.Speed_1G & !eng->env.MAC2_1Gvld ) {
+				printf("\nMAC2 don't support 1Gbps !!!\n");
+				return( Finish_Check( eng, Err_Flag_MACMode ) );
+			}
+			if ( !eng->env.MAC2_vld ) {
+				printf("\nMAC2 not valid !!!\n");
+				return( Finish_Check( eng, Err_Flag_MACMode ) );
+			}
+		}
+		else {
+			if ( eng->run.MAC_idx == 2 )
+				if ( eng->arg.GEn_PHYAdrInv ) {
+					eng->phy.PHY_BASE    = MAC_BASE4;
+					eng->run.MAC_idx_PHY = 3;
+				} else {
+					eng->phy.PHY_BASE    = MAC_BASE3;
+					eng->run.MAC_idx_PHY = 2;
+				}
+			else
+				if ( eng->arg.GEn_PHYAdrInv ) {
+					eng->phy.PHY_BASE    = MAC_BASE3;
+					eng->run.MAC_idx_PHY = 2;
+				} else {
+					eng->phy.PHY_BASE    = MAC_BASE4;
+					eng->run.MAC_idx_PHY = 3;
+				}
+
+			eng->env.MAC_1Gvld = 0;
+			eng->env.MAC_RMII  = 1;
+
+			if ( eng->run.Speed_1G ) {
+				printf("\nMAC3/MAC4 don't support 1Gbps !!!\n");
+				return( Finish_Check( eng, Err_Flag_MACMode ) );
+			}
+		} // End if ( eng->run.MAC_idx == 0 )
+
+		if ( !eng->env.MAC_1Gvld )
+			eng->run.Speed_org[ 0 ] = 0;
+
+		if ( ( eng->ModeSwitch == MODE_NSCI ) && ( !eng->env.MAC_RMII ) ) {
+			printf("\nNCSI must be RMII interface !!!\n");
+			return( Finish_Check( eng, Err_Flag_MACMode ) );
+		}
+
+		//------------------------------
+		// [Env]setup MHCLK_Ratio
+		//------------------------------
+#ifdef AST1010_CHIP
+		// Check bit 13:12
+		// The STA of the AST1010 is MHCLK 100 MHz
+		eng->env.MHCLK_Ratio = ( eng->reg.SCU_008 >> 12 ) & 0x3;
+		if ( eng->env.MHCLK_Ratio != 0x0 ) {
+			FindErr( eng, Err_Flag_MHCLK_Ratio );
+//			return( Finish_Check( eng, Err_Flag_MHCLK_Ratio ) );
+		}
+#elif defined(AST2500_IOMAP)
+		eng->env.MHCLK_Ratio = ( eng->reg.SCU_008 >> 16 ) & 0x7;
+		if ( eng->env.MAC_atlast_1Gvld ) {
+			if ( eng->env.MHCLK_Ratio != 2 ) {
+				FindErr( eng, Err_Flag_MHCLK_Ratio );
+//				return( Finish_Check( eng, Err_Flag_MHCLK_Ratio ) );
+			}
+		}
+		else {
+			if ( eng->env.MHCLK_Ratio != 4 ) {
+				FindErr( eng, Err_Flag_MHCLK_Ratio );
+//				return( Finish_Check( eng, Err_Flag_MHCLK_Ratio ) );
+			}
+		}
+#else
+		if ( eng->env.AST2300 ) {
+			eng->env.MHCLK_Ratio = ( eng->reg.SCU_008 >> 16 ) & 0x7;
+			if ( eng->env.MAC_atlast_1Gvld ) {
+				if ( ( eng->env.MHCLK_Ratio == 0 ) || ( eng->env.MHCLK_Ratio > 2 ) ) {
+					FindErr( eng, Err_Flag_MHCLK_Ratio );
+//					return( Finish_Check( eng, Err_Flag_MHCLK_Ratio ) );
+				}
+			}
+			else {
+				if ( eng->env.MHCLK_Ratio != 4 ) {
+					FindErr( eng, Err_Flag_MHCLK_Ratio );
+//					return( Finish_Check( eng, Err_Flag_MHCLK_Ratio ) );
+				}
+			}
+		} // End if ( eng->env.AST2300 )
+#endif
+
+//------------------------------------------------------------
+// Parameter Initial
+//------------------------------------------------------------
+		//------------------------------
+		// [Reg]setup SCU_004_rstbit
+		// [Reg]setup SCU_004_mix
+		// [Reg]setup SCU_004_dis
+		// [Reg]setup SCU_004_en
+		//------------------------------
+#ifdef AST1010_CHIP
+		eng->reg.SCU_004_rstbit = 0x00000010; //Reset Engine
+#elif defined(AST2500_IOMAP)
+		if ( eng->arg.GEn_PHYAdrInv ) {
+			eng->reg.SCU_004_rstbit = 0x00001800; //Reset Engine
+		}
+		else {
+			if ( eng->run.MAC_idx == 1 )
+				eng->reg.SCU_004_rstbit = 0x00001000; //Reset Engine
+			else
+				eng->reg.SCU_004_rstbit = 0x00000800; //Reset Engine
+		}
+#else
+//		if ( eng->arg.GEn_PHYAdrInv ) {
+			if ( eng->env.AST2300 )
+				eng->reg.SCU_004_rstbit = 0x0c001800; //Reset Engine
+			else
+				eng->reg.SCU_004_rstbit = 0x00001800; //Reset Engine
+//		}
+//		else {
+//			switch ( eng->run.MAC_idx ) {
+//				case 3: eng->reg.SCU_004_rstbit = 0x08000000; break; //Reset Engine
+//				case 2: eng->reg.SCU_004_rstbit = 0x04000000; break; //Reset Engine
+//				case 1: eng->reg.SCU_004_rstbit = 0x00001000; break; //Reset Engine
+//				case 0: eng->reg.SCU_004_rstbit = 0x00000800; break; //Reset Engine
+//			}
+//		}
+#endif
+		eng->reg.SCU_004_mix = eng->reg.SCU_004;
+		eng->reg.SCU_004_en  = eng->reg.SCU_004_mix & (~eng->reg.SCU_004_rstbit);
+		eng->reg.SCU_004_dis = eng->reg.SCU_004_mix |   eng->reg.SCU_004_rstbit;
+
+		//------------------------------
+		// [Reg]setup SCU_00c_clkbit
+		// [Reg]setup SCU_00c_mix
+		// [Reg]setup SCU_00c_dis
+		// [Reg]setup SCU_00c_en
+		//------------------------------
+#ifdef AST1010_CHIP
+		eng->reg.SCU_00c_clkbit = 0x00000040;
+#else
+		if ( eng->env.AST2300 ) {
+//			if ( eng->arg.GEn_PHYAdrInv ) {
+				if ( eng->env.MAC34_vld )
+					eng->reg.SCU_00c_clkbit = 0x00f00000; //Clock Stop Control
+				else
+					eng->reg.SCU_00c_clkbit = 0x00300000; //Clock Stop Control
+//			}
+//			else {
+//				switch ( eng->run.MAC_idx ) {
+//					case 3: eng->reg.SCU_00c_clkbit = 0x00800000; break; //Clock Stop Control
+//					case 2: eng->reg.SCU_00c_clkbit = 0x00400000; break; //Clock Stop Control
+//					case 1: eng->reg.SCU_00c_clkbit = 0x00200000; break; //Clock Stop Control
+//					case 0: eng->reg.SCU_00c_clkbit = 0x00100000; break; //Clock Stop Control
+//			}
+		}
+		else {
+			eng->reg.SCU_00c_clkbit = 0x00000000;
+		} // End if ( eng->env.AST2300 )
+#endif
+		eng->reg.SCU_00c_mix = eng->reg.SCU_00c;
+		eng->reg.SCU_00c_en  = eng->reg.SCU_00c_mix & (~eng->reg.SCU_00c_clkbit);
+		eng->reg.SCU_00c_dis = eng->reg.SCU_00c_mix |   eng->reg.SCU_00c_clkbit;
+
+		//------------------------------
+		// [Reg]setup SCU_048_mix
+		// [Reg]setup SCU_048_check
+		// [Reg]setup SCU_048_default
+		// [Reg]setup SCU_074_mix
+		//------------------------------
+#ifdef AST1010_CHIP
+		eng->reg.SCU_048_mix     = ( eng->reg.SCU_048 & 0xfefff0ff );
+		eng->reg.SCU_048_check   = ( eng->reg.SCU_048 & 0x01000f00 );
+		eng->reg.SCU_048_default =   SCU_48h_AST1010  & 0x01000f00;
+#else
+  #ifdef AST2500_IOMAP
+		eng->reg.SCU_048_mix     = ( eng->reg.SCU_048 & 0xfc000000 );
+		eng->reg.SCU_048_check   = ( eng->reg.SCU_048 & 0x03ffffff );
+		eng->reg.SCU_048_default =   SCU_48h_AST2500  & 0x03ffffff;
+
+		if ( eng->arg.GEn_RMII_50MOut & eng->env.MAC_RMII ) {
+			switch ( eng->run.MAC_idx ) {
+				case 1: eng->reg.SCU_048_mix = eng->reg.SCU_048_mix | 0x40000000; break;
+				case 0: eng->reg.SCU_048_mix = eng->reg.SCU_048_mix | 0x20000000; break;
+			}
+		}
+  #else
+		eng->reg.SCU_048_mix     = ( eng->reg.SCU_048 & 0xf0000000 );
+		if ( eng->env.MAC34_vld ) {
+			eng->reg.SCU_048_check   = ( eng->reg.SCU_048 & 0x0fffffff );
+			eng->reg.SCU_048_default =    SCU_48h_AST2300 & 0x0fffffff;
+		}
+		else {
+			eng->reg.SCU_048_check   = ( eng->reg.SCU_048 & 0x0300ffff );
+			eng->reg.SCU_048_default =    SCU_48h_AST2300 & 0x0300ffff;
+		}
+  #endif
+#endif
+		eng->reg.SCU_074_mix = eng->reg.SCU_074;
+
+		//------------------------------
+		// [Reg]setup MAC_050
+		//------------------------------
+		if ( eng->ModeSwitch == MODE_NSCI )
+			// Set to 100Mbps and Enable RX broabcast packets and CRC_APD and Full duplex
+			eng->reg.MAC_050 = 0x000a0500;// [100Mbps] RX_BROADPKT_EN & CRC_APD & Full duplex
+//			eng->reg.MAC_050 = 0x000a4500;// [100Mbps] RX_BROADPKT_EN & RX_ALLADR & CRC_APD & Full duplex
+		else {
+#ifdef Enable_MAC_ExtLoop
+//			eng->reg.MAC_050 = 0x00000100;// Full duplex
+			eng->reg.MAC_050 = 0x00004100;// RX_ALLADR & Full duplex
+#else
+			eng->reg.MAC_050 = 0x00004500;// RX_ALLADR & CRC_APD & Full duplex
+#endif
+#ifdef Enable_Runt
+			eng->reg.MAC_050 = eng->reg.MAC_050 | 0x00001000;
+#endif
+#if defined(PHY_SPECIAL)
+			eng->reg.MAC_050 = eng->reg.MAC_050 | 0x00002000;
+#elif defined(Enable_Jumbo)
+			eng->reg.MAC_050 = eng->reg.MAC_050 | 0x00002000;
+#endif
+		} // End if ( eng->ModeSwitch == MODE_NSCI )
+
+//------------------------------------------------------------
+// Descriptor Number
+//------------------------------------------------------------
+		//------------------------------
+		// [Dat]setup Des_Num
+		// [Dat]setup DMABuf_Size
+		// [Dat]setup DMABuf_Num
+		//------------------------------
+		if ( eng->ModeSwitch == MODE_DEDICATED ) {
+#ifdef Enable_Jumbo
+			DES_LowNumber = 1;
+#else
+			DES_LowNumber = eng->run.TM_IOTiming;
+#endif
+			if ( eng->arg.GEn_SkipChkPHY && ( eng->arg.GTestMode == 0 ) )
+				eng->dat.Des_Num = 114;//for SMSC's LAN9303 issue
+			else {
+#ifdef AST1010_CHIP
+				eng->dat.Des_Num = ( eng->run.IO_Bund ) ? 100 : 256;
+#else
+				switch ( eng->arg.GSpeed ) {
+					case SET_1GBPS          : eng->dat.Des_Num = ( eng->run.IO_Bund ) ? 100 : ( DES_LowNumber ) ? 512 : 4096; break;
+					case SET_100MBPS        : eng->dat.Des_Num = ( eng->run.IO_Bund ) ? 100 : ( DES_LowNumber ) ? 512 : 4096; break;
+					case SET_10MBPS         : eng->dat.Des_Num = ( eng->run.IO_Bund ) ? 100 : ( DES_LowNumber ) ? 100 :  830; break;
+					case SET_1G_100M_10MBPS : eng->dat.Des_Num = ( eng->run.IO_Bund ) ? 100 : ( DES_LowNumber ) ? 100 :  830; break;
+					case SET_100M_10MBPS    : eng->dat.Des_Num = ( eng->run.IO_Bund ) ? 100 : ( DES_LowNumber ) ? 100 :  830; break;
+				}
+#endif
+			} // End if ( eng->arg.GEn_SkipChkPHY && ( eng->arg.GTestMode == 0 ) )
+#ifdef SelectDesNumber
+			eng->dat.Des_Num = SelectDesNumber;
+#endif
+#ifdef USE_LPC
+			eng->dat.Des_Num /= 8;
+#endif
+#ifdef ENABLE_ARP_2_WOL
+			if ( eng->arg.GTestMode == 4 )
+				eng->dat.Des_Num = 1;
+#endif
+			eng->dat.Des_Num_Org = eng->dat.Des_Num;
+			eng->dat.DMABuf_Size = DMA_BufSize; //keep in order: Des_Num --> DMABuf_Size --> DMABuf_Num
+			eng->dat.DMABuf_Num  = DMA_BufNum;  //keep in order: Des_Num --> DMABuf_Size --> DMABuf_Num
+
+			if ( DbgPrn_Info ) {
+				printf("CheckBuf_MBSize : %ld\n",       eng->run.CheckBuf_MBSize);
+				printf("LOOP_CheckNum   : %ld\n",       eng->run.LOOP_CheckNum);
+				printf("Des_Num         : %ld\n",       eng->dat.Des_Num);
+				printf("DMA_BufSize     : %ld bytes\n", eng->dat.DMABuf_Size);
+				printf("DMA_BufNum      : %ld\n",       eng->dat.DMABuf_Num);
+				printf("DMA_PakSize     : %d\n",        DMA_PakSize);
+				printf("\n");
+			}
+			if ( 2 > eng->dat.DMABuf_Num )
+				return( Finish_Check( eng, Err_Flag_DMABufNum ) );
+		} // End if ( eng->ModeSwitch == MODE_DEDICATED )
+
+//------------------------------------------------------------
+// Setup Running Parameter
+//------------------------------------------------------------
+#ifdef Enable_MAC_ExtLoop
+		eng->run.TDES_BASE = RDES_BASE1;
+		eng->run.RDES_BASE = RDES_BASE1;
+#else
+		eng->run.TDES_BASE = TDES_BASE1;
+		eng->run.RDES_BASE = RDES_BASE1;
+#endif
+
+		if ( eng->run.TM_IOTiming || eng->run.IO_Bund )
+			eng->run.IO_MrgChk = 1;
+		else
+			eng->run.IO_MrgChk = 0;
+
+		eng->phy.Adr         = eng->arg.GPHYADR;
+		eng->phy.loop_phy    = eng->arg.GEn_IntLoopPHY;
+		eng->phy.default_phy = eng->run.TM_DefaultPHY;
+
+		eng->run.LOOP_MAX = eng->arg.GLOOP_MAX;
+		Calculate_LOOP_CheckNum( eng );
+
+	} // End if (RUN_STEP >= 1)
+
+//------------------------------------------------------------
+// SCU Initial
+//------------------------------------------------------------
+	if ( RUN_STEP >= 2 ) {
+		get_mac_info( eng );
+		Setting_scu( eng );
+		init_scu1( eng );
+	}
+
+	if ( RUN_STEP >= 3 ) {
+//		init_scu_macrst( eng );
+		init_scu_macdis( eng );
+		init_scu_macen( eng );
+		if ( eng->ModeSwitch ==  MODE_DEDICATED ) {
+#if defined(PHY_GPIO)
+			phy_gpio_init( eng );
+#endif
+#if defined(PHY_SPECIAL)
+			special_PHY_MDIO_init( eng );
+#endif
+			eng->phy.PHYAdrValid = find_phyadr( eng );
+			if ( eng->phy.PHYAdrValid == TRUE ) {
+				phy_sel( eng, phyeng );
+			}
+		}
+	}
+
+//------------------------------------------------------------
+// Data Initial
+//------------------------------------------------------------
+	if (RUN_STEP >= 4) {
+		if ( eng->ModeSwitch ==  MODE_DEDICATED ) {
+			if ( eng->run.TM_Burst )
+				setup_arp ( eng );
+			eng->dat.FRAME_LEN = (ULONG *)malloc( eng->dat.Des_Num    * sizeof( ULONG ) );
+			eng->dat.wp_lst    = (ULONG *)malloc( eng->dat.Des_Num    * sizeof( ULONG ) );
+
+			if ( !eng->dat.FRAME_LEN )
+				return( Finish_Check( eng, Err_Flag_MALLOC_FrmSize ) );
+			if ( !eng->dat.wp_lst )
+				return( Finish_Check( eng, Err_Flag_MALLOC_LastWP ) );
+
+			// Setup data and length
+#if defined(PHY_SPECIAL)
+			special_PHY_buf_init( eng );
+#endif
+			TestingSetup ( eng );
+		} 
+		else {
+			if ( eng->arg.GARPNumCnt != 0 )
+				setup_arp ( eng );
+		}// End if ( eng->ModeSwitch ==  MODE_DEDICATED )
+
+		init_iodelay( eng );
+		eng->run.Speed_idx = 0;
+		if ( !eng->io.Dly_3Regiser )
+			if ( get_iodelay( eng ) )
+				return( Finish_Check( eng, 0 ) );
+
+	} // End if (RUN_STEP >= 4)
+
+//------------------------------------------------------------
+// main
+//------------------------------------------------------------
+	if (RUN_STEP >= 5) {
+#ifdef  DbgPrn_FuncHeader
+		printf("Speed_org: %d %d %d\n", eng->run.Speed_org[ 0 ], eng->run.Speed_org[ 1 ], eng->run.Speed_org[ 2 ]);
+		Debug_delay();
+#endif
+
+		eng->flg.AllFail = 1;
+#ifdef Enable_LOOP_INFINI
+LOOP_INFINI:;
+#endif
+		for ( eng->run.Speed_idx = 0; eng->run.Speed_idx < 3; eng->run.Speed_idx++ )
+			eng->run.Speed_sel[ (int)eng->run.Speed_idx ] = eng->run.Speed_org[ (int)eng->run.Speed_idx ];
+
+		//------------------------------
+		// [Start] The loop of different speed
+		//------------------------------
+		for ( eng->run.Speed_idx = 0; eng->run.Speed_idx < 3; eng->run.Speed_idx++ ) {
+			eng->flg.Flag_PrintEn = 1;
+			if ( eng->run.Speed_sel[ (int)eng->run.Speed_idx ] ) {
+				// Setting speed of LAN
+				if      ( eng->run.Speed_sel[ 0 ] ) eng->reg.MAC_050_Speed = eng->reg.MAC_050 | 0x0000020f;
+				else if ( eng->run.Speed_sel[ 1 ] ) eng->reg.MAC_050_Speed = eng->reg.MAC_050 | 0x0008000f;
+				else                                eng->reg.MAC_050_Speed = eng->reg.MAC_050 | 0x0000000f;
+#ifdef Enable_CLK_Stable
+//				init_scu_macdis( eng );
+//				init_scu_macen( eng );
+				Write_Reg_SCU_DD( 0x0c, eng->reg.SCU_00c_dis );//Clock Stop Control
+				Read_Reg_SCU_DD( 0x0c );
+				Write_Reg_MAC_DD( eng, 0x50, eng->reg.MAC_050_Speed & 0xfffffff0 );
+				Write_Reg_SCU_DD( 0x0c, eng->reg.SCU_00c_en );//Clock Stop Control
+#endif
+
+				// Setting check owner time out
+				if      ( eng->run.Speed_sel[ 0 ] ) eng->run.TIME_OUT_Des = eng->run.TIME_OUT_Des_PHYRatio * TIME_OUT_Des_1G;
+				else if ( eng->run.Speed_sel[ 1 ] ) eng->run.TIME_OUT_Des = eng->run.TIME_OUT_Des_PHYRatio * TIME_OUT_Des_100M;
+				else                                eng->run.TIME_OUT_Des = eng->run.TIME_OUT_Des_PHYRatio * TIME_OUT_Des_10M;
+
+				if ( eng->run.TM_WaitStart )
+					eng->run.TIME_OUT_Des = eng->run.TIME_OUT_Des * 10000;
+
+				// Setting the LAN speed
+				if ( eng->ModeSwitch ==  MODE_DEDICATED ) {
+					// Test three speed of LAN, we will modify loop number
+					if ( ( eng->arg.GSpeed == SET_1G_100M_10MBPS ) || ( eng->arg.GSpeed == SET_100M_10MBPS ) ) {
+						if      ( eng->run.Speed_sel[ 0 ] ) eng->run.LOOP_MAX = eng->arg.GLOOP_MAX;
+						else if ( eng->run.Speed_sel[ 1 ] ) eng->run.LOOP_MAX = eng->arg.GLOOP_MAX / 100;
+						else                                eng->run.LOOP_MAX = eng->arg.GLOOP_MAX / 1000;
+
+						if ( !eng->run.LOOP_MAX )
+							eng->run.LOOP_MAX = 1;
+
+						Calculate_LOOP_CheckNum( eng );
+					}
+
+					//------------------------------
+					// PHY Initial
+					//------------------------------
+					if ( eng->env.AST1100 )
+						init_scu2 ( eng );
+
+#ifdef SUPPORT_PHY_LAN9303
+					if ( eng->arg.GEn_InitPHY )
+						LAN9303( LAN9303_I2C_BUSNUM, eng->arg.GPHYADR, eng->run.Speed_idx, eng->arg.GEn_IntLoopPHY | (eng->run.TM_Burst<<1) | eng->run.TM_IEEE );
+#elif defined(PHY_SPECIAL)
+					if ( eng->arg.GEn_InitPHY )
+						special_PHY_reg_init( eng );
+#else
+					if ( phyeng->fp_set != 0 ) {
+						init_phy( eng, phyeng );
+  #ifdef Delay_PHYRst
+//						DELAY( Delay_PHYRst * 10 );
+  #endif
+					}
+#endif
+
+					if ( eng->env.AST1100 )
+						init_scu3 ( eng );
+
+					if ( eng->flg.Err_Flag )
+						return( Finish_Check( eng, 0 ) );
+				} // End if ( eng->ModeSwitch ==  MODE_DEDICATED )
+
+				//------------------------------
+				// [Start] The loop of different IO strength
+				//------------------------------
+				for ( eng->io.Str_i = 0; eng->io.Str_i <= eng->io.Str_max; eng->io.Str_i++ ) {
+					//------------------------------
+					// Print Header of report to monitor and log file
+					//------------------------------
+					if ( eng->io.Dly_3Regiser )
+						if ( get_iodelay( eng ) )
+							return( Finish_Check( eng, 0 ) );
+
+					if ( eng->run.IO_MrgChk ) {
+						if ( eng->run.TM_IOStrength ) {
+#ifdef AST1010_CHIP
+							eng->io.Str_val = eng->io.Str_reg_mask | ( ( eng->io.Str_i ) ? 0xc000 : 0x0 );
+#else
+							eng->io.Str_val = eng->io.Str_reg_mask | ( eng->io.Str_i << eng->io.Str_shf );
+#endif
+//printf("\nIOStrength_val= %08lx, ", eng->io.Str_val);
+//printf("SCU90h: %08lx ->", Read_Reg_SCU_DD( 0x90 ));
+							Write_Reg_SCU_DD( eng->io.Str_reg_idx, eng->io.Str_val );
+//printf(" %08lx\n", Read_Reg_SCU_DD( 0x90 ));
+						}
+
+						if ( eng->run.IO_Bund )
+							PrintIO_Header( eng, FP_LOG );
+						if ( eng->run.TM_IOTiming )
+							PrintIO_Header( eng, FP_IO );
+						PrintIO_Header( eng, STD_OUT );
+					}
+					else {
+						if ( eng->ModeSwitch == MODE_DEDICATED ) {
+							if ( !eng->run.TM_Burst )
+								Print_Header( eng, FP_LOG );
+							Print_Header( eng, STD_OUT );
+						}
+					} // End if ( eng->run.IO_MrgChk )
+
+					//------------------------------
+					// [Start] The loop of different IO out delay
+					//------------------------------
+					for ( eng->io.Dly_out = eng->io.Dly_out_str; eng->io.Dly_out <= eng->io.Dly_out_end; eng->io.Dly_out+=eng->io.Dly_out_cval ) {
+						if ( eng->run.IO_MrgChk ) {
+							eng->io.Dly_out_selval  = eng->io.value_ary[ eng->io.Dly_out ];
+							eng->io.Dly_out_reg_hit = ( eng->io.Dly_out_reg == eng->io.Dly_out_selval ) ? 1 : 0;
+#ifdef Enable_Fast_SCU
+							init_scu_macdis( eng );
+							Write_Reg_SCU_DD( eng->io.Dly_reg_idx, eng->reg.SCU_048_mix | ( eng->io.Dly_out_selval << eng->io.Dly_out_shf ) );
+							init_scu_macen( eng );
+#endif
+							if ( eng->run.TM_IOTiming )
+								PrintIO_LineS( eng, FP_IO );
+							PrintIO_LineS( eng, STD_OUT );
+						} // End if ( eng->run.IO_MrgChk )
+
+#ifdef Enable_Fast_SCU
+						//------------------------------
+						// SCU Initial
+						//------------------------------
+//						init_scu_macrst( eng );
+
+						//------------------------------
+						// MAC Initial
+						//------------------------------
+						init_mac( eng );
+						if ( eng->flg.Err_Flag )
+							return( Finish_Check( eng, 0 ) );
+#endif
+						//------------------------------
+						// [Start] The loop of different IO in delay
+						//------------------------------
+						for ( eng->io.Dly_in = eng->io.Dly_in_str; eng->io.Dly_in <= eng->io.Dly_in_end; eng->io.Dly_in+=eng->io.Dly_in_cval ) {
+							if ( eng->run.IO_MrgChk ) {
+								eng->io.Dly_in_selval  = eng->io.value_ary[ eng->io.Dly_in ];
+								eng->io.Dly_val = ( eng->io.Dly_in_selval  << eng->io.Dly_in_shf  )
+								                | ( eng->io.Dly_out_selval << eng->io.Dly_out_shf );
+
+//printf("\nDly_val= %08lx, ", eng->io.Dly_val);
+//printf("SCU%02lxh: %08lx ->", eng->io.Dly_reg_idx, Read_Reg_SCU_DD( eng->io.Dly_reg_idx ) );
+								init_scu_macdis( eng );
+								Write_Reg_SCU_DD( eng->io.Dly_reg_idx, eng->reg.SCU_048_mix | eng->io.Dly_val );
+								init_scu_macen( eng );
+//printf(" %08lx\n", Read_Reg_SCU_DD( eng->io.Dly_reg_idx ) );
+							} // End if ( eng->run.IO_MrgChk )
+#ifdef Enable_Fast_SCU
+#else
+							//------------------------------
+							// SCU Initial
+							//------------------------------
+//							init_scu_macrst( eng );
+
+							//------------------------------
+							// MAC Initial
+							//------------------------------
+							init_mac( eng );
+							if ( eng->flg.Err_Flag )
+								return( Finish_Check( eng, 0 ) );
+#endif
+							// Testing
+							if ( eng->ModeSwitch == MODE_NSCI )
+								eng->io.Dly_result = phy_ncsi( eng );
+							else
+								eng->io.Dly_result = TestingLoop( eng, eng->run.LOOP_CheckNum );
+							eng->io.dlymap[ eng->io.Dly_in ][ eng->io.Dly_out ] = eng->io.Dly_result;
+
+							// Display to Log file and monitor
+							if ( eng->run.IO_MrgChk ) {
+								if ( eng->run.TM_IOTiming )
+									PrintIO_Line( eng, FP_IO );
+								PrintIO_Line( eng, STD_OUT );
+
+								FPri_ErrFlag( eng, FP_LOG );
+								PrintIO_Line_LOG( eng );
+
+								eng->flg.Wrn_Flag  = 0;
+								eng->flg.Err_Flag  = 0;
+								eng->flg.Des_Flag  = 0;
+								eng->flg.NCSI_Flag = 0;
+							} //End if ( eng->run.IO_MrgChk )
+						} // End for ( eng->io.Dly_in = eng->io.Dly_in_str; eng->io.Dly_in <= eng->io.Dly_in_end; eng->io.Dly_in+=eng->io.Dly_in_cval )
+
+
+						if ( eng->run.IO_MrgChk ) {
+							if ( eng->run.TM_IOTiming ) {
+								PRINTF( FP_IO, "\n" );
+							}
+							printf("\n");
+						}
+					} // End for ( eng->io.Dly_out = eng->io.Dly_out_str; eng->io.Dly_out <= eng->io.Dly_out_end; eng->io.Dly_out+=eng->io.Dly_out_cval )
+
+
+					//------------------------------
+					// End
+					//------------------------------
+					if ( eng->run.IO_MrgChk ) {
+						for ( eng->io.Dly_out = eng->io.Dly_out_min; eng->io.Dly_out <= eng->io.Dly_out_max; eng->io.Dly_out++ )
+							for ( eng->io.Dly_in = eng->io.Dly_in_min; eng->io.Dly_in <= eng->io.Dly_in_max; eng->io.Dly_in++ )
+								if ( eng->io.dlymap[ eng->io.Dly_in ][ eng->io.Dly_out ] ) {
+									if ( eng->run.TM_IOTiming ) {
+										for ( j = eng->io.Dly_out_min; j <= eng->io.Dly_out_max; j++ ) {
+											for ( i = eng->io.Dly_in_min; i <= eng->io.Dly_in_max; i++ )
+												if ( eng->io.dlymap[i][j] )
+													{ PRINTF( FP_IO, "x " ); }
+												else
+													{ PRINTF( FP_IO, "o " ); }
+											PRINTF( FP_IO, "\n" );
+										}
+									} // End if ( eng->run.TM_IOTiming )
+
+									FindErr( eng, Err_Flag_IOMargin );
+									goto Find_Err_Flag_IOMargin;
+								} // End if ( eng->io.dlymap[ eng->io.Dly_in ][ eng->io.Dly_out ] )
+					} // End if ( eng->run.IO_MrgChk )
+
+Find_Err_Flag_IOMargin:
+					if ( !eng->run.TM_Burst )
+						FPri_ErrFlag( eng, FP_LOG );
+					if ( eng->run.TM_IOTiming )
+						FPri_ErrFlag( eng, FP_IO );
+
+					FPri_ErrFlag( eng, STD_OUT );
+
+					Wrn_Flag_allapeed  = Wrn_Flag_allapeed  | eng->flg.Wrn_Flag;
+					Err_Flag_allapeed  = Err_Flag_allapeed  | eng->flg.Err_Flag;
+					Des_Flag_allapeed  = Des_Flag_allapeed  | eng->flg.Err_Flag;
+					NCSI_Flag_allapeed = NCSI_Flag_allapeed | eng->flg.Err_Flag;
+					eng->flg.Wrn_Flag  = 0;
+					eng->flg.Err_Flag  = 0;
+					eng->flg.Des_Flag  = 0;
+					eng->flg.NCSI_Flag = 0;
+				} // End for ( eng->io.Str_i = 0; eng->io.Str_i <= eng->io.Str_max; eng->io.Str_i++ ) {
+
+				if ( eng->ModeSwitch == MODE_DEDICATED ) {
+#ifdef PHY_SPECIAL
+					if ( eng->arg.GEn_InitPHY )
+						special_PHY_recov( eng );
+#else
+//					if ( ( eng->io.Dly_result == 0 ) & ( phyeng->fp_clr != 0 ) )
+					if ( phyeng->fp_clr != 0 )
+						recov_phy( eng, phyeng );
+#endif
+				}
+
+				eng->run.Speed_sel[ (int)eng->run.Speed_idx ] = 0;
+			} // End if ( eng->run.Speed_sel[ eng->run.Speed_idx ] )
+
+			eng->flg.Flag_PrintEn = 0;
+		} // End for ( eng->run.Speed_idx = 0; eng->run.Speed_idx < 3; eng->run.Speed_idx++ )
+
+		eng->flg.Wrn_Flag  = Wrn_Flag_allapeed;
+		eng->flg.Err_Flag  = Err_Flag_allapeed;
+		eng->flg.Des_Flag  = Des_Flag_allapeed;
+		eng->flg.NCSI_Flag = NCSI_Flag_allapeed;
+
+#ifdef Enable_LOOP_INFINI
+		if ( eng->flg.Err_Flag == 0 ) {
+  #if defined(ENABLE_LOG_FILE)
+			if ( eng->fp_log ) {
+				fclose( eng->fp_log );
+				eng->fp_log = fopen(FileName,"w");
+			}
+  #endif
+			goto LOOP_INFINI;
+		}
+#endif
+	} // End if (RUN_STEP >= 5)
+
+	return( Finish_Check( eng, 0 ) );
+}
--- uboot_old/oem/ami/standalone/nettest/Makefile	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/Makefile	2018-06-22 17:51:23.921169873 +0800
@@ -0,0 +1,100 @@
+#
+# (C) Copyright 2000-2006
+# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
+#
+# See file CREDITS for list of people who contributed to this
+# project.
+#
+# This program is free software; you can redistribute it and/or
+# modify it under the terms of the GNU General Public License as
+# published by the Free Software Foundation; either version 2 of
+# the License, or (at your option) any later version.
+#
+# This program is distributed in the hope that it will be useful,
+# but WITHOUT ANY WARRANTY; without even the implied warranty of
+# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+# GNU General Public License for more details.
+#
+# You should have received a copy of the GNU General Public License
+# along with this program; if not, write to the Free Software
+# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+# MA 02111-1307 USA
+#
+
+include $(TOPDIR)/config.mk
+
+ELF-$(ARCH)  :=
+ELF-$(BOARD) :=
+ELF-$(CPU)   :=
+ELF-y        := nettest
+ENTRY        := entry
+
+#ELF-$(CONFIG_SMC91111)           += smc91111_eeprom
+#ELF-$(CONFIG_SMC911X)            += smc911x_eeprom
+#ELF-$(CONFIG_SPI_FLASH_ATMEL)    += atmel_df_pow2
+#ELF-i386                         += 82559_eeprom
+#ELF-mpc5xxx                      += interrupt
+#ELF-mpc8xx                       += test_burst timer
+#ELF-mpc8260                      += mem_to_mem_idma2intr
+#ELF-ppc                          += sched
+#ELF-oxc                          += eepro100_eeprom
+
+#
+# Some versions of make do not handle trailing white spaces properly;
+# leading to build failures. The problem was found with GNU Make 3.80.
+# Using 'strip' as a workaround for the problem.
+#
+ELF	:= $(strip $(ELF-y) $(ELF-$(ARCH)) $(ELF-$(BOARD)) $(ELF-$(CPU)))
+
+BIN	:= $(addsuffix .bin,$(ELF))
+
+SRCS	= entry.c stdalone.c
+
+#NETTEST
+SRCS	+= MACTEST.c DRAM_SPI.c IO.c LIB.c MAC.c NCSI.c PCI_SPI.c PHY.c SPIM.c LAN9303.c STDUBOOT.c PHYGPIO.c PHYSPECIAL.c
+
+#SLT
+SRCS	+= STRESS.c
+
+COBJS	:= $(SRCS:.c=.o)
+
+LIB	:= $(obj)libstubs.o
+
+LIBCOBJS = stubs.o
+
+LIBOBJS	= $(addprefix $(obj),$(LIBAOBJS) $(LIBCOBJS))
+
+OBJS	:= $(addprefix $(obj),$(COBJS))
+ELF	:= $(addprefix $(obj),$(ELF))
+BIN	:= $(addprefix $(obj),$(BIN))
+
+gcclibdir := $(shell dirname `$(CC) -print-libgcc-file-name`)
+
+# We don't want gcc reordering functions if possible.  This ensures that an
+# application's entry point will be the first function in the application's
+# source file.
+#CFLAGS_NTR := $(call cc-option,-fno-toplevel-reorder)
+#CFLAGS += $(CFLAGS_NTR)
+
+all:	$(obj).depend $(OBJS) $(LIB) $(ELF) $(BIN)
+
+#########################################################################
+$(LIB):	$(obj).depend $(LIBOBJS)
+	$(call cmd_link_o_target, $(LIBOBJS))
+
+$(ELF):		$(OBJS) $(LIB)
+		$(LD) $(LDFLAGS) -g -Ttext $(CONFIG_STANDALONE_LOAD_ADDR) \
+		-o $@ -e $(ENTRY) $(OBJS) $(LIB) $(PLATFORM_LIBS)
+
+$(BIN):
+$(obj)%.bin:	$(obj)%
+		$(OBJCOPY) -O binary $< $@ 2>/dev/null
+
+#########################################################################
+
+# defines $(obj).depend target
+include $(SRCTREE)/rules.mk
+
+sinclude $(obj).depend
+
+#########################################################################
--- uboot_old/oem/ami/standalone/nettest/NCSI.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/NCSI.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,998 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define NCSI_C
+static const char ThisFile[] = "NCSI.c";
+
+#include "SWFUNC.H"
+
+#if defined(SLT_UBOOT)
+  #include <common.h>
+  #include <command.h>
+  #include "COMMINF.H"
+  #include "NCSI.H"
+  #include "IO.H"
+#endif
+#if defined(DOS_ALONE)
+  #include <stdio.h>
+  #include <stdlib.h>
+  #include <conio.h>
+  #include <string.h>
+  #include "COMMINF.H"
+  #include "NCSI.H"
+  #include "IO.H"
+#endif
+#if defined(LinuxAP)
+  #include <stdio.h>
+  #include <string.h>
+  #include "COMMINF.H"
+  #include "NCSI.H"
+  #include "IO.H"
+#endif
+#if defined(SLT_NEW_ARCH)
+  #include <string.h>
+  #include "COMMINF.H"
+  #include "NCSI.H"
+  #include "IO.H"
+#endif
+
+
+//------------------------------------------------------------
+int FindErr_NCSI (MAC_ENGINE *eng, int value) {
+	eng->flg.NCSI_Flag = eng->flg.NCSI_Flag | value;
+	eng->flg.Err_Flag  = eng->flg.Err_Flag | Err_Flag_NCSI_LinkFail;
+	if ( DbgPrn_ErrFlg )
+		printf("\nErr_Flag: [%08lx] NCSI_Flag: [%08lx]\n", eng->flg.Err_Flag, eng->flg.NCSI_Flag);
+
+	return(1);
+}
+
+//------------------------------------------------------------
+// PHY IC(NC-SI)
+//------------------------------------------------------------
+void ncsi_reqdump (MAC_ENGINE *eng, NCSI_Command_Packet *in) {
+	int     i;
+	PRINTF( FP_LOG, "[NCSI-Request] DA             : %02x %02x %02x %02x %02x %02x\n", in->DA[ 0 ], in->DA[ 1 ], in->DA[ 2 ], in->DA[ 3 ], in->DA[ 4 ] , in->DA[ 5 ]);
+	PRINTF( FP_LOG, "[NCSI-Request] SA             : %02x %02x %02x %02x %02x %02x\n", in->SA[ 0 ], in->SA[ 1 ], in->SA[ 2 ], in->SA[ 3 ], in->SA[ 4 ] , in->SA[ 5 ]);
+	PRINTF( FP_LOG, "[NCSI-Request] EtherType      : %04x\n", SWAP_2B_BEDN( in->EtherType )             );//DMTF NC-SI
+	PRINTF( FP_LOG, "[NCSI-Request] MC_ID          : %02x\n", in->MC_ID                                 );//Management Controller should set this field to 0x00
+	PRINTF( FP_LOG, "[NCSI-Request] Header_Revision: %02x\n", in->Header_Revision                       );//For NC-SI 1.0 spec, this field has to set 0x01
+//	PRINTF( FP_LOG, "[NCSI-Request] Reserved_1     : %02x\n", in->Reserved_1                            ); //Reserved has to set to 0x00
+	PRINTF( FP_LOG, "[NCSI-Request] IID            : %02x\n", in->IID                                   );//Instance ID
+	PRINTF( FP_LOG, "[NCSI-Request] Command        : %02x\n", in->Command                               );
+	PRINTF( FP_LOG, "[NCSI-Request] ChID           : %02x\n", in->ChID                                  );
+	PRINTF( FP_LOG, "[NCSI-Request] Payload_Length : %04x\n", SWAP_2B_BEDN( in->Payload_Length )        );//Payload Length = 12 bits, 4 bits are reserved
+//	PRINTF( FP_LOG, "[NCSI-Request] Reserved_2     : %04x\n", in->Reserved_2                            );
+//	PRINTF( FP_LOG, "[NCSI-Request] Reserved_3     : %04x\n", in->Reserved_3                            );
+//	PRINTF( FP_LOG, "[NCSI-Request] Reserved_4     : %04x\n", in->Reserved_4                            );
+//	PRINTF( FP_LOG, "[NCSI-Request] Reserved_5     : %04x\n", in->Reserved_5                            );
+	PRINTF( FP_LOG, "[NCSI-Request] Response_Code  : %04x\n", SWAP_2B_BEDN( in->Response_Code )         );
+	PRINTF( FP_LOG, "[NCSI-Request] Reason_Code    : %04x\n", SWAP_2B_BEDN( in->Reason_Code )           );
+	for ( i = 0; i < SWAP_2B_BEDN( in->Payload_Length ); i++ ) {
+		switch ( i % 4 ) {
+			case 0	: PRINTF( FP_LOG, "[NCSI-Request] Payload_Data   : %02x", in->Payload_Data[ i ]); break;
+			case 3	: PRINTF( FP_LOG, " %02x\n", in->Payload_Data[ i ]); break;
+			default	: PRINTF( FP_LOG, " %02x", in->Payload_Data[ i ]); break;
+		}
+	}
+	if ( ( i % 4 ) != 3 )
+		PRINTF( FP_LOG, "\n");
+}
+void ncsi_respdump (MAC_ENGINE *eng, NCSI_Response_Packet *in) {
+	int     i;
+//	PRINTF( FP_LOG, "[NCSI-Respond] DA             : %02x %02x %02x %02x %02x %02x\n", in->DA[ 5 ], in->DA[ 4 ], in->DA[ 3 ], in->DA[ 2 ], in->DA[ 1] , in->DA[ 0 ]);
+//	PRINTF( FP_LOG, "[NCSI-Respond] SA             : %02x %02x %02x %02x %02x %02x\n", in->SA[ 5 ], in->SA[ 4 ], in->SA[ 3 ], in->SA[ 2 ], in->SA[ 1] , in->SA[ 0 ]);
+	PRINTF( FP_LOG, "[NCSI-Respond] DA             : %02x %02x %02x %02x %02x %02x\n", in->DA[ 0 ], in->DA[ 1 ], in->DA[ 2 ], in->DA[ 3 ], in->DA[ 4 ] , in->DA[ 5 ]);
+	PRINTF( FP_LOG, "[NCSI-Respond] SA             : %02x %02x %02x %02x %02x %02x\n", in->SA[ 0 ], in->SA[ 1 ], in->SA[ 2 ], in->SA[ 3 ], in->SA[ 4 ] , in->SA[ 5 ]);
+	PRINTF( FP_LOG, "[NCSI-Respond] EtherType      : %04x\n", SWAP_2B_BEDN( in->EtherType )             );//DMTF NC-SI
+	PRINTF( FP_LOG, "[NCSI-Respond] MC_ID          : %02x\n", in->MC_ID                                 );//Management Controller should set this field to 0x00
+	PRINTF( FP_LOG, "[NCSI-Respond] Header_Revision: %02x\n", in->Header_Revision                       );//For NC-SI 1.0 spec, this field has to set 0x01
+//	PRINTF( FP_LOG, "[NCSI-Respond] Reserved_1     : %02x\n", in->Reserved_1                            ); //Reserved has to set to 0x00
+	PRINTF( FP_LOG, "[NCSI-Respond] IID            : %02x\n", in->IID                                   );//Instance ID
+	PRINTF( FP_LOG, "[NCSI-Respond] Command        : %02x\n", in->Command                               );
+	PRINTF( FP_LOG, "[NCSI-Respond] ChID           : %02x\n", in->ChID                                  );
+	PRINTF( FP_LOG, "[NCSI-Respond] Payload_Length : %04x\n", SWAP_2B_BEDN( in->Payload_Length )        );//Payload Length = 12 bits, 4 bits are reserved
+//	PRINTF( FP_LOG, "[NCSI-Respond] Reserved_2     : %04x\n", in->Reserved_2                            );
+//	PRINTF( FP_LOG, "[NCSI-Respond] Reserved_3     : %04x\n", in->Reserved_3                            );
+//	PRINTF( FP_LOG, "[NCSI-Respond] Reserved_4     : %04x\n", in->Reserved_4                            );
+//	PRINTF( FP_LOG, "[NCSI-Respond] Reserved_5     : %04x\n", in->Reserved_5                            );
+	PRINTF( FP_LOG, "[NCSI-Respond] Response_Code  : %04x\n", SWAP_2B_BEDN( in->Response_Code )         );
+	PRINTF( FP_LOG, "[NCSI-Respond] Reason_Code    : %04x\n", SWAP_2B_BEDN( in->Reason_Code )           );
+	for ( i = 0; i < SWAP_2B_BEDN( in->Payload_Length ); i++ ) {
+		switch ( i % 4 ) {
+			case 0	: PRINTF( FP_LOG, "[NCSI-Respond] Payload_Data   : %02x", in->Payload_Data[ i ]); break;
+			case 3	: PRINTF( FP_LOG, " %02x\n", in->Payload_Data[ i ]); break;
+			default	: PRINTF( FP_LOG, " %02x", in->Payload_Data[ i ]); break;
+		}
+	}
+	if ( ( i % 4 ) != 3 )
+		PRINTF( FP_LOG, "\n");
+}
+
+//------------------------------------------------------------
+void NCSI_PrintCommandStr (MAC_ENGINE *eng, unsigned char command, unsigned iid) {
+	switch ( command & 0x80 ) {
+		case 0x80   : sprintf(eng->dat.NCSI_CommandStr, "IID:%3d [%02x:Respond]", iid, command); break;
+		default     : sprintf(eng->dat.NCSI_CommandStr, "IID:%3d [%02x:Request]", iid, command); break;
+	}
+	switch ( command & 0x7f ) {
+		case 0x00   : sprintf(eng->dat.NCSI_CommandStr, "%s[CLEAR_INITIAL_STATE                ]", eng->dat.NCSI_CommandStr); break;
+		case 0x01   : sprintf(eng->dat.NCSI_CommandStr, "%s[SELECT_PACKAGE                     ]", eng->dat.NCSI_CommandStr); break;
+		case 0x02   : sprintf(eng->dat.NCSI_CommandStr, "%s[DESELECT_PACKAGE                   ]", eng->dat.NCSI_CommandStr); break;
+		case 0x03   : sprintf(eng->dat.NCSI_CommandStr, "%s[ENABLE_CHANNEL                     ]", eng->dat.NCSI_CommandStr); break;
+		case 0x04   : sprintf(eng->dat.NCSI_CommandStr, "%s[DISABLE_CHANNEL                    ]", eng->dat.NCSI_CommandStr); break;
+		case 0x05   : sprintf(eng->dat.NCSI_CommandStr, "%s[RESET_CHANNEL                      ]", eng->dat.NCSI_CommandStr); break;
+		case 0x06   : sprintf(eng->dat.NCSI_CommandStr, "%s[ENABLE_CHANNEL_NETWORK_TX          ]", eng->dat.NCSI_CommandStr); break;
+		case 0x07   : sprintf(eng->dat.NCSI_CommandStr, "%s[DISABLE_CHANNEL_NETWORK_TX         ]", eng->dat.NCSI_CommandStr); break;
+		case 0x08   : sprintf(eng->dat.NCSI_CommandStr, "%s[AEN_ENABLE                         ]", eng->dat.NCSI_CommandStr); break;
+		case 0x09   : sprintf(eng->dat.NCSI_CommandStr, "%s[SET_LINK                           ]", eng->dat.NCSI_CommandStr); break;
+		case 0x0A   : sprintf(eng->dat.NCSI_CommandStr, "%s[GET_LINK_STATUS                    ]", eng->dat.NCSI_CommandStr); break;
+		case 0x0B   : sprintf(eng->dat.NCSI_CommandStr, "%s[SET_VLAN_FILTER                    ]", eng->dat.NCSI_CommandStr); break;
+		case 0x0C   : sprintf(eng->dat.NCSI_CommandStr, "%s[ENABLE_VLAN                        ]", eng->dat.NCSI_CommandStr); break;
+		case 0x0D   : sprintf(eng->dat.NCSI_CommandStr, "%s[DISABLE_VLAN                       ]", eng->dat.NCSI_CommandStr); break;
+		case 0x0E   : sprintf(eng->dat.NCSI_CommandStr, "%s[SET_MAC_ADDRESS                    ]", eng->dat.NCSI_CommandStr); break;
+		case 0x10   : sprintf(eng->dat.NCSI_CommandStr, "%s[ENABLE_BROADCAST_FILTERING         ]", eng->dat.NCSI_CommandStr); break;
+		case 0x11   : sprintf(eng->dat.NCSI_CommandStr, "%s[DISABLE_BROADCAST_FILTERING        ]", eng->dat.NCSI_CommandStr); break;
+		case 0x12   : sprintf(eng->dat.NCSI_CommandStr, "%s[ENABLE_GLOBAL_MULTICAST_FILTERING  ]", eng->dat.NCSI_CommandStr); break;
+		case 0x13   : sprintf(eng->dat.NCSI_CommandStr, "%s[DISABLE_GLOBAL_MULTICAST_FILTERING ]", eng->dat.NCSI_CommandStr); break;
+		case 0x14   : sprintf(eng->dat.NCSI_CommandStr, "%s[SET_NCSI_FLOW_CONTROL              ]", eng->dat.NCSI_CommandStr); break;
+		case 0x15   : sprintf(eng->dat.NCSI_CommandStr, "%s[GET_VERSION_ID                     ]", eng->dat.NCSI_CommandStr); break;
+		case 0x16   : sprintf(eng->dat.NCSI_CommandStr, "%s[GET_CAPABILITIES                   ]", eng->dat.NCSI_CommandStr); break;
+		case 0x17   : sprintf(eng->dat.NCSI_CommandStr, "%s[GET_PARAMETERS                     ]", eng->dat.NCSI_CommandStr); break;
+		case 0x18   : sprintf(eng->dat.NCSI_CommandStr, "%s[GET_CONTROLLER_PACKET_STATISTICS   ]", eng->dat.NCSI_CommandStr); break;
+		case 0x19   : sprintf(eng->dat.NCSI_CommandStr, "%s[GET_NCSI_STATISTICS                ]", eng->dat.NCSI_CommandStr); break;
+		case 0x1A   : sprintf(eng->dat.NCSI_CommandStr, "%s[GET_NCSI_PASS_THROUGH_STATISTICS   ]", eng->dat.NCSI_CommandStr); break;
+		case 0x50   : sprintf(eng->dat.NCSI_CommandStr, "%s[OEM_COMMAND                        ]", eng->dat.NCSI_CommandStr); break;
+		default     : sprintf(eng->dat.NCSI_CommandStr, "%s Not Support Command", eng->dat.NCSI_CommandStr); break ;
+	}
+} // End void NCSI_PrintCommandStr (MAC_ENGINE *eng, unsigned char command, unsigned iid)
+
+//------------------------------------------------------------
+void NCSI_PrintCommandType (MAC_ENGINE *eng, unsigned char command, unsigned iid) {
+	NCSI_PrintCommandStr( eng, command, iid );
+	printf("[NCSI-commd]%s\n", eng->dat.NCSI_CommandStr);
+}
+
+//------------------------------------------------------------
+void NCSI_PrintCommandType2File (MAC_ENGINE *eng, unsigned char command, unsigned iid) {
+	NCSI_PrintCommandStr( eng, command, iid );
+	PRINTF( FP_LOG, "[NCSI-commd]%s\n", eng->dat.NCSI_CommandStr );
+}
+
+//------------------------------------------------------------
+void NCSI_Struct_Initialize_SLT (MAC_ENGINE *eng) {
+	int        i;
+	ULONG      NCSI_RxDatBase;
+
+	eng->run.NCSI_RxTimeOutScale = 1;
+
+	for (i = 0; i < 6; i++) {
+		eng->ncsi_req.DA[ i ] = 0xFF;
+		eng->ncsi_req.SA[ i ] = eng->inf.SA[ i ];
+	}
+	eng->ncsi_req.EtherType       = SWAP_2B_BEDN( 0x88F8 ); // EtherType = 0x88F8 (DMTF NC-SI) page 50, table 8, NC-SI spec. version 1.0.0
+
+	eng->ncsi_req.MC_ID           = 0;
+	eng->ncsi_req.Header_Revision = 0x01;
+	eng->ncsi_req.Reserved_1      = 0;
+	eng->ncsi_req.IID             = 0;
+//	eng->ncsi_req.Command         = 0;
+//	eng->ncsi_req.ChID            = 0;
+//	eng->ncsi_req.Payload_Length  = 0;
+
+	eng->ncsi_req.Response_Code   = 0;
+	eng->ncsi_req.Reason_Code     = 0;
+	eng->ncsi_req.Reserved_2      = 0;
+	eng->ncsi_req.Reserved_3      = 0;
+
+	eng->dat.NCSI_TxByteBUF = (unsigned char *) &eng->dat.NCSI_TxDWBUF[0];
+	eng->dat.NCSI_RxByteBUF = (unsigned char *) &eng->dat.NCSI_RxDWBUF[0];
+
+	eng->run.NCSI_TxDesBase = eng->run.TDES_BASE;//base for read/write
+	Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase + 0x04, 0                        );
+	Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase + 0x08, 0                        );
+	Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase + 0x0C, AT_MEMRW_BUF( DMA_BASE ) );
+
+	eng->run.NCSI_RxDesBase = eng->run.RDES_BASE;//base for read/write
+	NCSI_RxDatBase = AT_MEMRW_BUF( NCSI_RxDMA_BASE );//base of the descriptor
+
+	for (i = 0; i < NCSI_RxDESNum - 1; i++) {
+		Write_Mem_Des_NCSI_DD( ( eng->run.NCSI_RxDesBase        ), 0x00000000     );
+		Write_Mem_Des_NCSI_DD( ( eng->run.NCSI_RxDesBase + 0x04 ), 0x00000000     );
+		Write_Mem_Des_NCSI_DD( ( eng->run.NCSI_RxDesBase + 0x08 ), 0x00000000     );
+		Write_Mem_Des_NCSI_DD( ( eng->run.NCSI_RxDesBase + 0x0C ), NCSI_RxDatBase );
+		eng->run.NCSI_RxDesBase += 16;
+		NCSI_RxDatBase += NCSI_RxDMA_PakSize;
+	}
+	Write_Mem_Des_NCSI_DD( ( eng->run.NCSI_RxDesBase        ), EOR_IniVal     );
+	Write_Mem_Des_NCSI_DD( ( eng->run.NCSI_RxDesBase + 0x04 ), 0x00000000     );
+	Write_Mem_Des_NCSI_DD( ( eng->run.NCSI_RxDesBase + 0x08 ), 0x00000000     );
+//	Write_Mem_Des_NCSI_DD( ( eng->run.NCSI_RxDesBase + 0x0C ), (NCSI_RxDatBase + CPU_BUS_ADDR_SDRAM_OFFSET) ); // 20130730
+	Write_Mem_Des_NCSI_DD( ( eng->run.NCSI_RxDesBase + 0x0C ), NCSI_RxDatBase ); // 20130730
+
+	eng->run.NCSI_RxDesBase = eng->run.RDES_BASE;//base for read/write
+}
+
+//------------------------------------------------------------
+void Calculate_Checksum_NCSI (MAC_ENGINE *eng, unsigned char *buffer_base, int Length) {
+	ULONG      CheckSum = 0;
+	ULONG      Data;
+	ULONG      Data1;
+	int        i;
+
+	// Calculate checksum is from byte 14 of ethernet Haeder and Control packet header
+	// Page 50, NC-SI spec. ver. 1.0.0 form DMTF
+	for (i = 14; i < Length; i += 2 ) {
+		Data      = buffer_base[i];
+		Data1     = buffer_base[i + 1];
+		CheckSum += ((Data << 8) + Data1);
+	}
+	eng->dat.Payload_Checksum_NCSI = SWAP_4B_BEDN(~(CheckSum) + 1); //2's complement
+}
+
+//------------------------------------------------------------
+// return 0: it is PASS
+// return 1: it is FAIL
+//------------------------------------------------------------
+char NCSI_Rx_SLT (MAC_ENGINE *eng) {
+	int        timeout = 0;
+	int        bytesize;
+	int        dwsize;
+	int        i;
+	int        retry   = 0;
+	char       ret     = 1;
+
+	ULONG      NCSI_RxDatBase;
+	ULONG      NCSI_RxDesDat;
+	ULONG      NCSI_RxData;
+	ULONG      NCSI_BufData;
+
+	do {
+		Write_Reg_MAC_DD( eng, 0x1C, 0x00000000 );//Rx Poll
+
+		timeout = 0;
+		do {
+			NCSI_RxDesDat = Read_Mem_Des_NCSI_DD( eng->run.NCSI_RxDesBase );
+			if ( ++timeout > TIME_OUT_NCSI * eng->run.NCSI_RxTimeOutScale ) {
+				PRINTF( FP_LOG, "[RxDes] DesOwn timeout     %08lx\n", NCSI_RxDesDat );
+				return( FindErr( eng, Err_Flag_NCSI_Check_RxOwnTimeOut ) );
+			}
+		} while( HWOwnRx( NCSI_RxDesDat ) );
+
+		if ( NCSI_RxDesDat & Check_ErrMask_ALL ) {
+#ifdef CheckRxErr
+			if ( NCSI_RxDesDat & Check_ErrMask_RxErr ) {
+				PRINTF( FP_LOG, "[RxDes] Error RxErr        %08lx\n", NCSI_RxDesDat );
+				eng->dat.NCSI_RxEr = 1;
+//				FindErr_Des( eng, Des_Flag_RxErr );
+			}
+#endif // End CheckRxErr
+
+#ifdef CheckCRC
+			if ( NCSI_RxDesDat & Check_ErrMask_CRC ) {
+				PRINTF( FP_LOG, "[RxDes] Error CRC          %08lx\n", NCSI_RxDesDat );
+				FindErr_Des( eng, Des_Flag_CRC );
+			}
+#endif // End CheckCRC
+
+#ifdef CheckFTL
+			if ( NCSI_RxDesDat & Check_ErrMask_FTL ) {
+				PRINTF( FP_LOG, "[RxDes] Error FTL          %08lx\n", NCSI_RxDesDat );
+				FindErr_Des( eng, Des_Flag_FTL );
+			}
+#endif // End CheckFTL
+
+#ifdef CheckRunt
+			if ( NCSI_RxDesDat & Check_ErrMask_Runt ) {
+				PRINTF( FP_LOG, "[RxDes] Error Runt         %08lx\n", NCSI_RxDesDat );
+				FindErr_Des( eng, Des_Flag_Runt );
+			}
+#endif // End CheckRunt
+
+#ifdef CheckOddNibble
+			if ( NCSI_RxDesDat & Check_ErrMask_OddNibble ) {
+				PRINTF( FP_LOG, "[RxDes] Odd Nibble         %08lx\n", NCSI_RxDesDat );
+				FindErr_Des( eng, Des_Flag_OddNibble );
+			}
+#endif // End CheckOddNibble
+
+#ifdef CheckRxFIFOFull
+			if ( NCSI_RxDesDat & Check_ErrMask_RxFIFOFull ) {
+				PRINTF( FP_LOG, "[RxDes] Error Rx FIFO Full %08lx\n", NCSI_RxDesDat );
+				FindErr_Des( eng, Des_Flag_RxFIFOFull );
+			}
+#endif // End CheckRxFIFOFull
+		}
+
+		// Get point of RX DMA buffer
+		NCSI_RxDatBase = AT_BUF_MEMRW( Read_Mem_Des_NCSI_DD( eng->run.NCSI_RxDesBase + 0x0C ) );//base for read/write
+		NCSI_RxData    = SWAP_4B_LEDN_NCSI( SWAP_4B_LEDN( Read_Mem_Dat_NCSI_DD( NCSI_RxDatBase + 0x0C ) ) );
+
+		// Get RX valid data in offset 00h of RXDS#0
+#ifdef NCSI_Skip_RxCRCData
+		bytesize  = (NCSI_RxDesDat & 0x3fff) - 4;
+#else
+		bytesize  = (NCSI_RxDesDat & 0x3fff);
+#endif
+		// Fill up to multiple of 4
+		if ( ( bytesize % 4 ) != 0 )
+			dwsize = ( bytesize >> 2 ) + 1;
+		else
+			dwsize = bytesize >> 2;
+
+		if ( eng->arg.GEn_PrintNCSI ) {
+#ifdef NCSI_Skip_RxCRCData
+			PRINTF( FP_LOG ,"----->[Rx] %d bytes(%xh) [Remove CRC data]\n", bytesize, bytesize );
+#else
+			PRINTF( FP_LOG ,"----->[Rx] %d bytes(%xh)\n", bytesize, bytesize );
+#endif
+			for (i = 0; i < dwsize - 1; i++) {
+				NCSI_BufData = SWAP_4B_LEDN_NCSI( Read_Mem_Dat_NCSI_DD( NCSI_RxDatBase + ( i << 2 ) ) );
+				PRINTF( FP_LOG ,"      [Rx]%02d:%08lx %08lx\n", i, NCSI_BufData, SWAP_4B( NCSI_BufData ) );
+			}
+
+			i = ( dwsize - 1 );
+			NCSI_BufData = SWAP_4B_LEDN_NCSI( Read_Mem_Dat_NCSI_DD( NCSI_RxDatBase + ( i << 2 ) ) );
+			switch ( bytesize % 4 ) {
+				case 0  : PRINTF( FP_LOG ,"      [Rx]%02d:%08lx %08lx\n",                          i, NCSI_BufData & SWAP_4B_LEDN_NCSI( 0xffffffff ), SWAP_4B( NCSI_BufData ) & SWAP_4B_BEDN_NCSI( 0xffffffff ) ); break;
+				case 3  : PRINTF( FP_LOG ,"      [Rx]%02d:%08lx %08lx [%08lx %08lx][%08x %08x]\n", i, NCSI_BufData & SWAP_4B_LEDN_NCSI( 0x00ffffff ), SWAP_4B( NCSI_BufData ) & SWAP_4B_BEDN_NCSI( 0x00ffffff ), NCSI_BufData, SWAP_4B( NCSI_BufData ), SWAP_4B_LEDN_NCSI( 0x00ffffff ), SWAP_4B_BEDN_NCSI( 0x00ffffff ) ); break;
+				case 2  : PRINTF( FP_LOG ,"      [Rx]%02d:%08lx %08lx [%08lx %08lx][%08x %08x]\n", i, NCSI_BufData & SWAP_4B_LEDN_NCSI( 0x0000ffff ), SWAP_4B( NCSI_BufData ) & SWAP_4B_BEDN_NCSI( 0x0000ffff ), NCSI_BufData, SWAP_4B( NCSI_BufData ), SWAP_4B_LEDN_NCSI( 0x0000ffff ), SWAP_4B_BEDN_NCSI( 0x0000ffff ) ); break;
+				case 1  : PRINTF( FP_LOG ,"      [Rx]%02d:%08lx %08lx [%08lx %08lx][%08x %08x]\n", i, NCSI_BufData & SWAP_4B_LEDN_NCSI( 0x000000ff ), SWAP_4B( NCSI_BufData ) & SWAP_4B_BEDN_NCSI( 0x000000ff ), NCSI_BufData, SWAP_4B( NCSI_BufData ), SWAP_4B_LEDN_NCSI( 0x000000ff ), SWAP_4B_BEDN_NCSI( 0x000000ff ) ); break;
+				default : PRINTF( FP_LOG ,"      [Rx]%02d:error", i ); break;
+			}
+		}
+
+		// EtherType field of the response packet should be 0x88F8
+//
+		if ( ( NCSI_RxData & 0xffff ) == 0xf888 ) {
+			for (i = 0; i < dwsize; i++)
+				eng->dat.NCSI_RxDWBUF[i] = SWAP_4B_LEDN_NCSI( Read_Mem_Dat_NCSI_DD( NCSI_RxDatBase + ( i << 2 ) ) );
+
+			memcpy ( &eng->ncsi_rsp, eng->dat.NCSI_RxByteBUF, bytesize );
+
+			if ( eng->arg.GEn_PrintNCSI )
+				PRINTF( FP_LOG ,"[Frm-NCSI][Rx IID:%2d]\n", eng->ncsi_rsp.IID );
+
+			if ( ( eng->ncsi_rsp.IID == 0x0 ) && ( eng->ncsi_rsp.Command == 0xff ) ) { // AEN Packet
+				if ( eng->arg.GEn_PrintNCSI )
+					PRINTF( FP_LOG ,"[Frm-NCSI][AEN Packet]Type:%2d\n", SWAP_2B_BEDN( eng->ncsi_rsp.Reason_Code ) & 0xff );
+			}
+			else {
+				ret = 0;
+			}
+		}
+		else {
+			if ( eng->arg.GEn_PrintNCSI )
+				PRINTF( FP_LOG, "[Frm-Skip] Not NCSI Response: [%08lx & %08x = %08lx]!=[%08x]\n", NCSI_RxData, 0xffff, NCSI_RxData & 0xffff, 0xf888 );
+		} // End if ( ( NCSI_RxData & 0xffff ) == 0xf888 )
+
+		if ( HWEOR( NCSI_RxDesDat ) ) {
+			// it is last the descriptor in the receive Ring
+			Write_Mem_Des_NCSI_DD( eng->run.NCSI_RxDesBase     , EOR_IniVal    );
+			eng->run.NCSI_RxDesBase = eng->run.RDES_BASE;//base for read/write
+		}
+		else {
+			Write_Mem_Des_NCSI_DD( eng->run.NCSI_RxDesBase     , 0x00000000    );
+			eng->run.NCSI_RxDesBase += 16;
+		}
+
+		if ( ret == 0 )
+			break;
+		retry++;
+	} while ( retry < NCSI_RxDESNum );
+
+	if ( ( ret == 0 ) && eng->arg.GEn_PrintNCSI ) {
+#ifdef Print_DetailFrame
+		ncsi_respdump ( eng, &eng->ncsi_rsp );
+#else
+		PRINTF( FP_LOG, "[NCSI-Respond] ETyp:%04x MC_ID:%02x HeadVer:%02x IID:%02x Comm:%02x ChlID:%02x PayLen:%04x ResCd:%02x ReaCd:%02x\n",
+		SWAP_2B_BEDN( eng->ncsi_rsp.EtherType ),
+		eng->ncsi_rsp.MC_ID,
+		eng->ncsi_rsp.Header_Revision,
+		eng->ncsi_rsp.IID,
+		eng->ncsi_rsp.Command,
+		eng->ncsi_rsp.ChID,
+		SWAP_2B_BEDN( eng->ncsi_rsp.Payload_Length ),
+		SWAP_2B_BEDN( eng->ncsi_rsp.Response_Code ),
+		SWAP_2B_BEDN( eng->ncsi_rsp.Reason_Code ));
+#endif
+
+		NCSI_PrintCommandType2File( eng, eng->ncsi_rsp.Command, eng->ncsi_rsp.IID );
+	}
+
+	return( ret );
+} // End char NCSI_Rx_SLT (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+char NCSI_Tx (MAC_ENGINE *eng, unsigned char command, unsigned char allid, unsigned short length) {
+	int        bytesize;
+	int        dwsize;
+	int        i;
+	int        timeout = 0;
+	ULONG      NCSI_TxDesDat;
+
+	eng->ncsi_req.IID++;
+	eng->ncsi_req.Command        = command;
+	eng->ncsi_req.ChID           = allid;
+	eng->ncsi_req.Payload_Length = SWAP_2B_BEDN( length );
+
+	memcpy (  eng->dat.NCSI_TxByteBUF               , &eng->ncsi_req                 , 30     );
+	memcpy ( (eng->dat.NCSI_TxByteBUF + 30         ), &eng->dat.NCSI_Payload_Data    , length );
+	Calculate_Checksum_NCSI( eng, eng->dat.NCSI_TxByteBUF, 30 + length );
+	memcpy ( (eng->dat.NCSI_TxByteBUF + 30 + length), &eng->dat.Payload_Checksum_NCSI, 4      );
+
+	// Header of NC-SI command format is 34 bytes. page 58, NC-SI spec. ver 1.0.0 from DMTF
+	// The minimum size of a NC-SI package is 64 bytes.
+	bytesize = 34 + length;
+	if ( bytesize < 60 ) {
+		memset ( eng->dat.NCSI_TxByteBUF + bytesize, 0, 60 - bytesize );
+		bytesize = 60;
+	}
+
+	// Fill up to multiple of 4
+	//    dwsize = (bytesize + 3) >> 2;
+	if ( ( bytesize % 4 ) != 0 )
+		dwsize = ( bytesize >> 2 ) + 1;
+	else
+		dwsize = bytesize >> 2;
+
+	if ( eng->arg.GEn_PrintNCSI ) {
+		if ( bytesize % 4 )
+			memset ( eng->dat.NCSI_TxByteBUF + bytesize, 0, (dwsize << 2) - bytesize );
+
+		PRINTF( FP_LOG ,"----->[Tx] %d bytes(%xh)\n", bytesize, bytesize );
+		for ( i = 0; i < dwsize-1; i++ )
+			PRINTF( FP_LOG, "      [Tx]%02d:%08lx %08lx\n", i, eng->dat.NCSI_TxDWBUF[i], SWAP_4B( eng->dat.NCSI_TxDWBUF[i] ) );
+
+		i = dwsize - 1;
+		switch ( bytesize % 4 ) {
+			case 0  : PRINTF( FP_LOG ,"      [Tx]%02d:%08lx %08lx\n",                          i, eng->dat.NCSI_TxDWBUF[i] & SWAP_4B_LEDN_NCSI( 0xffffffff ), SWAP_4B( eng->dat.NCSI_TxDWBUF[i] ) & SWAP_4B_BEDN_NCSI( 0xffffffff ) ); break;
+			case 3  : PRINTF( FP_LOG ,"      [Tx]%02d:%08lx %08lx [%08lx %08lx][%08x %08x]\n", i, eng->dat.NCSI_TxDWBUF[i] & SWAP_4B_LEDN_NCSI( 0x00ffffff ), SWAP_4B( eng->dat.NCSI_TxDWBUF[i] ) & SWAP_4B_BEDN_NCSI( 0x00ffffff ), eng->dat.NCSI_TxDWBUF[i], SWAP_4B( eng->dat.NCSI_TxDWBUF[i] ), SWAP_4B_LEDN_NCSI( 0x00ffffff ), SWAP_4B_BEDN_NCSI( 0x00ffffff ) ); break;
+			case 2  : PRINTF( FP_LOG ,"      [Tx]%02d:%08lx %08lx [%08lx %08lx][%08x %08x]\n", i, eng->dat.NCSI_TxDWBUF[i] & SWAP_4B_LEDN_NCSI( 0x0000ffff ), SWAP_4B( eng->dat.NCSI_TxDWBUF[i] ) & SWAP_4B_BEDN_NCSI( 0x0000ffff ), eng->dat.NCSI_TxDWBUF[i], SWAP_4B( eng->dat.NCSI_TxDWBUF[i] ), SWAP_4B_LEDN_NCSI( 0x0000ffff ), SWAP_4B_BEDN_NCSI( 0x0000ffff ) ); break;
+			case 1  : PRINTF( FP_LOG ,"      [Tx]%02d:%08lx %08lx [%08lx %08lx][%08x %08x]\n", i, eng->dat.NCSI_TxDWBUF[i] & SWAP_4B_LEDN_NCSI( 0x000000ff ), SWAP_4B( eng->dat.NCSI_TxDWBUF[i] ) & SWAP_4B_BEDN_NCSI( 0x000000ff ), eng->dat.NCSI_TxDWBUF[i], SWAP_4B( eng->dat.NCSI_TxDWBUF[i] ), SWAP_4B_LEDN_NCSI( 0x000000ff ), SWAP_4B_BEDN_NCSI( 0x000000ff ) ); break;
+			default : PRINTF( FP_LOG ,"      [Tx]%02d:error", i ); break;
+		}
+		PRINTF( FP_LOG ,"[Frm-NCSI][Tx IID:%2d]\n", eng->ncsi_req.IID );
+	}
+
+	// Copy data to DMA buffer
+	for ( i = 0; i < dwsize; i++ )
+		Write_Mem_Dat_NCSI_DD( DMA_BASE + ( i << 2 ), SWAP_4B_LEDN_NCSI( eng->dat.NCSI_TxDWBUF[i] ) );
+
+	// Setting one TX descriptor
+//	Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase + 0x04, 0                        );
+//	Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase + 0x08, 0                        );
+//	Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase + 0x0C, AT_MEMRW_BUF( DMA_BASE ) );
+	Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase       , 0xf0008000 + bytesize );
+
+//	Write_Reg_MAC_DD( eng, 0x40, eng->reg.MAC_040 ); // 20170505
+
+	// Fire
+	Write_Reg_MAC_DD( eng, 0x18, 0x00000000 );//Tx Poll
+
+	do {
+		NCSI_TxDesDat = Read_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase );
+		if ( ++timeout > TIME_OUT_NCSI ) {
+			PRINTF( FP_LOG, "[TxDes] DesOwn timeout     %08lX\n", NCSI_TxDesDat );
+			return( FindErr( eng, Err_Flag_NCSI_Check_TxOwnTimeOut  ));
+		}
+	} while ( HWOwnTx( NCSI_TxDesDat ) );
+
+	if ( eng->arg.GEn_PrintNCSI ) {
+#ifdef Print_DetailFrame
+		ncsi_reqdump ( eng, &eng->ncsi_req );
+#else
+		PRINTF( FP_LOG, "[NCSI-Request] ETyp:%04x MC_ID:%02x HeadVer:%02x IID:%02x Comm:%02x ChlID:%02x PayLen:%04x\n",
+		SWAP_2B_BEDN( eng->ncsi_req.EtherType ),
+		eng->ncsi_req.MC_ID,
+		eng->ncsi_req.Header_Revision,
+		eng->ncsi_req.IID,
+		eng->ncsi_req.Command,
+		eng->ncsi_req.ChID,
+		SWAP_2B_BEDN( eng->ncsi_req.Payload_Length ) );
+#endif
+
+		NCSI_PrintCommandType2File( eng, eng->ncsi_req.Command, eng->ncsi_req.IID );
+	}
+#ifdef Print_PackageName
+	NCSI_PrintCommandType( eng, eng->ncsi_req.Command, eng->ncsi_req.IID );
+#endif
+
+	return(0);
+} // End char NCSI_Tx (MAC_ENGINE *eng, unsigned char command, unsigned char allid, unsigned short length)
+
+//------------------------------------------------------------
+char NCSI_ARP (MAC_ENGINE *eng) {
+	int        i;
+	int        timeout = 0;
+	ULONG      NCSI_TxDesDat;
+
+	if ( eng->arg.GEn_PrintNCSI )
+		PRINTF( FP_LOG ,"----->[ARP] 60 bytes x%ld\n", eng->arg.GARPNumCnt );
+
+	for (i = 0; i < 15; i++) {
+		if ( eng->arg.GEn_PrintNCSI )
+			PRINTF( FP_LOG, "      [Tx%02d] %08lx %08lx\n", i, eng->dat.ARP_data[i], SWAP_4B( eng->dat.ARP_data[i] ) );
+
+		Write_Mem_Dat_NCSI_DD( DMA_BASE + ( i << 2 ), eng->dat.ARP_data[i] );
+	}
+
+//	Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase + 0x04, 0                        );
+//	Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase + 0x08, 0                        );
+//	Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase + 0x0C, AT_MEMRW_BUF( DMA_BASE ) );
+	for (i = 0; i < eng->arg.GARPNumCnt; i++) {
+		Write_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase      , 0xf0008000 + 60);
+		Write_Reg_MAC_DD( eng, 0x18, 0x00000000 );//Tx Poll
+
+		do {
+			NCSI_TxDesDat = Read_Mem_Des_NCSI_DD( eng->run.NCSI_TxDesBase );
+			if ( ++timeout > TIME_OUT_NCSI ) {
+				PRINTF( FP_LOG, "[TxDes-ARP] DesOwn timeout %08lx\n", NCSI_TxDesDat );
+				return( FindErr( eng, Err_Flag_NCSI_Check_ARPOwnTimeOut ) );
+			}
+		} while ( HWOwnTx( NCSI_TxDesDat ) );
+	}
+	return(0);
+} // End char NCSI_ARP (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+char NCSI_SentWaitPacket (MAC_ENGINE *eng, unsigned char command, unsigned char allid, unsigned short length) {
+	int        Retry = 0;
+
+	do {
+		if ( NCSI_Tx( eng, command, allid, length ) )
+			return( 1 );
+
+#ifdef NCSI_EnableDelay_EachPackage
+		DELAY( Delay_EachPackage );
+#endif
+		if ( NCSI_Rx_SLT( eng ) )
+			return( 2 );
+
+		if (    ( eng->ncsi_rsp.IID           != eng->ncsi_req.IID                        )
+		     || ( eng->ncsi_rsp.Command       != ( command | 0x80 )                       )
+		     || ( eng->ncsi_rsp.Response_Code != SWAP_2B_BEDN( COMMAND_COMPLETED ) ) ) {
+			if ( eng->arg.GEn_PrintNCSI ) {
+				PRINTF( FP_LOG, "Retry: Command = %x, Response_Code = %x", eng->ncsi_req.Command, SWAP_2B_BEDN( eng->ncsi_rsp.Response_Code ) );
+				switch ( SWAP_2B_BEDN( eng->ncsi_rsp.Response_Code ) ) {
+					case COMMAND_COMPLETED  	: PRINTF( FP_LOG, "(completed  )\n" ); break;
+					case COMMAND_FAILED     	: PRINTF( FP_LOG, "(failed     )\n" ); break;
+					case COMMAND_UNAVAILABLE	: PRINTF( FP_LOG, "(unavailable)\n" ); break;
+					case COMMAND_UNSUPPORTED	: PRINTF( FP_LOG, "(unsupported)\n" ); break;
+					default                 	: PRINTF( FP_LOG, "(-----------)\n" ); break;
+				}
+			}
+			Retry++;
+		}
+		else {
+			return( 0 );
+		}
+	} while (Retry <= SENT_RETRY_COUNT);
+
+	return( 3 );
+} // End char NCSI_SentWaitPacket (unsigned char command, unsigned char id, unsigned short length)
+
+//------------------------------------------------------------
+char Clear_Initial_State_SLT (MAC_ENGINE *eng) {//Command:0x00
+	char       return_value;
+
+	eng->flg.Bak_Err_Flag  = eng->flg.Err_Flag;
+	eng->flg.Bak_NCSI_Flag = eng->flg.NCSI_Flag;
+
+	return_value = NCSI_SentWaitPacket( eng, CLEAR_INITIAL_STATE, eng->ncsi_cap.All_ID, 0 );//Internal Channel ID = 0
+
+	eng->flg.Err_Flag  = eng->flg.Bak_Err_Flag;
+	eng->flg.NCSI_Flag = eng->flg.Bak_NCSI_Flag;
+	return( return_value );//Internal Channel ID = 0
+}
+
+//------------------------------------------------------------
+char Select_Package_SLT (MAC_ENGINE *eng, char skipflag) {//Command:0x01
+	char       return_value;
+
+	if ( skipflag ) {
+		eng->flg.Bak_Err_Flag  = eng->flg.Err_Flag;
+		eng->flg.Bak_NCSI_Flag = eng->flg.NCSI_Flag;
+	}
+
+	memset ((void *)eng->dat.NCSI_Payload_Data, 0, 4);
+	eng->dat.NCSI_Payload_Data[ 3 ] = 1; //Arbitration Disable
+	return_value = NCSI_SentWaitPacket( eng, SELECT_PACKAGE, ( eng->ncsi_cap.Package_ID << 5 ) + 0x1F, 4 );//Internal Channel ID = 0x1F, 0x1F means all channel
+	if ( return_value )
+		FindErr_NCSI( eng, NCSI_Flag_Select_Package );
+
+	if ( skipflag ) {
+		eng->flg.Err_Flag  = eng->flg.Bak_Err_Flag;
+		eng->flg.NCSI_Flag = eng->flg.Bak_NCSI_Flag;
+	}
+	return( return_value );
+}
+
+//------------------------------------------------------------
+void Select_Active_Package_SLT (MAC_ENGINE *eng) {//Command:0x01
+	memset ((void *)eng->dat.NCSI_Payload_Data, 0, 4);
+	eng->dat.NCSI_Payload_Data[ 3 ] = 1; //Arbitration Disable
+
+	if ( NCSI_SentWaitPacket( eng, SELECT_PACKAGE, ( eng->ncsi_cap.Package_ID << 5 ) + 0x1F, 4 ) ) //Internal Channel ID = 0x1F, 0x1F means all channel
+		FindErr_NCSI( eng, NCSI_Flag_Select_Active_Package );
+}
+
+//------------------------------------------------------------
+void DeSelect_Package_SLT (MAC_ENGINE *eng) {//Command:0x02
+	if ( NCSI_SentWaitPacket( eng, DESELECT_PACKAGE, ( eng->ncsi_cap.Package_ID << 5 ) + 0x1F, 0 ) ) //Internal Channel ID = 0x1F, 0x1F means all channel
+		FindErr_NCSI( eng, NCSI_Flag_Deselect_Package );
+
+#ifdef NCSI_EnableDelay_DeSelectPackage
+	DELAY( Delay_DeSelectPackage );
+#endif
+}
+
+//------------------------------------------------------------
+void Enable_Channel_SLT (MAC_ENGINE *eng) {//Command:0x03
+	if ( NCSI_SentWaitPacket( eng, ENABLE_CHANNEL, eng->ncsi_cap.All_ID, 0 ) )
+		FindErr_NCSI( eng, NCSI_Flag_Enable_Channel );
+}
+
+//------------------------------------------------------------
+void Disable_Channel_SLT (MAC_ENGINE *eng, char skipflag) {//Command:0x04
+	if ( skipflag ) {
+		eng->flg.Bak_Err_Flag  = eng->flg.Err_Flag;
+		eng->flg.Bak_NCSI_Flag = eng->flg.NCSI_Flag;
+	}
+
+	memset ((void *)eng->dat.NCSI_Payload_Data, 0, 4);
+	eng->dat.NCSI_Payload_Data[ 3 ] = 0x1; //ALD
+	if ( NCSI_SentWaitPacket( eng, DISABLE_CHANNEL, eng->ncsi_cap.All_ID, 4 ) )
+		FindErr_NCSI( eng, NCSI_Flag_Disable_Channel );
+
+	if ( skipflag ) {
+		eng->flg.Err_Flag  = eng->flg.Bak_Err_Flag;
+		eng->flg.NCSI_Flag = eng->flg.Bak_NCSI_Flag;
+	}
+}
+
+//------------------------------------------------------------
+void Enable_Network_TX_SLT (MAC_ENGINE *eng) {//Command:0x06
+	if ( NCSI_SentWaitPacket( eng, ENABLE_CHANNEL_NETWORK_TX, eng->ncsi_cap.All_ID, 0 ) )
+		FindErr_NCSI( eng, NCSI_Flag_Enable_Network_TX );
+}
+
+//------------------------------------------------------------
+void Disable_Network_TX_SLT (MAC_ENGINE *eng) {//Command:0x07
+	if ( NCSI_SentWaitPacket( eng, DISABLE_CHANNEL_NETWORK_TX, eng->ncsi_cap.All_ID, 0 ) )
+		FindErr_NCSI( eng, NCSI_Flag_Disable_Network_TX );
+}
+
+//------------------------------------------------------------
+void Set_Link_SLT (MAC_ENGINE *eng) {//Command:0x09
+	memset ((void *)eng->dat.NCSI_Payload_Data, 0, 8);
+	eng->dat.NCSI_Payload_Data[ 2 ] = 0x02; //full duplex
+//	eng->dat.NCSI_Payload_Data[ 3 ] = 0x04; //100M, auto-disable
+	eng->dat.NCSI_Payload_Data[ 3 ] = 0x05; //100M, auto-enable
+
+	if ( NCSI_SentWaitPacket( eng, SET_LINK, eng->ncsi_cap.All_ID, 8 ) )
+		FindErr_NCSI( eng, NCSI_Flag_Set_Link );
+}
+
+//------------------------------------------------------------
+char Get_Link_Status_SLT (MAC_ENGINE *eng) {//Command:0x0a
+	if ( NCSI_SentWaitPacket( eng, GET_LINK_STATUS, eng->ncsi_cap.All_ID, 0 ) )
+		return(0);
+	else {
+		if ( eng->ncsi_rsp.Payload_Data[ 3 ] & 0x20 ) {
+			if ( eng->ncsi_rsp.Payload_Data[ 3 ] & 0x40 ) {
+				if ( eng->ncsi_rsp.Payload_Data[ 3 ] & 0x01 )
+					return(1); //Link Up or Not
+				else
+					return(0);
+			}
+			else
+				return(0); //Auto Negotiate did not finish
+		}
+		else {
+			if ( eng->ncsi_rsp.Payload_Data[ 3 ] & 0x01 )
+				return(1); //Link Up or Not
+			else
+				return(0);
+		}
+	}
+} // End char Get_Link_Status_SLT (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void Enable_Set_MAC_Address_SLT (MAC_ENGINE *eng) {//Command:0x0e
+
+#if !defined(MELLANOX_CONNECTX_4)
+	int        i;
+
+	for ( i = 0; i < 6; i++ )
+		eng->dat.NCSI_Payload_Data[ i ] = eng->ncsi_req.SA[ i ];
+	eng->dat.NCSI_Payload_Data[ 6 ] = 1; //MAC Address Num = 1 --> address filter 1, fixed in sample code
+
+	if ( eng->ncsi_req.SA[ 0 ] & 0x1 )
+		eng->dat.NCSI_Payload_Data[ 7 ] = MULTICAST + ENABLE_MAC_ADDRESS_FILTER; //AT + E
+	else
+		eng->dat.NCSI_Payload_Data[ 7 ] = UNICAST   + ENABLE_MAC_ADDRESS_FILTER; //AT + E
+#else
+	eng->dat.NCSI_Payload_Data[ 0 ] = 0xC0;
+	eng->dat.NCSI_Payload_Data[ 1 ] = 0xC2;
+	eng->dat.NCSI_Payload_Data[ 2 ] = 0xC4;
+	eng->dat.NCSI_Payload_Data[ 3 ] = 0xC8;
+	eng->dat.NCSI_Payload_Data[ 4 ] = 0xCC;
+	eng->dat.NCSI_Payload_Data[ 5 ] = 0xB0;
+	eng->dat.NCSI_Payload_Data[ 6 ] = 1; //MAC Address Num = 1 --> address filter 1, fixed in sample code
+
+	eng->dat.NCSI_Payload_Data[ 7 ] = UNICAST   + ENABLE_MAC_ADDRESS_FILTER; //AT + E
+#endif
+
+	if ( NCSI_SentWaitPacket( eng, SET_MAC_ADDRESS, eng->ncsi_cap.All_ID, 8 ) )
+		FindErr_NCSI( eng, NCSI_Flag_Enable_Set_MAC_Address );
+}
+
+//------------------------------------------------------------
+void Enable_Broadcast_Filter_SLT (MAC_ENGINE *eng) {//Command:0x10
+	memset ((void *)eng->dat.NCSI_Payload_Data, 0, 4);
+	eng->dat.NCSI_Payload_Data[ 3 ] = 0xF; //ARP, DHCP, NetBIOS
+
+	if ( NCSI_SentWaitPacket( eng, ENABLE_BROADCAST_FILTERING, eng->ncsi_cap.All_ID, 4 ) )
+		FindErr_NCSI( eng, NCSI_Flag_Enable_Broadcast_Filter );
+}
+
+//------------------------------------------------------------
+void Get_Version_ID_SLT (MAC_ENGINE *eng) {//Command:0x15
+	if ( NCSI_SentWaitPacket( eng, GET_VERSION_ID, eng->ncsi_cap.All_ID, 0 ) )
+		FindErr_NCSI( eng, NCSI_Flag_Get_Version_ID );
+	else {
+#ifdef Print_Version_ID
+		printf("NCSI Version        : %02x %02x %02x %02x\n", eng->ncsi_rsp.Payload_Data[  0 ], eng->ncsi_rsp.Payload_Data[  1 ], eng->ncsi_rsp.Payload_Data[  2 ], eng->ncsi_rsp.Payload_Data[  3 ]);
+		printf("NCSI Version        : %02x %02x %02x %02x\n", eng->ncsi_rsp.Payload_Data[  4 ], eng->ncsi_rsp.Payload_Data[  5 ], eng->ncsi_rsp.Payload_Data[  6 ], eng->ncsi_rsp.Payload_Data[  7 ]);
+		printf("Firmware Name String: %02x %02x %02x %02x\n", eng->ncsi_rsp.Payload_Data[  8 ], eng->ncsi_rsp.Payload_Data[  9 ], eng->ncsi_rsp.Payload_Data[ 10 ], eng->ncsi_rsp.Payload_Data[ 11 ]);
+		printf("Firmware Name String: %02x %02x %02x %02x\n", eng->ncsi_rsp.Payload_Data[ 12 ], eng->ncsi_rsp.Payload_Data[ 13 ], eng->ncsi_rsp.Payload_Data[ 14 ], eng->ncsi_rsp.Payload_Data[ 15 ]);
+		printf("Firmware Name String: %02x %02x %02x %02x\n", eng->ncsi_rsp.Payload_Data[ 16 ], eng->ncsi_rsp.Payload_Data[ 17 ], eng->ncsi_rsp.Payload_Data[ 18 ], eng->ncsi_rsp.Payload_Data[ 19 ]);
+		printf("Firmware Version    : %02x %02x %02x %02x\n", eng->ncsi_rsp.Payload_Data[ 20 ], eng->ncsi_rsp.Payload_Data[ 21 ], eng->ncsi_rsp.Payload_Data[ 22 ], eng->ncsi_rsp.Payload_Data[ 23 ]);
+		printf("PCI DID/VID         : %02x %02x/%02x %02x\n", eng->ncsi_rsp.Payload_Data[ 24 ], eng->ncsi_rsp.Payload_Data[ 25 ], eng->ncsi_rsp.Payload_Data[ 26 ], eng->ncsi_rsp.Payload_Data[ 27 ]);
+		printf("PCI SSID/SVID       : %02x %02x/%02x %02x\n", eng->ncsi_rsp.Payload_Data[ 28 ], eng->ncsi_rsp.Payload_Data[ 29 ], eng->ncsi_rsp.Payload_Data[ 30 ], eng->ncsi_rsp.Payload_Data[ 31 ]);
+		printf("Manufacturer ID     : %02x %02x %02x %02x\n", eng->ncsi_rsp.Payload_Data[ 32 ], eng->ncsi_rsp.Payload_Data[ 33 ], eng->ncsi_rsp.Payload_Data[ 34 ], eng->ncsi_rsp.Payload_Data[ 35 ]);
+		printf("Checksum            : %02x %02x %02x %02x\n", eng->ncsi_rsp.Payload_Data[ 36 ], eng->ncsi_rsp.Payload_Data[ 37 ], eng->ncsi_rsp.Payload_Data[ 38 ], eng->ncsi_rsp.Payload_Data[ 39 ]);
+#endif
+		eng->ncsi_cap.PCI_DID_VID    = (eng->ncsi_rsp.Payload_Data[ 24 ]<<24)
+		                             | (eng->ncsi_rsp.Payload_Data[ 25 ]<<16)
+		                             | (eng->ncsi_rsp.Payload_Data[ 26 ]<< 8)
+		                             | (eng->ncsi_rsp.Payload_Data[ 27 ]    );
+		eng->ncsi_cap.ManufacturerID = (eng->ncsi_rsp.Payload_Data[ 32 ]<<24)
+		                             | (eng->ncsi_rsp.Payload_Data[ 33 ]<<16)
+		                             | (eng->ncsi_rsp.Payload_Data[ 34 ]<< 8)
+		                             | (eng->ncsi_rsp.Payload_Data[ 35 ]    );
+	}
+} // End void Get_Version_ID_SLT (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void Get_Capabilities_SLT (MAC_ENGINE *eng) {//Command:0x16
+	if ( NCSI_SentWaitPacket( eng, GET_CAPABILITIES, eng->ncsi_cap.All_ID, 0 ) )
+		FindErr_NCSI( eng, NCSI_Flag_Get_Capabilities );
+	else {
+//		eng->ncsi_cap.Capabilities_Flags                   = (eng->ncsi_rsp.Payload_Data[  0 ]<<24)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[  1 ]<<16)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[  2 ]<< 8)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[  3 ]    );
+//		eng->ncsi_cap.Broadcast_Packet_Filter_Capabilities = (eng->ncsi_rsp.Payload_Data[  4 ]<<24)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[  5 ]<<16)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[  6 ]<< 8)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[  7 ]    );
+//		eng->ncsi_cap.Multicast_Packet_Filter_Capabilities = (eng->ncsi_rsp.Payload_Data[  8 ]<<24)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[  9 ]<<16)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[ 10 ]<< 8)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[ 11 ]    );
+//		eng->ncsi_cap.Buffering_Capabilities               = (eng->ncsi_rsp.Payload_Data[ 12 ]<<24)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[ 13 ]<<16)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[ 14 ]<< 8)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[ 15 ]    );
+//		eng->ncsi_cap.AEN_Control_Support                  = (eng->ncsi_rsp.Payload_Data[ 16 ]<<24)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[ 17 ]<<16)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[ 18 ]<< 8)
+//		                                                   | (eng->ncsi_rsp.Payload_Data[ 19 ]    );
+//		eng->ncsi_cap.VLAN_Filter_Count                    =  eng->ncsi_rsp.Payload_Data[ 20 ];
+		eng->ncsi_cap.Mixed_Filter_Count                   =  eng->ncsi_rsp.Payload_Data[ 21 ];
+//		eng->ncsi_cap.Multicast_Filter_Count               =  eng->ncsi_rsp.Payload_Data[ 22 ];
+		eng->ncsi_cap.Unicast_Filter_Count                 =  eng->ncsi_rsp.Payload_Data[ 23 ];
+//		eng->ncsi_cap.VLAN_Mode_Support                    =  eng->ncsi_rsp.Payload_Data[ 26 ]
+		eng->ncsi_cap.Channel_Count                        =  eng->ncsi_rsp.Payload_Data[ 27 ];
+	}
+}
+
+//------------------------------------------------------------
+void Get_Controller_Packet_Statistics_SLT (MAC_ENGINE *eng) {//Command:0x18
+	if ( NCSI_SentWaitPacket( eng, GET_CONTROLLER_PACKET_STATISTICS, eng->ncsi_cap.All_ID, 0 ) )
+		FindErr_NCSI( eng, NCSI_Flag_Get_Controller_Packet_Statistics );
+}
+
+//------------------------------------------------------------
+char phy_ncsi (MAC_ENGINE *eng) {
+	ULONG      pkg_idx;
+	ULONG      chl_idx;
+	ULONG      select_flag[ MAX_PACKAGE_NUM ];
+	ULONG      Re_Send;
+	ULONG      Link_Status;
+
+	eng->dat.NCSI_RxEr  = 0;
+	eng->dat.number_chl = 0;
+	eng->dat.number_pak = 0;
+	eng->ncsi_cap.Package_ID = 0;
+	eng->ncsi_cap.Channel_ID = 0x1F;
+	eng->ncsi_cap.All_ID     = 0x1F;
+	PRINTF( FP_LOG, "\n\n======> Start:\n" );
+
+	NCSI_Struct_Initialize_SLT( eng );
+
+#ifdef NCSI_Skip_Phase1_DeSelectPackage
+#else
+	//NCSI Start
+	//Disable Channel then DeSelect Package
+	for (pkg_idx = 0; pkg_idx < MAX_PACKAGE_NUM; pkg_idx++) {
+		eng->ncsi_cap.Package_ID = pkg_idx;
+		eng->ncsi_cap.Channel_ID = 0x1F;
+		eng->ncsi_cap.All_ID     = ( eng->ncsi_cap.Package_ID << 5) + eng->ncsi_cap.Channel_ID;
+
+		select_flag[ pkg_idx ] = Select_Package_SLT ( eng, 1 ); //skipflag// Command:0x01
+
+		if ( select_flag[ pkg_idx ] == 0 ) {
+			if ( !eng->run.IO_MrgChk ) {
+				printf("----Find Package ID: %d\n", eng->ncsi_cap.Package_ID);
+				PRINTF(FP_LOG, "----Find Package ID: %d\n", eng->ncsi_cap.Package_ID );
+			}
+			for ( chl_idx = 0; chl_idx < MAX_CHANNEL_NUM; chl_idx++ ) {
+				eng->ncsi_cap.Channel_ID = chl_idx;
+				eng->ncsi_cap.All_ID     = ( eng->ncsi_cap.Package_ID << 5) + eng->ncsi_cap.Channel_ID;
+
+				Disable_Channel_SLT( eng, 1 );//skipflag // Command: 0x04
+			}
+  #ifdef NCSI_Skip_DeSelectPackage
+  #else
+			DeSelect_Package_SLT ( eng ); // Command:0x02
+  #endif
+		} else {
+			if ( !eng->run.IO_MrgChk ) {
+				printf("----Absence of Package ID: %ld\n", pkg_idx);
+				PRINTF( FP_LOG, "----Absence of Package ID: %ld\n", pkg_idx );
+			}
+		} // End if ( select_flag[ pkg_idx ] == 0 )
+	} // End for (pkg_idx = 0; pkg_idx < MAX_PACKAGE_NUM; pkg_idx++)
+#endif
+
+	//Select Package
+	for ( pkg_idx = 0; pkg_idx < MAX_PACKAGE_NUM; pkg_idx++ ) {
+		eng->ncsi_cap.Package_ID = pkg_idx;
+		eng->ncsi_cap.Channel_ID = 0x1F;
+		eng->ncsi_cap.All_ID     = ( eng->ncsi_cap.Package_ID << 5) + eng->ncsi_cap.Channel_ID;
+
+#ifdef NCSI_Skip_Phase1_DeSelectPackage
+		select_flag[ pkg_idx ] = Select_Package_SLT ( eng, 1 ); //skipflag//Command:0x01
+#endif
+
+		if ( select_flag[ pkg_idx ] == 0 ) {
+			//eng->run.NCSI_RxTimeOutScale = 1000;
+			eng->run.NCSI_RxTimeOutScale = 10;
+
+#ifdef NCSI_Skip_Phase1_DeSelectPackage
+#else
+			Select_Package_SLT ( eng, 0 );//Command:0x01
+#endif
+			eng->dat.number_pak++;
+			if ( !eng->run.IO_MrgChk ) {
+				printf("====Find Package ID: %d\n", eng->ncsi_cap.Package_ID);
+				PRINTF(FP_LOG, "====Find Package ID: %d\n", eng->ncsi_cap.Package_ID );
+			}
+
+			// Scan all channel in the package
+			for ( chl_idx = 0; chl_idx < MAX_CHANNEL_NUM; chl_idx++ ) {
+				eng->ncsi_cap.Channel_ID = chl_idx;
+				eng->ncsi_cap.All_ID     = ( eng->ncsi_cap.Package_ID << 5) + eng->ncsi_cap.Channel_ID;
+
+				if ( Clear_Initial_State_SLT( eng ) == 0 ) { //Command:0x00
+					eng->dat.number_chl++;
+					if ( !eng->run.IO_MrgChk ) {
+						printf("--------Find Channel ID: %d\n", eng->ncsi_cap.Channel_ID);
+						PRINTF( FP_LOG, "--------Find Channel ID: %d\n", eng->ncsi_cap.Channel_ID );
+					}
+
+					// Get Version and Capabilities
+					Get_Version_ID_SLT( eng );          //Command:0x15
+					Get_Capabilities_SLT( eng );        //Command:0x16
+					Select_Active_Package_SLT( eng );   //Command:0x01
+					Enable_Set_MAC_Address_SLT( eng );  //Command:0x0e
+					Enable_Broadcast_Filter_SLT( eng ); //Command:0x10
+
+					// Enable TX
+					Enable_Network_TX_SLT( eng );       //Command:0x06
+
+					// Enable Channel
+					Enable_Channel_SLT( eng );          //Command:0x03
+
+					// Get Link Status
+					Re_Send = 0;
+					do {
+#ifdef NCSI_EnableDelay_GetLinkStatus
+						if ( Re_Send )
+							DELAY( Delay_GetLinkStatus );
+#endif
+
+						Link_Status = Get_Link_Status_SLT( eng );//Command:0x0a
+						if ( Link_Status == LINK_UP ) {
+							if ( eng->arg.GARPNumCnt )
+								NCSI_ARP ( eng );
+							break;
+						} // End if ( Link_Status == LINK_UP )
+					} while ( Re_Send++ <= 2 );
+
+					if ( !eng->run.IO_MrgChk ) {
+						if ( Link_Status == LINK_UP ) {
+							printf("        This Channel is LINK_UP (MFC:%d, UFC:%d, CC:%d)\n", eng->ncsi_cap.Mixed_Filter_Count, eng->ncsi_cap.Unicast_Filter_Count, eng->ncsi_cap.Channel_Count);
+							PRINTF( FP_LOG, "        This Channel is LINK_UP (MFC:%d, UFC:%d, CC:%d)\n", eng->ncsi_cap.Mixed_Filter_Count, eng->ncsi_cap.Unicast_Filter_Count, eng->ncsi_cap.Channel_Count);
+						}
+						else {
+							printf("        This Channel is LINK_DOWN (MFC:%d, UFC:%d, CC:%d)\n", eng->ncsi_cap.Mixed_Filter_Count, eng->ncsi_cap.Unicast_Filter_Count, eng->ncsi_cap.Channel_Count);
+							PRINTF( FP_LOG, "        This Channel is LINK_DOWN (MFC:%d, UFC:%d, CC:%d)\n", eng->ncsi_cap.Mixed_Filter_Count, eng->ncsi_cap.Unicast_Filter_Count, eng->ncsi_cap.Channel_Count);
+						}
+					}
+
+#ifdef NCSI_Skip_DiSChannel
+#else
+					if ( eng->run.TM_NCSI_DiSChannel ) {
+						// Disable TX
+						Disable_Network_TX_SLT( eng ); //Command:0x07
+						// Disable Channel
+						Disable_Channel_SLT( eng, 0 );    //Command:0x04
+					}
+#endif
+				} // End if ( Clear_Initial_State_SLT( eng, chl_idx ) == 0 )
+			} // End for ( chl_idx = 0; chl_idx < MAX_CHANNEL_NUM; chl_idx++ )
+
+#ifdef NCSI_Skip_DeSelectPackage
+#else
+			DeSelect_Package_SLT ( eng );//Command:0x02
+#endif
+			eng->run.NCSI_RxTimeOutScale = 1;
+		}
+		else {
+			if ( !eng->run.IO_MrgChk ) {
+				printf("====Absence of Package ID: %ld\n", pkg_idx);
+				PRINTF( FP_LOG, "====Absence of Package ID: %ld\n", pkg_idx );
+			}
+		} // End if ( select_flag[pkg_idx] == 0 )
+	} // End for ( pkg_idx = 0; pkg_idx < MAX_PACKAGE_NUM; pkg_idx++ )
+
+	if ( eng->dat.number_pak == 0                       ) FindErr( eng, Err_Flag_NCSI_No_PHY      );
+	if ( eng->dat.number_pak != eng->arg.GPackageTolNum ) FindErr( eng, Err_Flag_NCSI_Package_Num );
+	if ( eng->dat.number_chl != eng->arg.GChannelTolNum ) FindErr( eng, Err_Flag_NCSI_Channel_Num );
+//	if ( eng->dat.number_chl == 0                       ) FindErr( eng );
+
+	if ( eng->flg.Err_Flag ) {
+		if ( eng->dat.NCSI_RxEr )
+			FindErr_Des( eng, Des_Flag_RxErr );
+		return(1);
+	}
+	else {
+		if ( eng->dat.NCSI_RxEr ) {
+			eng->flg.Wrn_Flag = eng->flg.Wrn_Flag | Wrn_Flag_RxErFloatting;
+			if ( eng->arg.GEn_SkipRxEr ) {
+				eng->flg.AllFail = 0;
+				return(0);
+			}
+			else {
+				FindErr_Des( eng, Des_Flag_RxErr );
+				return(1);
+			}
+		}
+		else {
+			eng->flg.AllFail = 0;
+			return(0);
+		}
+	}
+}
--- uboot_old/oem/ami/standalone/nettest/NCSI.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/NCSI.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,156 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef NCSI_H
+#define NCSI_H
+
+#include "TYPEDEF.H"
+
+#define NCSI_RxDMA_PakSize                       2048
+#define NCSI_RxDMA_BASE                          ( DMA_BASE + 0x00100000 )
+
+//---------------------------------------------------------
+// Define
+//---------------------------------------------------------
+#define MAX_PACKAGE_NUM                          8      // 1 ~ 8
+#define MAX_CHANNEL_NUM                          4      // 1 ~ 32
+
+//---------------------------------------------------------
+// Function
+//---------------------------------------------------------
+#define SENT_RETRY_COUNT                         1
+#define NCSI_RxDESNum                            2048
+
+#define NCSI_Skip_RxCRCData
+//#define NCSI_Skip_Phase1_DeSelectPackage
+//#define NCSI_Skip_DeSelectPackage
+//#define NCSI_Skip_DiSChannel
+//#define NCSI_EnableDelay_DeSelectPackage
+//#define NCSI_EnableDelay_GetLinkStatus
+//#define NCSI_EnableDelay_EachPackage
+//#define Print_Version_ID
+//#define Print_PackageName
+#define Print_DetailFrame
+
+//---------------------------------------------------------
+// Delay (ms)
+//---------------------------------------------------------
+#define Delay_EachPackage                        1000
+#define Delay_DeSelectPackage                    50
+#define Delay_GetLinkStatus                      50
+
+//---------------------------------------------------------
+// PCI DID/VID & Manufacturer ID
+//---------------------------------------------------------
+#define ManufacturerID_Intel                     0x00000157     //343
+#define ManufacturerID_Broadcom                  0x0000113d     //4413
+#define ManufacturerID_Mellanox                  0x000002c9     //713
+#define ManufacturerID_Mellanox1                 0x00008119     //33049
+#define ManufacturerID_Emulex                    0x0000006c     //108
+
+//PCI VID: [163c]intel
+//PCI VID: [8086]Intel Corporation
+//PCI VID: [8087]Intel
+//PCI VID: [14e4]Broadcom Corporation
+//PCI VID: [15b3]Mellanox
+//PCI VID: [10df]Emulex
+#define PCI_DID_VID_Intel_82574L                 0x10d38086     // IntelR 82574L Gigabit Ethernet Controller
+#define PCI_DID_VID_Intel_82575_10d6             0x10d68086     // 82566 DM-2-gigabyte
+#define PCI_DID_VID_Intel_82575_10a7             0x10a78086     // 82575EB Gigabit Network Connection
+#define PCI_DID_VID_Intel_82575_10a9             0x10a98086     // 82575EB Gigabit Network Connection
+#define PCI_DID_VID_Intel_82576_10c9             0x10c98086     //*82576 Gigabit ET Dual Port Server Adapter
+#define PCI_DID_VID_Intel_82576_10e6             0x10e68086     // 82576 Gigabit Network Connection
+#define PCI_DID_VID_Intel_82576_10e7             0x10e78086     // 82576 Gigabit Network Connection
+#define PCI_DID_VID_Intel_82576_10e8             0x10e88086     // E64750-xxx Intel Gigabit ET Quad Port Server Adapter
+#define PCI_DID_VID_Intel_82576_1518             0x15188086     // 82576NS SerDes Gigabit Network Connectio
+#define PCI_DID_VID_Intel_82576_1526             0x15268086     // Intel Gigabit ET2 Quad Port Server Adapter
+#define PCI_DID_VID_Intel_82576_150a             0x150a8086     // 82576NS Gigabit Ethernet Controller
+#define PCI_DID_VID_Intel_82576_150d             0x150d8086     // 82576 Gigabit Backplane Connection
+#define PCI_DID_VID_Intel_82599_10fb             0x10fb8086     // 10 Gb Ethernet controller
+#define PCI_DID_VID_Intel_82599_1557             0x15578086     // 82599EN
+#define PCI_DID_VID_Intel_I210_1533              0x15338086     //
+#define PCI_DID_VID_Intel_I210_1537              0x15378086     //???
+#define PCI_DID_VID_Intel_I350_1521              0x15218086     //
+#define PCI_DID_VID_Intel_I350_1523              0x15238086     //
+#define PCI_DID_VID_Intel_X540                   0x15288086     //
+#define PCI_DID_VID_Intel_X550                   0x15638086     //
+#define PCI_DID_VID_Intel_Broadwell_DE           0x15ab8086     //PCH
+#define PCI_DID_VID_Intel_X722_37d0              0x37d08086     //
+#define PCI_DID_VID_Broadcom_BCM5718             0x165614e4     //
+#define PCI_DID_VID_Broadcom_BCM5719             0x165714e4     //
+#define PCI_DID_VID_Broadcom_BCM5720             0x165f14e4     //
+#define PCI_DID_VID_Broadcom_BCM5725             0x164314e4     //
+#define PCI_DID_VID_Broadcom_BCM57810S           0x168e14e4     //
+#define PCI_DID_VID_Broadcom_Cumulus             0x16ca14e4     //
+#define PCI_DID_VID_Broadcom_BCM57302            0x16c914e4     //
+#define PCI_DID_VID_Broadcom_BCM957452           0x16f114e4     //
+#define PCI_DID_VID_Mellanox_ConnectX_3_1003     0x100315b3     //*
+#define PCI_DID_VID_Mellanox_ConnectX_3_1007     0x100715b3     //ConnectX-3 Pro
+#define PCI_DID_VID_Mellanox_ConnectX_4          0x101515b3     //*
+#define PCI_DID_VID_Emulex_40G                   0x072010df     //
+
+//---------------------------------------------------------
+// NCSI Parameter
+//---------------------------------------------------------
+//Command and Response Type
+#define CLEAR_INITIAL_STATE                      0x00           //M
+#define SELECT_PACKAGE                           0x01           //M
+#define DESELECT_PACKAGE                         0x02           //M
+#define ENABLE_CHANNEL                           0x03           //M
+#define DISABLE_CHANNEL                          0x04           //M
+#define RESET_CHANNEL                            0x05           //M
+#define ENABLE_CHANNEL_NETWORK_TX                0x06           //M
+#define DISABLE_CHANNEL_NETWORK_TX               0x07           //M
+#define AEN_ENABLE                               0x08
+#define SET_LINK                                 0x09           //M
+#define GET_LINK_STATUS                          0x0A           //M
+#define SET_VLAN_FILTER                          0x0B           //M
+#define ENABLE_VLAN                              0x0C           //M
+#define DISABLE_VLAN                             0x0D           //M
+#define SET_MAC_ADDRESS                          0x0E           //M
+#define ENABLE_BROADCAST_FILTERING               0x10           //M
+#define DISABLE_BROADCAST_FILTERING              0x11           //M
+#define ENABLE_GLOBAL_MULTICAST_FILTERING        0x12
+#define DISABLE_GLOBAL_MULTICAST_FILTERING       0x13
+#define SET_NCSI_FLOW_CONTROL                    0x14
+#define GET_VERSION_ID                           0x15           //M
+#define GET_CAPABILITIES                         0x16           //M
+#define GET_PARAMETERS                           0x17           //M
+#define GET_CONTROLLER_PACKET_STATISTICS         0x18
+#define GET_NCSI_STATISTICS                      0x19
+#define GET_NCSI_PASS_THROUGH_STATISTICS         0x1A
+
+//Standard Response Code
+#define COMMAND_COMPLETED                        0x00
+#define COMMAND_FAILED                           0x01
+#define COMMAND_UNAVAILABLE                      0x02
+#define COMMAND_UNSUPPORTED                      0x03
+
+//Standard Reason Code
+#define NO_ERROR                                 0x0000
+#define INTERFACE_INITIALIZATION_REQUIRED        0x0001
+#define PARAMETER_IS_INVALID                     0x0002
+#define CHANNEL_NOT_READY                        0x0003
+#define PACKAGE_NOT_READY                        0x0004
+#define INVALID_PAYLOAD_LENGTH                   0x0005
+#define UNKNOWN_COMMAND_TYPE                     0x7FFF
+
+//SET_MAC_ADDRESS
+#define UNICAST                                  ( 0x00 << 5 )
+#define MULTICAST                                ( 0x01 << 5 )
+#define DISABLE_MAC_ADDRESS_FILTER               0x00
+#define ENABLE_MAC_ADDRESS_FILTER                0x01
+
+//GET_LINK_STATUS
+#define LINK_DOWN                                0
+#define LINK_UP                                  1
+
+#endif // NCSI_H
--- uboot_old/oem/ami/standalone/nettest/PCI_SPI.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/PCI_SPI.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,84 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define PCI_SPI_C
+static const char ThisFile[] = "PCI_SPI.c";
+
+#include "SWFUNC.H"
+
+#ifdef SLT_UBOOT
+  #include <common.h>
+  #include <command.h>
+#endif
+#ifdef DOS_ALONE
+  #include <stdio.h>
+  #include <stdlib.h>
+  #include <conio.h>
+  #include <string.h>
+  #include <dos.h>
+#endif
+
+#include "DEF_SPI.H"
+#include "LIB.H"
+#include "TYPEDEF.H"
+
+#ifdef SPI_BUS
+ULONG  GetPCIInfo (DEVICE_PCI_INFO  *VGAPCIInfo)
+{
+    ULONG ulPCIBaseAddress, MMIOBaseAddress, LinearAddressBase, busnum, data;
+
+    ulPCIBaseAddress = FindPCIDevice (0x1A03, 0x2000, ACTIVE);
+    busnum = 0;
+    while (ulPCIBaseAddress == 0 && busnum < 256) {
+        ulPCIBaseAddress = FindPCIDevice (0x1A03, 0x2000, busnum);
+        if (ulPCIBaseAddress == 0) {
+            ulPCIBaseAddress = FindPCIDevice (0x1688, 0x2000, busnum);
+        }
+        if (ulPCIBaseAddress == 0) {
+            ulPCIBaseAddress = FindPCIDevice (0x1A03, 0x1160, busnum);
+        }
+        if (ulPCIBaseAddress == 0) {
+            ulPCIBaseAddress = FindPCIDevice (0x1A03, 0x1180, busnum);
+        }
+        busnum++;
+    }
+    printf("ulPCIBaseAddress = %lx\n", ulPCIBaseAddress);
+    if (ulPCIBaseAddress != 0) {
+        VGAPCIInfo->ulPCIConfigurationBaseAddress = ulPCIBaseAddress;
+        VGAPCIInfo->usVendorID = ReadPCIReg(ulPCIBaseAddress, 0, 0xFFFF);
+        VGAPCIInfo->usDeviceID = ReadPCIReg(ulPCIBaseAddress, 0, 0xFFFF0000) >> 16;
+        LinearAddressBase = ReadPCIReg (ulPCIBaseAddress, 0x10, 0xFFFFFFF0);
+        VGAPCIInfo->ulPhysicalBaseAddress = MapPhysicalToLinear (LinearAddressBase, 64 * 1024 * 1024 + 0x200000);
+        MMIOBaseAddress = ReadPCIReg (ulPCIBaseAddress, 0x14, 0xFFFF0000);
+        VGAPCIInfo->ulMMIOBaseAddress = MapPhysicalToLinear (MMIOBaseAddress, 64 * 1024 * 1024);
+        VGAPCIInfo->usRelocateIO = ReadPCIReg (ulPCIBaseAddress, 0x18, 0x0000FF80);
+        OUTDWPORT(0xcf8, ulPCIBaseAddress + 0x4);
+        data = INDWPORT(0xcfc);
+        OUTDWPORT(0xcfc, data | 0x3);
+        return    TRUE;
+    }
+    else {
+        return    FALSE;
+    }
+} // End ULONG  GetPCIInfo (DEVICE_PCI_INFO  *VGAPCIInfo)
+
+BOOLEAN  GetDevicePCIInfo (VIDEO_ENGINE_INFO *VideoEngineInfo)
+{
+    if (GetPCIInfo (&VideoEngineInfo->VGAPCIInfo) == TRUE) {
+        return    TRUE;
+    }
+    else {
+        printf("Can not find PCI device!\n");
+        exit(0);
+        return    FALSE;
+    }
+} // End
+#endif // End ifdef SPI_BUS
--- uboot_old/oem/ami/standalone/nettest/PHY.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/PHY.c	2018-06-22 17:51:23.921169873 +0800
@@ -0,0 +1,2462 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define PHY_C
+static const char ThisFile[] = "PHY.c";
+//#define PHY_debug
+//#define PHY_debug_set_clr
+//#define Realtek_debug
+
+#ifdef Realtek_debug
+int     GPIO_20h_Value;
+int     GPIO_24h_Value;
+#endif
+
+#include "SWFUNC.H"
+#include "COMMINF.H"
+
+#if defined(SLT_UBOOT)
+  #include <common.h>
+  #include <command.h>
+  #include "STDUBOOT.H"
+#endif
+#if defined(DOS_ALONE)
+  #include <stdio.h>
+  #include <stdlib.h>
+  #include <conio.h>
+  #include <string.h>
+#endif
+
+#include "PHY.H"
+#include "TYPEDEF.H"
+#include "IO.H"
+
+//------------------------------------------------------------
+// PHY R/W basic
+//------------------------------------------------------------
+void phy_write (MAC_ENGINE *eng, int adr, ULONG data) {
+#if defined(PHY_GPIO)
+        phy_gpio_write( eng, adr, data );
+#else
+        int        timeout = 0;
+
+        if ( eng->inf.NewMDIO ) {
+                Write_Reg_PHY_DD( eng, 0x60, ( data << 16 ) | MAC_PHYWr_New | ( eng->phy.Adr << 5 ) | ( adr & 0x1f ) );
+
+                while ( Read_Reg_PHY_DD( eng, 0x60 ) & MAC_PHYBusy_New ) {
+                        if ( ++timeout > TIME_OUT_PHY_RW ) {
+                                if ( !eng->run.TM_Burst )
+                                        PRINTF( FP_LOG, "[PHY-Write] Time out: %08lx\n", Read_Reg_PHY_DD( eng, 0x60 ) );
+
+                                FindErr( eng, Err_Flag_PHY_TimeOut_RW );
+                                break;
+                        }
+                }
+        }
+        else {
+                Write_Reg_PHY_DD( eng, 0x64, data );
+
+                Write_Reg_PHY_DD( eng, 0x60, MDC_Thres | MAC_PHYWr | ( eng->phy.Adr << 16 ) | ( ( adr & 0x1f ) << 21 ) );
+
+                while ( Read_Reg_PHY_DD( eng, 0x60 ) & MAC_PHYWr ) {
+                        if ( ++timeout > TIME_OUT_PHY_RW ) {
+                                if ( !eng->run.TM_Burst )
+                                        PRINTF( FP_LOG, "[PHY-Write] Time out: %08lx\n", Read_Reg_PHY_DD( eng, 0x60 ) );
+
+                                FindErr( eng, Err_Flag_PHY_TimeOut_RW );
+                                break;
+                        }
+                }
+        } // End if ( eng->inf.NewMDIO )
+#endif /* defined(PHY_GPIO) */
+
+#ifdef PHY_debug
+        if ( 1 ) {
+#else
+        if ( DbgPrn_PHYRW ) {
+#endif
+                printf("[Wr ]%02d: 0x%04lx (%02d:%08lx)\n", adr, data, eng->phy.Adr, eng->phy.PHY_BASE );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "[Wr ]%02d: 0x%04lx (%02d:%08lx)\n", adr, data, eng->phy.Adr, eng->phy.PHY_BASE );
+        }
+
+} // End void phy_write (int adr, ULONG data)
+
+//------------------------------------------------------------
+ULONG phy_read (MAC_ENGINE *eng, int adr) {
+        ULONG      read_value;
+
+#if defined(PHY_GPIO)
+        read_value = phy_gpio_read( eng, adr );
+#else
+        int        timeout = 0;
+
+        if ( eng->inf.NewMDIO ) {
+                Write_Reg_PHY_DD( eng, 0x60, MAC_PHYRd_New | (eng->phy.Adr << 5) | ( adr & 0x1f ) );
+
+                while ( Read_Reg_PHY_DD( eng, 0x60 ) & MAC_PHYBusy_New ) {
+                        if ( ++timeout > TIME_OUT_PHY_RW ) {
+                                if ( !eng->run.TM_Burst )
+                                        PRINTF( FP_LOG, "[PHY-Read] Time out: %08lx\n", Read_Reg_PHY_DD( eng, 0x60 ) );
+
+                                FindErr( eng, Err_Flag_PHY_TimeOut_RW );
+                                break;
+                        }
+                }
+
+  #ifdef Delay_PHYRd
+                DELAY( Delay_PHYRd );
+  #endif
+                read_value = Read_Reg_PHY_DD( eng, 0x64 ) & 0xffff;
+        }
+        else {
+                Write_Reg_PHY_DD( eng, 0x60, MDC_Thres | MAC_PHYRd | (eng->phy.Adr << 16) | ((adr & 0x1f) << 21) );
+
+                while ( Read_Reg_PHY_DD( eng, 0x60 ) & MAC_PHYRd ) {
+                        if ( ++timeout > TIME_OUT_PHY_RW ) {
+                                if ( !eng->run.TM_Burst )
+                                        PRINTF( FP_LOG, "[PHY-Read] Time out: %08lx\n", Read_Reg_PHY_DD( eng, 0x60 ) );
+
+                                FindErr( eng, Err_Flag_PHY_TimeOut_RW );
+                                break;
+                        }
+                }
+
+  #ifdef Delay_PHYRd
+                DELAY( Delay_PHYRd );
+  #endif
+                read_value = Read_Reg_PHY_DD( eng, 0x64 ) >> 16;
+        }
+#endif /* End defined(PHY_GPIO) */
+
+#ifdef PHY_debug
+        if ( 1 ) {
+#else
+        if ( DbgPrn_PHYRW ) {
+#endif
+                printf("[Rd ]%02d: 0x%04lx (%02d:%08lx)\n", adr, read_value, eng->phy.Adr, eng->phy.PHY_BASE );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "[Rd ]%02d: 0x%04lx (%02d:%08lx)\n", adr, read_value, eng->phy.Adr, eng->phy.PHY_BASE );
+        }
+
+        return( read_value );
+} // End ULONG phy_read (MAC_ENGINE *eng, int adr)
+
+//------------------------------------------------------------
+void phy_Read_Write (MAC_ENGINE *eng, int adr, ULONG clr_mask, ULONG set_mask) {
+#ifdef PHY_debug
+        if ( 1 ) {
+#else
+        if ( DbgPrn_PHYRW ) {
+#endif
+                printf("[RW ]%02d: clr:0x%04lx: set:0x%04lx (%02d:%08lx)\n", adr, clr_mask, set_mask, eng->phy.Adr, eng->phy.PHY_BASE);
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "[RW ]%02d: clr:0x%04lx: set:0x%04lx (%02d:%08lx)\n", adr, clr_mask, set_mask, eng->phy.Adr, eng->phy.PHY_BASE);
+        }
+        phy_write( eng, adr, ((phy_read( eng, adr ) & (~clr_mask)) | set_mask) );
+}
+
+//------------------------------------------------------------
+void phy_out (MAC_ENGINE *eng, int adr) {
+        printf("%02d: %04lx\n", adr, phy_read( eng, adr ));
+}
+
+//------------------------------------------------------------
+//void phy_outchg (MAC_ENGINE *eng,  int adr) {
+//      ULONG   PHY_valold = 0;
+//      ULONG   PHY_val;
+//
+//      while (1) {
+//              PHY_val = phy_read( eng, adr );
+//              if (PHY_valold != PHY_val) {
+//                      printf("%02ld: %04lx\n", adr, PHY_val);
+//                      PHY_valold = PHY_val;
+//              }
+//      }
+//}
+
+//------------------------------------------------------------
+void phy_dump (MAC_ENGINE *eng) {
+        int        index;
+
+        printf("[PHY%d][%d]----------------\n", eng->run.MAC_idx + 1, eng->phy.Adr);
+        for (index = 0; index < 32; index++) {
+                printf("%02d: %04lx ", index, phy_read( eng, index ));
+
+                if ((index % 8) == 7)
+                        printf("\n");
+        }
+}
+
+//------------------------------------------------------------
+void phy_id (MAC_ENGINE *eng, BYTE option) {
+
+        ULONG      reg_adr;
+        CHAR       PHY_ADR_org;
+
+        PHY_ADR_org = eng->phy.Adr;
+        for ( eng->phy.Adr = 0; eng->phy.Adr < 32; eng->phy.Adr++ ) {
+
+                PRINTF(option, "[%02d] ", eng->phy.Adr);
+
+                for ( reg_adr = 2; reg_adr <= 3; reg_adr++ ) {
+                        PRINTF(option, "%ld:%04lx ", reg_adr, phy_read( eng, reg_adr ));
+                }
+
+                if ( ( eng->phy.Adr % 4 ) == 3 ) {
+                        PRINTF(option, "\n");
+                }
+        }
+        eng->phy.Adr = PHY_ADR_org;
+}
+
+//------------------------------------------------------------
+void phy_delay (int dt) {
+#ifdef Realtek_debug
+Write_Reg_GPIO_DD( 0x20, GPIO_20h_Value & 0xffbfffff);
+//      delay_hwtimer( dt );
+#endif
+#ifdef PHY_debug
+        printf("delay %d ms\n", dt);
+#endif
+        DELAY( dt );
+
+#ifdef Realtek_debug
+Write_Reg_GPIO_DD( 0x20, GPIO_20h_Value );
+#endif
+}
+
+//------------------------------------------------------------
+// PHY IC basic
+//------------------------------------------------------------
+void phy_basic_setting (MAC_ENGINE *eng) {
+        phy_Read_Write( eng,  0, 0x7140, eng->phy.PHY_00h ); //clr set
+#ifdef PHY_debug_set_clr
+        if ( 1 ) {
+#else
+        if ( DbgPrn_PHYRW ) {
+#endif
+                printf("[Set]00: 0x%04lx (%02d:%08lx)\n", phy_read( eng, PHY_REG_BMCR ), eng->phy.Adr, eng->phy.PHY_BASE );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "[Set]00: 0x%04lx (%02d:%08lx)\n", phy_read( eng, PHY_REG_BMCR ), eng->phy.Adr, eng->phy.PHY_BASE );
+        }
+}
+
+//------------------------------------------------------------
+void phy_Wait_Reset_Done (MAC_ENGINE *eng) {
+        int        timeout = 0;
+
+        while (  phy_read( eng, PHY_REG_BMCR ) & 0x8000 ) {
+                if (++timeout > TIME_OUT_PHY_Rst) {
+                        if ( !eng->run.TM_Burst )
+                                PRINTF( FP_LOG, "[PHY-Reset] Time out: %08lx\n", Read_Reg_PHY_DD( eng, 0x60 ) );
+
+                        FindErr( eng, Err_Flag_PHY_TimeOut_Rst );
+                        break;
+                }
+        }//wait Rst Done
+
+#ifdef PHY_debug_set_clr
+        if ( 1 ) {
+#else
+        if ( DbgPrn_PHYRW ) {
+#endif
+                printf("[Clr]00: 0x%04lx (%02d:%08lx)\n", phy_read( eng, PHY_REG_BMCR ), eng->phy.Adr, eng->phy.PHY_BASE );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "[Clr]00: 0x%04lx (%02d:%08lx)\n", phy_read( eng, PHY_REG_BMCR ), eng->phy.Adr, eng->phy.PHY_BASE );
+        }
+#ifdef Delay_PHYRst
+        DELAY( Delay_PHYRst );
+#endif
+}
+
+//------------------------------------------------------------
+void phy_Reset (MAC_ENGINE *eng) {
+        phy_basic_setting( eng );
+
+//      phy_Read_Write( eng,  0, 0x0000, 0x8000 | eng->phy.PHY_00h );//clr set//Rst PHY
+        phy_Read_Write( eng,  0, 0x7140, 0x8000 | eng->phy.PHY_00h );//clr set//Rst PHY
+        phy_Wait_Reset_Done( eng );
+
+        phy_basic_setting( eng );
+#ifdef Delay_PHYRst
+        DELAY( Delay_PHYRst );
+#endif
+}
+
+//------------------------------------------------------------
+void phy_check_register (MAC_ENGINE *eng, ULONG adr, ULONG check_mask, ULONG check_value, ULONG hit_number, char *runname) {
+        USHORT     wait_phy_ready = 0;
+        USHORT     hit_count = 0;
+
+        while ( wait_phy_ready < 1000 ) {
+                if ( (phy_read( eng, adr ) & check_mask) == check_value ) {
+                        if ( ++hit_count >= hit_number ) {
+                                break;
+                        }
+                        else {
+                                phy_delay(1);
+                        }
+                } else {
+                        hit_count = 0;
+                        wait_phy_ready++;
+                        phy_delay(10);
+                }
+        }
+        if ( hit_count < hit_number ) {
+                printf("Timeout: %s\n", runname);
+                PRINTF( FP_LOG, "Timeout: %s\n", runname);
+        }
+}
+
+//------------------------------------------------------------
+// PHY IC
+//------------------------------------------------------------
+void recov_phy_marvell (MAC_ENGINE *eng) {//88E1111
+        if ( eng->run.TM_Burst ) {
+        }
+        else if ( eng->phy.loop_phy ) {
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng,  9, eng->phy.PHY_09h );
+
+                        phy_Reset( eng );
+
+                        phy_write( eng, 29, 0x0007 );
+                        phy_Read_Write( eng, 30, 0x0008, 0x0000 );//clr set
+                        phy_write( eng, 29, 0x0010 );
+                        phy_Read_Write( eng, 30, 0x0002, 0x0000 );//clr set
+                        phy_write( eng, 29, 0x0012 );
+                        phy_Read_Write( eng, 30, 0x0001, 0x0000 );//clr set
+
+                        phy_write( eng, 18, eng->phy.PHY_12h );
+                }
+        }
+}
+
+//------------------------------------------------------------
+void phy_marvell (MAC_ENGINE *eng) {//88E1111
+//      int        Retry;
+
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Marvell] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        if ( eng->run.TM_Burst ) {
+                phy_Reset( eng );
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_Reset( eng );
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        eng->phy.PHY_09h = phy_read( eng, PHY_GBCR );
+                        eng->phy.PHY_12h = phy_read( eng, PHY_INER );
+                        phy_write( eng, 18, 0x0000 );
+                        phy_Read_Write( eng,  9, 0x0000, 0x1800 );//clr set
+                }
+
+                phy_Reset( eng );
+
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 29, 0x0007 );
+                        phy_Read_Write( eng, 30, 0x0000, 0x0008 );//clr set
+                        phy_write( eng, 29, 0x0010 );
+                        phy_Read_Write( eng, 30, 0x0000, 0x0002 );//clr set
+                        phy_write( eng, 29, 0x0012 );
+                        phy_Read_Write( eng, 30, 0x0000, 0x0001 );//clr set
+                }
+        }
+
+        if ( !eng->phy.loop_phy )
+                phy_check_register ( eng, 17, 0x0400, 0x0400, 1, "wait 88E1111 link-up");
+//      Retry = 0;
+//      do {
+//              eng->phy.PHY_11h = phy_read( eng, PHY_SR );
+//      } while ( !( ( eng->phy.PHY_11h & 0x0400 ) | eng->phy.loop_phy | ( Retry++ > 20 ) ) );
+}
+
+//------------------------------------------------------------
+void recov_phy_marvell0 (MAC_ENGINE *eng) {//88E1310
+        if ( eng->run.TM_Burst ) {
+        }
+        else if ( eng->phy.loop_phy ) {
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 22, 0x0006 );
+                        phy_Read_Write( eng, 16, 0x0020, 0x0000 );//clr set
+                        phy_write( eng, 22, 0x0000 );
+                }
+        }
+}
+
+//------------------------------------------------------------
+void phy_marvell0 (MAC_ENGINE *eng) {//88E1310
+//      int        Retry;
+
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Marvell] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        phy_write( eng, 22, 0x0002 );
+
+        eng->phy.PHY_15h = phy_read( eng, 21 );
+        if ( eng->phy.PHY_15h & 0x0030 ) {
+                printf("\n\n[Warning] Page2, Register 21, bit 4~5 must be 0 [Reg15_2:%04lx]\n\n", eng->phy.PHY_15h);
+                if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Page2, Register 21, bit 4~5 must be 0 [Reg15_2:%04lx]\n\n", eng->phy.PHY_15h );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Page2, Register 21, bit 4~5 must be 0 [Reg15_2:%04lx]\n\n", eng->phy.PHY_15h );
+
+                phy_write( eng, 21, eng->phy.PHY_15h & 0xffcf ); // Set [5]Rx Dly, [4]Tx Dly to 0
+        }
+phy_read( eng, 21 ); // v069
+        phy_write( eng, 22, 0x0000 );
+
+        if ( eng->run.TM_Burst ) {
+                phy_Reset( eng );
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_write( eng, 22, 0x0002 );
+
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_Read_Write( eng, 21, 0x6040, 0x0040 );//clr set
+                }
+                else if ( eng->run.Speed_sel[ 1 ] ) {
+                        phy_Read_Write( eng, 21, 0x6040, 0x2000 );//clr set
+                }
+                else {
+                        phy_Read_Write( eng, 21, 0x6040, 0x0000 );//clr set
+                }
+                phy_write( eng, 22, 0x0000 );
+                phy_Reset(  eng  );
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 22, 0x0006 );
+                        phy_Read_Write( eng, 16, 0x0000, 0x0020 );//clr set
+phy_read( eng, 16 ); // v069
+                        phy_write( eng, 22, 0x0000 );
+                }
+
+                phy_Reset( eng );
+phy_read( eng, 0 ); // v069
+        }
+
+        if ( !eng->phy.loop_phy )
+                phy_check_register ( eng, 17, 0x0400, 0x0400, 1, "wait 88E1310 link-up");
+//      Retry = 0;
+//      do {
+//              eng->phy.PHY_11h = phy_read( eng, PHY_SR );
+//      } while ( !( ( eng->phy.PHY_11h & 0x0400 ) | eng->phy.loop_phy | ( Retry++ > 20 ) ) );
+}
+
+//------------------------------------------------------------
+void recov_phy_marvell1 (MAC_ENGINE *eng) {//88E6176
+        CHAR       PHY_ADR_org;
+
+        PHY_ADR_org = eng->phy.Adr;
+        for ( eng->phy.Adr = 16; eng->phy.Adr <= 22; eng->phy.Adr++ ) {
+                if ( eng->run.TM_Burst ) {
+                }
+                else {
+                        phy_write( eng,  6, eng->phy.PHY_06hA[eng->phy.Adr-16] );//06h[5]P5 loopback, 06h[6]P6 loopback
+                }
+        }
+        for ( eng->phy.Adr = 21; eng->phy.Adr <= 22; eng->phy.Adr++ ) {
+                phy_write( eng,  1, 0x0003 ); //01h[1:0]00 = 10 Mbps, 01 = 100 Mbps, 10 = 1000 Mbps, 11 = Speed is not forced.
+        }
+        eng->phy.Adr = PHY_ADR_org;
+}
+
+//------------------------------------------------------------
+void phy_marvell1 (MAC_ENGINE *eng) {//88E6176
+//      ULONG      PHY_01h;
+        CHAR       PHY_ADR_org;
+
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Marvell] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        if ( eng->run.TM_Burst ) {
+                printf("This mode doesn't support in 88E6176.\n");
+				PHY_ADR_org = eng->phy.Adr;
+        } else {
+                //The 88E6176 is switch with 7 Port(P0~P6) and the PHYAdr will be fixed at 0x10~0x16, and only P5/P6 can be connected to the MAC.
+                //Therefor, the 88E6176 only can run the internal loopback.
+                PHY_ADR_org = eng->phy.Adr;
+                for ( eng->phy.Adr = 16; eng->phy.Adr <= 20; eng->phy.Adr++ ) {
+                        eng->phy.PHY_06hA[eng->phy.Adr-16] = phy_read( eng, PHY_ANER );
+                        phy_write( eng,  6, 0x0000 );//06h[5]P5 loopback, 06h[6]P6 loopback
+                }
+
+                for ( eng->phy.Adr = 21; eng->phy.Adr <= 22; eng->phy.Adr++ ) {
+//                      PHY_01h = phy_read( eng, PHY_REG_BMSR );
+//                      if      ( eng->run.Speed_sel[ 0 ] ) phy_write( eng,  1, (PHY_01h & 0xfffc) | 0x0002 );//[1:0]00 = 10 Mbps, 01 = 100 Mbps, 10 = 1000 Mbps, 11 = Speed is not forced.
+//                      else if ( eng->run.Speed_sel[ 1 ] ) phy_write( eng,  1, (PHY_01h & 0xfffc) | 0x0001 );//[1:0]00 = 10 Mbps, 01 = 100 Mbps, 10 = 1000 Mbps, 11 = Speed is not forced.
+//                      else                              phy_write( eng,  1, (PHY_01h & 0xfffc)          );//[1:0]00 = 10 Mbps, 01 = 100 Mbps, 10 = 1000 Mbps, 11 = Speed is not forced.
+                        if      ( eng->run.Speed_sel[ 0 ] ) phy_write( eng,  1, 0x0002 );//01h[1:0]00 = 10 Mbps, 01 = 100 Mbps, 10 = 1000 Mbps, 11 = Speed is not forced.
+                        else if ( eng->run.Speed_sel[ 1 ] ) phy_write( eng,  1, 0x0001 );//01h[1:0]00 = 10 Mbps, 01 = 100 Mbps, 10 = 1000 Mbps, 11 = Speed is not forced.
+                        else                                phy_write( eng,  1, 0x0000 );//01h[1:0]00 = 10 Mbps, 01 = 100 Mbps, 10 = 1000 Mbps, 11 = Speed is not forced.
+
+                        eng->phy.PHY_06hA[eng->phy.Adr-16] = phy_read( eng, PHY_ANER );
+                        if ( eng->phy.Adr == 21 ) phy_write( eng,  6, 0x0020 );//06h[5]P5 loopback, 06h[6]P6 loopback
+                        else                      phy_write( eng,  6, 0x0040 );//06h[5]P5 loopback, 06h[6]P6 loopback
+                }
+                eng->phy.Adr = PHY_ADR_org;
+        }
+}
+
+//------------------------------------------------------------
+void recov_phy_marvell2 (MAC_ENGINE *eng) {//88E1512//88E15 10/12/14/18
+        if ( eng->run.TM_Burst ) {
+        }
+        else if ( eng->phy.loop_phy ) {
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        // Enable Stub Test
+                        // switch page 6
+                        phy_write( eng, 22, 0x0006 );
+                        phy_Read_Write( eng, 18, 0x0008, 0x0000 );//clr set
+                        phy_write( eng, 22, 0x0000 );
+                }
+        }
+}
+
+//------------------------------------------------------------
+void phy_marvell2 (MAC_ENGINE *eng) {//88E1512//88E15 10/12/14/18
+//      int        Retry = 0;
+//      ULONG      temp_reg;
+
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Marvell] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+//      eng->run.TIME_OUT_Des_PHYRatio = 10;
+
+        // switch page 2
+        phy_write( eng, 22, 0x0002 );
+        eng->phy.PHY_15h = phy_read( eng, 21 );
+        if ( eng->phy.PHY_15h & 0x0030 ) {
+                printf("\n\n[Warning] Page2, Register 21, bit 4~5 must be 0 [Reg15h_2:%04lx]\n\n", eng->phy.PHY_15h);
+                if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Page2, Register 21, bit 4~5 must be 0 [Reg15h_2:%04lx]\n\n", eng->phy.PHY_15h );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Page2, Register 21, bit 4~5 must be 0 [Reg15h_2:%04lx]\n\n", eng->phy.PHY_15h );
+
+                phy_write( eng, 21, eng->phy.PHY_15h & 0xffcf );
+        }
+        phy_write( eng, 22, 0x0000 );
+
+
+        if ( eng->run.TM_Burst ) {
+                phy_Reset( eng );
+        }
+        else if ( eng->phy.loop_phy ) {
+                // Internal loopback funciton only support in copper mode
+                // switch page 18
+                phy_write( eng, 22, 0x0012 );
+                eng->phy.PHY_14h = phy_read( eng, 20 );
+                // Change mode to Copper mode
+//              if ( eng->phy.PHY_14h & 0x0020 ) {
+                if ( ( eng->phy.PHY_14h & 0x003f ) != 0x0010 ) {
+                        printf("\n\n[Warning] Internal loopback funciton only support in copper mode[%04lx]\n\n", eng->phy.PHY_14h);
+                        if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Internal loopback funciton only support in copper mode[%04lx]\n\n", eng->phy.PHY_14h);
+                        if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Internal loopback funciton only support in copper mode[%04lx]\n\n", eng->phy.PHY_14h);
+
+                        phy_write( eng, 20, ( eng->phy.PHY_14h & 0xffc0 ) | 0x8010 );
+                        // do software reset
+                        phy_check_register ( eng, 20, 0x8000, 0x0000, 1, "wait 88E15 10/12/14/18 mode reset");
+//                      do {
+//                              temp_reg = phy_read( eng, 20 );
+//                      } while ( ( (temp_reg & 0x8000) == 0x8000 ) & (Retry++ < 20) );
+                }
+
+                // switch page 2
+                phy_write( eng, 22, 0x0002 );
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_Read_Write( eng, 21, 0x2040, 0x0040 );//clr set
+                }
+                else if ( eng->run.Speed_sel[ 1 ] ) {
+                        phy_Read_Write( eng, 21, 0x2040, 0x2000 );//clr set
+                }
+                else {
+                        phy_Read_Write( eng, 21, 0x2040, 0x0000 );//clr set
+                }
+                phy_write( eng, 22, 0x0000 );
+
+                phy_Reset( eng );
+
+                //Internal loopback at 100Mbps need delay 400~500 ms
+//              DELAY( 400 );//Still fail at 100Mbps
+//              DELAY( 500 );//All Pass
+                if ( !eng->run.Speed_sel[ 0 ] ) {
+                        phy_check_register ( eng, 17, 0x0040, 0x0040, 10, "wait 88E15 10/12/14/18 link-up");
+                        phy_check_register ( eng, 17, 0x0040, 0x0000, 10, "wait 88E15 10/12/14/18 link-up");
+                        phy_check_register ( eng, 17, 0x0040, 0x0040, 10, "wait 88E15 10/12/14/18 link-up");
+                }
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        // Enable Stub Test
+                        // switch page 6
+                        phy_write( eng, 22, 0x0006 );
+                        phy_Read_Write( eng, 18, 0x0000, 0x0008 );//clr set
+                        phy_write( eng, 22, 0x0000 );
+                }
+
+                phy_Reset( eng );
+                phy_check_register ( eng, 17, 0x0400, 0x0400, 10, "wait 88E15 10/12/14/18 link-up");
+        }
+
+//      if ( !eng->phy.loop_phy )
+////    if ( !eng->run.TM_Burst )
+//              phy_check_register ( eng, 17, 0x0400, 0x0400, 10, "wait 88E15 10/12/14/18 link-up");
+////    Retry = 0;
+////    do {
+////            eng->phy.PHY_11h = phy_read( eng, PHY_SR );
+////    } while ( !( ( eng->phy.PHY_11h & 0x0400 ) | eng->phy.loop_phy | ( Retry++ > 20 ) ) );
+}
+
+//------------------------------------------------------------
+void phy_marvell3 (MAC_ENGINE *eng) {//88E3019
+#ifdef PHY_debug
+        if ( 1 ) {
+#else
+        if ( DbgPrn_PHYName ) {
+#endif
+                printf("--->(%04lx %04lx)[Marvell] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "--->(%04lx %04lx)[Marvell] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+        }
+
+        //Reg1ch[11:10]: MAC Interface Mode
+        // 00 => RGMII where receive clock trnasitions when data transitions
+        // 01 => RGMII where receive clock trnasitions when data is stable
+        // 10 => RMII
+        // 11 => MII
+        eng->phy.PHY_1ch = phy_read( eng, 28 );
+        if ( eng->env.MAC_RMII ) {
+                if ( ( eng->phy.PHY_1ch & 0x0c00 ) != 0x0800 ) {
+                        printf("\n\n[Warning] Register 28, bit 10~11 must be 2 (RMII Mode)[Reg1ch:%04lx]\n\n", eng->phy.PHY_1ch);
+                        eng->phy.PHY_1ch = ( eng->phy.PHY_1ch & 0xf3ff ) | 0x0800;
+                        phy_write( eng, 28, eng->phy.PHY_1ch );
+//                      phy_write( eng,  0, phy_read( eng,  0 ) | 0x8000 );
+//                      phy_Wait_Reset_Done( eng );
+                }
+        } else {
+                if ( ( eng->phy.PHY_1ch & 0x0c00 ) != 0x0000 ) {
+                        printf("\n\n[Warning] Register 28, bit 10~11 must be 0 (RGMIIRX Edge-align Mode)[Reg1ch:%04lx]\n\n", eng->phy.PHY_1ch);
+                        eng->phy.PHY_1ch = ( eng->phy.PHY_1ch & 0xf3ff ) | 0x0000;
+                        phy_write( eng, 28, eng->phy.PHY_1ch );
+//                      phy_write( eng,  0, phy_read( eng,  0 ) | 0x8000 );
+//                      phy_Wait_Reset_Done( eng );
+                }
+        }
+
+        if ( eng->run.TM_Burst ) {
+                phy_Reset( eng );
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_Reset( eng );
+        }
+        else {
+                phy_Reset( eng );
+        }
+
+        phy_check_register ( eng, 17, 0x0400, 0x0400, 1, "wait 88E3019 link-up");
+}
+
+//------------------------------------------------------------
+void phy_broadcom (MAC_ENGINE *eng) {//BCM5221
+    ULONG      reg;
+
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Broadcom] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        phy_Reset( eng );
+
+        if ( eng->run.TM_IEEE ) {
+                if ( eng->arg.GIEEE_sel == 0 ) {
+                        phy_write( eng, 25, 0x1f01 );//Force MDI  //Measuring from channel A
+                }
+                else {
+                        phy_Read_Write( eng, 24, 0x0000, 0x4000 );//clr set//Force Link
+//                      phy_write( eng,  0, eng->phy.PHY_00h );
+//                      phy_write( eng, 30, 0x1000 );
+                }
+        }
+        else
+        {
+                // we can check link status from register 0x18
+                if ( eng->run.Speed_sel[ 1 ] ) {
+                        do {
+                                reg = phy_read( eng, 0x18 ) & 0xF;
+                        } while ( reg != 0x7 );
+                }
+                else {
+                        do {
+                        reg = phy_read( eng, 0x18 ) & 0xF;
+                        } while ( reg != 0x1 );
+                }
+        }
+}
+
+//------------------------------------------------------------
+void recov_phy_broadcom0 (MAC_ENGINE *eng) {//BCM54612
+        phy_write( eng,  0, eng->phy.PHY_00h );
+        phy_write( eng,  9, eng->phy.PHY_09h );
+//      phy_write( eng, 24, eng->phy.PHY_18h | 0xf007 );//write reg 18h, shadow value 111
+//      phy_write( eng, 28, eng->phy.PHY_1ch | 0x8c00 );//write reg 1Ch, shadow value 00011
+
+        if ( eng->run.TM_Burst ) {
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_write( eng,  0, eng->phy.PHY_00h );
+        }
+        else {
+        }
+}
+
+//------------------------------------------------------------
+//internal loop 1G  : no  loopback stub
+//internal loop 100M: Don't support(?)
+//internal loop 10M : Don't support(?)
+void phy_broadcom0 (MAC_ENGINE *eng) {//BCM54612
+        ULONG      PHY_new;
+
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Broadcom] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        eng->phy.PHY_00h = phy_read( eng, PHY_REG_BMCR );
+        eng->phy.PHY_09h = phy_read( eng, PHY_GBCR );
+        phy_write( eng, 24, 0x7007 );//read reg 18h, shadow value 111
+        eng->phy.PHY_18h = phy_read( eng, 24 );
+        phy_write( eng, 28, 0x0c00 );//read reg 1Ch, shadow value 00011
+        eng->phy.PHY_1ch = phy_read( eng, 28 );
+
+        if ( eng->phy.PHY_18h & 0x0100 ) {
+                PHY_new = ( eng->phy.PHY_18h & 0x0af0 ) | 0xf007;
+                printf("\n\n[Warning] Shadow value 111, Register 24, bit 8 must be 0 [Reg18h_7:%04lx->%04lx]\n\n", eng->phy.PHY_18h, PHY_new);
+                if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Shadow value 111, Register 24, bit 8 must be 0 [Reg18h_7:%04lx->%04lx]\n\n", eng->phy.PHY_18h, PHY_new );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Shadow value 111, Register 24, bit 8 must be 0 [Reg18h_7:%04lx->%04lx]\n\n", eng->phy.PHY_18h, PHY_new );
+
+                phy_write( eng, 24, PHY_new ); // Disable RGMII RXD to RXC Skew
+        }
+        if ( eng->phy.PHY_1ch & 0x0200 ) {
+                PHY_new = ( eng->phy.PHY_1ch & 0x0000 ) | 0x8c00;
+                printf("\n\n[Warning] Shadow value 00011, Register 28, bit 9 must be 0 [Reg1ch_3:%04lx->%04lx]\n\n", eng->phy.PHY_1ch, PHY_new);
+                if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Shadow value 00011, Register 28, bit 9 must be 0 [Reg1ch_3:%04lx->%04lx]\n\n", eng->phy.PHY_1ch, PHY_new );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Shadow value 00011, Register 28, bit 9 must be 0 [Reg1ch_3:%04lx->%04lx]\n\n", eng->phy.PHY_1ch, PHY_new );
+
+                phy_write( eng, 28, PHY_new );// Disable GTXCLK Clock Delay Enable
+        }
+
+        if ( eng->run.TM_Burst ) {
+                phy_basic_setting( eng );
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_basic_setting( eng );
+
+                // Enable Internal Loopback mode
+                // Page 58, BCM54612EB1KMLG_Spec.pdf
+                phy_write( eng,  0, 0x5140 );
+#ifdef Delay_PHYRst
+                phy_delay( Delay_PHYRst );
+#endif
+                /* Only 1G Test is PASS, 100M and 10M is false @20130619 */
+
+// Waiting for BCM FAE's response
+//              if ( eng->run.Speed_sel[ 0 ] ) {
+//                      // Speed 1G
+//                      // Enable Internal Loopback mode
+//                      // Page 58, BCM54612EB1KMLG_Spec.pdf
+//                      phy_write( eng,  0, 0x5140 );
+//              }
+//              else if ( eng->run.Speed_sel[ 1 ] ) {
+//                      // Speed 100M
+//                      // Enable Internal Loopback mode
+//                      // Page 58, BCM54612EB1KMLG_Spec.pdf
+//                      phy_write( eng,  0, 0x7100 );
+//                      phy_write( eng, 30, 0x1000 );
+//              }
+//              else if ( eng->run.Speed_sel[ 2 ] ) {
+//                      // Speed 10M
+//                      // Enable Internal Loopback mode
+//                      // Page 58, BCM54612EB1KMLG_Spec.pdf
+//                      phy_write( eng,  0, 0x5100 );
+//                      phy_write( eng, 30, 0x1000 );
+//              }
+//
+#ifdef Delay_PHYRst
+//              phy_delay( Delay_PHYRst );
+#endif
+        }
+        else {
+
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        // Page 60, BCM54612EB1KMLG_Spec.pdf
+                        // need to insert loopback plug
+                        phy_write( eng,  9, 0x1800 );
+                        phy_write( eng,  0, 0x0140 );
+                        phy_write( eng, 24, 0x8400 ); // Enable Transmit test mode
+                }
+                else if ( eng->run.Speed_sel[ 1 ] ) {
+                        // Page 60, BCM54612EB1KMLG_Spec.pdf
+                        // need to insert loopback plug
+                        phy_write( eng,  0, 0x2100 );
+                        phy_write( eng, 24, 0x8400 ); // Enable Transmit test mode
+                }
+                else {
+                        // Page 60, BCM54612EB1KMLG_Spec.pdf
+                        // need to insert loopback plug
+                        phy_write( eng,  0, 0x0100 );
+                        phy_write( eng, 24, 0x8400 ); // Enable Transmit test mode
+                }
+#ifdef Delay_PHYRst
+                phy_delay( Delay_PHYRst );
+                phy_delay( Delay_PHYRst );
+#endif                
+        }
+}
+
+//------------------------------------------------------------
+void phy_realtek (MAC_ENGINE *eng) {//RTL8201N
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Realtek] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        phy_Reset( eng );
+}
+
+//------------------------------------------------------------
+//internal loop 100M: Don't support
+//internal loop 10M : no  loopback stub
+void phy_realtek0 (MAC_ENGINE *eng) {//RTL8201E
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Realtek] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        eng->phy.RMIICK_IOMode = eng->phy.RMIICK_IOMode | PHY_Flag_RMIICK_IOMode_RTL8201E;
+
+        phy_Reset( eng );
+
+        eng->phy.PHY_19h = phy_read( eng, 25 );
+        //Check RMII Mode
+        if ( ( eng->phy.PHY_19h & 0x0400 ) == 0x0 ) {
+                phy_write( eng, 25, eng->phy.PHY_19h | 0x0400 );
+                printf("\n\n[Warning] Register 25, bit 10 must be 1 [Reg19h:%04lx]\n\n", eng->phy.PHY_19h);
+                if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Register 25, bit 10 must be 1 [Reg19h:%04lx]\n\n", eng->phy.PHY_19h );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Register 25, bit 10 must be 1 [Reg19h:%04lx]\n\n", eng->phy.PHY_19h );
+        }
+        //Check TXC Input/Output Direction
+        if ( eng->arg.GEn_RMIIPHY_IN == 0 ) {
+                if ( ( eng->phy.PHY_19h & 0x0800 ) == 0x0800 ) {
+                        phy_write( eng, 25, eng->phy.PHY_19h & 0xf7ff );
+                        printf("\n\n[Warning] Register 25, bit 11 must be 0 (TXC should be output mode)[Reg19h:%04lx]\n\n", eng->phy.PHY_19h);
+                        if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Register 25, bit 11 must be 0 (TXC should be output mode)[Reg19h:%04lx]\n\n", eng->phy.PHY_19h );
+                        if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Register 25, bit 11 must be 0 (TXC should be output mode)[Reg19h:%04lx]\n\n", eng->phy.PHY_19h );
+                }
+        } else {
+                if ( ( eng->phy.PHY_19h & 0x0800 ) == 0x0000 ) {
+                        phy_write( eng, 25, eng->phy.PHY_19h | 0x0800 );
+                        printf("\n\n[Warning] Register 25, bit 11 must be 1 (TXC should be input mode)[Reg19h:%04lx]\n\n", eng->phy.PHY_19h);
+                        if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Register 25, bit 11 must be 1 (TXC should be input mode)[Reg19h:%04lx]\n\n", eng->phy.PHY_19h );
+                        if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Register 25, bit 11 must be 1 (TXC should be input mode)[Reg19h:%04lx]\n\n", eng->phy.PHY_19h );
+                }
+        }
+
+        if ( eng->run.TM_IEEE ) {
+                phy_write( eng, 31, 0x0001 );
+                if ( eng->arg.GIEEE_sel == 0 ) {
+                        phy_write( eng, 25, 0x1f01 );//Force MDI  //Measuring from channel A
+                }
+                else {
+                        phy_write( eng, 25, 0x1f00 );//Force MDIX //Measuring from channel B
+                }
+                phy_write( eng, 31, 0x0000 );
+        }
+}
+
+//------------------------------------------------------------
+void recov_phy_realtek1 (MAC_ENGINE *eng) {//RTL8211D
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        if ( eng->run.Speed_sel[ 0 ] ) {
+                                if ( eng->arg.GIEEE_sel == 0 ) {//Test Mode 1
+                                        //Rev 1.2
+                                        phy_write( eng, 31, 0x0002 );
+                                        phy_write( eng,  2, 0xc203 );
+                                        phy_write( eng, 31, 0x0000 );
+                                        phy_write( eng,  9, 0x0000 );
+                                }
+                                else {//Test Mode 4
+                                        //Rev 1.2
+                                        phy_write( eng, 31, 0x0000 );
+                                        phy_write( eng,  9, 0x0000 );
+                                }
+                        }
+                        else if ( eng->run.Speed_sel[ 1 ] ) {
+                                //Rev 1.2
+                                phy_write( eng, 23, 0x2100 );
+                                phy_write( eng, 16, 0x016e );
+                        }
+                        else {
+                                //Rev 1.2
+                                phy_write( eng, 31, 0x0006 );
+                                phy_write( eng,  0, 0x5a00 );
+                                phy_write( eng, 31, 0x0000 );
+                        }
+                } else {
+                        phy_Reset( eng );
+                } // End if ( eng->run.TM_IEEE )
+        }
+        else if ( eng->phy.loop_phy ) {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 31, 0x0000 ); // new in Rev. 1.6
+                        phy_write( eng,  0, 0x1140 ); // new in Rev. 1.6
+                        phy_write( eng, 20, 0x8040 ); // new in Rev. 1.6
+                }
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 31, 0x0001 );
+                        phy_write( eng,  3, 0xdf41 );
+                        phy_write( eng,  2, 0xdf20 );
+                        phy_write( eng,  1, 0x0140 );
+                        phy_write( eng,  0, 0x00bb );
+                        phy_write( eng,  4, 0xb800 );
+                        phy_write( eng,  4, 0xb000 );
+
+                        phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng, 26, 0x0020 ); // Rev. 1.2
+                        phy_write( eng, 26, 0x0040 ); // new in Rev. 1.6
+                        phy_write( eng,  0, 0x1140 );
+//                      phy_write( eng, 21, 0x0006 ); // Rev. 1.2
+                        phy_write( eng, 21, 0x1006 ); // new in Rev. 1.6
+                        phy_write( eng, 23, 0x2100 );
+//              }
+//              else if ( eng->run.Speed_sel[ 1 ] ) {//option
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0200 );
+//                      phy_write( eng,  0, 0x1200 );
+//              }
+//              else if ( eng->run.Speed_sel[ 2 ] ) {//option
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0200 );
+//                      phy_write( eng,  4, 0x05e1 );
+//                      phy_write( eng,  0, 0x1200 );
+                }
+                phy_Reset( eng );
+                phy_delay(2000);
+        } // End if ( eng->run.TM_Burst )
+} // End void recov_phy_realtek1 (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+//internal loop 1G  : no  loopback stub
+//internal loop 100M: no  loopback stub
+//internal loop 10M : no  loopback stub
+void phy_realtek1 (MAC_ENGINE *eng) {//RTL8211D
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Realtek] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        if ( eng->run.Speed_sel[ 0 ] ) {
+                                if ( eng->arg.GIEEE_sel == 0 ) {//Test Mode 1
+                                        //Rev 1.2
+                                        phy_write( eng, 31, 0x0002 );
+                                        phy_write( eng,  2, 0xc22b );
+                                        phy_write( eng, 31, 0x0000 );
+                                        phy_write( eng,  9, 0x2000 );
+                                }
+                                else {//Test Mode 4
+                                        //Rev 1.2
+                                        phy_write( eng, 31, 0x0000 );
+                                        phy_write( eng,  9, 0x8000 );
+                                }
+                        }
+                        else if ( eng->run.Speed_sel[ 1 ] ) {
+                                if ( eng->arg.GIEEE_sel == 0 ) {//From Channel A
+                                        //Rev 1.2
+                                        phy_write( eng, 23, 0xa102 );
+                                        phy_write( eng, 16, 0x01ae );//MDI
+                                }
+                                else {//From Channel B
+                                        //Rev 1.2
+                                        phy_Read_Write( eng, 17, 0x0008, 0x0000 ); // clr set
+                                        phy_write( eng, 23, 0xa102 );         // MDI
+                                        phy_write( eng, 16, 0x010e );
+                                }
+                        }
+                        else {
+                                if ( eng->arg.GIEEE_sel == 0 ) {//Diff. Voltage/TP-IDL/Jitter: Pseudo-random pattern
+                                        phy_write( eng, 31, 0x0006 );
+                                        phy_write( eng,  0, 0x5a21 );
+                                        phy_write( eng, 31, 0x0000 );
+                                }
+                                else if ( eng->arg.GIEEE_sel == 1 ) {//Harmonic: pattern
+                                        phy_write( eng, 31, 0x0006 );
+                                        phy_write( eng,  2, 0x05ee );
+                                        phy_write( eng,  0, 0xff21 );
+                                        phy_write( eng, 31, 0x0000 );
+                                }
+                                else {//Harmonic: “00” pattern
+                                        phy_write( eng, 31, 0x0006 );
+                                        phy_write( eng,  2, 0x05ee );
+                                        phy_write( eng,  0, 0x0021 );
+                                        phy_write( eng, 31, 0x0000 );
+                                }
+                        }
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_Reset( eng );
+
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 20, 0x0042 );//new in Rev. 1.6
+                }
+        }
+        else {
+        // refer to RTL8211D Register for Manufacture Test_V1.6.pdf
+        // MDI loop back
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 31, 0x0001 );
+                        phy_write( eng,  3, 0xff41 );
+                        phy_write( eng,  2, 0xd720 );
+                        phy_write( eng,  1, 0x0140 );
+                        phy_write( eng,  0, 0x00bb );
+                        phy_write( eng,  4, 0xb800 );
+                        phy_write( eng,  4, 0xb000 );
+
+                        phy_write( eng, 31, 0x0007 );
+                        phy_write( eng, 30, 0x0040 );
+                        phy_write( eng, 24, 0x0008 );
+
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  9, 0x0300 );
+                        phy_write( eng, 26, 0x0020 );
+                        phy_write( eng,  0, 0x0140 );
+                        phy_write( eng, 23, 0xa101 );
+                        phy_write( eng, 21, 0x0200 );
+                        phy_write( eng, 23, 0xa121 );
+                        phy_write( eng, 23, 0xa161 );
+                        phy_write( eng,  0, 0x8000 );
+                        phy_Wait_Reset_Done( eng );
+
+//                      phy_delay(200); // new in Rev. 1.6
+                        phy_delay(5000); // 20150504
+//              }
+//              else if ( eng->run.Speed_sel[ 1 ] ) {//option
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0000 );
+//                      phy_write( eng,  4, 0x0061 );
+//                      phy_write( eng,  0, 0x1200 );
+//                      phy_delay(5000);
+//              }
+//              else if ( eng->run.Speed_sel[ 2 ] ) {//option
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0000 );
+//                      phy_write( eng,  4, 0x05e1 );
+//                      phy_write( eng,  0, 0x1200 );
+//                      phy_delay(5000);
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+} // End void phy_realtek1 (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void recov_phy_realtek2 (MAC_ENGINE *eng) {//RTL8211E
+#ifdef Realtek_debug
+printf ("\nClear RTL8211E [Start] =====>\n");
+#endif
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        if ( eng->run.Speed_sel[ 0 ] ) {
+                                //Rev 1.2
+                                phy_write( eng, 31, 0x0000 );
+                                phy_write( eng,  9, 0x0000 );
+                        }
+                        else if ( eng->run.Speed_sel[ 1 ] ) {
+                                //Rev 1.2
+                                phy_write( eng, 31, 0x0007 );
+                                phy_write( eng, 30, 0x002f );
+                                phy_write( eng, 23, 0xd88f );
+                                phy_write( eng, 30, 0x002d );
+                                phy_write( eng, 24, 0xf050 );
+                                phy_write( eng, 31, 0x0000 );
+                                phy_write( eng, 16, 0x006e );
+                        }
+                        else {
+                                //Rev 1.2
+                                phy_write( eng, 31, 0x0006 );
+                                phy_write( eng,  0, 0x5a00 );
+                                phy_write( eng, 31, 0x0000 );
+                        }
+                        //Rev 1.2
+                        phy_write( eng, 31, 0x0005 );
+                        phy_write( eng,  5, 0x8b86 );
+                        phy_write( eng,  6, 0xe201 );
+                        phy_write( eng, 31, 0x0007 );
+                        phy_write( eng, 30, 0x0020 );
+                        phy_write( eng, 21, 0x1108 );
+                        phy_write( eng, 31, 0x0000 );
+                }
+                else {
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        //Rev 1.5  //not stable
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  0, 0x8000 );
+//                      phy_Wait_Reset_Done( eng );
+//                      phy_delay(30);
+//                      phy_write( eng, 23, 0x2160 );
+//                      phy_write( eng, 31, 0x0007 );
+//                      phy_write( eng, 30, 0x0040 );
+//                      phy_write( eng, 24, 0x0004 );
+//                      phy_write( eng, 24, 0x1a24 );
+//                      phy_write( eng, 25, 0xfd00 );
+//                      phy_write( eng, 24, 0x0000 );
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  0, 0x1140 );
+//                      phy_write( eng, 26, 0x0040 );
+//                      phy_write( eng, 31, 0x0007 );
+//                      phy_write( eng, 30, 0x002f );
+//                      phy_write( eng, 23, 0xd88f );
+//                      phy_write( eng, 30, 0x0023 );
+//                      phy_write( eng, 22, 0x0300 );
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng, 21, 0x1006 );
+//                      phy_write( eng, 23, 0x2100 );
+
+                        //Rev 1.6
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  0, 0x8000 );
+#ifdef Realtek_debug
+#else
+                        phy_Wait_Reset_Done( eng );
+                        phy_delay(30);
+#endif
+
+                        phy_write( eng, 31, 0x0007 );
+                        phy_write( eng, 30, 0x0042 );
+                        phy_write( eng, 21, 0x0500 );
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  0, 0x1140 );
+                        phy_write( eng, 26, 0x0040 );
+                        phy_write( eng, 31, 0x0007 );
+                        phy_write( eng, 30, 0x002f );
+                        phy_write( eng, 23, 0xd88f );
+                        phy_write( eng, 30, 0x0023 );
+                        phy_write( eng, 22, 0x0300 );
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng, 21, 0x1006 );
+                        phy_write( eng, 23, 0x2100 );
+                }
+//              else if ( eng->run.Speed_sel[ 1 ] ) {//option
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0200 );
+//                      phy_write( eng,  0, 0x1200 );
+//              }
+//              else if ( eng->run.Speed_sel[ 2 ] ) {//option
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0200 );
+//                      phy_write( eng,  4, 0x05e1 );
+//                      phy_write( eng,  0, 0x1200 );
+//              }
+                else {
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  0, 0x1140 );
+                }
+#ifdef Realtek_debug
+#else
+                // Check register 0x11 bit10 Link OK or not OK
+                phy_check_register ( eng, 17, 0x0c02, 0x0000, 10, "clear RTL8211E");
+#endif
+        }
+#ifdef Realtek_debug
+printf ("\nClear RTL8211E [End] =====>\n");
+#endif
+} // End void recov_phy_realtek2 (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+//internal loop 1G  : no  loopback stub
+//internal loop 100M: no  loopback stub
+//internal loop 10M : no  loopback stub
+void phy_realtek2 (MAC_ENGINE *eng) {//RTL8211E
+        USHORT     check_value;
+#ifdef Realtek_debug
+printf ("\nSet RTL8211E [Start] =====>\n");
+GPIO_20h_Value = Read_Reg_GPIO_DD( 0x20 );
+GPIO_24h_Value = Read_Reg_GPIO_DD( 0x24 ) | 0x00400000;
+
+Write_Reg_GPIO_DD( 0x24, GPIO_24h_Value );
+#endif
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Realtek] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+#ifdef Realtek_debug
+#else
+        phy_write( eng, 31, 0x0000 );
+        phy_Read_Write( eng,  0, 0x0000, 0x8000 | eng->phy.PHY_00h ); // clr set // Rst PHY
+        phy_Wait_Reset_Done( eng );
+        phy_delay(30);
+#endif
+
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        //Rev 1.2
+                        phy_write( eng, 31, 0x0005 );
+                        phy_write( eng,  5, 0x8b86 );
+                        phy_write( eng,  6, 0xe200 );
+                        phy_write( eng, 31, 0x0007 );
+                        phy_write( eng, 30, 0x0020 );
+                        phy_write( eng, 21, 0x0108 );
+                        phy_write( eng, 31, 0x0000 );
+
+                        if ( eng->run.Speed_sel[ 0 ] ) {
+                                //Rev 1.2
+                                phy_write( eng, 31, 0x0000 );
+
+                                if ( eng->arg.GIEEE_sel == 0 ) {
+                                        phy_write( eng,  9, 0x2000 );//Test Mode 1
+                                }
+                                else {
+                                        phy_write( eng,  9, 0x8000 );//Test Mode 4
+                                }
+                        }
+                        else if ( eng->run.Speed_sel[ 1 ] ) {
+                                //Rev 1.2
+                                phy_write( eng, 31, 0x0007 );
+                                phy_write( eng, 30, 0x002f );
+                                phy_write( eng, 23, 0xd818 );
+                                phy_write( eng, 30, 0x002d );
+                                phy_write( eng, 24, 0xf060 );
+                                phy_write( eng, 31, 0x0000 );
+
+                                if ( eng->arg.GIEEE_sel == 0 ) {
+                                        phy_write( eng, 16, 0x00ae );//From Channel A
+                                }
+                                else {
+                                        phy_write( eng, 16, 0x008e );//From Channel B
+                                }
+                        }
+                        else {
+                                //Rev 1.2
+                                phy_write( eng, 31, 0x0006 );
+                                if ( eng->arg.GIEEE_sel == 0 ) {//Diff. Voltage/TP-IDL/Jitter
+                                        phy_write( eng,  0, 0x5a21 );
+                                }
+                                else if ( eng->arg.GIEEE_sel == 1 ) {//Harmonic: “FF” pattern
+                                        phy_write( eng,  2, 0x05ee );
+                                        phy_write( eng,  0, 0xff21 );
+                                }
+                                else {//Harmonic: “00” pattern
+                                        phy_write( eng,  2, 0x05ee );
+                                        phy_write( eng,  0, 0x0021 );
+                                }
+                                phy_write( eng, 31, 0x0000 );
+                        }
+                }
+                else {
+                        phy_basic_setting( eng );
+                        phy_delay(30);
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+#ifdef Realtek_debug
+                phy_write( eng,  0, 0x0000 );
+                phy_write( eng,  0, 0x8000 );
+                phy_delay(60);
+                phy_write( eng,  0, eng->phy.PHY_00h );
+                phy_delay(60);
+#else
+                phy_basic_setting( eng );
+
+                phy_Read_Write( eng,  0, 0x0000, 0x8000 | eng->phy.PHY_00h );//clr set//Rst PHY
+                phy_Wait_Reset_Done( eng );
+                phy_delay(30);
+
+                phy_basic_setting( eng );
+                phy_delay(30);
+#endif
+        }
+        else {
+#ifdef Enable_Dual_Mode
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        check_value = 0x0c02 | 0xa000;
+                        //set GPIO
+                }
+                else if ( eng->run.Speed_sel[ 1 ] ) {
+                        check_value = 0x0c02 | 0x6000;
+                        //set GPIO
+                }
+                else if ( eng->run.Speed_sel[ 2 ] ) {
+                        check_value = 0x0c02 | 0x2000;
+                        //set GPIO
+                }
+#else
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        check_value = 0x0c02 | 0xa000;
+                        //Rev 1.5  //not stable
+//                      phy_write( eng, 23, 0x2160 );
+//                      phy_write( eng, 31, 0x0007 );
+//                      phy_write( eng, 30, 0x0040 );
+//                      phy_write( eng, 24, 0x0004 );
+//                      phy_write( eng, 24, 0x1a24 );
+//                      phy_write( eng, 25, 0x7d00 );
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng, 23, 0x2100 );
+//                      phy_write( eng, 31, 0x0007 );
+//                      phy_write( eng, 30, 0x0040 );
+//                      phy_write( eng, 24, 0x0000 );
+//                      phy_write( eng, 30, 0x0023 );
+//                      phy_write( eng, 22, 0x0006 );
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  0, 0x0140 );
+//                      phy_write( eng, 26, 0x0060 );
+//                      phy_write( eng, 31, 0x0007 );
+//                      phy_write( eng, 30, 0x002f );
+//                      phy_write( eng, 23, 0xd820 );
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng, 21, 0x0206 );
+//                      phy_write( eng, 23, 0x2120 );
+//                      phy_write( eng, 23, 0x2160 );
+
+                        //Rev 1.6
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  0, 0x8000 );
+//                      phy_Wait_Reset_Done( eng );
+//                      phy_delay(30);
+  #ifdef Realtek_debug
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  0, 0x8000 );
+                        phy_delay(60);
+  #endif
+
+                        phy_write( eng, 31, 0x0007 );
+                        phy_write( eng, 30, 0x0042 );
+                        phy_write( eng, 21, 0x2500 );
+                        phy_write( eng, 30, 0x0023 );
+                        phy_write( eng, 22, 0x0006 );
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  0, 0x0140 );
+                        phy_write( eng, 26, 0x0060 );
+                        phy_write( eng, 31, 0x0007 );
+                        phy_write( eng, 30, 0x002f );
+                        phy_write( eng, 23, 0xd820 );
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng, 21, 0x0206 );
+                        phy_write( eng, 23, 0x2120 );
+                        phy_write( eng, 23, 0x2160 );
+  #ifdef Realtek_debug
+                        phy_delay(600);
+  #else
+                        phy_delay(300);
+  #endif
+                }
+//              else if ( eng->run.Speed_sel[ 1 ] ) {//option
+//                      check_value = 0x0c02 | 0x6000;
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0000 );
+//                      phy_write( eng,  4, 0x05e1 );
+//                      phy_write( eng,  0, 0x1200 );
+//                      phy_delay(6000);
+//              }
+//              else if ( eng->run.Speed_sel[ 2 ] ) {//option
+//                      check_value = 0x0c02 | 0x2000;
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0000 );
+//                      phy_write( eng,  4, 0x0061 );
+//                      phy_write( eng,  0, 0x1200 );
+//                      phy_delay(6000);
+//              }
+                else {
+                        if ( eng->run.Speed_sel[ 1 ] )
+                                check_value = 0x0c02 | 0x6000;
+                        else
+                                check_value = 0x0c02 | 0x2000;
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  0, eng->phy.PHY_00h );
+  #ifdef Realtek_debug
+                        phy_delay(300);
+  #else
+                        phy_delay(150);
+  #endif
+                }
+#endif
+#ifdef Realtek_debug
+#else
+                // Check register 0x11 bit10 Link OK or not OK
+                phy_check_register ( eng, 17, 0x0c02 | 0xe000, check_value, 10, "set RTL8211E");
+#endif
+        }
+#ifdef Realtek_debug
+printf ("\nSet RTL8211E [End] =====>\n");
+#endif
+} // End void phy_realtek2 (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void recov_phy_realtek3 (MAC_ENGINE *eng) {//RTL8211C
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        if ( eng->run.Speed_sel[ 0 ] ) {
+                                phy_write( eng,  9, 0x0000 );
+                        }
+                        else if ( eng->run.Speed_sel[ 1 ] ) {
+                                phy_write( eng, 17, eng->phy.PHY_11h );
+                                phy_write( eng, 14, 0x0000 );
+                                phy_write( eng, 16, 0x00a0 );
+                        }
+                        else {
+//                              phy_write( eng, 31, 0x0006 );
+//                              phy_write( eng,  0, 0x5a00 );
+//                              phy_write( eng, 31, 0x0000 );
+                        }
+                }
+                else {
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 11, 0x0000 );
+                }
+                phy_write( eng, 12, 0x1006 );
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 31, 0x0001 );
+                        phy_write( eng,  4, 0xb000 );
+                        phy_write( eng,  3, 0xff41 );
+                        phy_write( eng,  2, 0xdf20 );
+                        phy_write( eng,  1, 0x0140 );
+                        phy_write( eng,  0, 0x00bb );
+                        phy_write( eng,  4, 0xb800 );
+                        phy_write( eng,  4, 0xb000 );
+
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng, 25, 0x8c00 );
+                        phy_write( eng, 26, 0x0040 );
+                        phy_write( eng,  0, 0x1140 );
+                        phy_write( eng, 14, 0x0000 );
+                        phy_write( eng, 12, 0x1006 );
+                        phy_write( eng, 23, 0x2109 );
+                }
+        }
+}
+
+//------------------------------------------------------------
+void phy_realtek3 (MAC_ENGINE *eng) {//RTL8211C
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Realtek] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        if ( eng->run.Speed_sel[ 0 ] ) {
+                                if ( eng->arg.GIEEE_sel == 0 ) {   //Test Mode 1
+                                        phy_write( eng,  9, 0x2000 );
+                                }
+                                else if ( eng->arg.GIEEE_sel == 1 ) {//Test Mode 2
+                                        phy_write( eng,  9, 0x4000 );
+                                }
+                                else if ( eng->arg.GIEEE_sel == 2 ) {//Test Mode 3
+                                        phy_write( eng,  9, 0x6000 );
+                                }
+                                else {                           //Test Mode 4
+                                        phy_write( eng,  9, 0x8000 );
+                                }
+                        }
+                        else if ( eng->run.Speed_sel[ 1 ] ) {
+                                eng->phy.PHY_11h = phy_read( eng, PHY_SR );
+                                phy_write( eng, 17, eng->phy.PHY_11h & 0xfff7 );
+                                phy_write( eng, 14, 0x0660 );
+
+                                if ( eng->arg.GIEEE_sel == 0 ) {
+                                        phy_write( eng, 16, 0x00a0 );//MDI  //From Channel A
+                                }
+                                else {
+                                        phy_write( eng, 16, 0x0080 );//MDIX //From Channel B
+                                }
+                        }
+                        else {
+//                              if ( eng->arg.GIEEE_sel == 0 ) {//Pseudo-random pattern
+//                                      phy_write( eng, 31, 0x0006 );
+//                                      phy_write( eng,  0, 0x5a21 );
+//                                      phy_write( eng, 31, 0x0000 );
+//                              }
+//                              else if ( eng->arg.GIEEE_sel == 1 ) {//“FF” pattern
+//                                      phy_write( eng, 31, 0x0006 );
+//                                      phy_write( eng,  2, 0x05ee );
+//                                      phy_write( eng,  0, 0xff21 );
+//                                      phy_write( eng, 31, 0x0000 );
+//                              }
+//                              else {//“00” pattern
+//                                      phy_write( eng, 31, 0x0006 );
+//                                      phy_write( eng,  2, 0x05ee );
+//                                      phy_write( eng,  0, 0x0021 );
+//                                      phy_write( eng, 31, 0x0000 );
+//                              }
+                        }
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_write( eng,  0, 0x9200 );
+                phy_Wait_Reset_Done( eng );
+                phy_delay(30);
+
+                phy_write( eng, 17, 0x401c );
+                phy_write( eng, 12, 0x0006 );
+
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 11, 0x0002 );
+                }
+                else {
+                        phy_basic_setting( eng );
+                }
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 31, 0x0001 );
+                        phy_write( eng,  4, 0xb000 );
+                        phy_write( eng,  3, 0xff41 );
+                        phy_write( eng,  2, 0xd720 );
+                        phy_write( eng,  1, 0x0140 );
+                        phy_write( eng,  0, 0x00bb );
+                        phy_write( eng,  4, 0xb800 );
+                        phy_write( eng,  4, 0xb000 );
+
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng, 25, 0x8400 );
+                        phy_write( eng, 26, 0x0020 );
+                        phy_write( eng,  0, 0x0140 );
+                        phy_write( eng, 14, 0x0210 );
+                        phy_write( eng, 12, 0x0200 );
+                        phy_write( eng, 23, 0x2109 );
+                        phy_write( eng, 23, 0x2139 );
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+} // End void phy_realtek3 (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+//external loop 100M: OK
+//external loop 10M : OK
+//internal loop 100M: no  loopback stub
+//internal loop 10M : no  loopback stub
+void phy_realtek4 (MAC_ENGINE *eng) {//RTL8201F
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Realtek] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        eng->phy.RMIICK_IOMode = eng->phy.RMIICK_IOMode | PHY_Flag_RMIICK_IOMode_RTL8201F;
+
+        phy_write( eng, 31, 0x0007 );
+        eng->phy.PHY_10h = phy_read( eng, 16 );
+        //Check RMII Mode
+        if ( ( eng->phy.PHY_10h & 0x0008 ) == 0x0 ) {
+                phy_write( eng, 16, eng->phy.PHY_10h | 0x0008 );
+                printf("\n\n[Warning] Page 7 Register 16, bit 3 must be 1 [Reg10h_7:%04lx]\n\n", eng->phy.PHY_10h);
+                if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Page 7 Register 16, bit 3 must be 1 [Reg10h_7:%04lx]\n\n", eng->phy.PHY_10h );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Page 7 Register 16, bit 3 must be 1 [Reg10h_7:%04lx]\n\n", eng->phy.PHY_10h );
+        }
+        //Check TXC Input/Output Direction
+        if ( eng->arg.GEn_RMIIPHY_IN == 0 ) {
+                if ( ( eng->phy.PHY_10h & 0x1000 ) == 0x1000 ) {
+                        phy_write( eng, 16, eng->phy.PHY_10h & 0xefff );
+                        printf("\n\n[Warning] Page 7 Register 16, bit 12 must be 0 (TXC should be output mode)[Reg10h_7:%04lx]\n\n", eng->phy.PHY_10h);
+                        if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Page 7 Register 16, bit 12 must be 0 (TXC should be output mode)[Reg10h_7:%04lx]\n\n", eng->phy.PHY_10h );
+                        if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Page 7 Register 16, bit 12 must be 0 (TXC should be output mode)[Reg10h_7:%04lx]\n\n", eng->phy.PHY_10h );
+                }
+        } else {
+                if ( ( eng->phy.PHY_10h & 0x1000 ) == 0x0000 ) {
+                        phy_write( eng, 16, eng->phy.PHY_10h | 0x1000 );
+                        printf("\n\n[Warning] Page 7 Register 16, bit 12 must be 1 (TXC should be input mode)[Reg10h_7:%04lx]\n\n", eng->phy.PHY_10h);
+                        if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Page 7 Register 16, bit 12 must be 1 (TXC should be input mode)[Reg10h_7:%04lx]\n\n", eng->phy.PHY_10h );
+                        if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Page 7 Register 16, bit 12 must be 1 (TXC should be input mode)[Reg10h_7:%04lx]\n\n", eng->phy.PHY_10h );
+                }
+        }
+        phy_write( eng, 31, 0x0000 );
+
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        //Rev 1.0
+                        phy_write( eng, 31, 0x0004 );
+                        phy_write( eng, 16, 0x4077 );
+                        phy_write( eng, 21, 0xc5a0 );
+                        phy_write( eng, 31, 0x0000 );
+
+                        if ( eng->run.Speed_sel[ 1 ] ) {
+                                phy_write( eng,  0, 0x8000 ); // Reset PHY
+                                phy_Wait_Reset_Done( eng );
+                                phy_write( eng, 24, 0x0310 ); // Disable ALDPS
+
+                                if ( eng->arg.GIEEE_sel == 0 ) {//From Channel A (RJ45 pair 1, 2)
+                                        phy_write( eng, 28, 0x40c2 ); //Force MDI
+                                }
+                                else {//From Channel B (RJ45 pair 3, 6)
+                                        phy_write( eng, 28, 0x40c0 ); //Force MDIX
+                                }
+                                phy_write( eng,  0, 0x2100 );       //Force 100M/Full Duplex)
+                        } else {
+                        }
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+                // Internal loopback
+                if ( eng->run.Speed_sel[ 1 ] ) {
+                        // Enable 100M PCS loop back; RTL8201(F_FL_FN)-VB-CG_DataSheet_1.6.pdf
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  0, 0x6100 );
+                        phy_write( eng, 31, 0x0007 );
+                        phy_write( eng, 16, 0x1FF8 );
+                        phy_write( eng, 16, 0x0FF8 );
+                        phy_write( eng, 31, 0x0000 );
+                        phy_delay(20);
+                } else if ( eng->run.Speed_sel[ 2 ] ) {
+                        // Enable 10M PCS loop back; RTL8201(F_FL_FN)-VB-CG_DataSheet_1.6.pdf
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  0, 0x4100 );
+                        phy_write( eng, 31, 0x0007 );
+                        phy_write( eng, 16, 0x1FF8 );
+                        phy_write( eng, 16, 0x0FF8 );
+                        phy_write( eng, 31, 0x0000 );
+                        phy_delay(20);
+                }
+        }
+        else {
+                // External loopback
+                if ( eng->run.Speed_sel[ 1 ] ) {
+                        // Enable 100M MDI loop back Nway option; RTL8201(F_FL_FN)-VB-CG_DataSheet_1.6.pdf
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  4, 0x01E1 );
+                        phy_write( eng,  0, 0x1200 );
+                } else if ( eng->run.Speed_sel[ 2 ] ) {
+                        // Enable 10M MDI loop back Nway option; RTL8201(F_FL_FN)-VB-CG_DataSheet_1.6.pdf
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  4, 0x0061 );
+                        phy_write( eng,  0, 0x1200 );
+                }
+//              phy_write( eng,  0, 0x8000 );
+//              while ( phy_read( eng, 0 ) != 0x3100 ) {}
+//              while ( phy_read( eng, 0 ) != 0x3100 ) {}
+//              phy_write( eng,  0, eng->phy.PHY_00h );
+////            phy_delay(100);
+//              phy_delay(400);
+
+                // Check register 0x1 bit2 Link OK or not OK
+                phy_check_register ( eng, 1, 0x0004, 0x0004, 10, "set RTL8201F");
+                phy_delay(300);
+        }
+}
+
+//------------------------------------------------------------
+void recov_phy_realtek5 (MAC_ENGINE *eng) {//RTL8211F
+#ifdef Realtek_debug
+printf ("\nClear RTL8211F [Start] =====>\n");
+#endif
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        if ( eng->run.Speed_sel[ 0 ] ) {
+                                //Rev 1.0
+                                phy_write( eng, 31, 0x0000 );
+                                phy_write( eng,  9, 0x0000 );
+                        }
+                        else if ( eng->run.Speed_sel[ 1 ] ) {
+                                //Rev 1.0
+                                phy_write( eng, 31, 0x0000 );
+                                phy_write( eng, 24, 0x2118 );//RGMII
+                                phy_write( eng,  9, 0x0200 );
+                                phy_write( eng,  0, 0x9200 );
+                                phy_Wait_Reset_Done( eng );
+                        }
+                        else {
+                                //Rev 1.0
+                                phy_write( eng, 31, 0x0c80 );
+                                phy_write( eng, 16, 0x5a00 );
+                                phy_write( eng, 31, 0x0000 );
+                                phy_write( eng,  4, 0x01e1 );
+                                phy_write( eng,  9, 0x0200 );
+                                phy_write( eng,  0, 0x9200 );
+                                phy_Wait_Reset_Done( eng );
+                        }
+                }
+                else {
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        //Rev 1.1
+                        phy_write( eng, 31, 0x0a43 );
+                        phy_write( eng, 24, 0x2118 );
+                        phy_write( eng,  0, 0x1040 );
+                }
+//              else if ( eng->run.Speed_sel[ 1 ] ) {//option
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0200 );
+//                      phy_write( eng,  0, 0x1200 );
+//              }
+//              else if ( eng->run.Speed_sel[ 2 ] ) {//option
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0200 );
+//                      phy_write( eng,  4, 0x01e1 );
+//                      phy_write( eng,  0, 0x1200 );
+//              }
+                else {
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  0, 0x1040 );
+                }
+
+#ifdef Realtek_debug
+#else
+                // Check register 0x1A bit2 Link OK or not OK
+                phy_write( eng, 31, 0x0a43 );
+                phy_check_register ( eng, 26, 0x0004, 0x0000, 10, "clear RTL8211F");
+                phy_write( eng, 31, 0x0000 );
+#endif
+        }
+#ifdef Realtek_debug
+printf ("\nClear RTL8211F [End] =====>\n");
+#endif
+}
+
+//------------------------------------------------------------
+void phy_realtek5 (MAC_ENGINE *eng) {//RTL8211F
+        USHORT     check_value;
+#ifdef Realtek_debug
+printf ("\nSet RTL8211F [Start] =====>\n");
+#endif
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Realtek] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        if ( eng->run.Speed_sel[ 0 ] ) {
+                                //Rev 1.0
+                                phy_write( eng, 31, 0x0000 );
+                                if ( eng->arg.GIEEE_sel == 0 ) {//Test Mode 1
+                                        phy_write( eng,  9, 0x0200 );
+                                }
+                                else if ( eng->arg.GIEEE_sel == 1 ) {//Test Mode 2
+                                        phy_write( eng,  9, 0x0400 );
+                                }
+                                else {//Test Mode 4
+                                        phy_write( eng,  9, 0x0800 );
+                                }
+                        }
+                        else if ( eng->run.Speed_sel[ 1 ] ) {//option
+                                //Rev 1.0
+                                phy_write( eng, 31, 0x0000 );
+                                if ( eng->arg.GIEEE_sel == 0 ) {//Output MLT-3 from Channel A
+                                        phy_write( eng, 24, 0x2318 );
+                                }
+                                else {//Output MLT-3 from Channel B
+                                        phy_write( eng, 24, 0x2218 );
+                                }
+                                phy_write( eng,  9, 0x0000 );
+                                phy_write( eng,  0, 0x2100 );
+                        }
+                        else {
+                                //Rev 1.0
+                                //0: For Diff. Voltage/TP-IDL/Jitter with EEE
+                                //1: For Diff. Voltage/TP-IDL/Jitter without EEE
+                                //2: For Harmonic (all "1" patten) with EEE
+                                //3: For Harmonic (all "1" patten) without EEE
+                                //4: For Harmonic (all "0" patten) with EEE
+                                //5: For Harmonic (all "0" patten) without EEE
+                                phy_write( eng, 31, 0x0000 );
+                                phy_write( eng,  9, 0x0000 );
+                                phy_write( eng,  4, 0x0061 );
+                                if ( (eng->arg.GIEEE_sel & 0x1) == 0 ) {//with EEE
+                                        phy_write( eng, 25, 0x0853 );
+                                }
+                                else {//without EEE
+                                        phy_write( eng, 25, 0x0843 );
+                                }
+                                phy_write( eng,  0, 0x9200 );
+                                phy_Wait_Reset_Done( eng );
+
+                                if ( (eng->arg.GIEEE_sel & 0x6) == 0 ) {//For Diff. Voltage/TP-IDL/Jitter
+                                        phy_write( eng, 31, 0x0c80 );
+                                        phy_write( eng, 18, 0x0115 );
+                                        phy_write( eng, 16, 0x5a21 );
+                                }
+                                else if ( (eng->arg.GIEEE_sel & 0x6) == 0x2 ) {//For Harmonic (all "1" patten)
+                                        phy_write( eng, 31, 0x0c80 );
+                                        phy_write( eng, 18, 0x0015 );
+                                        phy_write( eng, 16, 0xff21 );
+                                }
+                                else {//For Harmonic (all "0" patten)
+                                        phy_write( eng, 31, 0x0c80 );
+                                        phy_write( eng, 18, 0x0015 );
+                                        phy_write( eng, 16, 0x0021 );
+                                }
+                                phy_write( eng, 31, 0x0000 );
+                        }
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_Reset( eng );
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        check_value = 0x0004 | 0x0028;
+                        //Rev 1.1
+                        phy_write( eng, 31, 0x0a43 );
+                        phy_write( eng,  0, 0x8000 );
+#ifdef Realtek_debug
+                        phy_delay(60);
+#else
+                        phy_Wait_Reset_Done( eng );
+                        phy_delay(30);
+#endif
+
+                        phy_write( eng,  0, 0x0140 );
+                        phy_write( eng, 24, 0x2d18 );
+#ifdef Realtek_debug
+                        phy_delay(600);
+#else
+                        phy_delay(300);
+#endif
+                }
+//              else if ( eng->run.Speed_sel[ 1 ] ) {//option
+//                      check_value = 0x0004 | 0x0018;
+//                      phy_write( eng, 31, 0x0a43 );
+//                      phy_write( eng,  0, 0x8000 );
+//                      phy_Wait_Reset_Done( eng );
+//                      phy_delay(30);
+//
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0000 );
+//                      phy_write( eng,  4, 0x01e1 );
+//                      phy_write( eng,  0, 0x1200 );
+//                      phy_delay(6000);
+//              }
+//              else if ( eng->run.Speed_sel[ 2 ] ) {//option
+//                      check_value = 0x0004 | 0x0008;
+//                      phy_write( eng, 31, 0x0a43 );
+//                      phy_write( eng,  0, 0x8000 );
+//                      phy_Wait_Reset_Done( eng );
+//                      phy_delay(30);
+//
+//                      phy_write( eng, 31, 0x0000 );
+//                      phy_write( eng,  9, 0x0000 );
+//                      phy_write( eng,  4, 0x0061 );
+//                      phy_write( eng,  0, 0x1200 );
+//                      phy_delay(6000);
+//              }
+                else {
+                        if ( eng->run.Speed_sel[ 1 ] )
+                                check_value = 0x0004 | 0x0018;
+                        else
+                                check_value = 0x0004 | 0x0008;
+#ifdef Realtek_debug
+#else
+                        phy_write( eng, 31, 0x0a43 );
+                        phy_write( eng,  0, 0x8000 );
+                        phy_Wait_Reset_Done( eng );
+                        phy_delay(30);
+#endif
+
+                        phy_write( eng, 31, 0x0000 );
+                        phy_write( eng,  0, eng->phy.PHY_00h );
+#ifdef Realtek_debug
+                        phy_delay(300);
+#else
+                        phy_delay(150);
+#endif
+                }
+
+#ifdef Realtek_debug
+#else
+                // Check register 0x1A bit2 Link OK or not OK
+                phy_write( eng, 31, 0x0a43 );
+                phy_check_register ( eng, 26, 0x0004 | 0x0038, check_value, 10, "set RTL8211F");
+                phy_write( eng, 31, 0x0000 );
+#endif
+        }
+#ifdef Realtek_debug
+printf ("\nSet RTL8211F [End] =====>\n");
+#endif
+}
+
+//------------------------------------------------------------
+//It is a LAN Switch, only support 1G internal loopback test.
+void phy_realtek6 (MAC_ENGINE *eng) {//RTL8363S
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Realtek] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        if ( eng->run.TM_Burst ) {
+                printf("This mode doesn't support in RTL8363S.\n");
+        }
+        else if ( eng->phy.loop_phy ) {
+
+                // RXDLY2 and TXDLY2 of RTL8363S should set to LOW
+                phy_basic_setting( eng );
+
+                phy_Read_Write( eng,  0, 0x0000, 0x8000 | eng->phy.PHY_00h );//clr set//Rst PHY
+                phy_Wait_Reset_Done( eng );
+                phy_delay(30);
+
+                phy_basic_setting( eng );
+                phy_delay(30);
+        }
+        else {
+                printf("This mode doesn't support in RTL8363S\n");
+        }
+} // End void phy_realtek6 (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+void phy_smsc (MAC_ENGINE *eng) {//LAN8700
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[SMSC] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        phy_Reset( eng );
+}
+
+//------------------------------------------------------------
+void phy_micrel (MAC_ENGINE *eng) {//KSZ8041
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Micrel] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        phy_Reset( eng );
+
+//      phy_write( eng, 24, 0x0600 );
+}
+
+//------------------------------------------------------------
+void phy_micrel0 (MAC_ENGINE *eng) {//KSZ8031/KSZ8051
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Micrel] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        //For KSZ8051RNL only
+        //Reg1Fh[7] = 0(default): 25MHz Mode, XI, XO(pin 9, 8) is 25MHz(crystal/oscilator).
+        //Reg1Fh[7] = 1         : 50MHz Mode, XI(pin 9) is 50MHz(oscilator).
+        eng->phy.PHY_1fh = phy_read( eng, 31 );
+        if ( eng->phy.PHY_1fh & 0x0080 ) sprintf(eng->phy.PHYName, "%s-50MHz Mode", eng->phy.PHYName);
+        else                             sprintf(eng->phy.PHYName, "%s-25MHz Mode", eng->phy.PHYName);
+
+        if ( eng->run.TM_IEEE ) {
+                phy_Read_Write( eng,  0, 0x0000, 0x8000 | eng->phy.PHY_00h );//clr set//Rst PHY
+                phy_Wait_Reset_Done( eng );
+
+                phy_Read_Write( eng, 31, 0x0000, 0x2000 );//clr set//1Fh[13] = 1: Disable auto MDI/MDI-X
+                phy_basic_setting( eng );
+                phy_Read_Write( eng, 31, 0x0000, 0x0800 );//clr set//1Fh[11] = 1: Force link pass
+
+//              phy_delay(2500);//2.5 sec
+        }
+        else {
+                phy_Reset( eng );
+
+                //Reg16h[6] = 1         : RMII B-to-B override
+                //Reg16h[1] = 1(default): RMII override
+                phy_Read_Write( eng, 22, 0x0000, 0x0042 );//clr set
+        }
+
+        if ( eng->phy.PHY_1fh & 0x0080 )
+                phy_Read_Write( eng, 31, 0x0000, 0x0080 );//clr set//Reset PHY will clear Reg1Fh[7]
+}
+
+//------------------------------------------------------------
+//external loop 1G  : NOT Support
+//external loop 100M: OK
+//external loop 10M : OK
+//internal loop 1G  : no  loopback stub
+//internal loop 100M: no  loopback stub
+//internal loop 10M : no  loopback stub
+void phy_micrel1 (MAC_ENGINE *eng) {//KSZ9031
+//      int        temp;
+
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Micrel] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+/*
+        phy_write( eng, 13, 0x0002 );
+        phy_write( eng, 14, 0x0004 );
+        phy_write( eng, 13, 0x4002 );
+        temp = phy_read( eng, 14 );
+        //Reg2.4[ 7: 4]: RXDV Pad Skew
+        phy_write( eng, 14, temp & 0xff0f | 0x0000 );
+//      phy_write( eng, 14, temp & 0xff0f | 0x00f0 );
+printf("Reg2.4 = %04x -> %04x\n", temp, phy_read( eng, 14 ));
+
+        phy_write( eng, 13, 0x0002 );
+        phy_write( eng, 14, 0x0005 );
+        phy_write( eng, 13, 0x4002 );
+        temp = phy_read( eng, 14 );
+        //Reg2.5[15:12]: RXD3 Pad Skew
+        //Reg2.5[11: 8]: RXD2 Pad Skew
+        //Reg2.5[ 7: 4]: RXD1 Pad Skew
+        //Reg2.5[ 3: 0]: RXD0 Pad Skew
+        phy_write( eng, 14, 0x0000 );
+//      phy_write( eng, 14, 0xffff );
+printf("Reg2.5 = %04x -> %04x\n", temp, phy_read( eng, 14 ));
+
+        phy_write( eng, 13, 0x0002 );
+        phy_write( eng, 14, 0x0008 );
+        phy_write( eng, 13, 0x4002 );
+        temp = phy_read( eng, 14 );
+        //Reg2.8[9:5]: GTX_CLK Pad Skew
+        //Reg2.8[4:0]: RX_CLK Pad Skew
+//      phy_write( eng, 14, temp & 0xffe0 | 0x0000 );
+        phy_write( eng, 14, temp & 0xffe0 | 0x001f );
+printf("Reg2.8 = %04x -> %04x\n", temp, phy_read( eng, 14 ));
+*/
+
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        phy_Reset( eng );
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_Reset( eng );
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_Reset( eng );//DON'T support for 1G external loopback testing
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+}
+
+//------------------------------------------------------------
+//external loop 100M: OK
+//external loop 10M : OK
+//internal loop 100M: no  loopback stub
+//internal loop 10M : no  loopback stub
+void phy_micrel2 (MAC_ENGINE *eng) {//KSZ8081
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[Micrel] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        phy_Reset( eng );
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_Reset( eng );
+        }
+        else {
+                if ( eng->run.Speed_sel[ 1 ] )
+                        phy_Reset( eng );
+                else
+                        phy_Reset( eng );
+        }
+}
+
+//------------------------------------------------------------
+void recov_phy_vitesse (MAC_ENGINE *eng) {//VSC8601
+        if ( eng->run.TM_Burst ) {
+//              if ( eng->run.TM_IEEE ) {
+//              }
+//              else {
+//              }
+        }
+        else if ( eng->phy.loop_phy ) {
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        phy_write( eng, 24, eng->phy.PHY_18h );
+                        phy_write( eng, 18, eng->phy.PHY_12h );
+                }
+        }
+}
+
+//------------------------------------------------------------
+void phy_vitesse (MAC_ENGINE *eng) {//VSC8601
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)[VITESSE] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        phy_Reset( eng );
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_Reset( eng );
+        }
+        else {
+                if ( eng->run.Speed_sel[ 0 ] ) {
+                        eng->phy.PHY_18h = phy_read( eng, 24 );
+                        eng->phy.PHY_12h = phy_read( eng, PHY_INER );
+
+                        phy_Reset( eng );
+
+                        phy_write( eng, 24, eng->phy.PHY_18h | 0x0001 );
+                        phy_write( eng, 18, eng->phy.PHY_12h | 0x0020 );
+                }
+                else {
+                        phy_Reset( eng );
+                }
+        }
+}
+
+//------------------------------------------------------------
+void recov_phy_atheros (MAC_ENGINE *eng) {//AR8035
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                }
+                else {
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+        }
+        else {
+                phy_Read_Write( eng, 11, 0x0000, 0x8000 );//clr set//Disable hibernate: Reg0Bh[15] = 0
+                phy_Read_Write( eng, 17, 0x0001, 0x0000 );//clr set//Enable external loopback: Reg11h[0] = 1
+        }
+}
+
+//------------------------------------------------------------
+void phy_atheros (MAC_ENGINE *eng) {//AR8035
+#ifdef PHY_debug
+        if ( 1 ) {
+#else
+        if ( DbgPrn_PHYName ) {
+#endif
+                printf("--->(%04lx %04lx)[ATHEROS] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "--->(%04lx %04lx)[ATHEROS] %s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+        }
+
+        //Reg0b[15]: Power saving
+        phy_write( eng, 29, 0x000b );
+        eng->phy.PHY_1eh = phy_read( eng, 30 );
+        if ( eng->phy.PHY_1eh & 0x8000 ) {
+                printf("\n\n[Warning] Debug register offset = 11, bit 15 must be 0 [%04lx]\n\n", eng->phy.PHY_1eh);
+                if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Debug register offset = 11, bit 15 must be 0 [%04lx]\n\n", eng->phy.PHY_1eh);
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Debug register offset = 11, bit 15 must be 0 [%04lx]\n\n", eng->phy.PHY_1eh);
+
+                phy_write( eng, 30, eng->phy.PHY_1eh & 0x7fff );
+        }
+//      phy_write( eng, 30, (eng->phy.PHY_1eh & 0x7fff) | 0x8000 );
+
+        //Check RGMIIRXCK delay (Sel_clk125m_dsp)
+        phy_write( eng, 29, 0x0000 );
+        eng->phy.PHY_1eh = phy_read( eng, 30 );
+        if ( eng->phy.PHY_1eh & 0x8000 ) {
+                printf("\n\n[Warning] Debug register offset = 0, bit 15 must be 0 [%04lx]\n\n", eng->phy.PHY_1eh);
+                if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Debug register offset = 0, bit 15 must be 0 [%04lx]\n\n", eng->phy.PHY_1eh);
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Debug register offset = 0, bit 15 must be 0 [%04lx]\n\n", eng->phy.PHY_1eh);
+
+                phy_write( eng, 30, eng->phy.PHY_1eh & 0x7fff );
+        }
+//      phy_write( eng, 30, (eng->phy.PHY_1eh & 0x7fff) | 0x8000 );
+
+        //Check RGMIITXCK delay (rgmii_tx_clk_dly)
+        phy_write( eng, 29, 0x0005 );
+        eng->phy.PHY_1eh = phy_read( eng, 30 );
+        if ( eng->phy.PHY_1eh & 0x0100 ) {
+                printf("\n\n[Warning] Debug register offset = 5, bit 8 must be 0 [%04lx]\n\n", eng->phy.PHY_1eh);
+                if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Debug register offset = 5, bit 8 must be 0 [%04lx]\n\n", eng->phy.PHY_1eh);
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Debug register offset = 5, bit 8 must be 0 [%04lx]\n\n", eng->phy.PHY_1eh);
+
+                phy_write( eng, 30, eng->phy.PHY_1eh & 0xfeff );
+        }
+//      phy_write( eng, 30, (eng->phy.PHY_1eh & 0xfeff) | 0x0100 );
+
+        //Check CLK_25M output (Select_clk125m)
+        phy_write( eng, 13, 0x0007 );
+        phy_write( eng, 14, 0x8016 );
+        phy_write( eng, 13, 0x4007 );
+        eng->phy.PHY_0eh = phy_read( eng, 14 );
+        if ( (eng->phy.PHY_0eh & 0x0018) != 0x0018 ) {
+                 printf("\n\n[Warning] Device addrress = 7, Addrress ofset = 0x8016, bit 4~3 must be 3 [%04lx]\n\n", eng->phy.PHY_0eh);
+                if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Device addrress = 7, Addrress ofset = 0x8016, bit 4~3 must be 3 [%04lx]\n\n", eng->phy.PHY_0eh );
+                if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Device addrress = 7, Addrress ofset = 0x8016, bit 4~3 must be 3 [%04lx]\n\n", eng->phy.PHY_0eh );
+                printf("          The CLK_25M don't ouput 125MHz clock for the RGMIICK !!!\n\n");
+ 
+                phy_write( eng, 14, (eng->phy.PHY_0eh & 0xffe7) | 0x0018 );
+        }
+
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        phy_write( eng,  0, eng->phy.PHY_00h );
+                }
+                else {
+                        phy_write( eng,  0, eng->phy.PHY_00h );
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+                phy_write( eng,  0, eng->phy.PHY_00h );
+        }
+        else {
+                phy_Read_Write( eng, 11, 0x8000, 0x0000 );//clr set//Disable hibernate: Reg0Bh[15] = 0
+                phy_Read_Write( eng, 17, 0x0000, 0x0001 );//clr set//Enable external loopback: Reg11h[0] = 1
+
+                phy_write( eng,  0, eng->phy.PHY_00h | 0x8000 );
+#ifdef Delay_PHYRst
+                phy_delay( Delay_PHYRst );
+#endif
+//              if ( eng->run.Speed_sel[ 0 ] ) {
+//              }
+//              else if ( eng->run.Speed_sel[ 1 ] ) {
+//              }
+//              else if ( eng->run.Speed_sel[ 2 ] ) {
+//              }
+        }
+}
+
+//------------------------------------------------------------
+void phy_default (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+        printf("phy_default\n");
+        Debug_delay();
+#endif
+        if ( DbgPrn_PHYName )
+                printf("--->(%04lx %04lx)%s\n", eng->phy.PHY_ID2, eng->phy.PHY_ID3, eng->phy.PHYName);
+
+        phy_Reset( eng );
+}
+
+//------------------------------------------------------------
+// PHY Init
+//------------------------------------------------------------
+BOOLEAN find_phyadr (MAC_ENGINE *eng) {
+        ULONG      PHY_val;
+        BOOLEAN    ret = FALSE;
+        CHAR       PHY_ADR_org;
+
+#ifdef  DbgPrn_FuncHeader
+        printf("find_phyadr\n");
+        Debug_delay();
+#endif
+
+        if ( eng->env.AST2300 ) {
+#ifdef Force_Enable_NewMDIO
+                Write_Reg_PHY_DD( eng, 0x40, Read_Reg_PHY_DD( eng, 0x40 ) | 0x80000000 );
+#endif
+                eng->inf.NewMDIO = ( Read_Reg_PHY_DD( eng, 0x40 ) & 0x80000000 ) ? 1 : 0;
+        } else
+                eng->inf.NewMDIO = 0;
+
+        PHY_ADR_org = eng->phy.Adr;
+        // Check current PHY address by user setting
+        PHY_val = phy_read( eng, PHY_REG_ID_1 );
+        if ( PHY_IS_VALID( PHY_val ) ) {
+                ret = TRUE;
+        }
+        else if ( eng->arg.GEn_SkipChkPHY ) {
+                PHY_val = phy_read( eng, PHY_REG_BMCR );
+
+                if ( ( PHY_val & 0x8000 ) & eng->arg.GEn_InitPHY ) {
+                }
+                else {
+                        ret = TRUE;
+                }
+        }
+
+#ifdef Enable_SearchPHYID
+        if ( ret == FALSE ) {
+                // Scan PHY address from 0 to 31
+                if ( eng->arg.GEn_InitPHY )
+                        printf("Search PHY address\n");
+                for ( eng->phy.Adr = 0; eng->phy.Adr < 32; eng->phy.Adr++ ) {
+                        PHY_val = phy_read( eng, PHY_REG_ID_1 );
+                        if ( PHY_IS_VALID( PHY_val ) ) {
+                                ret = TRUE;
+                                break;
+                        }
+                }
+                // Don't find PHY address
+        }
+        if ( ret == FALSE )
+                eng->phy.Adr = eng->arg.GPHYADR;
+#endif
+
+        if ( eng->arg.GEn_InitPHY ) {
+                if ( ret == TRUE ) {
+                        if ( PHY_ADR_org != eng->phy.Adr ) {
+                                phy_id( eng, STD_OUT );
+                                if ( !eng->run.TM_Burst )
+                                        phy_id( eng, FP_LOG );
+                        }
+                }
+                else {
+                        phy_id( eng, STD_OUT );
+                        if ( !eng->run.TM_Burst )
+                                phy_id( eng, FP_LOG );
+
+                        FindErr( eng, Err_Flag_PHY_Type );
+                }
+        }
+
+        eng->phy.PHY_ID2 = phy_read( eng, PHY_REG_ID_1 );
+        eng->phy.PHY_ID3 = phy_read( eng, PHY_REG_ID_2 );
+
+        if      ( (eng->phy.PHY_ID2 == 0xffff) && ( eng->phy.PHY_ID3 == 0xffff ) && !eng->arg.GEn_SkipChkPHY ) {
+                sprintf( eng->phy.PHYName, "--" );
+                if ( eng->arg.GEn_InitPHY )
+                        FindErr( eng, Err_Flag_PHY_Type );
+        }
+#ifdef Enable_CheckZeroPHYID
+        else if ( (eng->phy.PHY_ID2 == 0x0000) && ( eng->phy.PHY_ID3 == 0x0000 ) && !eng->arg.GEn_SkipChkPHY ) {
+                sprintf( eng->phy.PHYName, "--" );
+                if ( eng->arg.GEn_InitPHY )
+                        FindErr( eng, Err_Flag_PHY_Type );
+        }
+#endif
+
+        return ret;
+} // End BOOLEAN find_phyadr (MAC_ENGINE *eng)
+
+//------------------------------------------------------------
+char phy_chk (MAC_ENGINE *eng, ULONG id2, ULONG id3, ULONG id3_mask) {
+        if ( ( eng->phy.PHY_ID2 == id2 ) && ( ( eng->phy.PHY_ID3 & id3_mask ) == ( id3 & id3_mask ) ) )
+                return(1);
+        else
+                return(0);
+}
+
+//------------------------------------------------------------
+void phy_set00h (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+        printf("phy_set00h\n");
+        Debug_delay();
+#endif
+
+        if ( eng->run.TM_Burst ) {
+                if ( eng->run.TM_IEEE ) {
+                        if      ( eng->run.Speed_sel[ 0 ] ) eng->phy.PHY_00h = 0x0140;
+                        else if ( eng->run.Speed_sel[ 1 ] ) eng->phy.PHY_00h = 0x2100;
+                        else                                eng->phy.PHY_00h = 0x0100;
+                }
+                else {
+                        if      ( eng->run.Speed_sel[ 0 ] ) eng->phy.PHY_00h = 0x0140;
+                        else if ( eng->run.Speed_sel[ 1 ] ) eng->phy.PHY_00h = 0x2100;
+                        else                                eng->phy.PHY_00h = 0x0100;
+//                      if      ( eng->run.Speed_sel[ 0 ] ) eng->phy.PHY_00h = 0x1140;
+//                      else if ( eng->run.Speed_sel[ 1 ] ) eng->phy.PHY_00h = 0x3100;
+//                      else                                eng->phy.PHY_00h = 0x1100;
+                }
+        }
+        else if ( eng->phy.loop_phy ) {
+                if      ( eng->run.Speed_sel[ 0 ] ) eng->phy.PHY_00h = 0x4140;
+                else if ( eng->run.Speed_sel[ 1 ] ) eng->phy.PHY_00h = 0x6100;
+                else                                eng->phy.PHY_00h = 0x4100;
+        }
+        else {
+                if      ( eng->run.Speed_sel[ 0 ] ) eng->phy.PHY_00h = 0x0140;
+                else if ( eng->run.Speed_sel[ 1 ] ) eng->phy.PHY_00h = 0x2100;
+                else                                eng->phy.PHY_00h = 0x0100;
+        }
+}
+
+//------------------------------------------------------------
+void phy_sel (MAC_ENGINE *eng, PHY_ENGINE *phyeng) {
+#ifdef  DbgPrn_FuncHeader
+        printf("phy_sel\n");
+        Debug_delay();
+#endif
+        if ( eng->phy.default_phy ) {
+                sprintf( eng->phy.PHYName, "default"); phyeng->fp_set = phy_default;
+        } else {
+                if      ( phy_chk( eng, 0x001c, 0xc916, 0xffff      ) ) { sprintf( eng->phy.PHYName, "RTL8211F"          ); phyeng->fp_set = phy_realtek5 ; phyeng->fp_clr = recov_phy_realtek5 ;}//RTL8211F         1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0x001c, 0xc915, 0xffff      ) ) { sprintf( eng->phy.PHYName, "RTL8211E"          ); phyeng->fp_set = phy_realtek2 ; phyeng->fp_clr = recov_phy_realtek2 ;}//RTL8211E         1G/100/10M  GMII(RTL8211EG only), RGMII
+                else if ( phy_chk( eng, 0x001c, 0xc914, 0xffff      ) ) { sprintf( eng->phy.PHYName, "RTL8211D"          ); phyeng->fp_set = phy_realtek1 ; phyeng->fp_clr = recov_phy_realtek1 ;}//RTL8211D         1G/100/10M  GMII(RTL8211DN/RTL8211DG only), MII(RTL8211DN/RTL8211DG only), RGMII
+                else if ( phy_chk( eng, 0x001c, 0xc912, 0xffff      ) ) { sprintf( eng->phy.PHYName, "RTL8211C"          ); phyeng->fp_set = phy_realtek3 ; phyeng->fp_clr = recov_phy_realtek3 ;}//RTL8211C         1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0x001c, 0xc930, 0xffff      ) ) { sprintf( eng->phy.PHYName, "RTL8363S"          ); phyeng->fp_set = phy_realtek6 ;                                      }//RTL8363S         1G/100/10M  RGMII Switch
+                else if ( phy_chk( eng, 0x001c, 0xc816, 0xffff      ) ) { sprintf( eng->phy.PHYName, "RTL8201F"          ); phyeng->fp_set = phy_realtek4 ;                                      }//RTL8201F            100/10M  MII, RMII(RMIICK I/O mode)
+                else if ( phy_chk( eng, 0x001c, 0xc815, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "RTL8201E"          ); phyeng->fp_set = phy_realtek0 ;                                      }//RTL8201E            100/10M  MII, RMII(RMIICK I/O mode)(RTL8201E(L)-VC only)
+                else if ( phy_chk( eng, 0x0000, 0x8201, PHYID3_Mask ) ) { sprintf( eng->phy.PHYName, "RTL8201N"          ); phyeng->fp_set = phy_realtek  ;                                      }//RTL8201N            100/10M  MII, RMII(RMIICK input mode)
+                else if ( phy_chk( eng, 0x0143, 0xbcb2, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "BCM5482"           ); phyeng->fp_set = phy_broadcom0; phyeng->fp_clr = recov_phy_broadcom0;}//BCM5482          1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0x0143, 0xbca0, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "BCM5481"           ); phyeng->fp_set = phy_broadcom0; phyeng->fp_clr = recov_phy_broadcom0;}//BCM5481          1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0x0362, 0x5e6a, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "BCM54612"          ); phyeng->fp_set = phy_broadcom0; phyeng->fp_clr = recov_phy_broadcom0;}//BCM54612         1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0x0362, 0x5d10, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "BCM54616S"         ); phyeng->fp_set = phy_broadcom0; phyeng->fp_clr = recov_phy_broadcom0;}//BCM54616S        1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0x0020, 0x60b0, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "BCM5464SR"         ); phyeng->fp_set = phy_broadcom0; phyeng->fp_clr = recov_phy_broadcom0;}//BCM5464SR        1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0x0020, 0x60c1, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "BCM5461S"          ); phyeng->fp_set = phy_broadcom0; phyeng->fp_clr = recov_phy_broadcom0;}//BCM5461S
+                else if ( phy_chk( eng, 0x600d, 0x84a2, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "BCM54210E"         ); phyeng->fp_set = phy_broadcom0; phyeng->fp_clr = recov_phy_broadcom0;}//BCM54210E
+                else if ( phy_chk( eng, 0x0143, 0xbd63, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "BCM54610C"         ); phyeng->fp_set = phy_broadcom0; phyeng->fp_clr = recov_phy_broadcom0;}//BCM54610C
+                else if ( phy_chk( eng, 0x0040, 0x61e0, PHYID3_Mask ) ) { sprintf( eng->phy.PHYName, "BCM5221"           ); phyeng->fp_set = phy_broadcom ;                                      }//BCM5221             100/10M  MII, RMII(RMIICK input mode)
+                else if ( phy_chk( eng, 0x0141, 0x0e22, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "88E3019"           ); phyeng->fp_set = phy_marvell3 ;                                      }//88E3019             100/10M  RGMII, MII, RMII(RMIICK input mode)
+                else if ( phy_chk( eng, 0x0141, 0x0dd0, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "88E15 10/12/14/18" ); phyeng->fp_set = phy_marvell2 ; phyeng->fp_clr = recov_phy_marvell2 ;}//88E1512          1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0xff00, 0x1761, 0xffff      ) ) { sprintf( eng->phy.PHYName, "88E6176(IntLoop)"  ); phyeng->fp_set = phy_marvell1 ; phyeng->fp_clr = recov_phy_marvell1 ;}//88E6176          1G/100/10M  2 RGMII Switch
+                else if ( phy_chk( eng, 0xff00, 0x1152, 0xffff      ) ) { sprintf( eng->phy.PHYName, "88E6320(IntLoop)"  ); phyeng->fp_set = phy_marvell1 ; phyeng->fp_clr = recov_phy_marvell1 ;}//88E6320          1G/100/10M  2 RGMII Switch
+                else if ( phy_chk( eng, 0x0141, 0x0e90, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "88E1310"           ); phyeng->fp_set = phy_marvell0 ; phyeng->fp_clr = recov_phy_marvell0 ;}//88E1310          1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0x0141, 0x0cc0, PHYID3_Mask ) ) { sprintf( eng->phy.PHYName, "88E1111"           ); phyeng->fp_set = phy_marvell  ; phyeng->fp_clr = recov_phy_marvell  ;}//88E1111          1G/100/10M  GMII, MII, RGMII
+                else if ( phy_chk( eng, 0x0022, 0x1555, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "KSZ8031/KSZ8051"   ); phyeng->fp_set = phy_micrel0  ;                                      }//KSZ8051/KSZ8031     100/10M  RMII(RMIICK input mode)
+                else if ( phy_chk( eng, 0x0022, 0x1622, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "KSZ9031"           ); phyeng->fp_set = phy_micrel1  ;                                      }//KSZ9031          1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0x0022, 0x1562, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "KSZ8081"           ); phyeng->fp_set = phy_micrel2  ;                                      }//KSZ8081             100/10M  MII, RMII(RMIICK input mode)
+                else if ( phy_chk( eng, 0x0022, 0x1512, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "KSZ8041"           ); phyeng->fp_set = phy_micrel   ;                                      }//KSZ8041             100/10M  RMII(RMIICK input mode)
+                else if ( phy_chk( eng, 0x004d, 0xd072, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "AR8035"            ); phyeng->fp_set = phy_atheros  ; phyeng->fp_clr = recov_phy_atheros  ;}//AR8035           1G/100/10M  RGMII
+                else if ( phy_chk( eng, 0x0007, 0xc0c4, PHYID3_Mask ) ) { sprintf( eng->phy.PHYName, "LAN8700"           ); phyeng->fp_set = phy_smsc     ;                                      }//LAN8700             100/10M  MII, RMII
+                else if ( phy_chk( eng, 0x0007, 0x0421, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "VSC8601"           ); phyeng->fp_set = phy_vitesse  ; phyeng->fp_clr = recov_phy_vitesse  ;}//VSC8601          1G/100/10M  RGMII
+#if defined(PHY_SPECIAL)                    
+                else if ( phy_chk( eng, 0x0143, 0xbd70, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "[S]BCM5387/BCM5389"); phyeng->fp_set = 0            ; phyeng->fp_clr = 0                  ;}//BCM5389          1G/100/10M  RGMII(IMP Port)
+                else if ( phy_chk( eng, 0x0000, 0x0000, 0xfff0      ) ) { sprintf( eng->phy.PHYName, "[S]BCM5396"        ); phyeng->fp_set = 0            ; phyeng->fp_clr = 0                  ;}//BCM5396          1G/100/10M  RGMII(IMP Port)
+#endif                    
+                else                                                    { sprintf( eng->phy.PHYName, "default"           ); phyeng->fp_set = phy_default  ;                                      }//
+
+#if defined(PHY_SPECIAL)
+                switch ( eng->phy.PHY_ID3 & 0xfff0 ) {
+                        case 0xbd70:    eng->phy.Adr = 30; eng->BCMIMP.PHY_sel = 0;                            break; // for BCM5387/BCM5389
+//                        case 0x0960:    eng->phy.Adr = 30; eng->BCMIMP.PHY_sel = 1; eng->GPIO.Dat_RdDelay = 1; break;
+//                        case 0x0000:    eng->phy.Adr = 30; eng->BCMIMP.PHY_sel = 1; eng->GPIO.Dat_RdDelay = 1; break;
+//                        default:        eng->phy.Adr = 30; eng->BCMIMP.PHY_sel = 1; eng->GPIO.Dat_RdDelay = 1; break;
+                        default:        eng->phy.Adr = 30; eng->BCMIMP.PHY_sel = 1;                            break; // for BCM5396
+                }
+#endif
+        }
+
+        if ( eng->arg.GEn_InitPHY ) {
+                if ( eng->arg.GDis_RecovPHY )
+                        phyeng->fp_clr = 0;
+        } else {
+                phyeng->fp_set = 0;
+                phyeng->fp_clr = 0;
+        }
+}
+
+//------------------------------------------------------------
+void recov_phy (MAC_ENGINE *eng, PHY_ENGINE *phyeng) {
+#ifdef  DbgPrn_FuncHeader
+        printf("recov_phy\n");
+        Debug_delay();
+#endif
+
+        (*phyeng->fp_clr)( eng );
+}
+
+//------------------------------------------------------------
+void init_phy (MAC_ENGINE *eng, PHY_ENGINE *phyeng) {
+#ifdef  DbgPrn_FuncHeader
+        printf("init_phy\n");
+        Debug_delay();
+#endif
+
+        if ( DbgPrn_PHYInit )
+                phy_dump( eng );
+
+        phy_set00h( eng );
+        (*phyeng->fp_set)( eng );
+
+        if ( DbgPrn_PHYInit )
+                phy_dump( eng );
+}
+
--- uboot_old/oem/ami/standalone/nettest/PHYGPIO.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/PHYGPIO.c	2018-06-22 17:51:23.921169873 +0800
@@ -0,0 +1,204 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define PHYGPIO_C
+static const char ThisFile[] = "PHYGPIO.c";
+
+#include "SWFUNC.H"
+#include "COMMINF.H"
+
+#if defined(SLT_UBOOT)
+  #include <common.h>
+  #include <command.h>
+  #include "STDUBOOT.H"
+#endif
+#if defined(DOS_ALONE)
+  #include <stdio.h>
+  #include <stdlib.h>
+  #include <conio.h>
+  #include <string.h>
+#endif
+
+#include "COMMINF.H"
+
+#if defined(PHY_GPIO)
+//------------------------------------------------------------
+// PHY Read/Write Using GPIO
+//------------------------------------------------------------
+//          AST2500  AST2400  AST2300  AST1010
+// MDC1  :  GPIOR6   GPIOR6   GPIOR6   GPIOK6
+// MDIO1 :  GPIOR7   GPIOR7   GPIOR7   GPIOK7
+// MDC2  :  GPIOA6   GPIOA6   GPIOA6   ------
+// MDIO2 :  GPIOA7   GPIOA7   GPIOA7   ------
+//------------------------------------------------------------
+
+//------------------------------------------------------------
+// Initial
+//------------------------------------------------------------
+void phy_gpio_init( MAC_ENGINE *eng )
+{
+#ifdef  DbgPrn_FuncHeader
+	printf("phy_gpio_init\n");
+	Debug_delay();
+#endif
+#ifndef AST1010_IOMAP
+	switch ( eng->run.MAC_idx_PHY ) {
+		case 0  :
+			eng->GPIO.IOAdr_Dat      = 0x00000080;
+			eng->GPIO.IOAdr_OE       = 0x00000084;
+			eng->GPIO.MDIO_shiftbit  = 15;
+			eng->GPIO.Mask_MDC       = 0x00004000;
+			eng->GPIO.Mask_MDIO      = 0x00008000;
+			break;
+		case 1  :
+			eng->GPIO.IOAdr_Dat      = 0x00000000;
+			eng->GPIO.IOAdr_OE       = 0x00000004;
+			eng->GPIO.MDIO_shiftbit  = 7;
+			eng->GPIO.Mask_MDC       = 0x00000040;
+			eng->GPIO.Mask_MDIO      = 0x00000080;
+			break;
+		default : break;
+	}
+#else
+	eng->GPIO.IOAdr_Dat      = 0x00000070;
+	eng->GPIO.IOAdr_OE       = 0x00000074;
+	eng->GPIO.MDIO_shiftbit  = 23;
+	eng->GPIO.Mask_MDC       = 0x00400000;
+	eng->GPIO.Mask_MDIO      = 0x00800000;
+#endif
+	eng->GPIO.Mask_all  = eng->GPIO.Mask_MDC | eng->GPIO.Mask_MDIO;
+
+	eng->GPIO.Value_Dat =  Read_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat ) & (~eng->GPIO.Mask_all);
+	eng->GPIO.Value_OE  = (Read_Reg_GPIO_DD( eng->GPIO.IOAdr_OE  ) & (~eng->GPIO.Mask_MDIO)) | eng->GPIO.Mask_MDC;
+	Write_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat, eng->GPIO.Value_Dat | eng->GPIO.Mask_all );
+	Write_Reg_GPIO_DD( eng->GPIO.IOAdr_OE , eng->GPIO.Value_OE                       );
+	
+	eng->GPIO.Dat_RdDelay = 0;
+}
+
+//------------------------------------------------------------
+// PHY R/W GPIO
+//------------------------------------------------------------
+void phy_gpio_wrbit ( MAC_ENGINE *eng, int value )
+{
+	if ( value & 0x8000 )
+	{
+		Write_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat, eng->GPIO.Value_Dat | eng->GPIO.Mask_MDIO );                 
+		PHY_GPIO_DELAY( PHY_GPIO_MDC_HalfPeriod );
+
+		Write_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat, eng->GPIO.Value_Dat | eng->GPIO.Mask_all  );
+		PHY_GPIO_DELAY( PHY_GPIO_MDC_HalfPeriod );
+	} else {
+		Write_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat, eng->GPIO.Value_Dat                       );
+		PHY_GPIO_DELAY( PHY_GPIO_MDC_HalfPeriod );
+
+		Write_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat, eng->GPIO.Value_Dat | eng->GPIO.Mask_MDC  );
+		PHY_GPIO_DELAY( PHY_GPIO_MDC_HalfPeriod );
+	}
+}
+
+char phy_gpio_rdbit ( MAC_ENGINE *eng )
+{
+	static char read_bit;
+	Write_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat, eng->GPIO.Value_Dat | eng->GPIO.Mask_MDIO );
+	PHY_GPIO_DELAY( PHY_GPIO_MDC_HalfPeriod );
+
+	read_bit = ( Read_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat ) >> eng->GPIO.MDIO_shiftbit ) & 0x1;
+	Write_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat, eng->GPIO.Value_Dat | eng->GPIO.Mask_all  );
+	PHY_GPIO_DELAY( PHY_GPIO_MDC_HalfPeriod );
+
+//	return ( ( Read_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat ) >> eng->GPIO.MDIO_shiftbit ) & 0x1 );
+	return ( read_bit );
+}
+
+void phy_gpio_write( MAC_ENGINE *eng, int regadr, int wrdata )
+{
+	static int phy_wr_cnt;
+	static int phy_wr_wrvalue;
+
+//#ifdef  DbgPrn_FuncHeader
+//	printf("phy_gpio_write\n");
+//	Debug_delay();
+//#endif
+#ifdef PHY_GPIO_ReadValueEveryTime
+	eng->GPIO.Value_Dat = Read_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat ) & (~eng->GPIO.Mask_all);
+	eng->GPIO.Value_OE  = Read_Reg_GPIO_DD( eng->GPIO.IOAdr_OE  );
+#endif
+	Write_Reg_GPIO_DD( eng->GPIO.IOAdr_OE, eng->GPIO.Value_OE | eng->GPIO.Mask_MDIO );
+
+	for ( phy_wr_cnt = 0; phy_wr_cnt < 36; phy_wr_cnt++ )
+		phy_gpio_wrbit ( eng, 0x8000 );
+
+	phy_wr_wrvalue = 0x5002 | (( eng->phy.Adr & 0x1f ) << 7 ) | (( regadr & 0x1f ) << 2 );
+	for ( phy_wr_cnt = 0; phy_wr_cnt < 16; phy_wr_cnt++ )
+	{
+		phy_gpio_wrbit ( eng, phy_wr_wrvalue );
+		phy_wr_wrvalue = phy_wr_wrvalue << 1;
+	}
+
+	phy_wr_wrvalue = wrdata;
+	for ( phy_wr_cnt = 0; phy_wr_cnt < 16; phy_wr_cnt++ )
+	{
+		phy_gpio_wrbit ( eng, phy_wr_wrvalue );
+		phy_wr_wrvalue = phy_wr_wrvalue << 1;
+	}
+
+	Write_Reg_GPIO_DD( eng->GPIO.IOAdr_OE, eng->GPIO.Value_OE );
+	phy_gpio_wrbit ( eng, 1 );
+	phy_gpio_wrbit ( eng, 1 );
+}
+
+ULONG phy_gpio_read( MAC_ENGINE *eng, int regadr )
+{
+	static int phy_rd_cnt;
+	static int phy_rd_wrvalue;
+	static int phy_rd_rdvalue;
+
+//#ifdef  DbgPrn_FuncHeader
+//	printf("phy_gpio_read\n");
+//	Debug_delay();
+//#endif
+#ifdef PHY_GPIO_ReadValueEveryTime
+	eng->GPIO.Value_Dat = Read_Reg_GPIO_DD( eng->GPIO.IOAdr_Dat ) & (~eng->GPIO.Mask_all);
+	eng->GPIO.Value_OE  = Read_Reg_GPIO_DD( eng->GPIO.IOAdr_OE  );
+#endif
+	Write_Reg_GPIO_DD( eng->GPIO.IOAdr_OE, eng->GPIO.Value_OE | eng->GPIO.Mask_MDIO );
+
+	for ( phy_rd_cnt = 0; phy_rd_cnt < 36; phy_rd_cnt++ )
+		phy_gpio_wrbit ( eng, 0x8000 );
+
+	phy_rd_wrvalue = 0x6000 | (( eng->phy.Adr & 0x1f ) << 7 ) | (( regadr & 0x1f ) << 2 );
+	for ( phy_rd_cnt = 0; phy_rd_cnt < 14; phy_rd_cnt++ )
+	{
+		phy_gpio_wrbit ( eng, phy_rd_wrvalue );
+		phy_rd_wrvalue = phy_rd_wrvalue << 1;
+	}
+
+	Write_Reg_GPIO_DD( eng->GPIO.IOAdr_OE, eng->GPIO.Value_OE );
+	phy_gpio_rdbit( eng );
+	phy_gpio_rdbit(eng );
+
+	for (phy_rd_cnt = 0; phy_rd_cnt < eng->GPIO.Dat_RdDelay; phy_rd_cnt++)
+		phy_gpio_rdbit( eng );
+
+	phy_rd_rdvalue = phy_gpio_rdbit( eng );
+	for (phy_rd_cnt = 0; phy_rd_cnt < 15; phy_rd_cnt++) {
+		phy_rd_rdvalue = ( phy_rd_rdvalue << 1 ) | phy_gpio_rdbit( eng );
+	}
+
+	phy_gpio_rdbit( eng );
+	phy_gpio_rdbit( eng );
+	phy_gpio_rdbit( eng );
+	phy_gpio_rdbit( eng );
+
+	return ( phy_rd_rdvalue );
+}
+#endif // defined(PHY_GPIO)
--- uboot_old/oem/ami/standalone/nettest/PHYGPIO.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/PHYGPIO.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,42 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef PHYGPIO_H
+#define PHYGPIO_H
+
+//#define PHY_GPIO_ReadValueEveryTime
+#define PHY_GPIO_MDC_HalfPeriod                           1
+
+
+#if defined(DOS_ALONE) || defined(SLT_NEW_ARCH) || defined(LinuxAP)
+//    #define PHY_GPIO_DELAY( x )                           delay_hwtimer( x )   // For hardware timer #2, the unit of delay() is ms.s.
+    #define PHY_GPIO_DELAY( x )                           Read_Reg_GPIO_DD( 0 )
+#elif defined(SLT_UBOOT)
+    #define PHY_GPIO_DELAY( x )                           udelay( x ) // For Uboot, the unit of udelay() is us.
+#else
+    #define PHY_GPIO_DELAY( x )                           usleep( x * 1000 )
+#endif
+
+typedef struct {
+	ULONG                IOAdr_Dat                               ;//__attribute__ ((aligned (4)));
+	ULONG                IOAdr_OE                                ;//__attribute__ ((aligned (4)));
+	ULONG                MDIO_shiftbit                           ;//__attribute__ ((aligned (4)));
+	ULONG                Mask_MDC                                ;//__attribute__ ((aligned (4)));
+	ULONG                Mask_MDIO                               ;//__attribute__ ((aligned (4)));
+	ULONG                Mask_all                                ;//__attribute__ ((aligned (4)));
+	ULONG                Value_Dat                               ;//__attribute__ ((aligned (4)));
+	ULONG                Value_OE                                ;//__attribute__ ((aligned (4)));
+
+	char                 Dat_RdDelay                             ;//__attribute__ ((aligned (4)));
+} PHY_GPIOstr;
+
+
+#endif // PHYGPIO_H
--- uboot_old/oem/ami/standalone/nettest/PHY.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/PHY.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,56 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef PHY_H
+#define PHY_H
+
+//
+// Define
+//
+#define Enable_SearchPHYID             //[ON] (Search vlid PHY ID)
+#ifndef PHY_SPECIAL
+#define Enable_CheckZeroPHYID          //[ON] (Check PHY ID with value 0)
+#endif
+
+#ifdef Enable_CheckZeroPHYID
+  #define PHY_IS_VALID( dat )      ( ( (dat & 0xffff) != 0xffff ) && ( ( dat & 0xffff ) != 0x0 ) )
+#else
+  #define PHY_IS_VALID( dat )      ( ( dat & 0xffff) != 0xffff )
+#endif
+
+// Define PHY basic register
+#define PHY_REG_BMCR    0x00 // Basic Mode Control Register
+#define PHY_REG_BMSR    0x01 // Basic Mode Status Register
+#define PHY_REG_ID_1    0x02
+#define PHY_REG_ID_2    0x03
+#define PHY_ANER        0x06 // Auto-negotiation Expansion Register
+#define PHY_GBCR        0x09 // 1000Base-T Control Register
+#define PHY_SR          0x11 // PHY Specific Status Register
+#define PHY_INER        0x12 // Interrupt Enable Register
+
+#define PHYID3_Mask                0xfc00         //0xffc0
+
+/* --- Note for SettingPHY chip ---
+void phy_xxxx (int loop_phy) {
+
+	if ( BurstEnable ) {
+        // IEEE test
+	}
+	else if (loop_phy) {
+        // Internal loop back
+	}
+	else {
+        // external loop back
+	}
+}
+----------------------------------- */
+
+#endif // PHY_H
--- uboot_old/oem/ami/standalone/nettest/PHYSPECIAL.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/PHYSPECIAL.c	2018-06-22 17:51:23.921169873 +0800
@@ -0,0 +1,899 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define PHYMISC_C
+static const char ThisFile[] = "PHYSPECIAL.c";
+//#define PHY_BCMIMP_debug
+
+#include "SWFUNC.H"
+#include "COMMINF.H"
+
+#if defined(SLT_UBOOT)
+  #include <common.h>
+  #include <command.h>
+  #include "STDUBOOT.H"
+#endif
+#if defined(DOS_ALONE)
+  #include <stdio.h>
+  #include <stdlib.h>
+  #include <conio.h>
+  #include <string.h>
+#endif
+
+#include "COMMINF.H"
+
+#ifdef PHY_SPECIAL
+
+/*
+ULONG   BCMIMP_sample_tx[PHY_BCMIMP_Pkt_DW_Len] = { //120 Byte, 30 DW
+	0xffffffff,
+	0x0000ffff, // SA:00-00-
+	0x12345678, // SA:78-56-34-12
+	0x00407488, // BRCM Type(0x8874), BRCM Tag:40-00-  //Egress Directed
+	0x00000800, // BRCM Tag:00-08                      //DSTPortID [3:0] = 0x8
+//	0x00607488, // BRCM Type(0x8874), BRCM Tag:60-00-  //Multiple Egress Directed
+//	0x0000ff01, // BRCM Tag:01-ff
+	0x00000000, // SimpleData_FixVal00
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, //
+	0x00000000, // SimpleData_FixVal11
+	0x937e9ade  // Original FCS
+//	0x84100dad  // MAC FCS(Tx)(BRCM Tag:40-00-00-08)
+//	0xdfb1c84d  // MAC FCS(Tx)(BRCM Tag:60-00-01-ff)
+//	0x0983fe7b  // MAC FCS(Rx)
+};
+*/
+
+/*
+ULONG   BCMIMP_sample_tx[PHY_BCMIMP_Pkt_DW_Len] = { //120 Byte, 30 DW
+	0xffffffff,
+	0x0000ffff, // SA:00-00-
+	0x12345678, // SA:78-56-34-12
+	0x00407488, // BRCM Type(0x8874), BRCM Tag:40-00-  //Egress Directed
+	0x55550800, // BRCM Tag:00-08                      //DSTPortID [3:0] = 0x8
+//	0x00607488, // BRCM Type(0x8874), BRCM Tag:60-00-  //Multiple Egress Directed
+//	0x5555ff01, // BRCM Tag:01-ff
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x7ba6d41a  // Original FCS
+//	0x84100dad  // MAC FCS(Tx)(BRCM Tag:40-00-00-08)
+//	0xdfb1c84d  // MAC FCS(Tx)(BRCM Tag:60-00-01-ff)
+//	0x0983fe7b  // MAC FCS(Rx)
+};
+*/
+
+ULONG   BCMIMP_sample_tx[PHY_BCMIMP_Pkt_DW_Len] = { //1524 Byte, 381 DW
+	0xffffffff,
+	0x0000ffff, // SA:00-00-
+	0x12345678, // SA:78-56-34-12
+	0x00407488, // [BCM5389] BRCM Type(0x8874), BRCM Tag:40-00-  //Egress Directed
+	0x55550800, // [BCM5389] BRCM Tag:00-08                      //DSTPortID [3:0] = 0x8
+//	0x00607488, // [BCM5389] BRCM Type(0x8874), BRCM Tag:60-00-  //Multiple Egress Directed
+//	0x5555ff01, // [BCM5389] BRCM Tag:01-ff
+
+//	0x00407488, // [BCM5396] BRCM Type(0x8874), BRCM Tag:40-00-  //Egress Directed
+//	0x55551000, // [BCM5396] BRCM Tag:00-10                      //DSTPortID [4:0] = 0x10
+////	0x01607488, // [BCM5396] BRCM Type(0x8874), BRCM Tag:60-01-  //Multiple Egress Directed
+////	0x5555ffff, // [BCM5396] BRCM Tag:ff-ff
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0xaaaaaaaa, //
+	0x5a5a5a5a, //
+	0xc3c3c3c3, //
+	0x96969696, //
+	0xf0f0f0f0, //
+	0x5555aaaa, //
+	0xffff0000, //
+	0x5a5aa5a5, //
+	0xc3c33c3c, // SimpleData_FixVal11
+	0x00000000, // SimpleData_FixVal00
+	0xffffffff, //
+	0x55555555, //
+	0x8dedb8b6  // Original FCS
+//	0x5604efcf  // MAC FCS(Tx)(BRCM Tag:40-00-00-08)
+//	0x36be85a5  // MAC FCS(Tx)(BRCM Tag:60-00-01-ff)
+//	0x9f4a367a  // MAC FCS(Rx)
+};
+
+//------------------------------------------------------------
+// BCM IMP Port
+//------------------------------------------------------------
+void BCMIMP_set_page (MAC_ENGINE *eng, ULONG page_num) {
+	phy_write( eng, 16, ( ( page_num & 0xff ) << 8 ) | 0x0001 );
+}
+
+//------------------------------------------------------------
+void BCMIMP_reg_write (MAC_ENGINE *eng, ULONG addr, ULONG wrdat_DW_MSB, ULONG wrdat_DW_LSB) {
+	int        timeout = 0;
+
+#ifdef PHY_BCMIMP_debug
+	printf("%s[Wr ]%02d: 0x%04lx 0x%04lx\n", eng->phy.PHYName, addr, wrdat_DW_MSB, wrdat_DW_LSB);
+#endif
+	phy_write( eng, 24,   wrdat_DW_LSB         & 0xffff );
+	phy_write( eng, 25, ( wrdat_DW_LSB >> 16 ) & 0xffff );
+	phy_write( eng, 26,   wrdat_DW_MSB         & 0xffff );
+	phy_write( eng, 27, ( wrdat_DW_MSB >> 16 ) & 0xffff );
+
+	phy_write( eng, 17, ( ( addr & 0xff ) << 8 )          ); //for BCM5396
+	phy_write( eng, 17, ( ( addr & 0xff ) << 8 ) | 0x0001 );
+
+	while ( ( phy_read ( eng, 17 ) & 0x0003 ) != 0x0 ) {
+		if (++timeout > PHY_BCMIMP_TIME_OUT_RW) {
+#ifdef PHY_BCMIMP_debug
+			printf("%s[Reg]17: 0x%04lx\n", eng->phy.PHYName, phy_read ( eng, 17 ));
+#endif
+			FindErr( eng, Err_Flag_PHY_TimeOut_RW );
+			break;
+		}
+	}
+}
+
+//------------------------------------------------------------
+void BCMIMP_reg_read_2ptr (MAC_ENGINE *eng, ULONG addr, ULONG *data_MSB, ULONG *data_LSB) {
+	int        timeout = 0;
+
+#ifdef PHY_BCMIMP_debug
+	printf("%s[RdS]%02d\n", eng->phy.PHYName, addr);
+#endif
+	phy_write( eng, 17, ( ( addr & 0xff ) << 8 )          ); //for BCM5396
+	phy_write( eng, 17, ( ( addr & 0xff ) << 8 ) | 0x0002 );
+
+	while ( ( phy_read ( eng, 17 ) & 0x0003 ) != 0x0 ) {
+		if (++timeout > PHY_BCMIMP_TIME_OUT_RW) {
+#ifdef PHY_BCMIMP_debug
+			printf("%s[Reg]17: 0x%04lx\n", eng->phy.PHYName, phy_read ( eng, 17 ));
+#endif
+			FindErr( eng, Err_Flag_PHY_TimeOut_RW );
+			break;
+		}
+	}
+
+	*data_LSB = phy_read ( eng, 24 ) | ( phy_read ( eng, 25 ) << 16 );
+	*data_MSB = phy_read ( eng, 26 ) | ( phy_read ( eng, 27 ) << 16 );
+#ifdef PHY_BCMIMP_debug
+	printf("%s[RdE]%02d: 0x%04lx 0x%04lx\n", eng->phy.PHYName, addr, eng->BCMIMP.rddata[1], eng->BCMIMP.rddata[0]);
+#endif
+}
+
+//------------------------------------------------------------
+void BCMIMP_reg_read  (MAC_ENGINE *eng, ULONG addr) {
+	BCMIMP_reg_read_2ptr( eng, addr, (eng->BCMIMP.rddata + 1), eng->BCMIMP.rddata );
+}
+
+//------------------------------------------------------------
+void BCMIMP_reg_chgcheck (MAC_ENGINE *eng, ULONG page, ULONG addr) {
+	ULONG	olddat[2];
+
+	olddat[0] = eng->BCMIMP.rddata[0];
+	olddat[1] = eng->BCMIMP.rddata[1];
+	BCMIMP_reg_read ( eng, addr );
+#ifdef PHY_BCMIMP_debug
+	PRINTF( STD_OUT, "[page %ld:0x%02lx] %08lx %08lx --> %08lx %08lx\n", page, addr, olddat[1], olddat[0], eng->BCMIMP.rddata[1], eng->BCMIMP.rddata[0]);
+#else
+	PRINTF( FP_LOG, "[page %ld:0x%02lx] %08lx %08lx --> %08lx %08lx\n", page, addr, olddat[1], olddat[0], eng->BCMIMP.rddata[1], eng->BCMIMP.rddata[0]);
+#endif
+}
+
+//------------------------------------------------------------
+void BCMIMP_init (MAC_ENGINE *eng) {
+	eng->BCMIMP.wait_init  = 1;
+}
+
+//------------------------------------------------------------
+void BCMIMP_MDIO_init (MAC_ENGINE *eng) {
+	phy_write( eng, 16, 0x0001 );
+}
+
+//------------------------------------------------------------
+void BCMIMP_buf_init (MAC_ENGINE *eng) {
+	eng->BCMIMP.pkt_tx_len = PHY_BCMIMP_Pkt_Byte_Len;
+	eng->BCMIMP.pkt_tx     = (ULONG *)BCMIMP_sample_tx;
+
+	if ( eng->BCMIMP.PHY_sel == 0 ) {
+		if ( eng->arg.GUserDVal == 1 ) {
+			eng->BCMIMP.pkt_tx[3] = 0x00607488;                                    // [BCM5389] BRCM Type(0x8874), BRCM Tag:60-00-
+			eng->BCMIMP.pkt_tx[4] = 0xff01 | (eng->BCMIMP.pkt_tx[4] & 0xffff0000); // [BCM5389] BRCM Tag:01-ff
+		} else {
+			eng->BCMIMP.pkt_tx[3] = 0x00407488;                                    // [BCM5389] BRCM Type(0x8874), BRCM Tag:40-00-
+			eng->BCMIMP.pkt_tx[4] = 0x0800 | (eng->BCMIMP.pkt_tx[4] & 0xffff0000); // [BCM5389] BRCM Tag:00-08
+		}
+	} else {
+		if ( eng->arg.GUserDVal == 1 ) {
+			eng->BCMIMP.pkt_tx[3] = 0x00407488;                                    // [BCM5396] BRCM Type(0x8874), BRCM Tag:40-00-
+			eng->BCMIMP.pkt_tx[4] = 0x1000 | (eng->BCMIMP.pkt_tx[4] & 0xffff0000); // [BCM5396] BRCM Tag:00-10                    
+		} else {
+			eng->BCMIMP.pkt_tx[3] = 0x01607488;                                    // [BCM5396] BRCM Type(0x8874), BRCM Tag:60-01-
+			eng->BCMIMP.pkt_tx[4] = 0xffff | (eng->BCMIMP.pkt_tx[4] & 0xffff0000); // [BCM5396] BRCM Tag:ff-ff
+		}
+	}
+
+	eng->BCMIMP.pkt_rx_len = eng->BCMIMP.pkt_tx_len - 6;
+	memcpy( (UCHAR *)eng->BCMIMP.pkt_rx, (UCHAR *)eng->BCMIMP.pkt_tx, eng->BCMIMP.pkt_tx_len);
+
+	eng->BCMIMP.pkt_rx[3] = 0x00007488
+	                      | ( ( ( eng->BCMIMP.pkt_rx_len >> 1 ) & 0xff ) << 24 )
+	                      | ( ( ( eng->BCMIMP.pkt_rx_len >> 9 ) & 0xff ) << 16 ); // BRCM Type(0x8874), BRCM Tag:00-00-
+	if ( eng->BCMIMP.PHY_sel == 0 ) {
+		eng->BCMIMP.pkt_rx[4] = 0x0800
+				      |   (   eng->BCMIMP.pkt_rx[4] & 0xffff0000   )
+				      |   (   eng->BCMIMP.pkt_rx_len        & 0x01 );         // BRCM Tag:00-08
+	} else {
+		eng->BCMIMP.pkt_rx[4] = 0x1000
+				      |   (   eng->BCMIMP.pkt_rx[4] & 0xffff0000   )
+				      |   (   eng->BCMIMP.pkt_rx_len        & 0x01 );         // BRCM Tag:00-10
+	}
+
+}
+
+//------------------------------------------------------------
+// BCM 5389
+//------------------------------------------------------------
+void BCM5389_counter (MAC_ENGINE *eng) {
+	int	port, page;
+	ULONG	dat[10];
+
+	PRINTF( FP_LOG, "     TxOctets           TxDropPkts | RxOctets            RxDropPkts RxGoodOctets\n");
+	page = 0x20;
+	for ( port = 0; port < 8; port++ ) {
+		BCMIMP_set_page ( eng, page );
+
+		BCMIMP_reg_read_2ptr ( eng, 0x00, &dat[1], &dat[0]); //[TxOctets    ] 64 bits
+		BCMIMP_reg_read_2ptr ( eng, 0x08, &dat[3], &dat[2]); //[TxDropPkts  ] 32 bits
+		BCMIMP_reg_read_2ptr ( eng, 0x44, &dat[5], &dat[4]); //[RxOctets    ] 64 bits
+		BCMIMP_reg_read_2ptr ( eng, 0x84, &dat[7], &dat[6]); //[RxDropPkts  ] 32 bits
+		BCMIMP_reg_read_2ptr ( eng, 0x7c, &dat[9], &dat[8]); //[RxGoodOctets] 64 bits
+		PRINTF( FP_LOG, "[P%d](%08lx %08lx) (%08lx) | (%08lx %08lx) (%08lx) (%08lx %08lx)\n", page - 0x20, dat[1], dat[0], dat[2], dat[5], dat[4], dat[6], dat[9], dat[8] );
+		page++;
+	}
+	phy_write( eng, 16, 0x0000 );
+}
+
+//------------------------------------------------------------
+void BCM5389_reg_init (MAC_ENGINE *eng) {
+	ULONG	dat_08;
+	ULONG	dat_0b;
+
+#ifdef PHY_BCM5389_Debug_Counter
+	if ( eng->BCMIMP.wait_init )
+		BCM5389_counter ( eng );
+#endif
+
+	//------------------------------
+	// Page 0
+	//------------------------------
+	// [page 0:0x0e] IMP Port State Override Register           //([3:0] = 0xb) && ([7] = 1) for 1G
+	// [page 0:0x0e] IMP Port State Override Register           //([3:0] = 0x7) && ([7] = 1) for 100M
+	// [page 0:0x0e] IMP Port State Override Register           //([3:0] = 0x3) && ([7] = 1) for 10M
+	// [page 0:0x60] IMP RGMII Control Register                 // [1:0] = 0x0
+	// [page 0:0x08] IMP Traffic Control Register               // [4:2] = 0x7 for Enable IMP Port to receive the other Port's data
+	// [page 0:0x08] IMP Traffic Control Register               // [4:2] = 0x0 for Disable IMP Port to receive the other Port's data
+	// [page 0:0x0b] Switch Mode Register                       // [  0] = 0x1 for Enable IMP Port to receive the other Port's data
+	// [page 0:0x0b] Switch Mode Register                       // [  0] = 0x0 for Disable IMP Port to receive the other Port's data
+	//------------------------------
+	BCMIMP_set_page ( eng, 0 );
+
+	//[page 0:0x0e] IMP Port State Override Register
+	BCMIMP_reg_read ( eng, 0x0e );
+	if ( eng->run.Speed_sel[ 0 ] )
+		BCMIMP_reg_write ( eng, 0x0e, 0x00000000, (eng->BCMIMP.rddata[0] & 0x0fffff70) | 0x0000008b ); // 8 bit //  1 G//[0]LINK, [1]FDX, [3:2]SPEED, [7]MII_SW_OR
+	else if ( eng->run.Speed_sel[ 1 ] )
+		BCMIMP_reg_write ( eng, 0x0e, 0x00000000, (eng->BCMIMP.rddata[0] & 0x0fffff70) | 0x00000087 ); // 8 bit //100 M//[0]LINK, [1]FDX, [3:2]SPEED, [7]MII_SW_OR
+	else
+		BCMIMP_reg_write ( eng, 0x0e, 0x00000000, (eng->BCMIMP.rddata[0] & 0x0fffff70) | 0x00000083 ); // 8 bit // 10 M//[0]LINK, [1]FDX, [3:2]SPEED, [7]MII_SW_OR
+#ifdef PHY_BCM5389_Debug_RWValue
+	BCMIMP_reg_chgcheck( eng, 0, 0x0e );
+#endif
+
+	if ( eng->BCMIMP.wait_init ) {
+		//[page 0:0x60] IMP RGMII Control Register
+		BCMIMP_reg_read ( eng, 0x60 );
+		if ( ( eng->BCMIMP.rddata[0] & 0x3 ) != 0 ) {
+			printf("\n\n[Warning] Page0, Register 0x60, bit 0~1 must be 0 [Reg60h_0:%08lx]\n\n", eng->BCMIMP.rddata[0] );
+			if ( eng->run.TM_IOTiming ) PRINTF( FP_IO, "\n\n[Warning] Page0, Register 0x60, bit 0~1 must be 0 [Reg60h_0:%08lx]\n\n", eng->BCMIMP.rddata[0] );
+			if ( !eng->run.TM_Burst ) PRINTF( FP_LOG, "\n\n[Warning] Page0, Register 0x60, bit 0~1 must be 0 [Reg60h_0:%08lx]\n\n", eng->BCMIMP.rddata[0] );
+			BCMIMP_reg_write ( eng, 0x60, 0x00000000, (eng->BCMIMP.rddata[0] & 0xfffffffc) ); // 8 bit //[0]TXC_DELAY, [1]RXC_DELAY
+		}
+#ifdef PHY_BCM5389_Debug_RWValue
+		BCMIMP_reg_chgcheck( eng, 0, 0x60 );
+#endif
+
+		if ( eng->phy.loop_phy ) {
+			// Enable IMP Port to receive the other Port's data
+			dat_08 = 0x0000001c;
+			dat_0b = 0x00000003;
+		} else {
+			// Disable IMP Port to receive the other Port's data
+			dat_08 = 0x00000000;
+			dat_0b = 0x00000002;
+		}
+		//[page 0:0x08] IMP Traffic Control Register
+		BCMIMP_reg_read ( eng, 0x08 );
+		BCMIMP_reg_write ( eng, 0x08, 0x00000000, (eng->BCMIMP.rddata[0] & 0xffffffe3) | dat_08 ); // 8 bit //[2]RX_BCST_EN, [3]RX_MCST_EN, [4]RX_UCST_EN
+#ifdef PHY_BCM5389_Debug_RWValue
+		BCMIMP_reg_chgcheck( eng, 0, 0x08 );
+#endif
+
+		//[page 0:0x0b] Switch Mode Register
+		BCMIMP_reg_read ( eng, 0x0b );
+		BCMIMP_reg_write ( eng, 0x0b, 0x00000000, (eng->BCMIMP.rddata[0] & 0xfffffffc) | dat_0b ); // 8 bit //[0]SW_FWDG_MODE, [1]SW_FWDG_EN
+#ifdef PHY_BCM5389_Debug_RWValue
+		BCMIMP_reg_chgcheck( eng, 0, 0x0b );
+#endif
+
+		//------------------------------
+		// Page 2
+		//------------------------------
+		// [page 2:0x00] Global Management Configuration Register   // [  7] = 0x1 for IMP Port Enable
+		//------------------------------
+		BCMIMP_set_page ( eng, 2 );
+
+		//[page 2:0x00] Global Management Configuration Register
+		BCMIMP_reg_read ( eng, 0x00 );
+		BCMIMP_reg_write ( eng, 0x00, 0x00000000, (eng->BCMIMP.rddata[0] & 0xffffff7f) | 0x00000080 ); // 8 bit //[7]IMP Port Enable
+#ifdef PHY_BCM5389_Debug_RWValue
+		BCMIMP_reg_chgcheck( eng, 2, 0x00 );
+#endif
+
+		eng->BCMIMP.wait_init = 0;
+	}
+
+	phy_write( eng, 16, 0x0000 );
+}
+
+//------------------------------------------------------------
+// BCM 5396
+//------------------------------------------------------------
+void BCM5396_reg_init (MAC_ENGINE *eng) {
+	//------------------------------
+	// Page 0
+	//------------------------------
+	// [page 0:0x70] Port 16 State Override Register            //([3:0] = 0xb) && ([7] = 1) for 1G
+	// [page 0:0x70] Port 16 State Override Register            //([3:0] = 0x7) && ([7] = 1) for 100M
+	// [page 0:0x70] Port 16 State Override Register            //([3:0] = 0x3) && ([7] = 1) for 10M
+	// [page 0:0x10] IMP Port (Port 16) Control Register        // [4:2] = 0x7 for Enable IMP Port to receive the other Port's data
+	// [page 0:0x20] Switch Mode Register                       // [  0] = 0x1 for Enable IMP Port to receive the other Port's data
+	//------------------------------
+	BCMIMP_set_page ( eng, 0 );
+
+	//[page 0:0x70] Port 16 State Override Register
+	BCMIMP_reg_read ( eng, 0x70 );
+	if ( eng->run.Speed_sel[ 0 ] )
+		BCMIMP_reg_write ( eng, 0x70, 0x00000000, (eng->BCMIMP.rddata[0] & 0x0fffff70) | 0x0000008b ); // 8 bit //  1 G//[0]Link State, [1]Duplex Mode, [3:2]Speed, [7]Software Override
+	else if ( eng->run.Speed_sel[ 1 ] )
+		BCMIMP_reg_write ( eng, 0x70, 0x00000000, (eng->BCMIMP.rddata[0] & 0x0fffff70) | 0x00000087 ); // 8 bit //100 M//[0]Link State, [1]Duplex Mode, [3:2]Speed, [7]Software Override
+	else
+		BCMIMP_reg_write ( eng, 0x70, 0x00000000, (eng->BCMIMP.rddata[0] & 0x0fffff70) | 0x00000083 ); // 8 bit // 10 M//[0]Link State, [1]Duplex Mode, [3:2]Speed, [7]Software Override
+#ifdef PHY_BCM5396_Debug_RWValue
+	BCMIMP_reg_chgcheck( eng, 0, 0x70 );
+#endif
+
+	if ( eng->BCMIMP.wait_init ) {
+		//[page 0:0x10] IMP Port (Port 16) Control Register
+		BCMIMP_reg_read ( eng, 0x10 );
+		BCMIMP_reg_write ( eng, 0x10, 0x00000000, (eng->BCMIMP.rddata[0] & 0xffffffe3) | 0x0000001c ); // 8 bit //[2]RX_BCST_EN, [3]RX_MCST_EN, [4]RX_UCST_EN
+#ifdef PHY_BCM5396_Debug_RWValue
+		BCMIMP_reg_chgcheck( eng, 0, 0x10 );
+#endif
+
+		//[page 0:0x20] Switch Mode Register
+		BCMIMP_reg_read ( eng, 0x20 );
+		BCMIMP_reg_write ( eng, 0x20, 0x00000000, (eng->BCMIMP.rddata[0] & 0xfffffffc) | 0x00000003 ); // 8 bit //[0]SW_FWDG_MODE, [1]SW_FWDG_EN
+#ifdef PHY_BCM5396_Debug_RWValue
+		BCMIMP_reg_chgcheck( eng, 0, 0x20 );
+#endif
+
+		//------------------------------
+		// Page 2
+		//------------------------------
+		// [page 2:0x00] Global Management Configuration Register   // [7:6] = 0x2 for IMP Port Enable
+		//------------------------------
+		BCMIMP_set_page ( eng, 2 );
+
+		//[page 2:0x00] Global Management Configuration Register
+		BCMIMP_reg_read ( eng, 0x00 );
+		BCMIMP_reg_write ( eng, 0x00, 0x00000000, (eng->BCMIMP.rddata[0] & 0xffffff3f) | 0x00000080 ); // 8 bit //[7:6]FRM_MNGT_PORT
+#ifdef PHY_BCM5396_Debug_RWValue
+		BCMIMP_reg_chgcheck( eng, 2, 0x00 );
+#endif
+
+		eng->BCMIMP.wait_init = 0;
+	}
+
+	phy_write( eng, 16, 0x0000 );
+}
+
+
+
+
+
+//------------------------------------------------------------
+// Special PHY
+//------------------------------------------------------------
+void special_PHY_init (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("special_PHY_init\n");
+	Debug_delay();
+#endif
+	BCMIMP_init( eng );
+}
+
+//------------------------------------------------------------
+void special_PHY_MDIO_init (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("special_PHY_MDIO_init\n");
+	Debug_delay();
+#endif
+	BCMIMP_MDIO_init( eng );
+}
+
+//------------------------------------------------------------
+void special_PHY_buf_init (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("special_PHY_buf_init\n");
+	Debug_delay();
+#endif
+	BCMIMP_buf_init( eng );
+}
+
+//------------------------------------------------------------
+void special_PHY_recov (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("special_PHY_recov\n");
+	Debug_delay();
+#endif
+}
+
+//------------------------------------------------------------
+void special_PHY_reg_init (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("special_PHY_reg_init\n");
+	Debug_delay();
+#endif
+	switch ( eng->BCMIMP.PHY_sel ) {
+		case 0:	BCM5389_reg_init( eng ); break;
+		case 1:	BCM5396_reg_init( eng ); break;
+		default: break;
+	}
+}
+
+//------------------------------------------------------------
+void special_PHY_debug (MAC_ENGINE *eng) {
+#ifdef  DbgPrn_FuncHeader
+	printf("special_PHY_debug\n");
+	Debug_delay();
+#endif
+#ifdef PHY_BCM5389_Debug_Counter
+	if ( eng->BCMIMP.PHY_sel == 0 )
+		BCM5389_counter( eng );
+#endif
+}
+
+//------------------------------------------------------------
+// be used in setup_framesize()
+ULONG special_PHY_FRAME_LEN (MAC_ENGINE *eng) {
+	return ( eng->BCMIMP.pkt_tx_len );
+}
+
+//------------------------------------------------------------
+// be used in setup_buf()
+ULONG *special_PHY_txpkt_ptr (MAC_ENGINE *eng) {
+	return ( (ULONG *)eng->BCMIMP.pkt_tx );
+}
+
+//------------------------------------------------------------
+// be used in check_Data()
+ULONG *special_PHY_rxpkt_ptr (MAC_ENGINE *eng) {
+	return ( (ULONG *)eng->BCMIMP.pkt_rx );
+}
+#endif // End PHY_SPECIAL
--- uboot_old/oem/ami/standalone/nettest/PHYSPECIAL.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/PHYSPECIAL.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,41 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef PHYSPECIAL_H
+#define PHYSPECIAL_H
+
+#define PHY_BCM5396_Debug_RWValue
+#define PHY_BCM5389_Debug_RWValue
+#define PHY_BCM5389_Debug_Counter
+
+
+#define PHY_BCMIMP_TIME_OUT_RW                  100       //10
+//#define PHY_BCMIMP_TIME_OUT_RW                  10       //10
+
+#define PHY_BCMIMP_Pkt_Byte_Len                 1524      //1524/1514
+#define PHY_BCMIMP_Pkt_DW_Len                    381
+//#define PHY_BCMIMP_Pkt_Byte_Len                 120      //120/110   
+//#define PHY_BCMIMP_Pkt_DW_Len                    30
+
+typedef struct {
+	ULONG                pkt_tx_len                              ;//__attribute__ ((aligned (4)));
+	ULONG                pkt_rx[PHY_BCMIMP_Pkt_DW_Len]           ;//__attribute__ ((aligned (4)));
+	ULONG                pkt_rx_len                              ;//__attribute__ ((aligned (4)));
+	ULONG               *pkt_tx                                  ;//__attribute__ ((aligned (4)));
+	char                 wait_init                               ;//__attribute__ ((aligned (4)));
+	ULONG                rddata[2];
+
+	char                 PHY_sel                                 ;//__attribute__ ((aligned (4)));
+} PHY_BCMIMP;
+
+
+
+#endif // PHYSPECIAL_H
--- uboot_old/oem/ami/standalone/nettest/SPIM.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/SPIM.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,64 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define SPIM_C
+static const char ThisFile[] = "SPIM.c";
+
+#include "SWFUNC.H"
+
+#ifdef SPI_BUS
+
+#include <stdio.h>
+#include <stdlib.h>
+#include <conio.h>
+#include <string.h>
+#include "TYPEDEF.H"
+#include "LIB_SPI.H"
+
+#define SPIM_CMD_WHA     0x01
+#define SPIM_CMD_RD      0x0B
+#define SPIM_CMD_DRD     0xBB
+#define SPIM_CMD_WR      0x02
+#define SPIM_CMD_DWR     0xD2
+#define SPIM_CMD_STA     0x05
+#define SPIM_CMD_ENBYTE  0x06
+#define SPIM_CMD_DISBYTE 0x04
+
+ULONG spim_cs;
+ULONG spim_base;
+ULONG spim_hadr;
+
+void spim_end()
+{
+  ULONG data;
+
+  data = MIndwm((ULONG)mmiobase, 0x1E620010 + (spim_cs << 2));
+  MOutdwm( (ULONG)mmiobase, 0x1E620010 + (spim_cs << 2), data | 0x4);
+  MOutdwm( (ULONG)mmiobase, 0x1E620010 + (spim_cs << 2), data);
+}
+
+//------------------------------------------------------------
+void spim_init(int cs)
+{
+  ULONG data;
+
+  spim_cs = cs;
+  MOutdwm( (ULONG)mmiobase, 0x1E620000, (0x2 << (cs << 1)) | (0x10000 << cs));
+  MOutdwm( (ULONG)mmiobase, 0x1E620010 + (cs << 2), 0x00000007);
+  MOutdwm( (ULONG)mmiobase, 0x1E620010 + (cs << 2), 0x00002003);
+  MOutdwm( (ULONG)mmiobase, 0x1E620004, 0x100 << cs);
+  data = MIndwm((ULONG)mmiobase, 0x1E620030 + (cs << 2));
+  spim_base = 0x20000000 | ((data & 0x007f0000) << 7);
+  MOutwm ( (ULONG)mmiobase, spim_base, SPIM_CMD_WHA);
+  spim_end();
+  spim_hadr = 0;
+}
+#endif // End SPI_BUS
--- uboot_old/oem/ami/standalone/nettest/stdalone.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/stdalone.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,44 @@
+#include "stdalone.h"
+#include "COMMINF.H"
+
+
+/*standalone application*/
+cmd_map_t cmd_map[CMD_MAX] = {
+	{"mactest", s_do_mactest},
+	{"mactestd", s_do_mactestd},
+	{"ncsitest", s_do_ncsitest},
+	{"ncsitestd", s_do_ncsitestd},
+	{"dramtest", s_do_dramtest},
+};
+
+extern int main_function(int argc, char * const argv[], char mode);
+extern int dram_stress_function(int argc, char * const argv[]);
+
+int s_do_mactest(int argc, char * const argv[])
+{
+    display_lantest_log_msg = 0;
+    return main_function( argc, argv, MODE_DEDICATED );
+}
+
+int s_do_mactestd(int argc, char * const argv[])
+{
+    display_lantest_log_msg = 1;
+    return main_function( argc, argv, MODE_DEDICATED );
+}
+
+int s_do_ncsitest(int argc, char * const argv[])
+{
+    display_lantest_log_msg = 0;
+    return main_function( argc, argv, MODE_NSCI );
+}
+
+int s_do_ncsitestd(int argc, char * const argv[])
+{
+    display_lantest_log_msg = 1;
+    return main_function( argc, argv, MODE_NSCI );
+}
+
+int s_do_dramtest (int argc, char * const argv[])
+{
+    return dram_stress_function( argc, argv);
+}
--- uboot_old/oem/ami/standalone/nettest/stdalone.h	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/stdalone.h	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,12 @@
+#define CMD_MAX 5
+
+typedef struct {
+	char cmdname[20];
+	int (*cmdfunc)(int argc, char * const argv[]);
+}cmd_map_t;
+
+int s_do_mactest (int argc, char * const argv[]);
+int s_do_mactestd (int argc, char * const argv[]);
+int s_do_ncsitest (int argc, char * const argv[]);
+int s_do_ncsitestd (int argc, char * const argv[]);
+int s_do_dramtest (int argc, char * const argv[]);
--- uboot_old/oem/ami/standalone/nettest/STDUBOOT.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/STDUBOOT.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,236 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#define STDUBOOT_C
+static const char ThisFile[] = "STDUBOOT.c";
+
+#include "SWFUNC.H"
+
+#ifdef SLT_UBOOT
+
+int isspace ( char c )
+{
+    if ( ( c == ' ' ) || ( c == 9 ) || ( c == 13 ) )
+        return 1;
+
+    return 0;
+}
+
+/*
+ * strtoul.c --
+ *
+ *  Source code for the "strtoul" library procedure.
+ *
+ * Copyright 1988 Regents of the University of California
+ * Permission to use, copy, modify, and distribute this
+ * software and its documentation for any purpose and without
+ * fee is hereby granted, provided that the above copyright
+ * notice appear in all copies.  The University of California
+ * makes no representations about the suitability of this
+ * software for any purpose.  It is provided "as is" without
+ * express or implied warranty.
+ */
+
+//#include <ctype.h>
+
+/*
+ * The table below is used to convert from ASCII digits to a
+ * numerical equivalent.  It maps from '0' through 'z' to integers
+ * (100 for non-digit characters).
+ */
+
+static char cvtIn[] = {
+    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,       /* '0' - '9' */
+    100, 100, 100, 100, 100, 100, 100,      /* punctuation */
+    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, /* 'A' - 'Z' */
+    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
+    30, 31, 32, 33, 34, 35,
+    100, 100, 100, 100, 100, 100,       /* punctuation */
+    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, /* 'a' - 'z' */
+    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
+    30, 31, 32, 33, 34, 35};
+
+/*
+ *----------------------------------------------------------------------
+ *
+ * strtoul --
+ *
+ *  Convert an ASCII string into an integer.
+ *
+ * Results:
+ *  The return value is the integer equivalent of string.  If endPtr
+ *  is non-NULL, then *endPtr is filled in with the character
+ *  after the last one that was part of the integer.  If string
+ *  doesn't contain a valid integer value, then zero is returned
+ *  and *endPtr is set to string.
+ *
+ * Side effects:
+ *  None.
+ *
+ *----------------------------------------------------------------------
+ */
+
+unsigned long int
+strtoul(char *string, char **endPtr, int base)
+    /* string;     String of ASCII digits, possibly
+                 * preceded by white space.  For bases
+                 * greater than 10, either lower- or
+                 * upper-case digits may be used.
+                 */
+    /* **endPtr;   Where to store address of terminating
+                 * character, or NULL. */
+    /* base;       Base for conversion.  Must be less
+                 * than 37.  If 0, then the base is chosen
+                 * from the leading characters of string:
+                 * "0x" means hex, "0" means octal, anything
+                 * else means decimal.
+                 */
+{
+    register char *p;
+    register unsigned long int result = 0;
+    register unsigned digit;
+    int anyDigits = 0;
+
+    /*
+     * Skip any leading blanks.
+     */
+
+    p = string;
+    while (isspace(*p)) {
+    p += 1;
+    }
+
+    /*
+     * If no base was provided, pick one from the leading characters
+     * of the string.
+     */
+
+    if (base == 0)
+    {
+    if (*p == '0') {
+        p += 1;
+        if (*p == 'x') {
+        p += 1;
+        base = 16;
+        } else {
+
+        /*
+         * Must set anyDigits here, otherwise "0" produces a
+         * "no digits" error.
+         */
+
+        anyDigits = 1;
+        base = 8;
+        }
+    }
+    else base = 10;
+    } else if (base == 16) {
+
+    /*
+     * Skip a leading "0x" from hex numbers.
+     */
+
+    if ((p[0] == '0') && (p[1] == 'x')) {
+        p += 2;
+    }
+    }
+
+    /*
+     * Sorry this code is so messy, but speed seems important.  Do
+     * different things for base 8, 10, 16, and other.
+     */
+
+    if (base == 8) {
+    for ( ; ; p += 1) {
+        digit = *p - '0';
+        if (digit > 7) {
+        break;
+        }
+        result = (result << 3) + digit;
+        anyDigits = 1;
+    }
+    } else if (base == 10) {
+    for ( ; ; p += 1) {
+        digit = *p - '0';
+        if (digit > 9) {
+        break;
+        }
+        result = (10*result) + digit;
+        anyDigits = 1;
+    }
+    } else if (base == 16) {
+    for ( ; ; p += 1) {
+        digit = *p - '0';
+        if (digit > ('z' - '0')) {
+        break;
+        }
+        digit = cvtIn[digit];
+        if (digit > 15) {
+        break;
+        }
+        result = (result << 4) + digit;
+        anyDigits = 1;
+    }
+    } else {
+    for ( ; ; p += 1) {
+        digit = *p - '0';
+        if (digit > ('z' - '0')) {
+        break;
+        }
+        digit = cvtIn[digit];
+        if (digit >= base) {
+        break;
+        }
+        result = result*base + digit;
+        anyDigits = 1;
+    }
+    }
+
+    /*
+     * See if there were any digits at all.
+     */
+
+    if (!anyDigits) {
+    p = string;
+    }
+
+    if (endPtr != 0) {
+    *endPtr = p;
+    }
+
+    return result;
+}
+
+// -----------------------------------------------------------------------------
+int atoi( char s[] )
+{
+
+    int i;
+    int ans = 0;
+
+    for( i = 0; s[i] >= '0' && s[i] <= '9'; ++i )
+        ans = ( 10 * ans ) + ( s[i] - '0' );
+
+    return ans;
+}
+
+// -----------------------------------------------------------------------------
+/* rand:return pseudo-random integer on 0...32767 */
+int rand(void)
+{
+    static unsigned long int next = 1;
+
+    next = next * 1103515245 + 12345;
+
+    return (unsigned int) ( next / 65536 ) % 32768;
+}
+
+#endif // End SLT_UBOOT
--- uboot_old/oem/ami/standalone/nettest/STDUBOOT.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/STDUBOOT.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,19 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef STDUBOOT_H
+#define STDUBOOT_H
+
+unsigned long int strtoul(char *string, char **endPtr, int base);
+int atoi( char s[] );
+int rand(void);
+
+#endif // End STDUBOOT_H
--- uboot_old/oem/ami/standalone/nettest/STRESS.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/STRESS.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,148 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+#define STRESS_C
+static const char ThisFile[] = "STRESS.c";
+
+#include "SWFUNC.H"
+#include "COMMINF.H"
+#include "IO.H"
+
+#define DRAM_MapAdr			81000000
+#define TIMEOUT_DRAM              5000000
+
+unsigned long int strtoul(char *string, char **endPtr, int base);
+
+
+/* ------------------------------------------------------------------------- */
+int MMCTestBurst(unsigned int datagen)
+{
+  unsigned int data;
+  unsigned int timeout = 0;
+  
+  WriteSOC_DD( 0x1E6E0070, 0x00000000 );
+  WriteSOC_DD( 0x1E6E0070, (0x000000C1 | (datagen << 3)) );
+  
+  do {
+    data = ReadSOC_DD( 0x1E6E0070 ) & 0x3000;
+
+    if( data & 0x2000 )
+      return(0);
+
+    if( ++timeout > TIMEOUT_DRAM ) {
+      printf("Timeout!!\n");
+      WriteSOC_DD( 0x1E6E0070, 0x00000000 );
+      
+      return(0);
+    }
+  } while( !data );
+  WriteSOC_DD( 0x1E6E0070, 0x00000000 );
+  
+  return(1);
+}
+
+/* ------------------------------------------------------------------------- */
+int MMCTestSingle(unsigned int datagen)
+{
+  unsigned int data;
+  unsigned int timeout = 0;
+
+  WriteSOC_DD( 0x1E6E0070, 0x00000000 );
+  WriteSOC_DD( 0x1E6E0070, (0x00000085 | (datagen << 3)) );
+  
+  do {
+    data = ReadSOC_DD( 0x1E6E0070 ) & 0x3000;
+    
+    if( data & 0x2000 )
+      return(0);
+    
+    if( ++timeout > TIMEOUT_DRAM ){
+      printf("Timeout!!\n");
+      WriteSOC_DD( 0x1E6E0070, 0x00000000 );
+      
+      return(0);
+    }
+  } while ( !data );
+  WriteSOC_DD( 0x1E6E0070, 0x00000000 );
+  
+  return(1);
+}
+
+/* ------------------------------------------------------------------------- */
+int MMCTest(void)
+{
+  unsigned int pattern;
+
+  pattern = ReadSOC_DD( 0x1E6E2078 );
+  printf("Pattern = %08X : ",pattern);
+
+  WriteSOC_DD(0x1E6E0074, (DRAM_MapAdr | 0x7fffff) );
+  WriteSOC_DD(0x1E6E007C, pattern );
+  
+  if(!MMCTestBurst(0))    return(0);
+  if(!MMCTestBurst(1))    return(0);
+  if(!MMCTestBurst(2))    return(0);
+  if(!MMCTestBurst(3))    return(0);
+  if(!MMCTestBurst(4))    return(0);
+  if(!MMCTestBurst(5))    return(0);
+  if(!MMCTestBurst(6))    return(0);
+  if(!MMCTestBurst(7))    return(0);
+  if(!MMCTestSingle(0))   return(0);
+  if(!MMCTestSingle(1))   return(0);
+  if(!MMCTestSingle(2))   return(0);
+  if(!MMCTestSingle(3))   return(0);
+  if(!MMCTestSingle(4))   return(0);
+  if(!MMCTestSingle(5))   return(0);
+  if(!MMCTestSingle(6))   return(0);
+  if(!MMCTestSingle(7))   return(0);
+
+  return(1);
+}
+
+/* ------------------------------------------------------------------------- */
+int dram_stress_function(int argc, char *argv[])
+{
+    unsigned int PassCnt     = 0;
+    unsigned int Testcounter = 0;
+    int          ret = 1;
+    char         *stop_at;
+
+    printf("**************************************************** \n");       
+    printf("*** ASPEED Stress DRAM                           *** \n");
+    printf("***                          20131107 for u-boot *** \n");
+    printf("**************************************************** \n"); 
+    printf("\n"); 
+  
+    if ( argc != 2 ){
+        ret = 0;
+        return ( ret );
+    }
+    else {
+        Testcounter = (unsigned int) strtoul(argv[1], &stop_at, 10);                     
+    }
+
+    WriteSOC_DD(0x1E6E0000, 0xFC600309);   
+
+    while( ( Testcounter > PassCnt ) || ( Testcounter == 0 ) ){
+        if( !MMCTest() ) {
+            printf("FAIL...%d/%d\n", PassCnt, Testcounter);
+            ret = 0;
+      
+            break;
+        }
+        else {
+            PassCnt++;
+            printf("Pass %d/%d\n", PassCnt, Testcounter);
+        }
+     } // End while()
+     
+     return( ret );
+}
+
--- uboot_old/oem/ami/standalone/nettest/stubs.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/stubs.c	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,237 @@
+#include <common.h>
+#include <exports.h>
+
+#ifndef GCC_VERSION
+#define GCC_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)
+#endif /* GCC_VERSION */
+
+#if defined(CONFIG_X86)
+/*
+ * x86 does not have a dedicated register to store the pointer to
+ * the global_data. Thus the jump table address is stored in a
+ * global variable, but such approach does not allow for execution
+ * from flash memory. The global_data address is passed as argv[-1]
+ * to the application program.
+ */
+static void **jt;
+gd_t *global_data;
+
+#define EXPORT_FUNC(x) \
+	asm volatile (			\
+"	.globl " #x "\n"		\
+#x ":\n"				\
+"	movl	%0, %%eax\n"		\
+"	movl	jt, %%ecx\n"		\
+"	jmp	*(%%ecx, %%eax)\n"	\
+	: : "i"(XF_ ## x * sizeof(void *)) : "eax", "ecx");
+#elif defined(CONFIG_PPC)
+/*
+ * r2 holds the pointer to the global_data, r11 is a call-clobbered
+ * register
+ */
+#define EXPORT_FUNC(x) \
+	asm volatile (			\
+"	.globl " #x "\n"		\
+#x ":\n"				\
+"	lwz	%%r11, %0(%%r2)\n"	\
+"	lwz	%%r11, %1(%%r11)\n"	\
+"	mtctr	%%r11\n"		\
+"	bctr\n"				\
+	: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "r11");
+#elif defined(CONFIG_ARM)
+/*
+ * r8 holds the pointer to the global_data, ip is a call-clobbered
+ * register
+ */
+#define EXPORT_FUNC(x) \
+	asm volatile (			\
+"	.globl " #x "\n"		\
+#x ":\n"				\
+"	ldr	ip, [r8, %0]\n"		\
+"	ldr	pc, [ip, %1]\n"		\
+	: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "ip");
+#elif defined(CONFIG_MIPS)
+/*
+ * k0 ($26) holds the pointer to the global_data; t9 ($25) is a call-
+ * clobbered register that is also used to set gp ($26). Note that the
+ * jr instruction also executes the instruction immediately following
+ * it; however, GCC/mips generates an additional `nop' after each asm
+ * statement
+ */
+#define EXPORT_FUNC(x) \
+	asm volatile (			\
+"	.globl " #x "\n"		\
+#x ":\n"				\
+"	lw	$25, %0($26)\n"		\
+"	lw	$25, %1($25)\n"		\
+"	jr	$25\n"			\
+	: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "t9");
+#elif defined(CONFIG_NIOS2)
+/*
+ * gp holds the pointer to the global_data, r8 is call-clobbered
+ */
+#define EXPORT_FUNC(x) \
+	asm volatile (			\
+"	.globl " #x "\n"		\
+#x ":\n"				\
+"	movhi	r8, %%hi(%0)\n"		\
+"	ori	r8, r0, %%lo(%0)\n"	\
+"	add	r8, r8, gp\n"		\
+"	ldw	r8, 0(r8)\n"		\
+"	ldw	r8, %1(r8)\n"		\
+"	jmp	r8\n"			\
+	: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "gp");
+#elif defined(CONFIG_M68K)
+/*
+ * d7 holds the pointer to the global_data, a0 is a call-clobbered
+ * register
+ */
+#define EXPORT_FUNC(x) \
+	asm volatile (			\
+"	.globl " #x "\n"		\
+#x ":\n"				\
+"	move.l	%%d7, %%a0\n"		\
+"	adda.l	%0, %%a0\n"		\
+"	move.l	(%%a0), %%a0\n"		\
+"	adda.l	%1, %%a0\n"		\
+"	move.l	(%%a0), %%a0\n"		\
+"	jmp	(%%a0)\n"			\
+	: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "a0");
+#elif defined(CONFIG_MICROBLAZE)
+/*
+ * r31 holds the pointer to the global_data. r5 is a call-clobbered.
+ */
+#define EXPORT_FUNC(x)				\
+	asm volatile (				\
+"	.globl " #x "\n"			\
+#x ":\n"					\
+"	lwi	r5, r31, %0\n"			\
+"	lwi	r5, r5, %1\n"			\
+"	bra	r5\n"				\
+	: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "r5");
+#elif defined(CONFIG_BLACKFIN)
+/*
+ * P3 holds the pointer to the global_data, P0 is a call-clobbered
+ * register
+ */
+#define EXPORT_FUNC(x)			\
+	asm volatile (			\
+"	.globl _" #x "\n_"		\
+#x ":\n"				\
+"	P0 = [P3 + %0]\n"		\
+"	P0 = [P0 + %1]\n"		\
+"	JUMP (P0)\n"			\
+	: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "P0");
+#elif defined(CONFIG_AVR32)
+/*
+ * r6 holds the pointer to the global_data. r8 is call clobbered.
+ */
+#define EXPORT_FUNC(x)					\
+	asm volatile(					\
+		"	.globl\t" #x "\n"		\
+		#x ":\n"				\
+		"	ld.w	r8, r6[%0]\n"		\
+		"	ld.w	pc, r8[%1]\n"		\
+		:					\
+		: "i"(offsetof(gd_t, jt)), "i"(XF_ ##x)	\
+		: "r8");
+#elif defined(CONFIG_SH)
+/*
+ * r13 holds the pointer to the global_data. r1 is a call clobbered.
+ */
+#define EXPORT_FUNC(x)					\
+	asm volatile (					\
+		"	.align	2\n"			\
+		"	.globl " #x "\n"		\
+		#x ":\n"				\
+		"	mov	r13, r1\n"		\
+		"	add	%0, r1\n"		\
+		"	mov.l @r1, r2\n"	\
+		"	add	%1, r2\n"		\
+		"	mov.l @r2, r1\n"	\
+		"	jmp	@r1\n"			\
+		"	nop\n"				\
+		"	nop\n"				\
+		: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "r1", "r2");
+#elif defined(CONFIG_SPARC)
+/*
+ * g7 holds the pointer to the global_data. g1 is call clobbered.
+ */
+#define EXPORT_FUNC(x)					\
+	asm volatile(					\
+"	.globl\t" #x "\n"				\
+#x ":\n"						\
+"	set %0, %%g1\n"					\
+"	or %%g1, %%g7, %%g1\n"				\
+"	ld [%%g1], %%g1\n"				\
+"	ld [%%g1 + %1], %%g1\n"				\
+"	jmp %%g1\n"					\
+"	nop\n"						\
+	: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "g1" );
+#elif defined(CONFIG_NDS32)
+/*
+ * r16 holds the pointer to the global_data. gp is call clobbered.
+ * not support reduced register (16 GPR).
+ */
+#define EXPORT_FUNC(x) \
+	asm volatile (			\
+"	.globl " #x "\n"		\
+#x ":\n"				\
+"	lwi	$r16, [$gp + (%0)]\n"	\
+"	lwi	$r16, [$r16 + (%1)]\n"	\
+"	jr	$r16\n"			\
+	: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "$r16");
+#elif defined(CONFIG_OPENRISC)
+/*
+ * r10 holds the pointer to the global_data, r13 is a call-clobbered
+ * register
+ */
+#define EXPORT_FUNC(x) \
+	asm volatile (			\
+"	.globl " #x "\n"		\
+#x ":\n"				\
+"	l.lwz	r13, %0(r10)\n"	\
+"	l.lwz	r13, %1(r13)\n"	\
+"	l.jr	r13\n"		\
+"	l.nop\n"				\
+	: : "i"(offsetof(gd_t, jt)), "i"(XF_ ## x * sizeof(void *)) : "r13");
+#else
+/*"	addi	$sp, $sp, -24\n"	\
+"	br	$r16\n"			\*/
+
+#error stubs definition missing for this architecture
+#endif
+
+/* This function is necessary to prevent the compiler from
+ * generating prologue/epilogue, preparing stack frame etc.
+ * The stub functions are special, they do not use the stack
+ * frame passed to them, but pass it intact to the actual
+ * implementation. On the other hand, asm() statements with
+ * arguments can be used only inside the functions (gcc limitation)
+ */
+#if GCC_VERSION < 3004
+static
+#endif /* GCC_VERSION */
+void __attribute__((unused)) dummy(void)
+{
+#include <_exports.h>
+}
+
+#include <asm/sections.h>
+
+void app_startup(char * const *argv)
+{
+	char *cp = __bss_start;
+
+	/* Zero out BSS */
+	while (cp < _end)
+		*cp++ = 0;
+
+#if defined(CONFIG_X86)
+	/* x86 does not have a dedicated register for passing global_data */
+	global_data = (gd_t *)argv[-1];
+	jt = global_data->jt;
+#endif
+}
+
+#undef EXPORT_FUNC
--- uboot_old/oem/ami/standalone/nettest/SWFUNC.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/SWFUNC.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,283 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef SWFUNC_H
+#define SWFUNC_H
+
+#if !defined(DOS_PCIE) && !defined(DOS_LPC) && !defined(NewSLT) && !defined(LinuxAP) && !defined(WINSLT)
+  #include <config.h> // for uboot system
+#endif
+
+//---------------------------------------------------------
+// Program information
+//---------------------------------------------------------
+//#define PHY_GPIO
+//#define PHY_SPECIAL
+//#define MELLANOX_CONNECTX_4 // for Mellanox ConnectX-4 OCP card
+
+#if defined(MELLANOX_CONNECTX_4)
+      #define    VER_NAME    "Ver 0.75 version @2017/05/25 1338 for Mellanox ConnectX-4 OCP card"
+#else
+  #ifdef PHY_SPECIAL
+    /* PHY in SPECIAL mode */
+    #if defined(PHY_GPIO)
+      #define    VER_NAME    "Verification @2016/07/21 1524 [GPIO]PHY_SPECIAL"
+    #else
+      #define    VER_NAME    "Verification @2016/07/21 1524 PHY_SPECIAL"
+    #endif /* End defined(PHY_GPIO) */
+  #else
+    /* PHY in Normal mode */
+    #if defined(PHY_GPIO)
+      #define    VER_NAME    "Ver 0.75 version @2017/05/25 1338 [GPIO]"
+    #else
+      #define    VER_NAME    "Ver 0.77 version @2017/12/20 1310"
+    #endif /* End defined(PHY_GPIO) */
+  #endif
+#endif // End MELLANOX_CONNECTX_4
+
+//#define  UBOOT_FOR_LINUX_4_9
+
+/* == Auto setting ==========================================  */
+/* ====  Support OS system                                     */
+/*        "LinuxAP"                                            */
+/*        "Windows"                                            */
+/*        "SLT_UBOOT"                                          */
+/*        "DOS_ALONE" for independent DOS program              */
+/*        "SLT_NEW_ARCH" for new arch. SLT tool                */
+/* ====  Support interface                                     */
+/*        "SPI_BUS" or "USE_LPC" or "USE_P2A"                  */
+/* ====  Support Chip                                          */
+/*        "AST1010_CHIP" or "AST2500_IOMAP" or "FPGA"          */
+#if defined(CONFIG_AST3200)
+  #define SLT_UBOOT
+  #define AST2500_IOMAP
+#endif
+
+#if defined(CONFIG_AST2500)
+  #define SLT_UBOOT
+  #define AST2500_IOMAP
+#endif
+
+#if defined(CONFIG_AST1500)
+  #define SLT_UBOOT
+  #define ISUBOOT_1_1_4
+  #define AST2500_IOMAP
+#endif
+
+#if defined(CONFIG_AST2400)
+  #define SLT_UBOOT
+#endif
+
+#if defined(CONFIG_AST2300)
+  #define SLT_UBOOT
+#endif
+
+#if defined(CONFIG_AST1010)
+  #define SLT_UBOOT
+  #define AST1010_CHIP
+#endif
+
+#if defined(DOS_PCIE)
+  #define DOS_ALONE
+  #define USE_P2A
+  #if defined(AST25XX)
+    #define AST2500_IOMAP
+  #endif
+#endif
+
+#if defined(DOS_LPC)
+  #define DOS_ALONE
+  #define USE_LPC
+  #if defined(AST25XX)
+    #define AST2500_IOMAP
+  #endif
+  #if defined(AST10XX)
+    #define AST1010_CHIP
+  #endif
+#endif
+
+#if defined(NewSLT) || defined(LinuxAP) || defined(WINSLT)
+  #ifndef  LinuxAP
+  #define SLT_NEW_ARCH
+  #endif
+  #if defined(SLT2500) || defined(SLT2510) || defined(SLT2520) || defined(SLT2530) || defined(SLT1520) || defined(SLT1525) || defined(SLT3200)
+    #define AST2500_IOMAP
+  #endif
+#endif
+
+/* ========================================================== */
+#if defined(SLT_UBOOT) && defined(AST1010_CHIP)
+    #define ENABLE_BIG_ENDIAN
+//  #if ((CONFIG_DRAM_SWAP_BASE & 0x80000000) == 0x0)
+//    #define ENABLE_BIG_ENDIAN_MEM
+//  #endif
+//  #if ((CONFIG_DRAM_SWAP_BASE & 0x80000000) == 0x0)
+//    #define ENABLE_BIG_ENDIAN_REG
+//  #endif
+#endif
+
+/* ========================================================== */
+#if !defined (SLT_UBOOT)
+  #define ENABLE_LOG_FILE
+#endif
+
+/* ========================================================== */
+#ifdef AST1010_CHIP
+  #ifdef SLT_UBOOT
+        #define AST1010_IOMAP                    1
+  #endif
+  #ifdef DOS_ALONE
+        #define AST1010_IOMAP                    2
+
+        // AST1010 only has LPC interface
+        #undef USE_P2A
+        #undef SPI_BUS
+        #define USE_LPC
+  #endif
+#endif
+
+#ifdef SLT_UBOOT
+  #ifdef ISUBOOT_1_1_4
+    #define NETESTCMD_MAX_ARGS CFG_MAXARGS
+  #else
+    #define NETESTCMD_MAX_ARGS CONFIG_SYS_MAXARGS
+  #endif
+#endif
+
+/* == Step 4:==========   Select PHY    ================== */
+//#define SUPPORT_PHY_LAN9303                                   // Initial PHY via I2C bus
+#define LAN9303_I2C_BUSNUM                       6              // 1-based
+#define LAN9303_I2C_ADR                          0x14
+
+/* ====================== Program ======================== */
+// The "PHY_NCSI" option is only for DOS compiler
+#if defined (PHY_NCSI)
+  #ifdef SLT_UBOOT
+        #error Wrong setting......
+  #endif
+#endif
+
+#if defined (PHY_NCSI)
+  #ifdef SUPPORT_PHY_LAN9303
+        #error Wrong setting (Can not support LAN9303)......
+  #endif
+#endif
+
+/* =================  Check setting  ===================== */
+#ifdef SLT_UBOOT
+  #ifdef DOS_ALONE
+        #error Can NOT support two OS
+  #endif
+#endif
+#ifdef DOS_ALONE
+  #ifdef SLT_UBOOT
+        #error Can NOT support two OS
+  #endif
+#endif
+
+#ifdef USE_P2A
+  #ifdef SLT_UBOOT
+        #error Can NOT be set PCI bus in Uboot
+  #endif
+#endif
+#ifdef USE_LPC
+  #ifdef SLT_UBOOT
+        #error Can NOT be set LPC bus in Uboot
+  #endif
+#endif
+#ifdef SPI_BUS
+  #ifdef SLT_UBOOT
+        #error Can NOT be set SPI bus in Uboot
+  #endif
+#endif
+
+/* ======================== Program flow control ======================== */
+#define RUN_STEP                                 5
+// 0: read_scu
+// 1: parameter setup
+// 2: init_scu1,
+// 3: init_scu_macrst
+// 4: Data Initial
+// 5: ALL
+
+/* ====================== Switch print debug message ====================== */
+//#define   DbgPrn_Enable_Debug_delay                //[off]
+//#define   DbgPrn_FuncHeader                        //[off]
+#define   DbgPrn_ErrFlg                          0
+#define   DbgPrn_BufAdr                          0 //1
+#define   DbgPrn_Bufdat                          0
+#define   DbgPrn_BufdatDetail                    0
+#define   DbgPrn_PHYRW                           0
+#define   DbgPrn_PHYInit                         0
+#define   DbgPrn_PHYName                         0
+#define   DbgPrn_DumpMACCnt                      0
+#define   DbgPrn_Info                            0 //1
+#define   DbgPrn_FRAME_LEN                       0
+
+
+/* ============ Enable or Disable Check item of the descriptor ============ */
+#ifdef Enable_MAC_ExtLoop
+#else
+    #define CheckRxOwn
+    #define CheckRxLen
+    #define CheckRxErr
+    #define CheckCRC
+    #define CheckFTL
+    #define CheckRunt
+//    #define CheckOddNibble
+    #define CheckRxFIFOFull
+  #ifdef Enable_MAC_ExtLoop_PakcegMode
+    #define CheckDataEveryTime
+  #else
+//    #define CheckDataEveryTime
+  #endif
+
+//    #define CheckRxbufUNAVA
+    #define CheckRPktLost
+//    #define CheckNPTxbufUNAVA
+    #define CheckTPktLost
+    #define CheckRxBuf
+#endif
+
+#ifdef CheckRxErr
+  #define Check_ErrMask_RxErr                    0x00040000 //bit18
+#else
+  #define Check_ErrMask_RxErr                    0x00000000
+#endif
+#ifdef CheckCRC
+  #define Check_ErrMask_CRC                      0x00080000 //bit19
+#else
+  #define Check_ErrMask_CRC                      0x00000000
+#endif
+#ifdef CheckFTL
+  #define Check_ErrMask_FTL                      0x00100000 //bit20
+#else
+  #define Check_ErrMask_FTL                      0x00000000
+#endif
+#ifdef CheckRunt
+  #define Check_ErrMask_Runt                     0x00200000 //bit21
+#else
+  #define Check_ErrMask_Runt                     0x00000000
+#endif
+#ifdef CheckOddNibble
+  #define Check_ErrMask_OddNibble                0x00400000 //bit22
+#else
+  #define Check_ErrMask_OddNibble                0x00000000
+#endif
+#ifdef CheckRxFIFOFull
+  #define Check_ErrMask_RxFIFOFull               0x00800000 //bit23
+#else
+  #define Check_ErrMask_RxFIFOFull               0x00000000
+#endif
+#define Check_ErrMask_ALL                        ( Check_ErrMask_RxErr | Check_ErrMask_CRC | Check_ErrMask_FTL | Check_ErrMask_Runt | Check_ErrMask_OddNibble | Check_ErrMask_RxFIFOFull )
+
+
+#endif // SWFUNC_H
--- uboot_old/oem/ami/standalone/nettest/TYPEDEF.H	1970-01-01 08:00:00.000000000 +0800
+++ uboot/oem/ami/standalone/nettest/TYPEDEF.H	2018-06-22 17:51:23.925169813 +0800
@@ -0,0 +1,114 @@
+/*
+ *  This program is distributed in the hope that it will be useful,
+ *  but WITHOUT ANY WARRANTY; without even the implied warranty of
+ *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ *  GNU General Public License for more details.
+ *
+ *  You should have received a copy of the GNU General Public License
+ *  along with this program; if not, write to the Free Software
+ *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef TYPEDEF_H
+#define TYPEDEF_H
+
+#include "SWFUNC.H"
+
+//
+// Define
+//
+#define   PCI                   1
+#define   PCIE                  2
+#define   AGP                   3
+#define   ACTIVE                4
+
+#if defined(LinuxAP)
+    #if defined(__amd64__) || defined(__powerpc64__) || defined(__aarch64__)
+    // 64
+        #ifndef ULONG
+        #define ULONG unsigned int
+        #endif
+
+        #ifndef LONG
+        #define LONG int
+        #endif
+
+        #ifndef BOOL
+        #define BOOL unsigned int
+        #endif
+
+    #else
+
+        #ifndef ULONG
+        #define ULONG unsigned long
+        #endif
+
+        #ifndef LONG
+        #define LONG long
+        #endif
+
+        #ifndef BOOL
+        #define BOOL unsigned long
+        #endif
+
+    #endif
+
+    #ifndef USHORT
+    #define USHORT unsigned short
+    #endif
+
+    #ifndef SHORT
+    #define SHORT short
+    #endif
+
+    #ifndef UCHAR
+    #define UCHAR unsigned char
+    #endif
+
+    #ifndef SCHAR
+    #define SCHAR   signed char
+    #endif
+
+    #ifndef CHAR
+    #define CHAR char
+    #endif
+
+    #ifndef BYTE
+    #define BYTE unsigned char
+    #endif
+
+    #ifndef VOID
+    #define VOID void
+    #endif
+
+    #ifndef TRUE
+    #define TRUE 1
+    #endif
+
+    #ifndef FALSE
+    #define FALSE 0
+    #endif
+
+    #ifndef BOOLEAN
+    #define BOOLEAN   unsigned char
+    #endif
+#else
+/* DOS Program */
+    #define     VOID      void
+    #define     FLONG     unsigned long
+    #define     ULONG     unsigned long
+    #define     USHORT    unsigned short
+    #define     UCHAR     unsigned char
+    #define     LONG      long
+    #define     SHORT     short
+    #define     CHAR      char
+    #define     BYTE      UCHAR
+    #define     BOOL      SHORT
+    #define     BOOLEAN   unsigned short
+    #define     PULONG    ULONG *
+    #define     SCHAR     signed char
+#endif
+    #define         TRUE      1
+    #define         FALSE     0
+
+#endif // TYPEDEF_H
