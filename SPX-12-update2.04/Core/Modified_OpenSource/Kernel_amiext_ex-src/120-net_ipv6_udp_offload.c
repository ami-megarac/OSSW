--- linux_org/net/ipv6/udp_offload.c	2019-01-22 15:53:43.926567518 +0800
+++ linux/net/ipv6/udp_offload.c	2019-01-22 16:19:00.334464764 +0800
@@ -51,6 +51,7 @@
 	int offset;
 	__wsum csum;
 	int tnl_hlen;
+	int err;
 
 	mss = skb_shinfo(skb)->gso_size;
 	if (unlikely(skb->len <= mss))
@@ -98,7 +99,10 @@
 		/* Find the unfragmentable header and shift it left by frag_hdr_sz
 		 * bytes to insert fragment header.
 		 */
-		unfrag_ip6hlen = ip6_find_1stfragopt(skb, &prevhdr);
+		err = ip6_find_1stfragopt(skb, &prevhdr);
+		if (err < 0)
+			return ERR_PTR(err);
+		unfrag_ip6hlen = err;
 		nexthdr = *prevhdr;
 		*prevhdr = NEXTHDR_FRAGMENT;
 		unfrag_len = (skb_network_header(skb) - skb_mac_header(skb)) +
