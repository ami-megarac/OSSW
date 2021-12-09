--- u-boot-2013.07/config.mk	2017-02-10 14:09:17.240420481 -0500
+++ uboot.new/config.mk	2017-02-10 14:09:38.255875192 -0500
@@ -218,6 +218,7 @@
 # This affects include/image.h, but including the board config file
 # is tricky, so manually define this options here.
 HOSTCFLAGS	+= -DCONFIG_FIT_SIGNATURE
+HOSTFITSIG	= HOST_CONFIG_FIT_SIGNATURE
 endif
 
 ifneq ($(CONFIG_SYS_TEXT_BASE),)
