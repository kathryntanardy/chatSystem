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

static pthread_cond_t s_receiveCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t s_receiveMutexVar = PTHREAD_MUTEX_INITIALIZER;

static List * receiveList;

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
        recvfrom(socketDescriptor, messageRx, MSG_MAX_LENGTH,
        0, (struct sockaddr *) &sinRemote, &sin_len);
      

        pthread_mutex_lock(&s_receiveMutexVar);
        {

            pthread_mutex_signal(&s_receiveCondVar);
        }
        pthread_mutex_unlock(&s_receiveMutexVar);

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

void systemInit(){
    receiveList = List_create();
    
}