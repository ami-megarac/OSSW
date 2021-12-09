--- linux_org/net/ipv4/af_inet.c	2019-01-22 13:53:31.863773644 +0800
+++ linux/net/ipv4/af_inet.c	2019-01-22 15:31:45.319675073 +0800
@@ -258,6 +258,9 @@
 	int try_loading_module = 0;
 	int err;
 
+	if (protocol >= IPPROTO_MAX)
+		return -EINVAL;	
+	
 	sock->state = SS_UNCONNECTED;
 
 	/* Look for the requested type/protocol pair. */
@@ -1426,6 +1429,19 @@
 	return pp;
 }
 
+static struct sk_buff **ipip_gro_receive(struct sk_buff **head,
+					 struct sk_buff *skb)
+{
+	if (NAPI_GRO_CB(skb)->encap_mark) {
+		NAPI_GRO_CB(skb)->flush = 1;
+		return NULL;
+	}
+
+	NAPI_GRO_CB(skb)->encap_mark = 1;
+
+	return inet_gro_receive(head, skb);
+}
+
 static int inet_gro_complete(struct sk_buff *skb, int nhoff)
 {
 	__be16 newlen = htons(skb->len - nhoff);
@@ -1664,7 +1680,7 @@
 	.callbacks = {
 		.gso_send_check = inet_gso_send_check,
 		.gso_segment = inet_gso_segment,
-		.gro_receive = inet_gro_receive,
+		.gro_receive	= ipip_gro_receive,		
 		.gro_complete = inet_gro_complete,
 	},
 };
