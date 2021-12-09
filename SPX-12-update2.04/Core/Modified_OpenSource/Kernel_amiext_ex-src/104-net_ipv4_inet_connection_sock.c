--- linux_org/net/ipv4/inet_connection_sock.c	2019-01-22 16:14:05.313116526 +0800
+++ linux/net/ipv4/inet_connection_sock.c	2019-01-22 16:15:57.612800767 +0800
@@ -680,6 +680,8 @@
 		inet_sk(newsk)->inet_sport = htons(inet_rsk(req)->ir_num);
 		newsk->sk_write_space = sk_stream_write_space;
 
+		inet_sk(newsk)->mc_list = NULL;
+		
 		newicsk->icsk_retransmits = 0;
 		newicsk->icsk_backoff	  = 0;
 		newicsk->icsk_probes_out  = 0;
