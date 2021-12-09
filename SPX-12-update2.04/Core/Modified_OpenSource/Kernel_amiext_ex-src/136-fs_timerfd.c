--- linux_org/fs/timerfd.c	2019-01-29 18:37:56.874307338 +0800
+++ linux/fs/timerfd.c	2019-01-29 18:39:44.055718010 +0800
@@ -39,6 +39,7 @@
 	int clockid;
 	struct rcu_head rcu;
 	struct list_head clist;
+	spinlock_t cancel_lock;
 	bool might_cancel;
 };
 
@@ -111,7 +112,7 @@
 	rcu_read_unlock();
 }
 
-static void timerfd_remove_cancel(struct timerfd_ctx *ctx)
+static void __timerfd_remove_cancel(struct timerfd_ctx *ctx)
 {
 	if (ctx->might_cancel) {
 		ctx->might_cancel = false;
@@ -121,6 +122,13 @@
 	}
 }
 
+static void timerfd_remove_cancel(struct timerfd_ctx *ctx)
+{
+	spin_lock(&ctx->cancel_lock);
+	__timerfd_remove_cancel(ctx);
+	spin_unlock(&ctx->cancel_lock);
+}
+
 static bool timerfd_canceled(struct timerfd_ctx *ctx)
 {
 	if (!ctx->might_cancel || ctx->moffs.tv64 != KTIME_MAX)
@@ -131,6 +139,7 @@
 
 static void timerfd_setup_cancel(struct timerfd_ctx *ctx, int flags)
 {
+	spin_lock(&ctx->cancel_lock);
 	if ((ctx->clockid == CLOCK_REALTIME ||
 	     ctx->clockid == CLOCK_REALTIME_ALARM) &&
 	    (flags & TFD_TIMER_ABSTIME) && (flags & TFD_TIMER_CANCEL_ON_SET)) {
@@ -140,9 +149,10 @@
 			list_add_rcu(&ctx->clist, &cancel_list);
 			spin_unlock(&cancel_lock);
 		}
-	} else if (ctx->might_cancel) {
-		timerfd_remove_cancel(ctx);
+	} else {
+		__timerfd_remove_cancel(ctx);
 	}
+	spin_unlock(&ctx->cancel_lock);
 }
 
 static ktime_t timerfd_get_remaining(struct timerfd_ctx *ctx)
@@ -325,6 +335,7 @@
 		return -ENOMEM;
 
 	init_waitqueue_head(&ctx->wqh);
+	spin_lock_init(&ctx->cancel_lock);
 	ctx->clockid = clockid;
 
 	if (isalarm(ctx))
