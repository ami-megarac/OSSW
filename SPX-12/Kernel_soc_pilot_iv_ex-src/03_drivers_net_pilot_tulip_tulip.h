--- linux-3.14.17/drivers/net/ethernet/pilot_tulip/tulip.h	2016-08-16 19:39:02.845128119 +0800
+++ linux-3.14.17.new/drivers/net/ethernet/pilot_tulip/tulip.h	2016-08-16 19:39:41.046018123 +0800
@@ -135,6 +135,11 @@
 	CSR13 = 0x68,
 	CSR14 = 0x70,
 	CSR15 = 0x78,
+        CSR16 = 0x80,
+	CSR17 = 0x88,
+	CSR18 = 0x90,
+	CSR19 = 0x98,
+	CSR20 = 0xA0
 
 };
 
@@ -358,6 +363,22 @@
 #endif
 };
 
+/* Flow Control Settings */
+#define PILOT_PAUSE_FR_ENABLE 1
+#define FLOW_CTRL_ENBALE      0xC0000000
+#define TX_PAUSE_ENABLE       0x20000000
+#define RX_PAUSE_ENABLE       0x10000000
+#define PAUSE_QUANTA_TIME     0x00000FFF
+#define PAUSE_THRESHOLD_VALUE 0x00800040
+
+typedef enum {
+	TULIP_FC_NONE = 0,
+	TULIP_FC_TX_PAUSE = 1,
+	TULIP_FC_RX_PAUSE = 2,
+	TULIP_FC_FULL = 3,
+	TULIP_FC_DEFAULT = 0xFF
+} tulip_fc_type;
+
 
 struct tulip_private {
 	const char *product_name;
