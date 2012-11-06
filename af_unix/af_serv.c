#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <error.h>
#include <time.h>
#include <stdint.h>
#include "af.h"

int main()
{
        int sock_fd;
        struct sockaddr_un local_addr;
        size_t addr_len;
        uint64_t counter = 0;
        time_t begin = 0, end;

        // create socket fd
        sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sock_fd < 0)
        {
                perror("socket");
                return -1;
        }

        // set local address
        local_addr.sun_family = AF_UNIX;
        strcpy(local_addr.sun_path, LOCAL_ADDR);
        unlink(local_addr.sun_path);
        addr_len = strlen(local_addr.sun_path) + sizeof(local_addr.sun_family);

        if (bind(sock_fd, (struct sockaddr*)&local_addr, addr_len) < 0)
        {
                perror("bind");
                close(sock_fd);
                return -1;
        }

        // process message
        for (;;)
        {
#define MSG_MAX 128
                ssize_t n;
                char mesg[MSG_MAX];

                n = recvfrom(sock_fd, mesg, MSG_MAX, 0, NULL, 0);
                if (!begin)
                        begin = time(NULL);
                //printf("n = %d\n", n);
                if (n>0)
                {
                        counter++;
                        mesg[n] = '\0';
                        //printf("msg: %s", mesg);
                        if (!strncmp(mesg, "exit", 4))
                                break;
                }
        }

        end = time(NULL);

        printf("time used: %d\n", (int)(end-begin));
        printf("mesg recv: %lld\n", counter);

        close(sock_fd);

        return 0;
}
