--- linux_org/net/socket.c	2019-01-22 20:12:57.751084907 +0800
+++ linux/net/socket.c	2019-01-22 13:58:41.577444679 +0800
@@ -1256,6 +1256,9 @@
 		return -EAFNOSUPPORT;
 	if (type < 0 || type >= SOCK_MAX)
 		return -EINVAL;
+	/* upper bound should be tested by per-protocol .create callbacks */
+	if (protocol < 0)
+		return -EINVAL;
 
 	/* Compatibility.
 
@@ -2407,31 +2410,31 @@
 			break;
 	}
 
-out_put:
-	fput_light(sock->file, fput_needed);
-
 	if (err == 0)
-		return datagrams;
+		goto out_put;
+
+	if (datagrams == 0) {
+		datagrams = err;
+		goto out_put;
+	}
 
-	if (datagrams != 0) {
+	/*
+	 * We may return less entries than requested (vlen) if the
+	 * sock is non block and there aren't enough datagrams...
+	 */
+	if (err != -EAGAIN) {
 		/*
-		 * We may return less entries than requested (vlen) if the
-		 * sock is non block and there aren't enough datagrams...
+		 * ... or  if recvmsg returns an error after we
+		 * received some datagrams, where we record the
+		 * error to return on the next call or if the
+		 * app asks about it using getsockopt(SO_ERROR).
 		 */
-		if (err != -EAGAIN) {
-			/*
-			 * ... or  if recvmsg returns an error after we
-			 * received some datagrams, where we record the
-			 * error to return on the next call or if the
-			 * app asks about it using getsockopt(SO_ERROR).
-			 */
-			sock->sk->sk_err = -err;
-		}
-
-		return datagrams;
+		sock->sk->sk_err = -err;
 	}
-
-	return err;
+out_put:
+	fput_light(sock->file, fput_needed);
+ 
+	return datagrams;
 }
 
 SYSCALL_DEFINE5(recvmmsg, int, fd, struct mmsghdr __user *, mmsg,
