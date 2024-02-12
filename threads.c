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

#define PORT 6000
#define MSG_MAX_LENGTH 512

static pthread_t keyboardThreadPID;
static pthread_t ReceiverThreadPID;

static int socketDescriptor;

void * keyboardThread();
void Keyboard_init();
void Keyboard_shutDown();
void Screen_init();
void Screen_shutDown();
void Send_init();
void Send_shutDown();

void * receiveThread(){
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(&addr, 0, sizeof(addr));
    
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

    bind(socketDescriptor, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    while(1){
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MSG_MAX_LENGTH];
        int bytesRx = recvfrom(socketDescriptor, messageRx, MSG_MAX_LENGTH,
        0, (struct sockaddr *) &sinRemote, &sin_len);

        int terminateldx = (bytesRx < MSG_MAX_LENGTH) ? bytesRx : MSG_MAX_LENGTH - 1;
        messageRx[terminateldx] = 0;
        //NOT FINISHED YET

    }
    close(socketDescriptor);
    return NULL;
}

void Receive_init(){
    pthread_create(
        &ReceiverThreadPID,
        NULL,
        receiveThread,
        NULL
    );
}
void Receive_shutDown();

