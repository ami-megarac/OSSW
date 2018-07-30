#include <linux/module.h>  
#include <linux/netdevice.h> 
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/version.h>

#define DRIVER_AUTHOR "Core Team"
#define DRIVER_DESC "Virtual Ethernet Driver"
#define DRIVER_LICENSE "GPL"

/* Module information */
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE ( DRIVER_LICENSE );

#define MAX_INSTANCES   4
#if 0
unsigned char MacAddr[MAX_INSTANCES][6]=
{
				{ 0x00,0x40,0xd9,0x99,0x88,0x78},
				{ 0x00,0x40,0xd9,0x99,0x88,0x79},
				{ 0x00,0x40,0xd9,0x99,0x88,0x7A},
				{ 0x00,0x40,0xd9,0x99,0x88,0x7B},
};
#endif
extern unsigned long enetaddr[][6];
extern struct net_device *virtual_net_init_module(struct net_device *udev);
extern void virtual_net_cleanup(void); 
extern int virtual_net_recv(struct net_device *dev, char *Buffer,int packet_length);
extern int virtual_net_xmit (struct sk_buff *skb,struct net_device *dev);

typedef struct {
	struct net_device *dev;
} upper_priv;
	
static int 
upper_net_open (struct net_device *dev)
{
	printk("upper_net_open called\n");
	dev->trans_start =  jiffies;
	netif_start_queue (dev);
	return 0;
}

static int 
upper_net_release (struct net_device *dev)
{
	printk ("upper_net_release called\n");
	netif_stop_queue(dev);
	return 0;
}


int
upper_net_recv(struct net_device *dev, char *Buffer,int packet_length)
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
upper_net_xmit (struct sk_buff *skb,struct net_device *dev)
{
	int length;
	upper_priv *p;
	
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
	if ( virtual_net_recv(p->dev,skb->data,length) == 1)
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
upper_net_get_stats(struct net_device *netdev)
{
	return &netdev->stats;
}


static int
upper_net_ioctl(struct net_device *netdev, struct ifreq *ifr, int cmd)
{
//	printk ("vrtual_net ioclt function called\n");
	return -EOPNOTSUPP;
	}

#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
static const struct net_device_ops upper_net_devOps =
{
    .ndo_open = upper_net_open,
    .ndo_stop = upper_net_release,
    .ndo_start_xmit = upper_net_xmit,
    .ndo_get_stats = upper_net_get_stats,
    .ndo_do_ioctl = upper_net_ioctl,
    .ndo_set_mac_address = eth_mac_addr,
};
#else
static void upper_net_setup(struct net_device *dev)
{
	dev->hard_start_xmit = upper_net_xmit;
	dev->get_stats = upper_net_get_stats;
	dev->open = upper_net_open;
	dev->stop = upper_net_release;
	dev->do_ioctl = upper_net_ioctl;
}
#endif 
static struct net_device *upper_net=NULL;



int upper_net_init_module (void)
{
	int result;
	upper_priv *p[MAX_INSTANCES];
	int i;
	int nic_cnt = CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT;

	printk ("upper_net: Initializing the  Module\n");
	for (i=0;i<MAX_INSTANCES;i++)
	{
		upper_net=alloc_etherdev(sizeof(upper_priv));
		if (!upper_net)
			return -ENOMEM;
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
		upper_net->netdev_ops = &upper_net_devOps;
#else
		upper_net_setup (upper_net);
#endif		
		upper_net->watchdog_timeo = 5 *HZ;
		strcpy(upper_net->name,"veth%d");
		memcpy(upper_net->dev_addr, enetaddr[nic_cnt + i],6);
		if ((result = register_netdev (upper_net))) 
		{
			printk ("upper_net: Error %d  initializing card upper_net card\n",result);
			return result;
		}
		p[i]=netdev_priv(upper_net);
		p[i]->dev=virtual_net_init_module(upper_net);

		if (p[i]->dev == NULL)
			return -ENOMEM;
	}
	printk ("upper_net device initialized\n");
	return 0;
}
	
void upper_net_cleanup (void)
{
	printk ("upper_net:Cleaning Up the Module\n");
	unregister_netdev (upper_net);
	virtual_net_cleanup();
	return;
}
	
module_init (upper_net_init_module);
module_exit (upper_net_cleanup);
