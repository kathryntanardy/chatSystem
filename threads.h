#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <pthread.h>
#include <string.h>

void freeNode(void *pItem);
void *keyboardThread();
void Keyboard_shutDown();
void Screen_shutDown();
void *screenThread();
void Send_shutDown();
void *sendThread();
void systemInit(char * myPort, struct sockaddr_in * sinp, char * remotePort, char* hostname);
void systemShutDown();

#endif 