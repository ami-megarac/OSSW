--- linux-3.14.17/arch/arm/mach-pilot/Makefile.boot	1970-01-01 05:30:00.000000000 +0530
+++ linux-3.14.17.orion/arch/arm/mach-pilot/Makefile.boot	2015-04-20 12:13:02.178227952 +0530
@@ -0,0 +1,4 @@
+zreladdr-y    := 0x81008000
+params_phys-y           := 0x81000100
+initrd_phys-y           := 0x81800000
+
