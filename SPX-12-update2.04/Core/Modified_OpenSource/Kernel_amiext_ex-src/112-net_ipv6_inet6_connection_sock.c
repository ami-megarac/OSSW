--- linux_org/net/ipv6/inet6_connection_sock.c	2019-01-22 13:52:48.805470717 +0800
+++ linux/net/ipv6/inet6_connection_sock.c	2019-01-22 14:05:00.939677727 +0800
@@ -78,7 +78,9 @@
 	memset(fl6, 0, sizeof(*fl6));
 	fl6->flowi6_proto = IPPROTO_TCP;
 	fl6->daddr = ireq->ir_v6_rmt_addr;
-	final_p = fl6_update_dst(fl6, np->opt, &final);
+	rcu_read_lock();
+	final_p = fl6_update_dst(fl6, rcu_dereference(np->opt), &final);
+	rcu_read_unlock();
 	fl6->saddr = ireq->ir_v6_loc_addr;
 	fl6->flowi6_oif = ireq->ir_iif;
 	fl6->flowi6_mark = sk->sk_mark;
@@ -212,7 +214,9 @@
 	fl6->fl6_dport = inet->inet_dport;
 	security_sk_classify_flow(sk, flowi6_to_flowi(fl6));
 
-	final_p = fl6_update_dst(fl6, np->opt, &final);
+	rcu_read_lock();
+	final_p = fl6_update_dst(fl6, rcu_dereference(np->opt), &final);
+	rcu_read_unlock();	
 
 	dst = __inet6_csk_dst_check(sk, np->dst_cookie);
 	if (!dst) {
@@ -246,7 +250,8 @@
 	/* Restore final destination back after routing done */
 	fl6.daddr = sk->sk_v6_daddr;
 
-	res = ip6_xmit(sk, skb, &fl6, np->opt, np->tclass);
+	res = ip6_xmit(sk, skb, &fl6, rcu_dereference(np->opt),
+		       np->tclass);
 	rcu_read_unlock();
 	return res;
 }
