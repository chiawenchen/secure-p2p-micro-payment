#ifndef _THREADPOOLCLASS_H_
#define _THREADPOOLCLASS_H_
#include <pthread.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>
#include "const.h"
#include "user_userList_class.h"
#include "clientClass.h"
#include <queue>
using namespace std;

class ThreadPool{
private:
    int threadNum;
  //  int freeThreads;//*
    pthread_mutex_t mutex; 
    pthread_cond_t condition_var;
    pthread_t* threads;
    queue<Client> waitingQueue;
    static void* work_static(void* thread);
    void work(void* thread);
    void workFunctions(Client aClient);
    UserList userMap;
public:
    ThreadPool();
    void connect(Client &c);
    void createThreads();
};

#endif
