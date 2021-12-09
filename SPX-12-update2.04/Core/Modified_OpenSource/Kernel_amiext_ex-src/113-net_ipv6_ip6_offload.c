--- linux_org/net/ipv6/ip6_offload.c	2019-01-22 15:21:46.907271971 +0800
+++ linux/net/ipv6/ip6_offload.c	2019-01-22 16:09:04.179010541 +0800
@@ -86,7 +86,6 @@
 	const struct net_offload *ops;
 	int proto;
 	struct frag_hdr *fptr;
-	unsigned int unfrag_ip6hlen;
 	u8 *prevhdr;
 	int offset = 0;
 	bool encap, udpfrag;
@@ -142,8 +141,12 @@
 		skb->network_header = (u8 *)ipv6h - skb->head;
 
 		if (udpfrag) {
-			unfrag_ip6hlen = ip6_find_1stfragopt(skb, &prevhdr);
-			fptr = (struct frag_hdr *)((u8 *)ipv6h + unfrag_ip6hlen);
+			int err = ip6_find_1stfragopt(skb, &prevhdr);
+			if (err < 0) {
+				kfree_skb_list(segs);
+				return ERR_PTR(err);
+			}
+			fptr = (struct frag_hdr *)((u8 *)ipv6h + err);
 			fptr->frag_off = htons(offset);
 			if (skb->next != NULL)
 				fptr->frag_off |= htons(IP6_MF);
@@ -276,6 +279,19 @@
 	return pp;
 }
 
+static struct sk_buff **sit_gro_receive(struct sk_buff **head,
+					struct sk_buff *skb)
+{
+	if (NAPI_GRO_CB(skb)->encap_mark) {
+		NAPI_GRO_CB(skb)->flush = 1;
+		return NULL;
+	}
+
+	NAPI_GRO_CB(skb)->encap_mark = 1;
+
+	return ipv6_gro_receive(head, skb);
+}
+
 static int ipv6_gro_complete(struct sk_buff *skb, int nhoff)
 {
 	const struct net_offload *ops;
@@ -303,7 +319,7 @@
 	.callbacks = {
 		.gso_send_check = ipv6_gso_send_check,
 		.gso_segment = ipv6_gso_segment,
-		.gro_receive = ipv6_gro_receive,
+		.gro_receive  = sit_gro_receive,
 		.gro_complete = ipv6_gro_complete,
 	},
 };
