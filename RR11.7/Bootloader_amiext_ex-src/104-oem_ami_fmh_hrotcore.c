--- u-boot-2013.07/oem/ami/fmh/hrotcore.c	2017-03-10 15:04:41.126941846 -0500
+++ uboot.new/oem/ami/fmh/hrotcore.c	2017-03-10 15:08:29.143276961 -0500
@@ -0,0 +1,115 @@
+# include <common.h>
+# include <config.h>
+# include <command.h>
+# include <flash.h>
+# include "cmd.h"
+# include <oem/ami/fmh/fmh.h>
+# include "hrot_ifc.h"
+#include "sha256.h"
+#include "rsa.h"
+
+#ifdef CONFIG_SPX_FEATURE_UBOOT_VALIDATE_SUPPORT
+	#define UBOOT_VALIDATE		1
+#else
+	#define UBOOT_VALIDATE          0
+#endif
+#define PROD_SIG_OFFSET		4096
+#define SIG_OFFSET		4088
+#define SIZE_OF_UBOOT_FMH	256
+#define SIGNATURE_LENGTH	256
+#define KEY_INFO_LENGTH		4096
+#define HASH_BLK_SIZE           262144
+
+static sha256_context ctx;
+static UINT32 boot_fmh_location;
+static char placeToHash[HASH_BLK_SIZE];
+
+void
+hrot_start(){
+	sha256_starts(&ctx);
+	return;
+}
+
+int
+hrot_update(unsigned short ModType, void *ModName, UINT32 location, UINT32 AllocSize, UINT32 fmhLocation, unsigned long startaddress){
+
+	int hs_size=HASH_BLK_SIZE;
+	UINT32 stopHash;
+        unsigned char *hash_data;
+        int rc = 0;
+	UINT32 hash_start, size_to_hash;
+
+	if(ModType == MODULE_BOOTLOADER){
+		boot_fmh_location = fmhLocation;
+	}
+
+	if((ModType != MODULE_JFFS2 && ModType != MODULE_BOOTLOADER) || (UBOOT_VALIDATE && ModType == MODULE_BOOTLOADER)){
+
+		if(ModType != MODULE_BOOTLOADER && ModType != MODULE_FMH_FIRMWARE){
+                        hash_start = startaddress + fmhLocation;
+                        size_to_hash = AllocSize;
+                }
+                else if(ModType == MODULE_FMH_FIRMWARE){
+                        hash_start = startaddress + fmhLocation;
+                        size_to_hash = AllocSize - PROD_SIG_OFFSET;
+                }
+                else{
+                        hash_start = startaddress + location;
+                	size_to_hash = fmhLocation + SIZE_OF_UBOOT_FMH;
+                }
+
+		stopHash = (size_to_hash+hash_start);
+                while(hash_start < stopHash)
+                {
+                        if((hash_start+hs_size) > stopHash && hash_start < stopHash){
+                                hs_size = stopHash - hash_start;
+                        }
+
+                        flash_read(hash_start, hs_size, placeToHash);
+                        hash_data = (rc == ERR_OK) ? (unsigned char*)placeToHash : NULL;
+
+                        if(hash_data == NULL){
+                                printf("ERROR setting hash data!\n");
+                                return 1;
+                        }
+
+                        sha256_update(&ctx, hash_data, hs_size);
+                        hash_start += hs_size;
+                }
+
+	}
+        return 0;
+}
+
+int
+hrot_finish(unsigned long startaddress){
+        unsigned char output [65];
+	//int len = 4096;//, siglen = 256;
+        UINT32 keyInfo = startaddress + boot_fmh_location - KEY_INFO_LENGTH;
+        UINT32 sigaddr = startaddress + CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE - SIG_OFFSET;
+        int rc = 0;
+        int size = 64;
+        char    linebuf[16384] = {0}, linebuf1[1024] = {0};
+        void *buf, *buf1;
+        int ret =0;
+
+        sha256_finish(&ctx, output);
+
+        rc = flash_read(keyInfo, KEY_INFO_LENGTH*4, linebuf);
+        buf = (rc == ERR_OK) ? linebuf : (void*)keyInfo;
+
+        rc = flash_read(sigaddr, SIGNATURE_LENGTH*4, linebuf1);
+        buf1 = (rc == ERR_OK) ? linebuf1 : (void*)sigaddr;
+
+        char *key_data = (char *)buf;
+        unsigned char *sig = (unsigned char *)buf1;
+        
+        if(0){
+                printf("%s", key_data);
+                printf("%s", sig);
+        }
+        ret = rsa_get_pub_key_uboot_verify(key_data, sig, SIGNATURE_LENGTH, output, size);
+        return ret;
+}
+
+
