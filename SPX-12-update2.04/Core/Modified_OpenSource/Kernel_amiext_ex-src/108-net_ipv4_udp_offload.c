--- linux_org/net/ipv4/udp_offload.c	2019-01-22 15:21:40.787525857 +0800
+++ linux/net/ipv4/udp_offload.c	2019-01-22 15:32:49.635786189 +0800
@@ -157,12 +157,12 @@
 	unsigned int hlen, off;
 	int flush = 1;
 
-	if (NAPI_GRO_CB(skb)->udp_mark ||
+	if (NAPI_GRO_CB(skb)->encap_mark ||
 	    (!skb->encapsulation && skb->ip_summed != CHECKSUM_COMPLETE))
 		goto out;
 
-	/* mark that this skb passed once through the udp gro layer */
-	NAPI_GRO_CB(skb)->udp_mark = 1;
+	/* mark that this skb passed once through the tunnel gro layer */
+	NAPI_GRO_CB(skb)->encap_mark = 1;
 
 	off  = skb_gro_offset(skb);
 	hlen = off + sizeof(*uh);
