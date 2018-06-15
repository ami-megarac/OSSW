--- u-boot/arch/arm/cpu/pilotcommon/soc_hw.h	1970-01-01 05:30:00.000000000 +0530
+++ u-boot.new/arch/arm/cpu/pilotcommon/soc_hw.h	2014-07-22 10:59:51.337016765 +0530
@@ -0,0 +1,54 @@
+#include <pilot_hw.h>
+
+#define WRITE_REG32(a,d)  (*((volatile u32 *) (a)) = d)
+#define WRITE_REG16(a,d)  (*((volatile u16 *) (a)) = d)
+#define WRITE_REG8(a,d)   (*((volatile u8 *) (a)) = d)
+
+#define READ_REG32(a)  (*((volatile u32 *)(a)))
+#define READ_REG16(a)  (*((volatile u16 *)(a)))
+#define READ_REG8(a)   (*((volatile u8 *)(a)))
+
+#define GPIO_BASE_OFFS   SE_GPIO_BASE
+#define GPIO_GPDO_OFFSET 0x08
+#define GPIO_GPDI_OFFSET 0x09
+#define GPIO_OUTPUT_ENB  0x01
+
+#define GPIO_TYPE_IN     0
+#define GPIO_TYPE_OUT    1
+#define GPIO_TYPE_INVALID 0xFF
+
+#define GPIO_DIR_IN      0
+#define GPIO_DIR_OUT     1
+
+#define GPIO_DATA_LOW    0
+#define GPIO_DATA_HIGH   1
+
+#define GPIO_PULL_DOWN   0
+#define GPIO_PULL_UP     4
+#define GPIO_PULL_NO     6
+
+#define GPIO_00_0	 0	/* GPIO_PORT ( 0, 0) */
+#define GPIO_00_1	 1	/* GPIO_PORT ( 0, 1) */
+
+#define REGISTER_TYPE_BIT8           8
+#define REGISTER_TYPE_BIT16          16
+#define REGISTER_TYPE_BIT32          32
+#define REGISTER_TYPE_INVALID        0xFF
+
+typedef struct
+{
+    unsigned char  num;
+    unsigned char  type;
+    unsigned char  initVal;
+    unsigned char  pull;
+}GPIO;
+
+
+typedef struct
+{
+    unsigned int  Address;
+    unsigned int  Mask;
+    unsigned int  Val;
+    unsigned char RegType;
+}HWREG;
+
