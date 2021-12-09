--- linux_org/net/ipv6/datagram.c	2019-01-22 13:52:48.793471190 +0800
+++ linux/net/ipv6/datagram.c	2019-01-22 14:03:20.326476800 +0800
@@ -167,8 +167,10 @@
 
 	security_sk_classify_flow(sk, flowi6_to_flowi(&fl6));
 
-	opt = flowlabel ? flowlabel->opt : np->opt;
+	rcu_read_lock();
+	opt = flowlabel ? flowlabel->opt : rcu_dereference(np->opt);
 	final_p = fl6_update_dst(&fl6, opt, &final);
+	rcu_read_unlock();
 
 	dst = ip6_dst_lookup_flow(sk, &fl6, final_p);
 	err = 0;
