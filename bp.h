#ifndef  __BIGPIG__ 
#define  __BIGPIG__

#define ISDEBUG 1
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
#include "pool.h"
#define PORT 80 
#define MAXLINE 10240
#define BUF_SIZE 10240 
#define ACCESS_LOG  "log/access_log"
#define SERVER_LOG  "log/server_log"
#define BASE_DIR    "www"
#define WORKER_NUM 2 
FILE *server_fp;
FILE *access_fp;
struct pool_node *mem_pool;
#endif

