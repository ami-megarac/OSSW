--- linux-3.14.17/arch/arm/mach-pilot/include/mach/pilot4-soc.h	1970-01-01 05:30:00.000000000 +0530
+++ linux-3.14.17.orion/arch/arm/mach-pilot/include/mach/pilot4-soc.h	2015-04-03 14:52:06.205240200 +0530
@@ -0,0 +1,53 @@
+/*
+ * Copyright (C) 2003-2015 Emulex. All rights reserved.
+ * EMULEX is a trademark of Emulex.
+ * www.emulex.com
+ * This program is free software; you can redistribute it and/or modify it under the terms of version 2
+ * of the GNU General Public License as published by the Free Software Foundation.
+ * This program is distributed in the hope that it will be useful. ALL EXPRESS OR IMPLIED CONDITIONS,
+ * REPRESENTATIONS AND WARRANTIES, INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
+ * PARTICULAR PURPOSE, OR NON-INFRINGEMENT, ARE DISCLAIMED, EXCEPT TO THE EXTENT THAT SUCH DISCLAIMERS
+ * ARE HELD TO BE LEGALLY INVALID. See the GNU General Public License for more details, a copy of which
+ * can be found in the file COPYING included with this package.
+ */
+
+#ifndef __MACH_PILOT4_SOC_H__
+#define __MACH_PILOT4_SOC_H__
+
+
+/* For now, all mappings are flat (physical = virtual)
+ */
+
+#define SCU_PERIPH_PHYS			0x40460000
+#define SCU_PERIPH_VIRT			0xFE46C000
+
+/* Virtual addresses now have to stay lower in newer kernels, so move the OCM down
+ * from 0xFFXXXXXX to 0xFEXXXXXX to make it work 
+ */
+#define OCM_LOW_PHYS      0xFFFC0000
+#define OCM_LOW_VIRT      0xFE100000
+
+#define OCM_HIGH_PHYS     0xFFFF1000
+#define OCM_HIGH_VIRT     0xFE200000
+
+/* The following are intended for the devices that are mapped early */
+
+#define SCU_PERIPH_BASE     IOMEM(SCU_PERIPH_VIRT)
+#define SCU_GIC_CPU_BASE    (SCU_PERIPH_BASE + 0x100)
+#define SCU_GLOBAL_TIMER_BASE   (SCU_PERIPH_BASE + 0x200)
+#define SCU_CPU_TIMER_BASE    (SCU_PERIPH_BASE + 0x600)
+#define SCU_GIC_DIST_BASE   (SCU_PERIPH_BASE + 0x1000)
+#define OCM_LOW_BASE      IOMEM(OCM_LOW_VIRT)
+#define OCM_HIGH_BASE     IOMEM(OCM_HIGH_VIRT)
+
+/* There are two OCM addresses needed for communication between CPUs in SMP.
+ * The memory addresses are in the high on-chip RAM and these addresses are
+ * mapped flat (virtual = physical). The memory must be mapped early and
+ * non-cached.
+ */
+#define BOOT_ADDR_OFFSET  0xEFF0
+#define BOOT_STATUS_OFFSET  0xEFF4
+#define BOOT_STATUS_CPU1_UP 1
+
+
+#endif
