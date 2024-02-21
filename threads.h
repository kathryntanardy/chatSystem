#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <pthread.h>
#include <string.h>

static void freeNode();
static void* keyboardThread();
static void Keyboard_shutDown();
static void Screen_shutDown();
static void * Screenthread();
static void Send_shutDown();
void systemInit();
void systemShutDown();

#endif 