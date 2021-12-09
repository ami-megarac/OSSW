--- u-boot-2013.07/oem/ami/fmh/cmd_fmh.c	2018-11-12 12:24:36.297470943 +0530
+++ uboot_new/oem/ami/fmh/cmd_fmh.c	2018-11-12 12:19:50.698429957 +0530
@@ -154,7 +154,63 @@
 	"<Arg>  (Arg = 0 to 2)"
 );
 
+int EnableSocflash (unsigned short InterfaceID)
+{
+	int reg = 0;
+	
+	*(volatile u32 *)(0x1e6e2000) = 0x1688A8A8;
+	if (InterfaceID == 0)
+		*(volatile u32 *)(0x1e6e2000 + 0x180) |= 0x2;
+	else if (InterfaceID == 1)
+	{
+		*(volatile u32 *)(0x1e789000 + 0x80) |= 0x100;
+		*(volatile u32 *)(0x1e789000 + 0x100) &= ~(0x40);
+		*(volatile u32 *)(0x1e6e2000 + 0x7C) = (0x100000);
+	}
+	*(volatile u32 *)(0x1e6e2000) = 0;
+	
+	reg = *(volatile u32 *)(0x1E6E2000 + 0x180);
+	if (reg & 0x2)
+		printf("soc flash mode via pcie is enable\n");
+	reg = *(volatile u32 *)(0x1E6E2000 + 0x70);
+	if (!(reg & 0x100000))
+		printf("soc flash mode via lpc is enable\n");
+	return 0;
+}
 
+int 
+do_bootmenu (cmd_tbl_t *cmdtp, int flag , int argc, char * const argv[]) 
+{ 
+	unsigned short PathID = 2;
+	unsigned short InterfaceID = 0; 
+	 
+	while (PathID >= 2) 
+	{ 
+		printf("------ Boot Options-------\n"); 
+		printf("\t0. Normal Boot\n");
+		printf("\t1. socflash Recovery\n");
+		printf("Select Boot Option:\n"); 
+		PathID = getc() - '0'; 
+		if (PathID == 1)
+		{
+			printf("\t0. socflash via pcie\n"); 
+			printf("\t1. socflash via lpc\n"); 
+			printf("Select Interface Option:\n"); 
+			InterfaceID = getc() - '0'; 
+		}
+	} 
+	if (PathID == 1)
+		return EnableSocflash(InterfaceID);
+	else
+		return BootFMH((unsigned short)1 << PathID); 
+} 
+
+U_BOOT_CMD(bootmenu,	1,	1,	do_bootmenu, 
+	"Show Boot Path menu", 
+	"" 
+); 
+ 
+ 
 /* Actual function implementing FMH Listing*/
 /* Returns 0 on success, else 1 on failure */
 int
