--- linux_org/net/ipv6/syncookies.c	2019-01-22 13:52:48.821470087 +0800
+++ linux/net/ipv6/syncookies.c	2019-01-22 14:08:50.115040044 +0800
@@ -239,7 +239,7 @@
 		memset(&fl6, 0, sizeof(fl6));
 		fl6.flowi6_proto = IPPROTO_TCP;
 		fl6.daddr = ireq->ir_v6_rmt_addr;
-		final_p = fl6_update_dst(&fl6, np->opt, &final);
+		final_p = fl6_update_dst(&fl6, rcu_dereference(np->opt), &final);		
 		fl6.saddr = ireq->ir_v6_loc_addr;
 		fl6.flowi6_oif = sk->sk_bound_dev_if;
 		fl6.flowi6_mark = sk->sk_mark;
