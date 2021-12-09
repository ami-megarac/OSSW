--- linux_org/include/linux/ipv6.h	2019-01-22 13:52:48.773471978 +0800
+++ linux/include/linux/ipv6.h	2019-01-22 14:00:07.849020859 +0800
@@ -210,7 +210,7 @@
 	struct ipv6_ac_socklist	*ipv6_ac_list;
 	struct ipv6_fl_socklist __rcu *ipv6_fl_list;
 
-	struct ipv6_txoptions	*opt;
+	struct ipv6_txoptions __rcu	*opt;
 	struct sk_buff		*pktoptions;
 	struct sk_buff		*rxpmtu;
 	struct {
