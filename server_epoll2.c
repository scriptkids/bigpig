/*try to use epoll 
 * epfd = epoll_create();
 * ev.events = 
 * ev.data.fd = 
 * epoll_ctl();
 * epoll_wait();
 * 检查epoll_wait的返回值，并for循环进行判断。当fd = listenfd时accept。
 * 不等于listenfd时，进行doit操作，对events[n].data.fd
 */
#include "bp.h"

#define MAX_EVENTS 1000
void init_server()
{
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
void run_server(int listenfd) 
{
    char buf[MAXLINE];
    struct sockaddr_in cliaddr;
    struct epoll_event ev,events[MAX_EVENTS];
    socklen_t clilen;
    int nfds, connfd, epfd;
    int i, num;
    pid_t pid;

    bzero(buf,sizeof(buf));
    /*fork出worker进程*/
    for (i = 0; i < WORKER_NUM; i++) {
        if ((pid = fork()) == 0) {
            break;
        } else if (pid < 0) {
            perror("fork error!");
            exit(1);
        }
    }

    if (pid > 0) {//master 
        /*do something*/
        char *string = "this is the master !\n";
        write(STDOUT_FILENO, string, strlen(string)); 

    } else {//worker
        NOTICE("a worker started pid is %d", getpid());
        epfd = epoll_create(20);
        ev.events = EPOLLIN;
        ev.data.fd = listenfd; 
        if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
          //  perror("epoll_ctl: listenfd"); 
          NOTICE("ADD listenfd to epoll fd error!");
        }
        while(1) { 
            nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
            NOTICE("pid is %d epoll_wait nfds == %d\n",getpid(), nfds);
            for (i = 0; i < nfds; i++) {
                if (events[i].data.fd == listenfd) {
                    connfd = accept(listenfd, (struct sockaddr *)\
                            &cliaddr, &clilen);
                    NOTICE("pid is %d connfd is %d",getpid(), connfd);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = connfd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev); 
                    access_log(access_fp, "%s connected at time %s", inet_ntoa(cliaddr.sin_addr), get_time());
                } else {
                    bzero(buf, sizeof(buf));
                    if ((num = recv(events[i].data.fd, buf, MAXLINE, 0)) < 0) {
                        NOTICE("pid is %d read error! fd is %d\n", getpid(), events[i].data.fd);
                        //not tested yet
                        close(events[i].data.fd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                        //shutdown(events[i].data.fd, SHUT_RDWR);
                    } else if (0 == num) {//close
                        NOTICE("pid is %d read error! fd is %d num =0;\n", getpid(), events[i].data.fd);
                        //need fix..
                        close(events[i].data.fd);
                        shutdown(events[i].data.fd, SHUT_RDWR);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    } else {
                        NOTICE("begin to handle request read %d", num);
                        mem_pool = create_pool(102400);
                        handle_request(events[i].data.fd, buf);
                        destory_pool(mem_pool);
                        close(events[i].data.fd);
                        NOTICE("pid is %d handle_request DONE!!", getpid());
                    }
                }
            }
            NOTICE("end while");
        }//end while
    }
}
void free_server()
{
    fclose(server_fp);
    fclose(access_fp);
}
int main(void)
{
    struct sockaddr_in servaddr;
    int listenfd; 
    
    init_server();
   
    listenfd = tcp_listen(&servaddr);
    
    access_log(server_fp, "server started at prort:%d\n", ntohs(servaddr.sin_port));
    run_server(listenfd);

    free_server();
    /*need to close epfd*/
    return 0;
}

