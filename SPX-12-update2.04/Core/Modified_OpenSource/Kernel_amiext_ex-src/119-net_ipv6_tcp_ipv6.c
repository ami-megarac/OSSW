--- linux_org/net/ipv6/tcp_ipv6.c	2019-01-22 13:52:48.829469771 +0800
+++ linux/net/ipv6/tcp_ipv6.c	2019-01-22 15:45:22.964560399 +0800
@@ -134,6 +134,7 @@
 	struct ipv6_pinfo *np = inet6_sk(sk);
 	struct tcp_sock *tp = tcp_sk(sk);
 	struct in6_addr *saddr = NULL, *final_p, final;
+	struct ipv6_txoptions *opt;
 	struct rt6_info *rt;
 	struct flowi6 fl6;
 	struct dst_entry *dst;
@@ -253,7 +254,8 @@
 	fl6.fl6_dport = usin->sin6_port;
 	fl6.fl6_sport = inet->inet_sport;
 
-	final_p = fl6_update_dst(&fl6, np->opt, &final);
+	opt = rcu_dereference_protected(np->opt, sock_owned_by_user(sk));
+	final_p = fl6_update_dst(&fl6, opt, &final);	
 
 	security_sk_classify_flow(sk, flowi6_to_flowi(&fl6));
 
@@ -282,9 +284,9 @@
 		tcp_fetch_timewait_stamp(sk, dst);
 
 	icsk->icsk_ext_hdr_len = 0;
-	if (np->opt)
-		icsk->icsk_ext_hdr_len = (np->opt->opt_flen +
-					  np->opt->opt_nflen);
+	if (opt)
+		icsk->icsk_ext_hdr_len = opt->opt_flen +
+					 opt->opt_nflen;
 
 	tp->rx_opt.mss_clamp = IPV6_MIN_MTU - sizeof(struct tcphdr) - sizeof(struct ipv6hdr);
 
@@ -487,7 +489,8 @@
 			fl6->flowlabel = ip6_flowlabel(ipv6_hdr(ireq->pktopts));
 
 		skb_set_queue_mapping(skb, queue_mapping);
-		err = ip6_xmit(sk, skb, fl6, np->opt, np->tclass);
+		err = ip6_xmit(sk, skb, fl6, rcu_dereference(np->opt),
+			       np->tclass);
 		err = net_xmit_eval(err);
 	}
 
@@ -1100,6 +1103,7 @@
 {
 	struct inet_request_sock *ireq;
 	struct ipv6_pinfo *newnp, *np = inet6_sk(sk);
+	struct ipv6_txoptions *opt;
 	struct tcp6_sock *newtcp6sk;
 	struct inet_sock *newinet;
 	struct tcp_sock *newtp;
@@ -1140,6 +1144,7 @@
 		newtp->af_specific = &tcp_sock_ipv6_mapped_specific;
 #endif
 
+		newnp->ipv6_mc_list = NULL;
 		newnp->ipv6_ac_list = NULL;
 		newnp->ipv6_fl_list = NULL;
 		newnp->pktoptions  = NULL;
@@ -1209,6 +1214,7 @@
 	   First: no IPv4 options.
 	 */
 	newinet->inet_opt = NULL;
+	newnp->ipv6_mc_list = NULL;
 	newnp->ipv6_ac_list = NULL;
 	newnp->ipv6_fl_list = NULL;
 
@@ -1238,13 +1244,15 @@
 	   but we make one more one thing there: reattach optmem
 	   to newsk.
 	 */
-	if (np->opt)
-		newnp->opt = ipv6_dup_options(newsk, np->opt);
-
+	opt = rcu_dereference(np->opt);
+	if (opt) {
+		opt = ipv6_dup_options(newsk, opt);
+		RCU_INIT_POINTER(newnp->opt, opt);
+	}
 	inet_csk(newsk)->icsk_ext_hdr_len = 0;
-	if (newnp->opt)
-		inet_csk(newsk)->icsk_ext_hdr_len = (newnp->opt->opt_nflen +
-						     newnp->opt->opt_flen);
+	if (opt)
+		inet_csk(newsk)->icsk_ext_hdr_len = opt->opt_nflen +
+						    opt->opt_flen;
 
 	tcp_sync_mss(newsk, dst_mtu(dst));
 	newtp->advmss = dst_metric_advmss(dst);
