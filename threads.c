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


#define MSG_MAX_LENGTH 10

static pthread_t keyboardThreadPID;
static pthread_t ReceiverThreadPID;
static pthread_t ScreenThreadPID;
static pthread_t SendThreadPID;

static pthread_cond_t s_receiveCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t s_receiveMutexVar = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t s_sendCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t s_sendMutexVar = PTHREAD_MUTEX_INITIALIZER;

static List * receiveList;
static List * sendList;

static int socketDescriptor;
static struct sockaddr_in sinRemote;
static struct sockaddr_in local;

static void * keyboardThread(){
    while (1){

        char message[MSG_MAX_LENGTH];
        fgets(message, MSG_MAX_LENGTH, stdin);
        size_t length = strlen(message);
        message[length-1] = '\0';

        if(message[0] == '!' && message[1] == '\0'){
            printf("hello\n");
            systemShutDown();
            exit(0);
        }
       

        pthread_mutex_lock(&s_sendMutexVar);
        {
            char * copiedMessage = malloc(length);
            strncpy(copiedMessage, message, length);
            copiedMessage[length - 1] = '\0';
            List_append(sendList, copiedMessage);
            pthread_cond_signal(&s_sendCondVar);
        }
        pthread_mutex_unlock(&s_sendMutexVar);
        
    
    }

    return NULL;
}

static void Keyboard_shutDown(){
    pthread_cancel(keyboardThreadPID);

    pthread_join(keyboardThreadPID, NULL);
}

static void freeNode(void* pItem){
    pItem = NULL;
}

static void * Screenthread(){
    printf("Enter message: \n");
    char * messagePointer; 

    while(1){

        pthread_mutex_lock(&s_receiveMutexVar);
        {
            pthread_cond_wait(&s_receiveCondVar, &s_receiveMutexVar);
            List_first(receiveList);
            messagePointer = List_remove(receiveList);
            printf("%s\n", messagePointer);
            printf("Enter message: \n");
            free(messagePointer);
        }
        pthread_mutex_unlock(&s_receiveMutexVar);
    }
   
    return NULL;
}


static void Screen_shutDown(){
    pthread_cancel(ScreenThreadPID);

    pthread_join(ScreenThreadPID, NULL);
}

static void * sendThread(){
    unsigned int sin_len = sizeof(sinRemote);

    while(1){
        pthread_mutex_lock(&s_sendMutexVar);
        {
            pthread_cond_wait(&s_sendCondVar,&s_sendMutexVar);
            List_first(sendList);
            char * messageTx = List_remove(sendList);
            printf("sent\n");
            sendto (socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *) &sinRemote, sin_len);
            free(messageTx);
        }
        pthread_mutex_unlock(&s_sendMutexVar);
    }
    
}


static void Send_shutDown(){
    pthread_cancel(SendThreadPID);

    pthread_join(SendThreadPID, NULL);
}

static void * receiveThread(){
    // struct sockaddr_in addr;

    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(PORT);
    // addr.sin_addr.s_addr = INADDR_ANY;
    // memset(&addr, 0, sizeof(addr));
    
    // socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

    // bind(socketDescriptor, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    char messageRx[MSG_MAX_LENGTH];
    while(1){
        struct sockaddr_in l_sinRemote;
        printf("Waiting for message\n");
        unsigned int sin_len = sizeof(sinRemote);
        memset(messageRx, 0, MSG_MAX_LENGTH);
        int bytesRx = recvfrom(socketDescriptor, messageRx, MSG_MAX_LENGTH, 0, (struct sockaddr *) &l_sinRemote, &sin_len);
        if(bytesRx > 0){
            printf("got it");
        }
        printf ("Received: %s\n", messageRx);

        // pthread_mutex_lock(&s_receiveMutexVar);
        // {
        //     char * message = malloc(MSG_MAX_LENGTH);
        //     strncpy(message, messageRx, terminateldx);
        //     message[terminateldx] = 0;
        //     List_append(receiveList, message);
        //     pthread_cond_signal(&s_receiveCondVar);
        // }
        // pthread_mutex_unlock(&s_receiveMutexVar);

    }

    return NULL;
}


static void Receive_shutDown(){
    pthread_cancel(ReceiverThreadPID);

    pthread_join(ReceiverThreadPID, NULL);
}

void systemInit(char* port0, struct sockaddr_in * sinp,char* peerPort){
    receiveList = List_create();
    sendList = List_create();

    printf("systemInit\n");

    printf("port0 = %d\n", atoi(port0));
    printf("port1 = %d\n", atoi(peerPort));

    char* port = port0;
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
 
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(port));
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    // struct sockaddr_in *sinp;
    // char *addr;
    // char buf[INET_ADDRSTRLEN];
    // sinp = (struct sockaddr_in *)aip->ai_addr;
    // addr = inet_ntop(AF_INET, &local.sin_addr, buf, INET_ADDRSTRLEN);
    // printf(" addr = %s, port = %d\n", addr?addr:"unknomw ", ntohs(local.sin_port));
    if (bind(socketDescriptor, (struct sockaddr *)&local, sizeof(struct sockaddr_in)) == -1){
        perror("bind failed");
        exit(1);
    }

    sinRemote.sin_family = AF_INET;
    sinRemote.sin_port = htons(atoi(peerPort));
    sinRemote.sin_addr.s_addr = sinp->sin_addr.s_addr;
    printf("inside: %s\n",  inet_ntoa(sinRemote.sin_addr));


    pthread_create(
        &keyboardThreadPID,
        NULL,
        keyboardThread,
        NULL
    );
    pthread_create(
        &ScreenThreadPID,
        NULL,
        Screenthread,
        NULL
    );
    pthread_create(
        &SendThreadPID,
        NULL,
        sendThread,
        NULL
    );
    pthread_create(
        &ReceiverThreadPID,
        NULL,
        receiveThread,
        NULL
    );
 
    // pthread_join(ReceiverThreadPID, NULL);
    pthread_join(keyboardThreadPID, NULL);
    // pthread_join(ScreenThreadPID, NULL);
    pthread_join(SendThreadPID, NULL);
 

}

void systemShutDown(){
    // Screen_shutDown();
    // Receive_shutDown();
    Send_shutDown();
    Keyboard_shutDown();

    // List_free(receiveList, freeNode);
    // List_free(sendList, freeNode);
}