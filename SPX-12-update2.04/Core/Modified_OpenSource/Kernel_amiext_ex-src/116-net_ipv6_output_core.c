--- linux_org/net/ipv6/output_core.c	2019-01-22 16:00:56.824645425 +0800
+++ linux/net/ipv6/output_core.c	2019-01-22 16:01:56.457056323 +0800
@@ -11,14 +11,13 @@
 int ip6_find_1stfragopt(struct sk_buff *skb, u8 **nexthdr)
 {
 	u16 offset = sizeof(struct ipv6hdr);
-	struct ipv6_opt_hdr *exthdr =
-				(struct ipv6_opt_hdr *)(ipv6_hdr(skb) + 1);
 	unsigned int packet_len = skb_tail_pointer(skb) -
 		skb_network_header(skb);
 	int found_rhdr = 0;
 	*nexthdr = &ipv6_hdr(skb)->nexthdr;
 
-	while (offset + 1 <= packet_len) {
+	while (offset <= packet_len) {
+		struct ipv6_opt_hdr *exthdr;
 
 		switch (**nexthdr) {
 
@@ -39,13 +38,15 @@
 			return offset;
 		}
 
-		offset += ipv6_optlen(exthdr);
-		*nexthdr = &exthdr->nexthdr;
+		if (offset + sizeof(struct ipv6_opt_hdr) > packet_len)
+			return -EINVAL;
 		exthdr = (struct ipv6_opt_hdr *)(skb_network_header(skb) +
 						 offset);
+		offset += ipv6_optlen(exthdr);
+		*nexthdr = &exthdr->nexthdr;						 
 	}
 
-	return offset;
+	return -EINVAL;
 }
 EXPORT_SYMBOL(ip6_find_1stfragopt);
 
