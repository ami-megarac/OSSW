--- linux_org/net/netlink/af_netlink.c	2019-01-22 15:17:53.429159446 +0800
+++ linux/net/netlink/af_netlink.c	2019-01-22 15:18:03.361809505 +0800
@@ -2642,6 +2642,7 @@
 	struct netlink_callback *cb;
 	struct sk_buff *skb = NULL;
 	struct nlmsghdr *nlh;
+	struct module *module;
 	int len, err = -ENOBUFS;
 	int alloc_size;
 
@@ -2691,9 +2692,11 @@
 		cb->done(cb);
 
 	nlk->cb_running = false;
+	module = cb->module;
+	skb = cb->skb;	
 	mutex_unlock(nlk->cb_mutex);
-	module_put(cb->module);
-	consume_skb(cb->skb);
+	module_put(module);
+	consume_skb(skb);
 	return 0;
 
 errout_skb:
