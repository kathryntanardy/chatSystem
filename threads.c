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
static pthread_t ScreenThreadPID;

static pthread_cond_t s_receiveCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t s_receiveMutexVar = PTHREAD_MUTEX_INITIALIZER;

static List * receiveList;

static int socketDescriptor;

static void * keyboardThread();
static void Keyboard_init();
static void Keyboard_shutDown();

static void freeNode(void* pItem){
    pItem = NULL;
}

static void * Screenthread(){
    printf("Enter message: ");
    char * messagePointer; 

    while(1){
        pthread_mutex_lock(&s_receiveMutexVar);
        {
            pthread_cond_wait(&s_receiveCondVar, &s_receiveMutexVar);
            List_first(receiveList);
            messagePointer = List_remove(receiveList);
            printf("%s\n", messagePointer);
            printf("Enter message: ");
            free(messagePointer);
        }
        pthread_mutex_unlock(&s_receiveMutexVar);
    }
   
    return NULL;
}

static void Screen_init(){
    pthread_create(
        &ScreenThreadPID,
        NULL,
        Screenthread,
        NULL
    );
}

static void Screen_shutDown(){
    pthread_cancel(ScreenThreadPID);

    pthread_join(ScreenThreadPID, NULL);
}

static void Send_init();

static void Send_shutDown();

static void * receiveThread(){
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
        int terminateldx = (bytesRx < MSG_MAX_LENGTH? bytesRx : MSG_MAX_LENGTH - 1);
        messageRx[terminateldx] = 0;
        int incMe = atoi(messageRx);

        pthread_mutex_lock(&s_receiveMutexVar);
        {
            char * message = malloc(MSG_MAX_LENGTH);
            strncpy(message, messageRx, terminateldx);
            message[terminateldx] = 0;
            List_append(receiveList, message);
            pthread_cond_signal(&s_receiveCondVar);
        }
        pthread_mutex_unlock(&s_receiveMutexVar);

    }
    close(socketDescriptor);
    return NULL;
}


static void Receive_init(){
    pthread_create(
        &ReceiverThreadPID,
        NULL,
        receiveThread,
        NULL
    );
}
static void Receive_shutDown(){
    pthread_cancel(ReceiverThreadPID);

    pthread_join(ReceiverThreadPID, NULL);
}

void systemInit(){
    receiveList = List_create();

    Receive_init();
    Screen_init();
    
}

void systemShutDown(){
    Screen_shutDown();
    Receive_shutDown();

    List_free(receiveList, freeNode);
}