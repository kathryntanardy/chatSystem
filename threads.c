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


#define MSG_MAX_LENGTH 512

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
        

        char* message = "test msg";
        while (1){
            
        }
        // //scanf("%s",&message);
        // read(STDIN_FILENO, message, MSG_MAX_LENGTH - 1);
        // size_t length = strlen(message);

        // if(length > 0 && message[length - 1] == '\n'){
        //     message[length - 1] = '\0';
        // }

        // // if(message[0] == '!' && length == 1){
        // //     Udp_shutdown();
        // //     exit(0);
        // // }

        // pthread_mutex_lock(&s_sendMutexVar);
        // {
        //     char * copiedMessage = malloc(length);
        //     strncpy(copiedMessage, message, length);
        //     copiedMessage[length - 1] = '\0';
        //     List_append(sendList, copiedMessage);
        //     pthread_cond_signal(&s_sendCondVar);
        // }
        // pthread_mutex_unlock(&s_sendMutexVar);
        
    
    }

    return NULL;
}

static void Keyboard_init(){
    pthread_create(
        &keyboardThreadPID,
        NULL,
        keyboardThread,
        NULL
    );
}

static void Keyboard_shutDown(){
    pthread_cancel(keyboardThreadPID);

    pthread_join(keyboardThreadPID, NULL);
}

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

static void * sendThread(int socketDescriptor, struct sockaddr_in sinRemote){
    unsigned int sin_len = sizeof(sinRemote);
    while(1){

        pthread_mutex_lock(&s_sendMutexVar);
        {
            if (List_count(sendList) == 0)
            pthread_cond_wait(&s_sendCondVar,&s_sendMutexVar);
            char * messageTx = List_remove(sendList);
            sendto (socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *) &sinRemote,sin_len);
            free(messageTx);
        }
        pthread_mutex_unlock(&s_sendMutexVar);
    }
    
}

static void Send_init(){
    pthread_create(
        &SendThreadPID,
        NULL,
        sendThread,
        NULL
    );
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
        // struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        // char messageRx[MSG_MAX_LENGTH];
        memset(messageRx, 0, MSG_MAX_LENGTH);
        int bytesRx = recvfrom(socketDescriptor, messageRx, MSG_MAX_LENGTH, 0, (struct sockaddr *) &sinRemote, &sin_len);
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

void systemInit(char* port0, struct addrinfo *alist){
    receiveList = List_create();
    sendList = List_create();

    char* port = port0;
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(port));
    local.sin_addr.s_addr = INADDR_ANY;

    // struct sockaddr_in *sinp;
    // char *addr;
    // char buf[INET_ADDRSTRLEN];
    // sinp = (struct sockaddr_in *)aip->ai_addr;
    // addr = inet_ntop(AF_INET, &local.sin_addr, buf, INET_ADDRSTRLEN);
    // printf(" addr = %s, port = %d\n", addr?addr:"unknow ", ntohs(local.sin_port));
    if (bind(socketDescriptor, (struct sockaddr *)&local, sizeof(struct sockaddr_in)) == -1){
        perror("bind");
        exit(1);
    }

    sinRemote = *(struct sockaddr_in *)alist->ai_addr;

    Keyboard_init();
    Receive_init();
    Screen_init();
    Send_init();

}

void systemShutDown(){
    Screen_shutDown();
    Receive_shutDown();
    Send_shutDown();
    Keyboard_shutDown();

    List_free(receiveList, freeNode);
    List_free(sendList, freeNode);
}