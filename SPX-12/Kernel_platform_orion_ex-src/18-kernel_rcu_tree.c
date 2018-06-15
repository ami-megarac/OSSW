--- linux_old/kernel/rcu/tree.c	2015-09-04 18:10:26.866593103 +0800
+++ linux/kernel/rcu/tree.c	2015-09-04 18:11:08.082593049 +0800
@@ -1228,6 +1228,22 @@
 }
 
 /*
+ * Awaken the grace-period kthread for the specified flavor of RCU.
+ * Don't do a self-awaken, and don't bother awakening when there is
+ * nothing for the grace-period kthread to do (as in several CPUs
+ * raced to awaken, and we lost), and finally don't try to awaken
+ * a kthread that has not yet been created.
+ */
+static void rcu_gp_kthread_wake(struct rcu_state *rsp)
+{
+	if (current == rsp->gp_kthread ||
+	    !ACCESS_ONCE(rsp->gp_flags) ||
+	    !rsp->gp_kthread)
+		return;
+	wake_up(&rsp->gp_wq);
+}
+
+/*
  * If there is room, assign a ->completed number to any callbacks on
  * this CPU that have not already been assigned.  Also accelerate any
  * callbacks that were previously assigned a ->completed number that has
@@ -1670,7 +1686,7 @@
 	struct rcu_state *rsp = container_of(work, struct rcu_state, wakeup_work);
 
 	/* Wake up rcu_gp_kthread() to start the grace period. */
-	wake_up(&rsp->gp_wq);
+	rcu_gp_kthread_wake(rsp);
 }
 
 /*
@@ -1746,7 +1762,7 @@
 {
 	WARN_ON_ONCE(!rcu_gp_in_progress(rsp));
 	raw_spin_unlock_irqrestore(&rcu_get_root(rsp)->lock, flags);
-	wake_up(&rsp->gp_wq);  /* Memory barrier implied by wake_up() path. */
+	rcu_gp_kthread_wake(rsp);
 }
 
 /*
@@ -2322,7 +2338,7 @@
 	}
 	rsp->gp_flags |= RCU_GP_FLAG_FQS;
 	raw_spin_unlock_irqrestore(&rnp_old->lock, flags);
-	wake_up(&rsp->gp_wq);  /* Memory barrier implied by wake_up() path. */
+	rcu_gp_kthread_wake(rsp);
 }
 
 /*
