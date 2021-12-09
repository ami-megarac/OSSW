--- linux_org/net/core/sock.c	2019-01-22 15:35:53.429597830 +0800
+++ linux/net/core/sock.c	2019-01-22 15:37:59.723880194 +0800
@@ -724,7 +724,7 @@
 		val = min_t(u32, val, sysctl_wmem_max);
 set_sndbuf:
 		sk->sk_userlocks |= SOCK_SNDBUF_LOCK;
-		sk->sk_sndbuf = max_t(u32, val * 2, SOCK_MIN_SNDBUF);
+		sk->sk_sndbuf = max_t(int, val * 2, SOCK_MIN_SNDBUF);
 		/* Wake up sending tasks if we upped the value. */
 		sk->sk_write_space(sk);
 		break;
@@ -760,7 +760,7 @@
 		 * returning the value we actually used in getsockopt
 		 * is the most desirable behavior.
 		 */
-		sk->sk_rcvbuf = max_t(u32, val * 2, SOCK_MIN_RCVBUF);
+		sk->sk_rcvbuf = max_t(int, val * 2, SOCK_MIN_RCVBUF);
 		break;
 
 	case SO_RCVBUFFORCE:
