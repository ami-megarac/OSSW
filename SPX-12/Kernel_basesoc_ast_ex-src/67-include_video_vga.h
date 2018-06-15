--- linux/include/video/vga.h	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/include/video/vga.h	2016-10-24 15:24:27.275690100 +0800
@@ -201,18 +201,18 @@
  
 static inline unsigned char vga_io_r (unsigned short port)
 {
-	return inb_p(port);
+	return inb_p((unsigned long)port);
 }
 
 static inline void vga_io_w (unsigned short port, unsigned char val)
 {
-	outb_p(val, port);
+	outb_p(val, (unsigned long)port);
 }
 
 static inline void vga_io_w_fast (unsigned short port, unsigned char reg,
 				  unsigned char val)
 {
-	outw(VGA_OUT16VAL (val, reg), port);
+	outw(VGA_OUT16VAL (val, reg), (unsigned long)port);
 }
 
 static inline unsigned char vga_mm_r (void __iomem *regbase, unsigned short port)
