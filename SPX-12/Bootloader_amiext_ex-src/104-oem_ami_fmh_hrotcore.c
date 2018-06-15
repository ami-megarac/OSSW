--- uboot_old/oem/ami/fmh/hrotcore.c	1970-01-01 05:30:00.000000000 +0530
+++ uboot/oem/ami/fmh/hrotcore.c	2017-10-13 15:51:34.162503551 +0530
@@ -0,0 +1,173 @@
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
+static UINT32 boot_fmh_location,product_info_offset=0;
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
+	if((ModType == MODULE_BOOTLOADER) && (strncmp((char *)ModName,"boot",sizeof("boot")) == 0)){
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
+			product_info_offset=startaddress + fmhLocation +AllocSize;
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
+						if ((ModType == MODULE_FMH_FIRMWARE) && (hash_start == (startaddress + fmhLocation)))
+						{
+							// Start with FmhLocation + 0x17 offset exclude 1 Byte FMH_Header_Checksum
+							memset(&hash_data[23], 0, 1);
+							// Start with FmhLocation + 0x32 offset exclude 4 Byte Module_Checksum
+							memset(&hash_data[50], 0, 4);
+						}
+
+                        sha256_update(&ctx, hash_data, hs_size);
+                        hash_start += hs_size;
+                }
+
+	}
+        return 0;
+}
+
+
+#define START_IDENTIFIER "$start$"
+#define FWIMG_IDENTIFIER "$fwimg$"
+#define END_IDENTIFIER   "$end$"
+
+int
+hrot_finish(unsigned long startaddress){
+        unsigned char output [65];
+	//int len = 4096;//, siglen = 256;
+        UINT32 keyInfo = startaddress + boot_fmh_location - KEY_INFO_LENGTH;
+        UINT32 sigaddr = product_info_offset  - PROD_SIG_OFFSET;
+        int rc = 0;
+        int size = 64;
+        char    linebuf[16384] = {0}, linebuf1[1024] = {0};
+        void *buf=NULL, *buf1=NULL;
+        int ret =0;
+		int sig_length;
+		char identifier[8];
+
+        sha256_finish(&ctx, output);
+
+        rc = flash_read(keyInfo, KEY_INFO_LENGTH*4, linebuf);
+        buf = (rc == ERR_OK) ? linebuf : (void*)keyInfo;
+
+	   	flash_read(sigaddr,sizeof(START_IDENTIFIER),identifier);
+		if(strncmp(identifier,START_IDENTIFIER,sizeof(START_IDENTIFIER)) != 0)
+		{
+			printf("unable to find signature start identifier :( \n");
+			return -1;
+		}
+	
+		sigaddr +=sizeof(START_IDENTIFIER);
+		// scan through hash area and find the proper hash for firmware image
+		while(product_info_offset > sigaddr )
+		{
+			flash_read(sigaddr,sizeof(identifier),identifier);
+			printf("temp %s\n",identifier);
+			sigaddr +=sizeof(identifier);
+				
+			if(strncmp(identifier,FWIMG_IDENTIFIER,sizeof(FWIMG_IDENTIFIER)) != 0)
+			{
+				if(strncmp(identifier,END_IDENTIFIER,strlen(END_IDENTIFIER)) != 0)
+				{
+					printf("unable to find signature for firmware image");
+					return -1;
+				}
+				flash_read(sigaddr,sizeof(sig_length),(char *) &sig_length);
+				sigaddr +=sizeof(sig_length)+sig_length;
+
+				continue;
+
+			}
+			flash_read(sigaddr,sizeof(sig_length),(char *) &sig_length);
+            sigaddr +=sizeof(sig_length);
+
+			//memset(linebuf1,0xff,sizeof(linebuf1));
+			rc = flash_read(sigaddr, sig_length, linebuf1);
+			buf1 = (rc == ERR_OK) ? linebuf1 : (void*)sigaddr;
+
+			printf("signature found\n");
+			break;
+
+		}
+		if(product_info_offset < sigaddr)
+		{
+			printf("signature not found :(");
+			return -1;
+		}
+
+        char *key_data = (char *)buf;
+        unsigned char *sig = (unsigned char *)buf1;
+        
+        if(0){
+                printf("%s", key_data);
+                printf("%s", sig);
+        }
+        ret = rsa_get_pub_key_uboot_verify(key_data, sig, sig_length, output, size);
+        return ret;
+}
+
+
