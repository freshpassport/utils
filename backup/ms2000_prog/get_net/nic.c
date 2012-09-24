#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <net/if.h> 
#include <netinet/in.h> 
#include <net/if_arp.h> 
#include <stdint.h>

#define MAX_NET_CARDS 16
#define LOOPBACK_IF_NAME "lo"
#define MAC_ADDR_LEN 6

void get_nic()
{
  int skfd, if_cnt, if_idx;
  struct ifreq if_buf[MAX_NET_CARDS];
  struct ifconf ifc;

  uint8_t mac_addr[6];
  uint32_t ip, netmask;
  struct sockaddr_in ip_addr, netmask_addr;

  if ((skfd=socket(AF_INET, SOCK_DGRAM, 0))<0)
  {
	perror("socket()");
  }

  ifc.ifc_len = sizeof(if_buf);
  ifc.ifc_buf = (char*)if_buf;
  if (ioctl(skfd, SIOCGIFCONF, (char*)&ifc))
  {
	perror("get nic base info");
  }

  if_cnt = (ifc.ifc_len) / (sizeof(struct ifreq));
  printf("if count: %d\n", if_cnt);

  for ( if_idx=0; if_idx<if_cnt; if_idx++ )
  {
	// ignore loopback
	if (!strcmp(LOOPBACK_IF_NAME, if_buf[if_idx].ifr_name))
	  continue;

	// check if nic is running
	if (ioctl(skfd, SIOCGIFFLAGS, (char*)(&if_buf[if_idx])))
	  continue;

	printf("-------------------------------\n");
	printf("Interface: %s\n", if_buf[if_idx].ifr_name);

	if (!ioctl(skfd, SIOCGIFHWADDR, (char*)(&if_buf[if_idx])))
	{
	  memcpy(mac_addr, (char*)(&(if_buf[if_idx].ifr_hwaddr.sa_data)), MAC_ADDR_LEN);
	  printf("HW: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
		  mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
	}

	if (!ioctl(skfd, SIOCGIFADDR, (char*)(&if_buf[if_idx])))
	{
	  memcpy(&ip_addr, (struct sockaddr_in*)(&(if_buf[if_idx].ifr_addr)), sizeof(struct sockaddr_in));
	  printf("IP: %s\n", inet_ntoa(ip_addr.sin_addr));
	}

	if (!ioctl(skfd, SIOCGIFNETMASK, (char*)(&if_buf[if_idx])))
	{
	  memcpy(&netmask_addr, (struct sockaddr_in*)(&(if_buf[if_idx].ifr_netmask)), sizeof(struct sockaddr_in));
	  printf("Mask: %s\n", inet_ntoa(netmask_addr.sin_addr));
	}
  }

  puts("");

  close(skfd);
}

int main()
{
  get_nic();
}

