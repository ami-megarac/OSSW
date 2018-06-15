--- u-boot/arch/arm/cpu/pilotcommon/pilotserial.h	1970-01-01 05:30:00.000000000 +0530
+++ u-boot.new/arch/arm/cpu/pilotcommon/pilotserial.h	2014-07-23 14:43:25.642234855 +0530
@@ -0,0 +1,50 @@
+#ifndef AST_SERIAL_H
+#define AST_SERIAL_H
+
+/*  -------------------------------------------------------------------------------
+ *   API
+ *  -------------------------------------------------------------------------------
+ */
+typedef unsigned long long  UINT64;
+typedef long long 			INT64;
+typedef	unsigned int		UINT32;
+typedef	int					INT32;
+typedef	unsigned short		UINT16;
+typedef	short				INT16;
+typedef unsigned char		UINT8;
+typedef char				INT8;
+typedef unsigned char		BOOL;
+
+#define FALSE	0
+#define TRUE	1
+
+extern void AST_SerialInit(UINT32 port, UINT32 baudrate, UINT32 parity,UINT32 num,UINT32 len);
+extern void AST_SetSerialFifoCtrl(UINT32 port, UINT32 level, UINT32 resettx, UINT32 resetrx);
+extern void AST_DisableSerialFifo(UINT32 port);
+extern void AST_SetSerialInt(UINT32 port, UINT32 IntMask);
+
+extern char AST_GetSerialChar(UINT32 port);
+extern void AST_PutSerialChar(UINT32 port, char Ch);
+extern void AST_PutSerialStr(UINT32 port,const char *Str);
+
+extern void AST_EnableSerialInt(UINT32 port, UINT32 mode);
+extern void AST_DisableSerialInt(UINT32 port, UINT32 mode);
+
+extern void AST_SerialRequestToSend(UINT32 port);
+extern void AST_SerialStopToSend(UINT32 port);
+extern void AST_SerialDataTerminalReady(UINT32 port);
+extern void AST_SerialDataTerminalNotReady(UINT32 port);
+
+extern void AST_SetSerialLineBreak(UINT32 port);
+extern void AST_SetSerialLoopBack(UINT32 port,UINT32 onoff);
+extern UINT32 AST_SerialIntIdentification(UINT32 port);
+
+extern UINT32 AST_ReadSerialLineStatus(UINT32 port);
+extern UINT32 AST_ReadSerialModemStatus(UINT32 port);
+
+extern void AST_SetSerialMode(UINT32 port, UINT32 mode);
+extern void AST_EnableIRMode(UINT32 port, UINT32 TxEnable, UINT32 RxEnable);
+extern int  AST_TestSerialForChar(UINT32 port);
+
+
+#endif
