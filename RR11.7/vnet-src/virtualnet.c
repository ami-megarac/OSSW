#include <linux/module.h>  
#include <linux/netdevice.h> 
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/version.h>

unsigned char BCastAddr[6]= { 0xFE,0xFF,0xFF,0xFF,0xFF,0xFF};
extern int upper_net_recv(struct net_device *dev, char *Buffer,int packet_length);
extern int upper_net_xmit (struct sk_buff *skb,struct net_device *dev);
typedef struct {
	struct net_device *dev;
} virtual_priv;
	
static int 
virtual_net_open (struct net_device *dev)
{
	printk("virtual_net_open called\n");
	dev->trans_start =  jiffies;
	netif_start_queue (dev);
	return 0;
}

static int 
virtual_net_release (struct net_device *dev)
{
	printk ("virtual_net_release called\n");
	netif_stop_queue(dev);
	return 0;
}

int
virtual_net_recv(struct net_device *dev,char *Buffer,int packet_length)
{
        struct sk_buff  * skb;
	unsigned char *data;

        if (packet_length == 0)
		return 1;

	skb = dev_alloc_skb(packet_length+2);

        if ( skb == NULL )
        {
        	printk(KERN_NOTICE "%s: Low memory, packet dropped.\n",	dev->name);
                dev->stats.rx_dropped++;
		return 1;
	}

        skb_reserve( skb, 2 );   /* 16 bit alignment */
        skb->dev = dev;

        data = skb_put( skb, packet_length);
	memcpy(data,Buffer,packet_length);
        skb->protocol = eth_type_trans(skb, dev );
        netif_rx(skb);
        dev->stats.rx_packets++;
	dev->stats.rx_bytes+=packet_length;

	return 0;


}


int
virtual_net_xmit (struct sk_buff *skb, struct net_device *dev)

{
	int length;
	virtual_priv *p;
	
	if (skb == NULL)
		return 0;

	netif_stop_queue(dev);
	if (skb->len < ETH_ZLEN)
	{
		if (skb_padto(skb,ETH_ZLEN))	//Error
		{
			netif_wake_queue(dev);
			return NETDEV_TX_OK;
		}

	}
	length=skb->len;

	p=netdev_priv(dev);

#if 1 
	if ( upper_net_recv(p->dev,skb->data,length) == 1)
		dev->stats.tx_dropped++;
	else
	{
		dev->stats.tx_packets++;
		dev->stats.tx_bytes+=length;
	}
#else
		dev->stats.tx_dropped++;
		dev->stats.tx_packets++;
		dev->stats.tx_bytes+=length;

#endif


	dev_kfree_skb_any(skb);
	dev->trans_start =  jiffies;
	netif_wake_queue(dev);
	return 0;
}



static struct net_device_stats *
virtual_net_get_stats(struct net_device *netdev)
{
	return &netdev->stats;
}


static int
virtual_net_ioctl(struct net_device *netdev, struct ifreq *ifr, int cmd)
{
	//printk ("vrtual_net ioclt function called\n");
	return -EOPNOTSUPP;
}
#if 0

static int 
vnet_netdev_event(struct notifier_block *this, unsigned long event, void *ptr)
{
#if (LINUX_VERSION_CODE >KERNEL_VERSION(3,4,11))
        struct net_device *dev = netdev_notifier_info_to_dev(ptr);
#else
        struct net_device *dev = ptr;
#endif

    /* Handle only Ethernet devices. Don't handle others */
    if (dev->type != ARPHRD_ETHER)
        return NOTIFY_DONE;


    return NOTIFY_DONE;
}



static struct notifier_block vnet_netdev_notifier = {
        .notifier_call = vnet_netdev_event,
};
#endif

#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
static const struct net_device_ops virtual_net_devOps =
{
    .ndo_open = virtual_net_open,
    .ndo_stop = virtual_net_release,
    .ndo_start_xmit = virtual_net_xmit,
    .ndo_get_stats = virtual_net_get_stats,
    .ndo_do_ioctl = virtual_net_ioctl,
    .ndo_set_mac_address = eth_mac_addr,
};
#else
static void virt_net_setup(struct net_device *dev)
{
	dev->hard_start_xmit = virtual_net_xmit;
	dev->get_stats = virtual_net_get_stats;
	dev->open = virtual_net_open;
        dev->stop = virtual_net_release;
        dev->do_ioctl = virtual_net_ioctl;
}
#endif
static struct net_device *virtual_net=NULL;

struct net_device * virtual_net_init_module (struct net_device *udev)
{
	int result;
	virtual_priv *p;

	printk ("virtual_net: Initializing the  Module\n");
	virtual_net=alloc_etherdev(sizeof(virtual_priv));
	if (!virtual_net)
		return NULL;
		
	p=netdev_priv(virtual_net);
	p->dev= udev;
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
	virtual_net->netdev_ops = &virtual_net_devOps;
#else	
	virt_net_setup (virtual_net);
#endif		
	virtual_net->watchdog_timeo = 5 *HZ;
	strcpy(virtual_net->name,"virt%d");
	memcpy(virtual_net->dev_addr,BCastAddr,6);
	if ((result = register_netdev (virtual_net))) 
	{
		printk ("virtual_net: Error %d  initializing card virtual_net card\n",result);
		return NULL;
	}
	printk ("virtual_net device initialized\n");
	return virtual_net;
}
	
void virtual_net_cleanup (void)
{
	printk ("virtual_net:Cleaning Up the Module\n");
	unregister_netdev (virtual_net);
	return;
}
	
