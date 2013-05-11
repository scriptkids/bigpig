#include "bp.h"
#define MAX_EVENTS 1000
enum process PROCESS_TYPE;
struct epoll_event ev,events[MAX_EVENTS];
int epfd;
void init_server()
{
    PROCESS_TYPE = MASTER;
    
    server_fp   =   fopen(SERVER_LOG, "a+");
    access_fp   =   fopen(ACCESS_LOG, "a+");
    if(NULL == server_fp) {
        NOTICE("open SERVER_LOG error!!");
        exit(1);
    }
    if(NULL == access_fp) {
        NOTICE("open ACCESS_LOG error!!");
        exit(1);
    }
}
void sig_chld(int signo)
{
    pid_t pid;
    int   stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        NOTICE("worker %d terminated\n", pid);
    }

    int ppid = fork();
    if (0 == ppid) {
        PROCESS_TYPE = WORKER;
        NOTICE("a worker process started %d", getpid());
    }
}
void sig_pipe(int signo)
{
    return;
}
void sig_int(int signo)
{
    NOTICE("receive SIG_INT !");
    return;
}
int set_noblock(int fd)
{
    int flags;
    /*GET fd flags*/
    if( (flags = fcntl(fd, F_GETFL, 0)) < 0) {
        NOTICE("F_GTEFL error");
        return 1;
    }

    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        NOTICE("F_SETFL error");
        return 1;
    }
    return 0;
}
void run_server(int listenfd)
{
    int i;
    pid_t pid;
    
    /*fork出worker进程*/
    for (i = 0; i < WORKER_NUM; i++) {
        if ((pid = fork()) == 0) {
            PROCESS_TYPE = WORKER;
            break;
        } else if (pid < 0) {
            NOTICE("fork error!");
            exit(1);
        }
    }

    if (MASTER == PROCESS_TYPE) {//master
        /*do something*/
        signal(SIGCHLD, sig_chld);
        //signal(SIGINT, sig_int);
        signal(SIGPIPE, sig_pipe);
        char *string = "this is the master !\n";
        write(STDOUT_FILENO, string, strlen(string));
        /*worker进程挂掉之后，master进程新fork出的worker进程退出循环*/ 
        while (1) {
            if (PROCESS_TYPE == WORKER)
                break;
            pause();
        }
    }

    if (WORKER == PROCESS_TYPE){//worker
        NOTICE("a worker started pid %d at %s at PORT %d", getpid(), get_time(), PORT);
        worker_process_cycle(listenfd);
    } 
}
void worker_process_cycle(int listenfd)
{
    int nfds, connfd ;
    int i, ret;
    struct sockaddr_in cliaddr;
    socklen_t clilen;

    epfd = epoll_create(20);
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        NOTICE("ADD listenfd to epoll fd error!");
    }
    while(1) {
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        DEBUG("pid is %d epoll_wait nfds == %d\n",getpid(), nfds);
        for (i = 0; i < nfds; i++) {
            if (events[i].data.fd == listenfd) {
                while (1) {
                    connfd = accept(listenfd, (struct sockaddr *)\
                            &cliaddr, &clilen);
                    if (connfd > 0) {
                        NOTICE("%s conected at time %s pid is %d connfd is %d", inet_ntoa(cliaddr.sin_addr), get_time(), getpid(), connfd);
                        if (-1 == set_noblock(connfd)) {
                            NOTICE("set_noblock error %d", connfd);
                            exit(1);
                        }
                        ev.events = EPOLLIN | EPOLLET;
                        ev.data.fd = connfd;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
                        access_log(access_fp, "%s connected at time %s", inet_ntoa(cliaddr.sin_addr), get_time());
                        break;
                    } else {
                        if (errno != EAGAIN) {
                            NOTICE("some error occur in accept loop");
                            break;
                        } else {
                            NOTICE("in accept errno == EAGAIN");
                            break;
                            //errno == EAGAIN
                        }
                    }
                }
            } else {
                struct pool_node *mem_pool;
                mem_pool = create_pool(1024*1024*1024);
                char *buf = get_memory(mem_pool, MAXLINE);

                ret = read_request(events[i].data.fd, buf);
                
                if (0 == ret) {  //服务器要求关闭连接
                    NOTICE("ret == 0");
                    close(events[i].data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                } else if (1 == ret){ //进入请求
                    NOTICE("handle_request ");
                    handle_request(events[i].data.fd, buf, mem_pool);
                    NOTICE("handle_request done");
                    
                    //关闭连接写这一部分; 
                    shutdown(events[i].data.fd, SHUT_WR);

                } else { //发生了错误。
                    close(events[i].data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    NOTICE("read error!");
                }
                destory_pool(mem_pool);
            }
        }
    }//end while
}

void free_server()
{
    fclose(server_fp);
    fclose(access_fp);
    //need fix
    //close(epfd) ;
}
int main(void)
{
    struct sockaddr_in servaddr;
    int listenfd;

    init_server();
    listenfd = tcp_listen(&servaddr);
    if (-1 == set_noblock(listenfd)) {
        NOTICE("set_noblock error!");
        exit(1);
    }
    access_log(server_fp, "server started at prort:%d\n", ntohs(servaddr.sin_port));
    run_server(listenfd);

    free_server();
    /*need to close epfd*/
    return 0;
}

