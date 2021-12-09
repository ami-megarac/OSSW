--- linux_org/net/ipv4/gre_offload.c	2019-01-22 15:21:34.643780986 +0800
+++ linux/net/ipv4/gre_offload.c	2019-01-22 15:32:23.973522681 +0800
@@ -145,6 +145,11 @@
 	struct packet_offload *ptype;
 	__be16 type;
 
+	if (NAPI_GRO_CB(skb)->encap_mark)
+		goto out;
+
+	NAPI_GRO_CB(skb)->encap_mark = 1;	
+	
 	off = skb_gro_offset(skb);
 	hlen = off + sizeof(*greh);
 	greh = skb_gro_header_fast(skb, off);
