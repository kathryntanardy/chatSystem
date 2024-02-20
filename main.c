#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h> //strncmp()
#include <unistd.h>
#include <signal.h>
#include "list.h"
//#include "threads.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 6000
#define MSG_MAX_LENGTH 512

int main ( int argc, char *argv[]){

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(&addr, 0, sizeof(addr));


    struct addrinfo hints;
    struct addrinfo *result;

        
    

    int socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketDescriptor == -1){
        printf("Error creating socket\n");
        return -1;
    }

    if (bind(socketDescriptor, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
        printf("Error binding socket\n");
        return -1;
    }
    

    return 0;

    
}