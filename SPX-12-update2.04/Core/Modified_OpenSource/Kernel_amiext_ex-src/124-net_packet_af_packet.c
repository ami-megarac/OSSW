--- linux_org/net/packet/af_packet.c	2019-01-22 15:38:46.098915244 +0800
+++ linux/net/packet/af_packet.c	2019-01-22 18:50:55.039338071 +0800
@@ -1453,13 +1453,17 @@
 		return -EINVAL;
 	}
 
-	if (!po->running)
-		return -EINVAL;
+	mutex_lock(&fanout_mutex);
+
+	err = -EINVAL;
+ 	if (!po->running)
+		goto out;
 
-	if (po->fanout)
-		return -EALREADY;
 
-	mutex_lock(&fanout_mutex);
+	err = -EALREADY;
+ 	if (po->fanout)
+		goto out;
+
 	match = NULL;
 	list_for_each_entry(f, &fanout_list, list) {
 		if (f->id == id &&
@@ -1515,17 +1519,16 @@
 	struct packet_sock *po = pkt_sk(sk);
 	struct packet_fanout *f;
 
-	f = po->fanout;
-	if (!f)
-		return;
-
 	mutex_lock(&fanout_mutex);
-	po->fanout = NULL;
+	f = po->fanout;
+	if (f) {
+		po->fanout = NULL;
 
-	if (atomic_dec_and_test(&f->sk_ref)) {
-		list_del(&f->list);
-		dev_remove_pack(&f->prot_hook);
-		kfree(f);
+		if (atomic_dec_and_test(&f->sk_ref)) {
+			list_del(&f->list);
+			dev_remove_pack(&f->prot_hook);
+			kfree(f);
+		}
 	}
 	mutex_unlock(&fanout_mutex);
 }
@@ -3264,19 +3267,25 @@
 
 		if (optlen != sizeof(val))
 			return -EINVAL;
-		if (po->rx_ring.pg_vec || po->tx_ring.pg_vec)
-			return -EBUSY;
 		if (copy_from_user(&val, optval, sizeof(val)))
 			return -EFAULT;
 		switch (val) {
 		case TPACKET_V1:
 		case TPACKET_V2:
 		case TPACKET_V3:
-			po->tp_version = val;
-			return 0;
+			break;
 		default:
 			return -EINVAL;
 		}
+		lock_sock(sk);
+		if (po->rx_ring.pg_vec || po->tx_ring.pg_vec) {
+			ret = -EBUSY;
+		} else {
+			po->tp_version = val;
+			ret = 0;
+		}
+		release_sock(sk);
+		return ret;		
 	}
 	case PACKET_RESERVE:
 	{
@@ -3284,12 +3293,19 @@
 
 		if (optlen != sizeof(val))
 			return -EINVAL;
-		if (po->rx_ring.pg_vec || po->tx_ring.pg_vec)
-			return -EBUSY;
 		if (copy_from_user(&val, optval, sizeof(val)))
 			return -EFAULT;
-		po->tp_reserve = val;
-		return 0;
+		if (val > INT_MAX)
+			return -EINVAL;		
+		lock_sock(sk);
+		if (po->rx_ring.pg_vec || po->tx_ring.pg_vec) {
+			ret = -EBUSY;
+		} else {
+			po->tp_reserve = val;
+			ret = 0;
+		}
+		release_sock(sk);
+		return ret;		
 	}
 	case PACKET_LOSS:
 	{
@@ -3739,6 +3755,7 @@
 	/* Added to avoid minimal code churn */
 	struct tpacket_req *req = &req_u->req;
 
+	lock_sock(sk);
 	/* Opening a Tx-ring is NOT supported in TPACKET_V3 */
 	if (!closing && tx_ring && (po->tp_version > TPACKET_V2)) {
 		WARN(1, "Tx-ring is not supported.\n");
@@ -3779,6 +3796,10 @@
 			goto out;
 		if (unlikely(req->tp_block_size & (PAGE_SIZE - 1)))
 			goto out;
+ 		if (po->tp_version >= TPACKET_V3 &&
+		    req->tp_block_size <=
+			  BLK_PLUS_PRIV((u64)req_u->req3.tp_sizeof_priv))
+ 			goto out;		
 		if (unlikely(req->tp_frame_size < po->tp_hdrlen +
 					po->tp_reserve))
 			goto out;
@@ -3788,6 +3809,8 @@
 		rb->frames_per_block = req->tp_block_size/req->tp_frame_size;
 		if (unlikely(rb->frames_per_block <= 0))
 			goto out;
+		if (unlikely(req->tp_block_size > UINT_MAX / req->tp_block_nr))
+			goto out;	
 		if (unlikely((rb->frames_per_block * req->tp_block_nr) !=
 					req->tp_frame_nr))
 			goto out;
@@ -3816,8 +3839,6 @@
 			goto out;
 	}
 
-	lock_sock(sk);
-
 	/* Detach socket from network */
 	spin_lock(&po->bind_lock);
 	was_running = po->running;
@@ -3865,11 +3886,11 @@
 		if (!tx_ring)
 			prb_shutdown_retire_blk_timer(po, tx_ring, rb_queue);
 	}
-	release_sock(sk);
 
 	if (pg_vec)
 		free_pg_vec(pg_vec, order, req->tp_block_nr);
 out:
+	release_sock(sk);
 	return err;
 }
 
