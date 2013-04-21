#ifndef  __BIGPIG__ 
#define  __BIGPIG__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/select.h>
#include <sys/epoll.h>

#include <time.h>

#include <pthread.h>

#include "log.h"
#include "file_ctrl.h"
#include "request.h"
#include "tcp_listen.h"
#define DEBUG 1
#define PORT 8889
#define MAXLINE 1024
#define BUFF_SIZE 1024
#define ACCESS_LOG  "log/access_log"
#define SERVER_LOG  "log/server_log"

FILE *server_fp;
FILE *access_fp;
#endif

