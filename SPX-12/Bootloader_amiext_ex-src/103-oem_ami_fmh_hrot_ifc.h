--- u-boot-2013.07/oem/ami/fmh/hrot_ifc.h	2017-02-10 13:49:15.602724039 -0500
+++ uboot.new/oem/ami/fmh/hrot_ifc.h	2017-02-10 13:50:53.049196613 -0500
@@ -0,0 +1,11 @@
+#ifndef __AMI_CMD_SECURE_BOOT_H__
+#define __AMI_CMD_SECURE_BOOT_H__
+
+
+typedef unsigned long   UINT32;
+void hrot_start(void);
+int hrot_update(unsigned short ModType, void *ModName, UINT32 location, UINT32 ModSize, UINT32 fmhLocation, unsigned long startaddress);
+int hrot_finish(unsigned long startaddress);
+extern int rsa_get_pub_key_uboot_verify(const char *key_val, unsigned char *enc_hash, unsigned int enc_hash_len, unsigned char *hash, unsigned int hash_len);
+
+#endif
