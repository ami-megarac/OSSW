--- u-boot-2013.07/include/exports.h	2016-12-02 12:08:56.067648997 +0800
+++ uboot.new/include/exports.h	2016-12-02 12:11:38.798041006 +0800
@@ -3,6 +3,10 @@
 
 #ifndef __ASSEMBLY__
 
+#ifdef CONFIG_EXPORT_ETH_FNS
+#include <net.h>
+#endif
+
 /* These are declarations of exported functions available in C code */
 unsigned long get_version(void);
 int  getc(void);
@@ -23,12 +27,25 @@
 int setenv (const char *varname, const char *varvalue);
 long simple_strtol(const char *cp,char **endp,unsigned int base);
 int strcmp(const char * cs,const char * ct);
+void *memset(void *s,int c,size_t count);
+void *memcpy(void *dest, const void *src, size_t count);
+char *strcpy(char *dest, const char *src);
+int sprintf(char *buf, const char *fmt, ...);
+int fprintf(int file, const char *fmt, ...);
 unsigned long ustrtoul(const char *cp, char **endp, unsigned int base);
 unsigned long long ustrtoull(const char *cp, char **endp, unsigned int base);
 #if defined(CONFIG_CMD_I2C)
+int i2c_probe (uchar);
 int i2c_write (uchar, uint, int , uchar* , int);
 int i2c_read (uchar, uint, int , uchar* , int);
 #endif
+#ifdef CONFIG_EXPORT_ETH_FNS
+int  eth_init(bd_t *bis);
+void eth_halt(void);
+int  eth_send(void *packet, int length);
+int  eth_rx(void);
+void net_set_udp_handler(rxhand_f*);
+#endif
 
 void app_startup(char * const *);
 
