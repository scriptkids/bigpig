#ifndef  __bigpig__
#define  __bigpig__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>

#include <sys/select.h>
#include <sys/epoll.h>

#include <pthread.h>

#define PORT 8889
#define MAXLINE 1024
#endif
