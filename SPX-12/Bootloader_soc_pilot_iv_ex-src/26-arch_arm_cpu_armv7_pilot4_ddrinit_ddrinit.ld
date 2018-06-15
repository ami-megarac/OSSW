--- u-boot-2013.07/arch/arm/cpu/armv7/pilot4/ddrinit/ddrinit.ld	1969-12-31 19:00:00.000000000 -0500
+++ mywork/arch/arm/cpu/armv7/pilot4/ddrinit/ddrinit.ld	2015-02-06 05:06:37.000000000 -0500
@@ -0,0 +1,44 @@
+MEMORY
+{
+	vectors : ORIGIN = 0x10000000, LENGTH = 0x0002140
+	ram		: ORIGIN = 0x10002140, LENGTH = 0x0001EEC
+}
+
+SECTIONS
+{
+
+	.text :
+	{
+		start.o	(startup)
+			*(.text)
+	} > vectors
+
+	.data :
+	{
+		*(.data)
+	} > ram
+
+	.rodata :
+	{
+		*(.rodata)
+	} > ram
+
+	.rodata.str1.1 :
+	{
+		*(.rodata)
+	} > ram
+
+	. = ALIGN(4);
+	__bss_start = .;
+	.bss :
+	{
+		*(.bss)
+	} > ram
+	__bss_end = .;
+
+	.stack :
+	{
+		*(.stack)
+		_src_addr = .;
+	} > ram
+}
