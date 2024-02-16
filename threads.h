#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <pthread.h>
#include <string.h>


void* keyboardThread();
void Keyboard_init();
void Keyboard_shutDown();
void Screen_init();
void Screen_shutDown();
void Send_init();
void Send_shutDown();
void Receive_init();
void Receive_shutDown();
void * receiveThread();
void systemInit();

#endif 