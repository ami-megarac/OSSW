--- linux_org/net/ipv4/ip_output.c	2019-01-22 18:53:30.017423411 +0800
+++ linux/net/ipv4/ip_output.c	2019-01-22 18:54:33.397237741 +0800
@@ -841,9 +841,11 @@
 		csummode = CHECKSUM_PARTIAL;
 
 	cork->length += length;
-	if (((length > mtu) || (skb && skb_is_gso(skb))) &&
+	if ((skb && skb_is_gso(skb)) ||
+	    ((length > mtu) &&
+	    (skb_queue_len(queue) <= 1) &&
 	    (sk->sk_protocol == IPPROTO_UDP) &&
-	    (rt->dst.dev->features & NETIF_F_UFO) && !rt->dst.header_len) {
+	    (rt->dst.dev->features & NETIF_F_UFO) && !rt->dst.header_len)) {
 		err = ip_ufo_append_data(sk, queue, getfrag, from, length,
 					 hh_len, fragheaderlen, transhdrlen,
 					 maxfraglen, flags);
@@ -1159,6 +1161,7 @@
 
 	cork->length += size;
 	if ((size + skb->len > mtu) &&
+		(skb_queue_len(&sk->sk_write_queue) == 1) &&
 	    (sk->sk_protocol == IPPROTO_UDP) &&
 	    (rt->dst.dev->features & NETIF_F_UFO)) {
 		skb_shinfo(skb)->gso_size = mtu - fragheaderlen;
