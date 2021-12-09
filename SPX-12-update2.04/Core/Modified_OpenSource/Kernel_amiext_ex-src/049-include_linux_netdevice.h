--- linux_org/include/linux/netdevice.h	2019-01-22 19:54:35.547414928 +0800
+++ linux/include/linux/netdevice.h	2019-01-22 15:30:09.781845320 +0800
@@ -1676,8 +1676,8 @@
 	/* Used in ipv6_gro_receive() */
 	u16	proto;
 
-	/* Used in udp_gro_receive */
-	u16	udp_mark;
+	/* Used in tunnel GRO receive */
+	 u16 encap_mark;
 
 	/* used to support CHECKSUM_COMPLETE for tunneling protocols */
 	__wsum	csum;
@@ -1762,6 +1762,9 @@
 #define NETDEV_CHANGEUPPER	0x0015
 #define NETDEV_RESEND_IGMP	0x0016
 #define NETDEV_PRECHANGEMTU	0x0017 /* notify before mtu change happened */
+#define NETDEV_LINK_UP     0x0101  /* Link up Notifier   */
+#define NETDEV_LINK_DOWN   0x0102  /* Link down Notifier */
+
 
 int register_netdevice_notifier(struct notifier_block *nb);
 int unregister_netdevice_notifier(struct notifier_block *nb);
@@ -2954,6 +2957,8 @@
 void netdev_state_change(struct net_device *dev);
 void netdev_notify_peers(struct net_device *dev);
 void netdev_features_change(struct net_device *dev);
+void netdev_link_up(struct net_device *dev);
+void netdev_link_down(struct net_device *dev);
 /* Load a device via the kmod */
 void dev_load(struct net *net, const char *name);
 struct rtnl_link_stats64 *dev_get_stats(struct net_device *dev,
