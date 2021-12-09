--- linux_org/net/ipv6/af_inet6.c	2019-01-22 13:53:31.867773487 +0800
+++ linux/net/ipv6/af_inet6.c	2019-01-22 14:02:35.423822221 +0800
@@ -110,6 +110,9 @@
 	int try_loading_module = 0;
 	int err;
 
+	if (protocol >= IPPROTO_MAX)
+		return -EINVAL;
+	
 	/* Look for the requested type/protocol pair. */
 lookup_protocol:
 	err = -ESOCKTNOSUPPORT;
@@ -428,9 +431,11 @@
 
 	/* Free tx options */
 
-	opt = xchg(&np->opt, NULL);
-	if (opt != NULL)
-		sock_kfree_s(sk, opt, opt->tot_len);
+	opt = xchg((__force struct ipv6_txoptions **)&np->opt, NULL);
+	if (opt) {
+		atomic_sub(opt->tot_len, &sk->sk_omem_alloc);
+		txopt_put(opt);
+	}
 }
 EXPORT_SYMBOL_GPL(inet6_destroy_sock);
 
@@ -659,7 +664,10 @@
 		fl6.fl6_sport = inet->inet_sport;
 		security_sk_classify_flow(sk, flowi6_to_flowi(&fl6));
 
-		final_p = fl6_update_dst(&fl6, np->opt, &final);
+		rcu_read_lock();
+		final_p = fl6_update_dst(&fl6, rcu_dereference(np->opt),
+					 &final);
+		rcu_read_unlock();
 
 		dst = ip6_dst_lookup_flow(sk, &fl6, final_p);
 		if (IS_ERR(dst)) {
