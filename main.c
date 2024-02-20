#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h> //strncmp()
#include <unistd.h>
#include <signal.h>
#include "list.h"
#include "threads.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>


int main(int argc, char **argv)
{
    if (argc != 4) {
        printf("Usag: ./a.out hostname|ip\n");
        exit(1);
    }

    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    struct sockaddr_in *sinp;
    char *hostname = argv[2];
    char buf[INET_ADDRSTRLEN];
    char *server = argv[3]; 
    const char *addr;
    hint.ai_family = AF_INET; 
    hint.ai_socktype = 0; 
    hint.ai_flags = AI_PASSIVE; 
    hint.ai_protocol = 0; 
    hint.ai_addrlen = 0; 
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if (getaddrinfo(hostname, server, &hint, &ailist) < 0)
    {
        printf("str_error = %s\n", gai_strerror(errno));
        return -1;
    }

    // for (aip = ailist; aip != NULL; aip = aip->ai_next)
    // {
    //     sinp = (struct sockaddr_in *)aip->ai_addr;
    //     addr = inet_ntop(AF_INET, &sinp->sin_addr, buf, INET_ADDRSTRLEN);
    //     printf(" addr = %s, port = %d\n", addr?addr:"unknow ", ntohs(sinp->sin_port));
    // }
    systemInit(&argv[1],&ailist);

    return 0;
}

// int main(int argc, char **argv)
// {
//     if (argc != 2) {
//         printf("Usag: ./a.out hostname|ip\n");
//         exit(1);
//     }
//     struct addrinfo hints;
//     struct addrinfo *res, *cur;
//     int ret;
//     struct sockaddr_in *addr;
//     char ipbuf[16];
//     int port;

//     memset(&hints, 0, sizeof(struct addrinfo));
//     hints.ai_family = AF_INET; /* Allow IPv4 */
//     hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */
//     hints.ai_protocol = 0; /* Any protocol */
//     hints.ai_socktype = SOCK_DGRAM;
//     ret = getaddrinfo(argv[1], NULL,&hints,&res);
//     if (ret < 0) {
//         fprintf(stderr, "%s\n", gai_strerror(ret));
//         exit(1);
//     }

//     for (cur = res; cur != NULL; cur = cur->ai_next) {
//         addr = (struct sockaddr_in *)cur->ai_addr;
//         printf("ip: %s\n", inet_ntop(AF_INET, &addr->sin_addr, ipbuf, 16));
//         printf("port: %d\n", inet_ntop(AF_INET, &addr->sin_port, (void *)&port, 2));
//         printf("port: %d\n", ntohs(addr->sin_port));

//     }
//     freeaddrinfo(res);
//     exit(0);
// }

