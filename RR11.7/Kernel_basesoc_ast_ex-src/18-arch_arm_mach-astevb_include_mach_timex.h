--- linux.org/arch/arm/mach-astevb/include/mach/timex.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/mach-astevb/include/mach/timex.h	2014-01-31 15:42:23.587921284 -0500
@@ -0,0 +1,25 @@
+/*
+ *  linux/include/asm-arm/arch-xxx/timex.h
+ *
+ *  ASPEED SOC architecture timex specifications
+ *
+ *  Copyright (C) 1999 ARM Limited
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
+ */
+
+#include <mach/platform.h>
+
+#define CLOCK_TICK_RATE		CONFIG_TIMER_CLK
