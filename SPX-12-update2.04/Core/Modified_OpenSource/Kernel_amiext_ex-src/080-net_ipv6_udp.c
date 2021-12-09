--- linux_org/net/ipv6/udp.c	2019-01-22 20:09:40.922293818 +0800
+++ linux/net/ipv6/udp.c	2019-01-22 14:12:12.804285642 +0800
@@ -389,6 +389,7 @@
 	int peeked, off = 0;
 	int err;
 	int is_udplite = IS_UDPLITE(sk);
+	bool checksum_valid = false;
 	int is_udp4;
 	bool slow;
 
@@ -420,11 +421,12 @@
 	 */
 
 	if (copied < ulen || UDP_SKB_CB(skb)->partial_cov) {
-		if (udp_lib_checksum_complete(skb))
+		checksum_valid = !udp_lib_checksum_complete(skb);
+		if (!checksum_valid)	
 			goto csum_copy_err;
 	}
 
-	if (skb_csum_unnecessary(skb))
+	if (checksum_valid || skb_csum_unnecessary(skb))
 		err = skb_copy_datagram_iovec(skb, sizeof(struct udphdr),
 					      msg->msg_iov, copied);
 	else {
@@ -515,8 +517,7 @@
 	}
 	unlock_sock_fast(sk, slow);
 
-	if (noblock)
-		return -EAGAIN;
+	cond_resched();
 
 	/* starting over for a new packet */
 	msg->msg_flags &= ~MSG_TRUNC;
@@ -1046,6 +1047,7 @@
 	DECLARE_SOCKADDR(struct sockaddr_in6 *, sin6, msg->msg_name);
 	struct in6_addr *daddr, *final_p, final;
 	struct ipv6_txoptions *opt = NULL;
+	struct ipv6_txoptions *opt_to_free = NULL;
 	struct ip6_flowlabel *flowlabel = NULL;
 	struct flowi6 fl6;
 	struct dst_entry *dst;
@@ -1198,8 +1200,10 @@
 			opt = NULL;
 		connected = 0;
 	}
-	if (opt == NULL)
-		opt = np->opt;
+	if (!opt) {
+		opt = txopt_get(np);
+		opt_to_free = opt;
+	}
 	if (flowlabel)
 		opt = fl6_merge_options(&opt_space, flowlabel, opt);
 	opt = ipv6_fixup_options(&opt_space, opt);
@@ -1299,6 +1303,7 @@
 out:
 	dst_release(dst);
 	fl6_sock_release(flowlabel);
+	txopt_put(opt_to_free);
 	if (!err)
 		return len;
 	/*
