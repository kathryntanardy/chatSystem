#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <pthread.h>
#include <string.h>

static void freeNode(void *pItem);
static void *keyboardThread();
static void Keyboard_shutDown();
static void Screen_shutDown();
static void *screenThread();
static void Send_shutDown();
static void *sendThread();
void systemInit(char * myPort, struct sockaddr_in * sinp, char * remotePort, char* hostname);
void systemShutDown();

#endif 