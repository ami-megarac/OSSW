--- linux_org/net/ipv6/xfrm6_mode_ro.c	2019-01-22 15:53:22.543475760 +0800
+++ linux/net/ipv6/xfrm6_mode_ro.c	2019-01-22 16:09:23.258230393 +0800
@@ -47,6 +47,8 @@
 	iph = ipv6_hdr(skb);
 
 	hdr_len = x->type->hdr_offset(x, skb, &prevhdr);
+	if (hdr_len < 0)
+		return hdr_len;
 	skb_set_mac_header(skb, (prevhdr - x->props.header_len) - skb->data);
 	skb_set_network_header(skb, -x->props.header_len);
 	skb->transport_header = skb->network_header + hdr_len;
