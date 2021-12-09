--- linux_org/net/ipv4/udp.c	2019-01-22 20:03:12.732505653 +0800
+++ linux/net/ipv4/udp.c	2019-01-22 18:55:56.001128801 +0800
@@ -785,7 +785,7 @@
 	if (is_udplite)  				 /*     UDP-Lite      */
 		csum = udplite_csum(skb);
 
-	else if (sk->sk_no_check == UDP_CSUM_NOXMIT) {   /* UDP csum disabled */
+	else if ((sk->sk_no_check == UDP_CSUM_NOXMIT) && !skb_is_gso(skb)) {   /* UDP csum disabled */
 
 		skb->ip_summed = CHECKSUM_NONE;
 		goto send;
@@ -1232,6 +1232,7 @@
 	int peeked, off = 0;
 	int err;
 	int is_udplite = IS_UDPLITE(sk);
+	bool checksum_valid = false;
 	bool slow;
 
 	if (flags & MSG_ERRQUEUE)
@@ -1257,11 +1258,12 @@
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
@@ -1317,8 +1319,7 @@
 	}
 	unlock_sock_fast(sk, slow);
 
-	if (noblock)
-		return -EAGAIN;
+	cond_resched();
 
 	/* starting over for a new packet */
 	msg->msg_flags &= ~MSG_TRUNC;
