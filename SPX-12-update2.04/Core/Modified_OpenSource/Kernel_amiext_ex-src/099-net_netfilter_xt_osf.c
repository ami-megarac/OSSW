--- linux_org/net/netfilter/xt_osf.c	2019-01-07 16:31:33.375895546 +0800
+++ linux/net/netfilter/xt_osf.c	2019-01-07 16:35:05.231284909 +0800
@@ -19,6 +19,7 @@
 #include <linux/module.h>
 #include <linux/kernel.h>
 
+#include <linux/capability.h>
 #include <linux/if.h>
 #include <linux/inetdevice.h>
 #include <linux/ip.h>
@@ -68,7 +69,10 @@
 	struct xt_osf_user_finger *f;
 	struct xt_osf_finger *kf = NULL, *sf;
 	int err = 0;
-
+	
+	if (!capable(CAP_NET_ADMIN))
+		return -EPERM;
+	
 	if (!osf_attrs[OSF_ATTR_FINGER])
 		return -EINVAL;
 
@@ -112,6 +116,9 @@
 	struct xt_osf_finger *sf;
 	int err = -ENOENT;
 
+	if (!capable(CAP_NET_ADMIN))
+		return -EPERM;	
+	
 	if (!osf_attrs[OSF_ATTR_FINGER])
 		return -EINVAL;
 
