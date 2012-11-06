#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include "af.h"

int main()
{
        int sock_fd;
        struct sockaddr_un serv_addr;
        socklen_t addr_len;
        char mesg[128];
        time_t cur_time;
        int i;

        sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sock_fd < 0)
        {
                perror("socket");
                return -1;
        }

        serv_addr.sun_family = AF_UNIX;
        strcpy(serv_addr.sun_path, LOCAL_ADDR);

        addr_len = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

        for (i=0; i<65535; i++)
        {
                cur_time = time(NULL);
                strcpy(mesg, asctime(localtime(&cur_time)));

                sendto(sock_fd, (char*)mesg, strlen(mesg), 0,
                                (struct sockaddr*)&serv_addr, addr_len);
        }

        strcpy(mesg, "exit");

        sendto(sock_fd, (char*)mesg, strlen(mesg), 0,
                        (struct sockaddr*)&serv_addr, addr_len);

        close(sock_fd);

        return 0;
}
