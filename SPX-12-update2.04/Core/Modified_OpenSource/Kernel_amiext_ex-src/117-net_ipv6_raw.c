--- linux_org/net/ipv6/raw.c	2019-01-22 13:52:48.817470244 +0800
+++ linux/net/ipv6/raw.c	2019-01-22 14:08:27.477263881 +0800
@@ -737,6 +737,7 @@
 static int rawv6_sendmsg(struct kiocb *iocb, struct sock *sk,
 		   struct msghdr *msg, size_t len)
 {
+	struct ipv6_txoptions *opt_to_free = NULL;
 	struct ipv6_txoptions opt_space;
 	DECLARE_SOCKADDR(struct sockaddr_in6 *, sin6, msg->msg_name);
 	struct in6_addr *daddr, *final_p, final;
@@ -842,8 +843,10 @@
 		if (!(opt->opt_nflen|opt->opt_flen))
 			opt = NULL;
 	}
-	if (opt == NULL)
-		opt = np->opt;
+	if (!opt) {
+		opt = txopt_get(np);
+		opt_to_free = opt;
+		}
 	if (flowlabel)
 		opt = fl6_merge_options(&opt_space, flowlabel, opt);
 	opt = ipv6_fixup_options(&opt_space, opt);
@@ -910,6 +913,7 @@
 	dst_release(dst);
 out:
 	fl6_sock_release(flowlabel);
+	txopt_put(opt_to_free);
 	return err<0?err:len;
 do_confirm:
 	dst_confirm(dst);
