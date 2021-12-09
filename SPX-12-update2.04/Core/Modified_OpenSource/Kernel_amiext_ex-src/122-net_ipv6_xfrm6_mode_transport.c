--- linux_org/net/ipv6/xfrm6_mode_transport.c	2019-01-22 15:53:29.159194454 +0800
+++ linux/net/ipv6/xfrm6_mode_transport.c	2019-01-22 16:09:58.465013300 +0800
@@ -28,6 +28,8 @@
 	iph = ipv6_hdr(skb);
 
 	hdr_len = x->type->hdr_offset(x, skb, &prevhdr);
+	if (hdr_len < 0)
+		return hdr_len;
 	skb_set_mac_header(skb, (prevhdr - x->props.header_len) - skb->data);
 	skb_set_network_header(skb, -x->props.header_len);
 	skb->transport_header = skb->network_header + hdr_len;
