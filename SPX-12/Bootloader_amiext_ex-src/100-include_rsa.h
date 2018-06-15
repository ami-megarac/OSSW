--- u-boot-2013.07/include/rsa.h	2017-02-10 16:04:04.097462886 -0500
+++ uboot.new/include/rsa.h	2017-02-10 16:06:42.106604557 -0500
@@ -31,6 +31,7 @@
 #include <errno.h>
 #include <image.h>
 
+#ifdef CONFIG_FIT_SIGNATURE
 #if IMAGE_ENABLE_SIGN
 /**
  * sign() - calculate and return signature for given input data
@@ -104,5 +105,6 @@
 	return -ENXIO;
 }
 #endif
-
+#endif //CONFIG_FIT_SIGNATURE
+int rsa_get_pub_key_uboot_verify(const char *key_val, unsigned char *enc_hash, unsigned int enc_hash_len, unsigned char *hash, unsigned int hash_len);
 #endif
