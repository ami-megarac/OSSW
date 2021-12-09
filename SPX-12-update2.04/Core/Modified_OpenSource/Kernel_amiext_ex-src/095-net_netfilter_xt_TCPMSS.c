diff -Naur linux_org/net/netfilter/xt_TCPMSS.c linux/net/netfilter/xt_TCPMSS.c
--- linux_org/net/netfilter/xt_TCPMSS.c	2018-12-24 15:50:01.680779980 +0800
+++ linux/net/netfilter/xt_TCPMSS.c	2018-12-24 15:49:05.023607472 +0800
@@ -104,7 +104,7 @@
 	tcph = (struct tcphdr *)(skb_network_header(skb) + tcphoff);
 	tcp_hdrlen = tcph->doff * 4;
 
-	if (len < tcp_hdrlen)
+	if (len < tcp_hdrlen || tcp_hdrlen < sizeof(struct tcphdr))
 		return -1;
 
 	if (info->mss == XT_TCPMSS_CLAMP_PMTU) {
@@ -156,6 +156,10 @@
 	if (len > tcp_hdrlen)
 		return 0;
 
+	/* tcph->doff has 4 bits, do not wrap it to 0 */
+	if (tcp_hdrlen >= 15 * 4)
+		return 0;	
+	
 	/*
 	 * MSS Option not found ?! add it..
 	 */
