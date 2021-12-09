--- linux_org/include/linux/netfilter/x_tables.h	2018-12-17 10:44:53.938110197 +0800
+++ linux/include/linux/netfilter/x_tables.h	2018-12-19 11:27:12.872117707 +0800
@@ -239,11 +239,18 @@
 int xt_register_matches(struct xt_match *match, unsigned int n);
 void xt_unregister_matches(struct xt_match *match, unsigned int n);
 
+int xt_check_entry_offsets(const void *base, const char *elems,
+			   unsigned int target_offset,
+			   unsigned int next_offset);
+
 int xt_check_match(struct xt_mtchk_param *, unsigned int size, u_int8_t proto,
 		   bool inv_proto);
 int xt_check_target(struct xt_tgchk_param *, unsigned int size, u_int8_t proto,
 		    bool inv_proto);
 
+void *xt_copy_counters_from_user(const void __user *user, unsigned int len,
+				 struct xt_counters_info *info, bool compat);
+
 struct xt_table *xt_register_table(struct net *net,
 				   const struct xt_table *table,
 				   struct xt_table_info *bootstrap,
@@ -421,7 +428,7 @@
 int xt_compat_calc_jump(u_int8_t af, unsigned int offset);
 
 int xt_compat_match_offset(const struct xt_match *match);
-int xt_compat_match_from_user(struct xt_entry_match *m, void **dstptr,
+void xt_compat_match_from_user(struct xt_entry_match *m, void **dstptr,
 			      unsigned int *size);
 int xt_compat_match_to_user(const struct xt_entry_match *m,
 			    void __user **dstptr, unsigned int *size);
@@ -431,6 +438,9 @@
 				unsigned int *size);
 int xt_compat_target_to_user(const struct xt_entry_target *t,
 			     void __user **dstptr, unsigned int *size);
+int xt_compat_check_entry_offsets(const void *base, const char *elems,
+				  unsigned int target_offset,
+				  unsigned int next_offset);
 
 #endif /* CONFIG_COMPAT */
 #endif /* _X_TABLES_H */
