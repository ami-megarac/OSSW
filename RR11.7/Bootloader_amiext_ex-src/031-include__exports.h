--- u-boot-2013.07/include/_exports.h	2016-12-02 12:08:59.242061000 +0800
+++ uboot.new/include/_exports.h	2016-12-02 12:13:55.706460997 +0800
@@ -22,11 +22,26 @@
 EXPORT_FUNC(strict_strtoul)
 EXPORT_FUNC(simple_strtol)
 EXPORT_FUNC(strcmp)
+EXPORT_FUNC(memset)
+EXPORT_FUNC(memcpy)
+EXPORT_FUNC(strcpy)
+EXPORT_FUNC(sprintf)
+EXPORT_FUNC(fprintf)
+#if defined(CONFIG_CMD_I2C)
+EXPORT_FUNC(i2c_probe)
 EXPORT_FUNC(i2c_write)
 EXPORT_FUNC(i2c_read)
+#endif
 EXPORT_FUNC(spi_init)
 EXPORT_FUNC(spi_setup_slave)
 EXPORT_FUNC(spi_free_slave)
 EXPORT_FUNC(spi_claim_bus)
 EXPORT_FUNC(spi_release_bus)
 EXPORT_FUNC(spi_xfer)
+#ifdef CONFIG_EXPORT_ETH_FNS
+EXPORT_FUNC(eth_init)
+EXPORT_FUNC(eth_halt)
+EXPORT_FUNC(eth_send)
+EXPORT_FUNC(eth_rx)
+EXPORT_FUNC(net_set_udp_handler)
+#endif
