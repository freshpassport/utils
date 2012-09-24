// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>

/*
int main(int argc, char* argv[])
{
	return 0;
}
*/

#define SERVPORT 3333
 
int main(int argc, char* argv[])
{
  int sockfd;
  char buff[10240];
  struct sockaddr_in serv_addr;
  WSAData wsaData;

  if (argc<2)
	{
		printf("input ip\n");
		return 0;
	}

  if (WSAStartup(MAKEWORD(2,1), &wsaData)!=0)
  {
	  perror("WSAStartup()");
	  return 0;
  }
 
  if ((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0)
  {
    perror("socket()");
    return 0;
  }
 
  memset(&serv_addr, 0, sizeof(struct sockaddr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port = htons(SERVPORT);
 
  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr))<0)
  {
    perror("connect()");
    closesocket(sockfd);
    return 0;
  }
 
  while(1)
  {
    //if (write(sockfd, buff, 10240)<0)
	if (send(sockfd, buff, 10240, 0)<0)
      break;
  }
 
  closesocket(sockfd);
  WSACleanup();
  return 0;
}