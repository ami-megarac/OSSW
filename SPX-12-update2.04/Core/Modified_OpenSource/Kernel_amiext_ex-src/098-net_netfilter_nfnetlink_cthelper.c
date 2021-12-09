--- linux_org/net/netfilter/nfnetlink_cthelper.c	2019-01-07 16:31:43.203659846 +0800
+++ linux/net/netfilter/nfnetlink_cthelper.c	2019-01-07 16:34:08.392057439 +0800
@@ -17,6 +17,7 @@
 #include <linux/types.h>
 #include <linux/list.h>
 #include <linux/errno.h>
+#include <linux/capability.h>
 #include <net/netlink.h>
 #include <net/sock.h>
 
@@ -292,6 +293,9 @@
 	struct nf_conntrack_tuple tuple;
 	int ret = 0, i;
 
+	if (!capable(CAP_NET_ADMIN))
+		return -EPERM;	
+	
 	if (!tb[NFCTH_NAME] || !tb[NFCTH_TUPLE])
 		return -EINVAL;
 
@@ -506,6 +510,9 @@
 	struct nf_conntrack_tuple tuple;
 	bool tuple_set = false;
 
+	if (!capable(CAP_NET_ADMIN))
+		return -EPERM;	
+	
 	if (nlh->nlmsg_flags & NLM_F_DUMP) {
 		struct netlink_dump_control c = {
 			.dump = nfnl_cthelper_dump_table,
@@ -578,6 +585,9 @@
 	bool tuple_set = false, found = false;
 	int i, j = 0, ret;
 
+	if (!capable(CAP_NET_ADMIN))
+		return -EPERM;	
+	
 	if (tb[NFCTH_NAME])
 		helper_name = nla_data(tb[NFCTH_NAME]);
 
