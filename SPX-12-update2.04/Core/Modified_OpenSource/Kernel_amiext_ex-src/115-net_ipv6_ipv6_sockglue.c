--- linux_org/net/ipv6/ipv6_sockglue.c	2019-01-22 13:52:48.813470401 +0800
+++ linux/net/ipv6/ipv6_sockglue.c	2019-01-22 14:07:30.068806323 +0800
@@ -110,10 +110,12 @@
 			icsk->icsk_ext_hdr_len = opt->opt_flen + opt->opt_nflen;
 			icsk->icsk_sync_mss(sk, icsk->icsk_pmtu_cookie);
 		}
-		opt = xchg(&inet6_sk(sk)->opt, opt);
+		opt = xchg((__force struct ipv6_txoptions **)&inet6_sk(sk)->opt,
+			   opt);
 	} else {
 		spin_lock(&sk->sk_dst_lock);
-		opt = xchg(&inet6_sk(sk)->opt, opt);
+		opt = xchg((__force struct ipv6_txoptions **)&inet6_sk(sk)->opt,
+			   opt);
 		spin_unlock(&sk->sk_dst_lock);
 	}
 	sk_dst_reset(sk);
@@ -214,8 +216,12 @@
 				sk->sk_family = PF_INET;
 			}
 			opt = xchg(&np->opt, NULL);
-			if (opt)
-				sock_kfree_s(sk, opt, opt->tot_len);
+			opt = xchg((__force struct ipv6_txoptions **)&np->opt,
+				   NULL);
+			if (opt) {
+				atomic_sub(opt->tot_len, &sk->sk_omem_alloc);
+				txopt_put(opt);
+			}
 			pktopt = xchg(&np->pktoptions, NULL);
 			kfree_skb(pktopt);
 
@@ -385,7 +391,8 @@
 		if (optname != IPV6_RTHDR && !ns_capable(net->user_ns, CAP_NET_RAW))
 			break;
 
-		opt = ipv6_renew_options(sk, np->opt, optname,
+		opt = rcu_dereference_protected(np->opt, sock_owned_by_user(sk));
+		opt = ipv6_renew_options(sk, opt, optname,		
 					 (struct ipv6_opt_hdr __user *)optval,
 					 optlen);
 		if (IS_ERR(opt)) {
@@ -414,8 +421,10 @@
 		retv = 0;
 		opt = ipv6_update_options(sk, opt);
 sticky_done:
-		if (opt)
-			sock_kfree_s(sk, opt, opt->tot_len);
+		if (opt) {
+			atomic_sub(opt->tot_len, &sk->sk_omem_alloc);
+			txopt_put(opt);
+		}
 		break;
 	}
 
@@ -468,6 +477,7 @@
 			break;
 
 		memset(opt, 0, sizeof(*opt));
+		atomic_set(&opt->refcnt, 1);
 		opt->tot_len = sizeof(*opt) + optlen;
 		retv = -EFAULT;
 		if (copy_from_user(opt+1, optval, optlen))
@@ -484,8 +494,10 @@
 		retv = 0;
 		opt = ipv6_update_options(sk, opt);
 done:
-		if (opt)
-			sock_kfree_s(sk, opt, opt->tot_len);
+		if (opt) {
+			atomic_sub(opt->tot_len, &sk->sk_omem_alloc);
+			txopt_put(opt);
+		}
 		break;
 	}
 	case IPV6_UNICAST_HOPS:
@@ -1090,10 +1102,11 @@
 	case IPV6_RTHDR:
 	case IPV6_DSTOPTS:
 	{
+		struct ipv6_txoptions *opt;
 
 		lock_sock(sk);
-		len = ipv6_getsockopt_sticky(sk, np->opt,
-					     optname, optval, len);
+		opt = rcu_dereference_protected(np->opt, sock_owned_by_user(sk));
+		len = ipv6_getsockopt_sticky(sk, opt, optname, optval, len);
 		release_sock(sk);
 		/* check if ipv6_getsockopt_sticky() returns err code */
 		if (len < 0)
