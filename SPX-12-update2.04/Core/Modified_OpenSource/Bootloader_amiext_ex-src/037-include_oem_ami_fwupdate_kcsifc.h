--- uboot_old/include/oem/ami/fwupdate/kcsifc.h	1970-01-01 05:30:00.000000000 +0530
+++ uboot/include/oem/ami/fwupdate/kcsifc.h	2017-08-15 13:15:41.560332882 +0530
@@ -0,0 +1,164 @@
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
+ ***                                                            **
+ ***            All Rights Reserved.                            **
+ ***                                                            **
+ ***        6145-F, Northbelt Parkway, Norcross,                **
+ ***                                                            **
+ ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ ***                                                            **
+ *****************************************************************
+ *****************************************************************
+ *****************************************************************/
+#ifndef KCSIFC_H
+#define KCSIFC_H
+
+#define	KCS_1_CHANNEL			0
+#define	KCS_2_CHANNEL			1
+
+extern volatile unsigned char *lpc_base[MAX_KCS_CHANNEL];
+
+#define WRITE_U8(reg,data)			*((LPC_REG_DT *)(reg)) = (data)
+#define READ_U8(reg)				*((LPC_REG_DT *)(reg))
+
+/*** Local Macro Definitions ***/
+/**
+ * @def WRITE_KCS_STATUS_REG
+ * @brief Writes a byte into KCS port status register.
+ * @param CHANNEL The KCS channel number.
+ * @param STATUS a byte data.
+ **/
+#define WRITE_KCS_STATUS_REG(CHANNEL, STATUS)			                 						 \
+do {                                     			                 			 			     \
+    switch (CHANNEL){                      				         						         \
+    case KCS_1_CHANNEL: WRITE_U8 (lpc_base[CHANNEL] + KCS_STATUS_1,STATUS|LPC_WRITE_STATUS_BIT); break;         \
+    case KCS_2_CHANNEL: WRITE_U8 (lpc_base[CHANNEL] + KCS_STATUS_2,STATUS|LPC_WRITE_STATUS_BIT); break;         \
+    }                                                  				    						 \
+} while (0)
+
+
+/**
+ * @def READ_KCS_STATUS_REG
+ * @brief Reads a byte from KCS port status register.
+ * @param CHANNEL The KCS channel number.
+ * @param STATUS a byte data.
+ **/
+#define READ_KCS_STATUS_REG(CHANNEL, STATUS)             								     \
+do {                                                     								     \
+    switch (CHANNEL) {                                   								     \
+    case KCS_1_CHANNEL: STATUS = READ_U8(lpc_base[CHANNEL] + KCS_STATUS_1); break;   	 \
+    case KCS_2_CHANNEL: STATUS = READ_U8(lpc_base[CHANNEL] + KCS_STATUS_2); break;   	 \
+    }                                                      									 \
+} while (0)
+
+/**
+ * @def KCS_DATA_OUT_REG
+ * @brief Writes a byte into KCS port data register.
+ * @param CHANNEL The KCS channel number.
+ * @param DATA a byte data.
+ **/
+#define KCS_DATA_OUT_REG(CHANNEL, DATA)        				            					 \
+do {                                           				             					 \
+    switch (CHANNEL) {                        				             					 \
+    case KCS_1_CHANNEL: WRITE_U8 (lpc_base[CHANNEL] + KCS_ODR1,DATA); break;           \
+    case KCS_2_CHANNEL: WRITE_U8 (lpc_base[CHANNEL] + KCS_ODR2,DATA); break;           \
+    }                                               				      					 \
+} while (0)
+
+
+/**
+ * @def KCS_DATA_IN_REG
+ * @brief Reads a byte from KCS port data register.
+ * @param CHANNEL The KCS channel number.
+ * @param DATA a byte data.
+ **/
+#define KCS_DATA_IN_REG(CHANNEL, DATA)        				   		    				 \
+do {                                        				  	 				         \
+    switch (CHANNEL) {                                     								 \
+    case KCS_1_CHANNEL: DATA = READ_U8(lpc_base[CHANNEL] + KCS_IDR1); break;   		 \
+    case KCS_2_CHANNEL: DATA = READ_U8(lpc_base[CHANNEL] + KCS_IDR2); break;   		 \
+    }                                                      				  				 \
+} while (0)
+
+
+/**
+ * @def KCS_CMD_REG
+ * @brief Reads a byte from KCS port command register.
+ * @param CHANNEL The KCS channel number.
+ * @param CMD a byte data.
+ **/
+#define KCS_CMD_REG(CHANNEL, CMD)         								                 \
+do {                                            				                         \
+    switch (CHANNEL) {                       				                             \
+    case KCS_1_CHANNEL: CMD = READ_U8(lpc_base[CHANNEL] + KCS_IDR1); break;   		 \
+    case KCS_2_CHANNEL: CMD = READ_U8(lpc_base[CHANNEL] + KCS_IDR2); break;   		 \
+    }                                            				         				 \
+} while (0)
+
+
+/** 
+ * @def CLEAR_IBF_STATUS
+**/
+#define CLEAR_IBF_STATUS(CHANNEL)
+
+/** 
+ * @def SET_OBF_STATUS
+**/
+#define SET_OBF_STATUS(CHANNEL)
+
+/**
+ * @def INIT_KCS
+ * @brief Enables KCS channel 1
+ **/
+#define INIT_KCS_HARDWARE() 			                    					\
+do {                                                        					\
+	CLEAR_IBF_STATUS(0);															\
+	CLEAR_IBF_STATUS(1);															\
+} while (0) 
+
+
+/** 
+ * @def SMS_KCS_BASE_ADDR
+ * @brief SMS KCS Base address
+**/
+//#define SMS_KCS_BASE_ADDR		0xCA2
+
+/** 
+ * @def OBF_BIT_SET
+ * @brief Macro to check if output buffer is full
+**/
+#define OBF_BIT_SET(STATUS_REG) (0 != ((STATUS_REG) & 0x01))
+
+/** 
+ * @def IBF_BIT_SET
+ * @brief Macro to check if input buffer is full
+**/
+#define IBF_BIT_SET(STATUS_REG) (0 != ((STATUS_REG) & 0x02))
+
+/** 
+ * @def SET_KCS_STATE
+ * @brief Macro to set the current state of the KCS Interface.
+**/
+#define SET_KCS_STATE(CHANNEL, STATE) 									\
+do {																	\
+	u8 Status = 0;														\
+	READ_KCS_STATUS_REG ((CHANNEL), Status);							\
+	Status = (((Status & LPC_READ_STATUS_BIT) & (~0xC0)) | (STATE));							\
+	WRITE_KCS_STATUS_REG ((CHANNEL),Status);							\
+} while (0)
+
+/** 
+ * @def IS_WRITE_TO_CMD_REG
+ * @brief Macro that returns true if last write was to command register.
+**/
+#define IS_WRITE_TO_CMD_REG(STATUS_REG) (0 != ((STATUS_REG) & 0x08))
+
+/** 
+ * @def IS_WRITE_TO_DATA_REG
+ * @brief Macro that returns true if last write was to data register.
+**/
+#define IS_WRITE_TO_DATA_REG(STATUS_REG) (0 == ((STATUS_REG) & 0x08))
+
+#endif  /* KCS_REGS_H */
