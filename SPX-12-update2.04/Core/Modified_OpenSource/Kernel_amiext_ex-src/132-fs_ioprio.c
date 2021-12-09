--- linux_org/fs/ioprio.c	2019-01-29 18:28:09.823027685 +0800
+++ linux/fs/ioprio.c	2019-01-29 18:28:48.925435642 +0800
@@ -149,8 +149,10 @@
 	if (ret)
 		goto out;
 	ret = IOPRIO_PRIO_VALUE(IOPRIO_CLASS_NONE, IOPRIO_NORM);
+	task_lock(p);	
 	if (p->io_context)
 		ret = p->io_context->ioprio;
+	task_unlock(p);	
 out:
 	return ret;
 }
