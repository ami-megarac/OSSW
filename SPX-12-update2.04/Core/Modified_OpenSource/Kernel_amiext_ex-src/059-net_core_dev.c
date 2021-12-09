--- linux_org/net/core/dev.c	2019-01-22 19:57:37.264776010 +0800
+++ linux/net/core/dev.c	2019-01-22 15:30:35.245377063 +0800
@@ -1154,6 +1154,18 @@
 	return err;
 }
 
+void netdev_link_up(struct net_device *dev)
+{
+	call_netdevice_notifiers(NETDEV_LINK_UP, dev);
+}
+EXPORT_SYMBOL(netdev_link_up);
+
+void netdev_link_down(struct net_device *dev)
+{
+	call_netdevice_notifiers(NETDEV_LINK_DOWN, dev);
+}
+EXPORT_SYMBOL(netdev_link_down);
+
 /**
  *	dev_set_alias - change ifalias of a device
  *	@dev: device
@@ -3907,7 +3919,7 @@
 		NAPI_GRO_CB(skb)->same_flow = 0;
 		NAPI_GRO_CB(skb)->flush = 0;
 		NAPI_GRO_CB(skb)->free = 0;
-		NAPI_GRO_CB(skb)->udp_mark = 0;
+		NAPI_GRO_CB(skb)->encap_mark = 0;
 
 		pp = ptype->callbacks.gro_receive(&napi->gro_list, skb);
 		break;
