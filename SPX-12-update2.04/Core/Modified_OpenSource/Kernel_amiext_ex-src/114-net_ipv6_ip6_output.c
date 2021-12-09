--- linux_org/net/ipv6/ip6_output.c	2019-01-22 15:53:38.038817314 +0800
+++ linux/net/ipv6/ip6_output.c	2019-01-22 18:59:04.136857354 +0800
@@ -565,7 +565,10 @@
 	u8 *prevhdr, nexthdr = 0;
 	struct net *net = dev_net(skb_dst(skb)->dev);
 
-	hlen = ip6_find_1stfragopt(skb, &prevhdr);
+	err = ip6_find_1stfragopt(skb, &prevhdr);
+	if (err < 0)
+		goto fail;
+	hlen = err;
 	nexthdr = *prevhdr;
 
 	mtu = ip6_skb_dst_mtu(skb);
@@ -1291,10 +1294,11 @@
 
 	skb = skb_peek_tail(&sk->sk_write_queue);
 	cork->length += length;
-	if (((length > mtu) ||
-	     (skb && skb_is_gso(skb))) &&
+	if ((skb && skb_is_gso(skb)) ||
+	    (((length + fragheaderlen) > mtu) &&
+	    (skb_queue_len(&sk->sk_write_queue) <= 1) &&
 	    (sk->sk_protocol == IPPROTO_UDP) &&
-	    (rt->dst.dev->features & NETIF_F_UFO)) {
+	    (rt->dst.dev->features & NETIF_F_UFO))) {
 		err = ip6_ufo_append_data(sk, getfrag, from, length,
 					  hh_len, fragheaderlen,
 					  transhdrlen, mtu, flags, rt);
