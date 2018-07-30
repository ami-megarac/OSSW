--- u-boot-2013.07/lib/rsa/rsa-verify.c	2017-02-10 17:42:47.539165620 -0500
+++ uboot.new/lib/rsa/rsa-verify.c	2017-02-10 16:56:53.412167318 -0500
@@ -21,6 +21,7 @@
 #include <fdtdec.h>
 #include <rsa.h>
 #include <sha1.h>
+#include <sha256.h>
 #include <asm/byteorder.h>
 #include <asm/errno.h>
 #include <asm/unaligned.h>
@@ -82,6 +83,37 @@
 	0x05, 0x00, 0x04, 0x14
 };
 
+static const uint8_t padding_sha256_rsa2048[RSA2048_BYTES - SHA256_SUM_LEN] = {
+        0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
+        0xff, 0xff, 0xff, 0xff, 0x00, 0x30, 0x31, 0x30,
+        0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65,
+        0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20
+};
+
 /**
  * subtract_modulus() - subtract modulus from the given value
  *
@@ -220,7 +252,7 @@
 
 	return 0;
 }
-
+#ifdef CONFIG_FIT_SIGNATURE 
 static int rsa_verify_key(const struct rsa_public_key *key, const uint8_t *sig,
 		const uint32_t sig_len, const uint8_t *hash)
 {
@@ -269,7 +301,7 @@
 
 	return 0;
 }
-
+#endif
 static void rsa_convert_big_endian(uint32_t *dst, const uint32_t *src, int len)
 {
 	int i;
@@ -277,7 +309,7 @@
 	for (i = 0; i < len; i++)
 		dst[i] = fdt32_to_cpu(src[len - 1 - i]);
 }
-
+#ifdef CONFIG_FIT_SIGNATURE
 static int rsa_verify_with_keynode(struct image_sign_info *info,
 		const void *hash, uint8_t *sig, uint sig_len, int node)
 {
@@ -383,3 +415,112 @@
 
 	return ret;
 }
+#endif
+static int read_prop(const char *key_val, int offset, int size){
+        int t=0;
+        int val = 0;
+        int tmp =0;
+        int count = size;
+        for(t = size+offset-1; t >= offset; t--){
+                tmp = key_val[t];
+                val = val + (tmp << ((--count)*8));
+        }
+        return val;
+}
+
+static int rsa_verify_key_SHA256(const struct rsa_public_key *key, const uint8_t *sig,
+                                                 const uint32_t sig_len, const uint8_t *hash, const uint32_t hash_len)
+{
+        const uint8_t *padding;
+        int pad_len;
+        int ret;
+        if (!key || !sig || !hash)
+                return -EIO;
+
+        if (sig_len != (key->len * sizeof(uint32_t))) {
+                printf("Signature is of incorrect length %d\n", sig_len);
+                return -EINVAL;
+        }
+
+        /* Sanity check for stack size */
+        if (sig_len > RSA_MAX_SIG_BITS / 8) {
+                printf("Signature length %u exceeds maximum %d\n", sig_len,
+                      RSA_MAX_SIG_BITS / 8);
+                return -EINVAL;
+        }
+
+        uint32_t buf[sig_len / sizeof(uint32_t)];
+
+        memcpy(buf, sig, sig_len);
+
+        ret = pow_mod(key, buf);
+        if (ret){
+                printf("error in pow_mod in rsa_verify_key\n");
+                return ret;
+        }
+        
+        padding = padding_sha256_rsa2048;
+        pad_len = RSA2048_BYTES - SHA256_SUM_LEN;
+
+        /* Check pkcs1.5 padding bytes. */
+        if (memcmp(buf, padding, pad_len)) {
+                printf("In RSAVerify(): Padding check failed!\n");
+              return -EINVAL;
+        }
+
+        /* Check hash. */
+        if (memcmp((uint8_t *)buf + pad_len, hash, sig_len - pad_len)) {
+                printf("In RSAVerify(): Hash check failed!\n");
+                return -EACCES;
+        }
+        return 0;
+}
+
+int rsa_get_pub_key_uboot_verify(const char *key_val, unsigned char *enc_hash,
+                                                  unsigned int enc_hash_len, unsigned char *hash, unsigned int hash_len)
+{
+        struct rsa_public_key key;
+        const void *modulus, *rr;
+        uint32_t hash_copy[16]={0};  
+        int ret, p;
+        
+        key.len = read_prop(key_val, 256, 2);
+        key.n0inv = read_prop(key_val, 100, 4);
+        modulus = &key_val[512]; 
+        rr = &key_val[1024]; 
+
+        /* Sanity check for stack size */
+        if (key.len > RSA_MAX_KEY_BITS || key.len < RSA_MIN_KEY_BITS) {
+                debug("RSA key bits %u outside allowed range %d..%d\n",
+                      key.len, RSA_MIN_KEY_BITS, RSA_MAX_KEY_BITS);
+                return -EFAULT;
+        }
+
+        key.len /= sizeof(uint32_t) * 8;
+        uint32_t key1[key.len], key2[key.len];
+        key.modulus = key1;
+        key.rr = key2;
+        rsa_convert_big_endian(key.modulus, modulus, key.len);
+        rsa_convert_big_endian(key.rr, rr, key.len);
+
+        if (!key.modulus || !key.rr) {
+                debug("%s: Out of memory", __func__);
+                return -ENOMEM;
+        }
+
+        debug("key length %d\n", key.len);
+
+        for(p=0; p < hash_len/sizeof(uint32_t); p++)
+        {
+                hash_copy[p] = *(((uint32_t *)(hash))+p);
+        }
+
+        ret = rsa_verify_key_SHA256(&key, enc_hash, enc_hash_len, (uint8_t *)(hash_copy), hash_len);
+
+        if (ret) {
+                printf("%s: RSA failed to verify: %d\n", __func__, ret);
+                return ret;
+        }
+        return 0;
+}
+
