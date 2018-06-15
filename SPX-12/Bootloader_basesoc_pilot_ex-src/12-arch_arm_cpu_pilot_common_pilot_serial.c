--- u-boot/arch/arm/cpu/pilotcommon/pilotserial.c	1970-01-01 05:30:00.000000000 +0530
+++ u-boot.new/arch/arm/cpu/pilotcommon/pilotserial.c	2014-07-30 16:40:43.716632243 +0530
@@ -0,0 +1,149 @@
+/*
+ * (C) Copyright 2002
+ * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ *
+ */
+
+#include <config.h>
+#include <serial.h>
+#include <linux/types.h>
+#include <asm/u-boot.h>
+#include <asm/global_data.h>
+#include "soc_hw.h"
+#include "pilotserial.h"
+
+DECLARE_GLOBAL_DATA_PTR;
+
+#ifdef CONFIG_SERIAL2
+	UINT32 DebugSerialPort = SE_UART_2_BASE ;
+#elif CONFIG_SERIAL3
+	UINT32 DebugSerialPort = SE_UART_3_BASE ;
+#elif CONFIG_SERIAL4
+	UINT32 DebugSerialPort = SE_UART_4_BASE;
+#else
+	#error "Bad: you didn't configure serial ..."
+#endif
+
+
+void
+serialhandler(void *arg)
+{
+	AST_SerialIntIdentification(DebugSerialPort);
+}
+
+#if 0
+void
+ll_serial_init(void)
+{
+	AST_SerialInit (DebugSerialPort, PILOT_BAUD_115200, PARITY_NONE, 0, 8 );
+	AST_SetSerialFifoCtrl(DebugSerialPort, 1, ENABLE, ENABLE);
+}
+#endif
+
+void
+pilot_serial_setbrg(void)
+{
+	unsigned int reg = 0;
+
+	switch (gd->baudrate)
+	{
+		case 9600:
+			reg = PILOT_BAUD_9600;
+			break;
+		case 19200:
+			reg = PILOT_BAUD_19200;
+			break;
+		case 38400:
+			reg = PILOT_BAUD_38400;
+			break;
+		case 57600:
+			reg = PILOT_BAUD_57600;
+			break;
+//#if (SYS_CLK == 22118400)
+		case 115200:
+			reg = PILOT_BAUD_115200;
+			break;
+//#endif
+		default:
+			reg=PILOT_BAUD_38400;
+			break;
+	}
+
+	/* Set Baud Rate */
+	AST_SerialInit (DebugSerialPort, reg, PARITY_NONE, 0, 8 );
+
+	/* Enable FiFo */
+	AST_SetSerialFifoCtrl(DebugSerialPort, 1, ENABLE, ENABLE);
+
+	return;
+}
+
+int
+pilot_serial_init(void)
+{
+	pilot_serial_setbrg();
+	return 0;
+}
+
+
+int
+pilot_serial_getc(void)
+{
+	return AST_GetSerialChar( DebugSerialPort );
+}
+
+
+void pilot_serial_putc(const char c)
+{
+	AST_PutSerialChar( DebugSerialPort, c );
+   	if(c == '\n')
+	   		AST_PutSerialChar(DebugSerialPort,'\r');
+
+}
+
+int pilot_serial_tstc(void)
+{
+	return AST_TestSerialForChar(DebugSerialPort);
+}
+
+
+void pilot_serial_puts (const char *s)
+{
+	AST_PutSerialStr(DebugSerialPort,s);
+}
+
+static struct serial_device pilot_serial_drv = {
+  .name = "pilot_serial",
+  .start  = pilot_serial_init,
+  .stop = NULL,
+  .setbrg = pilot_serial_setbrg,
+  .putc = pilot_serial_putc,
+  .puts = pilot_serial_puts,
+  .getc = pilot_serial_getc,
+  .tstc = pilot_serial_tstc,
+};
+
+void pilot_serial_initialize(void)
+{
+  serial_register(&pilot_serial_drv);
+}
+
+__weak struct serial_device *default_serial_console(void)
+{
+  return &pilot_serial_drv;
+}
+
