#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

typedef int bool;
#define true 1
#define false 0

#define MAX_NIC 10
#define IFC_BUF_LEN (sizeof(struct ifreq)*MAX_NIC)
//#define NIC_CONF_FILE "/etc/network/interfaces"
#define NIC_CONF_FILE "/home/gonglei/interfaces"
#define NIC_STAT_FILE "/proc/net/dev"

struct nic_ipconf
{
        struct sockaddr_in addr;
        struct sockaddr_in mask;
        struct sockaddr_in gateway;
};

struct nic_stat
{
	unsigned long long bytes, packets, errs, drop,\
		fifo, frame, compressed, multicast;
};

enum trunk_type
{
        TRUNKING = 0,
        LACP,
        MULTI_HOMED
};

struct nic_info
{
        char name[IFNAMSIZ];		// 网卡名称
        struct nic_ipconf active;	// 当前配置
        struct nic_ipconf conf;		// 配置文件的配置
        uint8_t mac[6];			// MAC地址
        short flags;			// 标志位，使用IFF_UP && IFF_RUNNING
						// 判断是否运行中
        int mtu;			// 9000表示巨帧
	struct nic_stat rx, tx;		// 接收/发送统计信息
};

struct nic_info nic_list[MAX_NIC];

//-----------------------------------
char* const ignore_list[] = {
	"lo", "mgmt0", NULL
};
char* const nic_name_map[] = {
	"mgmt1", "MGR",
	"eth0", "GE1",
	"eth1", "GE2",
	"eth2", "GE3",
	"eth3", "GE4",
	NULL
};
//-----------------------------------

/* 检查网卡名称是否在忽略的列表 */
bool valid_nic_name(const char *name)
{
	int i = 0;

	if (!name)
		return false;

	// check if list is valid
	if (sizeof(ignore_list) <= 0)
		return true;

	// loop check if nic name in the list
	while(ignore_list[i])
		if (!strcmp(ignore_list[i++], name))
			return false;

	// nothing found, is valid
	return true;
}

const char *nic_name_convert(const char *nic_name)
{
	int i = 0;
	char actual_name[IFNAMSIZ];

	if (!nic_name)
		return NULL;

	// name convert
	if (sizeof(nic_name_map))
		while(nic_name_map[i])
			if (!strcmp(nic_name_map[i++], nic_name))
				return strcpy(actual_name, nic_name_map[i]);

	return NULL;
}

/* 加网卡名称到列表，相同名称认为是一个网卡 */
size_t add_name2list(struct nic_info *list, size_t n, const char *name)
{
	int i;
	bool same_nic = false;

	if (!list)
		return 0;
	if (n<=0)
		return 0;
	if (!name)
		return 0;

	// 查一下已经获取的网卡数量
	for (i=0; i<n; i++)
	{
		if ('\0' == list[i].name[0])
			break;
		if (!strcmp(list[i].name, name))
			same_nic = true;
	}

	// 无同名网卡则加入列表
	if (!same_nic)
		strcpy(list[i++].name, name);

	return i;
}

struct nic_info *get_nic_by_name(struct nic_info *list, const char *name)
{
	struct nic_info *p;

	if (!list)
		return NULL;
	if (!name)
		return NULL;

	p = list;
	while('\0' != p->name[0])
	{
		if (!strcmp(p->name, name))
			return p;
		p++;
	}

	return NULL;
}

size_t wis_sys_nic_get(struct nic_info *nic_list, size_t n)
{
	int sockfd;
	struct ifconf ifc;
	struct ifreq *ifreq;
	unsigned char ifc_buff[IFC_BUF_LEN];

	size_t act_nics = 0;	// 实际获取到得网卡总数量
	FILE *fp;

	/* 交换变量 */
	struct nic_info *nic = NULL;
	char *p = NULL, *tmpname;
	char tmpbuf[128], confname[IFNAMSIZ], tmpline[1024];
	int i;
	struct ifreq tmpif;

	if (!nic_list)
		return 0;
	if (n<=0)
		return 0;

	// 获取活动网卡配置
	ifc.ifc_len = IFC_BUF_LEN;
	ifc.ifc_buf = ifc_buff;

	if ( (sockfd=socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return 0;

	if (ioctl(sockfd, SIOCGIFCONF, &ifc) != 0)
	{
		close(sockfd);
		return 0;
	}

	bzero(nic_list, sizeof(struct nic_info)*n);
	
	// 获取活动网卡列表
	ifreq = (struct ifreq*)ifc_buff;
	size_t x = (ifc.ifc_len / sizeof(struct ifreq));

	for ( ; x>0; x--)
	{
		tmpname = ifreq->ifr_name;
		if (valid_nic_name(tmpname))
			if ((act_nics=add_name2list(nic_list, n, tmpname))>=n)
				break;
		ifreq++;
	}
	close(sockfd);

	// 获取配置文件网卡列表
	if ( !(fp=fopen(NIC_CONF_FILE, "r")) )
		return 0;

	while(!feof(fp))
	{
		if (!fgets(tmpline, 1024, fp))
			continue;
		
		if (tmpline[0] == '#')
			continue;

		// 获取网卡名称
		if (!strncmp(tmpline, "iface", 5))
		{
			sscanf(tmpline, "iface %s %s", confname, tmpbuf);
			if (valid_nic_name(confname))
			{
				act_nics=add_name2list(nic_list, n, confname);
				nic = get_nic_by_name(nic_list, confname);
			}
			else
				nic = NULL;
		}

		p = strstr(tmpline, "address ");
		if (nic && p)
		{
			p += strlen("address ");
			nic->conf.addr.sin_addr.s_addr = inet_addr(p);
			continue;
		}

		p = strstr(tmpline, "netmask ");
		if (nic && p)
		{
			p += strlen("netmask ");
			nic->conf.mask.sin_addr.s_addr = inet_addr(p);
			continue;
		}

		p = strstr(tmpline, "gateway ");
		if (nic && p)
		{
			p += strlen("gateway ");
			nic->conf.gateway.sin_addr.s_addr = inet_addr(p);
			continue;
		}
	}
	fclose(fp);

	if (act_nics<=0)
		return 0;

	// 获取网卡基本信息
	if ( (sockfd=socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return 0;
	bzero(&tmpif, sizeof(struct ifreq));

	nic = nic_list;
	while ('\0' != nic->name[0])
	{
		strcpy(tmpif.ifr_name, nic->name);
		// MAC Address
		if (!ioctl(sockfd, SIOCGIFHWADDR, &tmpif))
			memcpy(nic->mac, tmpif.ifr_hwaddr.sa_data, 6);

		// IP Address
		if (!ioctl(sockfd, SIOCGIFADDR, &tmpif))
			memcpy(&nic->active.addr,
				(struct sockaddr_in*)&tmpif.ifr_addr,
				sizeof(struct sockaddr_in));

		// Netmask
		if (!ioctl(sockfd, SIOCGIFNETMASK, &tmpif))
			memcpy(&nic->active.mask,
				(struct sockaddr_in*)&tmpif.ifr_addr,
				sizeof(struct sockaddr_in));

		// TODO: Gateway

		// MTU
		if (!ioctl(sockfd, SIOCGIFMTU, &tmpif))
			nic->mtu = tmpif.ifr_mtu;

		// Flags
		if (!ioctl(sockfd, SIOCGIFFLAGS, &tmpif))
			nic->flags = tmpif.ifr_flags;
		nic++;
	}
	close(sockfd);

	// 获取统计信息
	if ((fp=fopen(NIC_STAT_FILE, "r"))!=NULL)
	{
		nic = nic_list;
		while(!feof(fp))
		{
			fgets(tmpline, 1024, fp);
			if ( ('\0'!=nic->name[0]) &&
				(p=strstr(tmpline, nic->name)) != NULL)
			{
				p += (strlen(nic->name) + 1);
				unsigned long long xx;
				sscanf(p, "%llu %llu %llu %llu %llu %llu %llu %llu %llu \
						%llu %llu %llu %llu %llu %llu %llu",
					&nic->rx.bytes, &nic->rx.packets,
					&nic->rx.errs, &nic->rx.drop,
					&nic->rx.fifo, &nic->rx.frame,
					&nic->rx.compressed, &nic->rx.multicast,

					&nic->tx.bytes, &nic->tx.packets,
					&nic->tx.errs, &nic->tx.drop,
					&nic->tx.fifo, &nic->tx.frame,
					&nic->tx.compressed, &nic->tx.multicast
				      );
				nic++;
			}
		}
		fclose(fp);
	}
	
	// 名称转换

	// debug
	for (i=0; i<act_nics; i++)
	{
		puts("--------------------");
		printf("name: %s\n", nic_list[i].name);
		printf("conf.addr: %s\n", inet_ntoa(nic_list[i].conf.addr.sin_addr));
		printf("conf.mask: %s\n", inet_ntoa(nic_list[i].conf.mask.sin_addr));
		printf("conf.geteway: %s\n", inet_ntoa(nic_list[i].conf.gateway.sin_addr));

		printf("active.addr: %s\n", inet_ntoa(nic_list[i].active.addr.sin_addr));
		printf("active.mask: %s\n", inet_ntoa(nic_list[i].active.mask.sin_addr));
		printf("active.geteway: %s\n", inet_ntoa(nic_list[i].active.gateway.sin_addr));

		puts("rx:");
		printf("  bytes: %llu\n", nic_list[i].rx.bytes);
		printf("  packets: %llu\n", nic_list[i].rx.packets);

		puts("tx:");
		printf("  bytes: %llu\n", nic_list[i].tx.bytes);
		printf("  packets: %llu\n", nic_list[i].tx.packets);
	}


	return act_nics;
}

size_t nic_info()
{
        int sockfd, i;
        struct ifconf ifc;
        unsigned char buf[IFC_BUF_LEN];
        struct ifreq *ifreq;

	struct ifreq tmpif;
        struct sockaddr_in *tmpaddr;
        char *tmpname;
	unsigned char tmpmac[6];
	short tmpflags;

        // init
        ifc.ifc_len = IFC_BUF_LEN;
        ifc.ifc_buf = buf;

        if ( (sockfd=socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        {
                perror("socket");
                exit(1);
        }

        // 获取网卡IP地址
        ioctl(sockfd, SIOCGIFCONF, &ifc);

        ifreq = (struct ifreq*)buf;
        size_t x = (ifc.ifc_len/sizeof(struct ifreq));

	printf("ifc.ifc_len = %d\n", ifc.ifc_len);
	printf(" %d netcard got!\n", (int)x);

        // set active ip
        for ( ; x>0; x--)
        {
                tmpname = ifreq->ifr_name;
                //set_nic_active_ip(tmpname, tmpaddr, NULL, NULL);

		// MAC Address
		bzero(&tmpif, sizeof(struct ifreq));
		strcpy(tmpif.ifr_name, tmpname);
		ioctl(sockfd, SIOCGIFHWADDR, &tmpif);
		memcpy(tmpmac, tmpif.ifr_hwaddr.sa_data, sizeof(tmpmac));
		printf("iface: %s\n", tmpname);
		printf("MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
			tmpmac[0], tmpmac[1], tmpmac[2],
			tmpmac[3], tmpmac[4], tmpmac[5]);

		// IP Address
		bzero(&tmpif, sizeof(struct ifreq));
		strcpy(tmpif.ifr_name, tmpname);
		ioctl(sockfd, SIOCGIFADDR, &tmpif);
		tmpaddr = (struct sockaddr_in*)&(tmpif.ifr_addr);
		printf("IP: %s\n", inet_ntoa(tmpaddr->sin_addr));

		// Netmask
		bzero(&tmpif, sizeof(struct ifreq));
		strcpy(tmpif.ifr_name, tmpname);
		ioctl(sockfd, SIOCGIFNETMASK, &tmpif);
		tmpaddr = (struct sockaddr_in*)&(tmpif.ifr_netmask);
		printf("Mask: %s\n", inet_ntoa(tmpaddr->sin_addr));

		// Broad Address
		bzero(&tmpif, sizeof(struct ifreq));
		strcpy(tmpif.ifr_name, tmpname);
		ioctl(sockfd, SIOCGIFBRDADDR, &tmpif);
		tmpaddr = (struct sockaddr_in*)&(tmpif.ifr_broadaddr);
		printf("Broad: %s\n", inet_ntoa(tmpaddr->sin_addr));

		// Metric
		bzero(&tmpif, sizeof(struct ifreq));
		strcpy(tmpif.ifr_name, tmpname);
		ioctl(sockfd, SIOCGIFMETRIC, &tmpif);
		printf("Metric: %d\n", tmpif.ifr_metric);

		// MTU
		bzero(&tmpif, sizeof(struct ifreq));
		strcpy(tmpif.ifr_name, tmpname);
		ioctl(sockfd, SIOCGIFMTU, &tmpif);
		printf("MTU: %d\n", tmpif.ifr_mtu);

		// dest addr
		bzero(&tmpif, sizeof(struct ifreq));
		strcpy(tmpif.ifr_name, tmpname);
		ioctl(sockfd, SIOCGIFDSTADDR, &tmpif);
		tmpaddr = (struct sockaddr_in*)&(tmpif.ifr_dstaddr);
		printf("DstAddr: %s\n", inet_ntoa(tmpaddr->sin_addr));

		// Flags
		bzero(&tmpif, sizeof(struct ifreq));
		strcpy(tmpif.ifr_name, tmpname);
		ioctl(sockfd, SIOCGIFFLAGS, &tmpif);
		tmpflags = tmpif.ifr_flags;
		printf("Flags: ");
		if (tmpflags & IFF_UP)
			printf("IFF_UP ");
		if (tmpflags & IFF_BROADCAST)
			printf("IFF_BROADCAST ");
		if (tmpflags & IFF_DEBUG)
			printf("IFF_DEBUG ");
		if (tmpflags & IFF_LOOPBACK)
			printf("IFF_LOOPBACK ");
		if (tmpflags & IFF_RUNNING)
			printf("IFF_RUNNING ");
		if (tmpflags & IFF_NOARP)
			printf("IFF_NOARP ");
		if (tmpflags & IFF_PROMISC)
			printf("IFF_PROMISC ");
		puts("");

		printf("-----------------------\n");

                ifreq++;
        }

#if 0
        for (i=0;i<act_cnt;i++)
        {
                printf("name = %s\n", nic_list[i].name);
                printf("addr: %s\n", inet_ntoa(nic_list[i].active.addr.sin_addr));
                printf("mask: %s\n", inet_ntoa(nic_list[i].active.mask.sin_addr));
        }
#endif

        close(sockfd);
}

int main()
{
        //return nic_info();
        return wis_sys_nic_get(nic_list, MAX_NIC);
}
