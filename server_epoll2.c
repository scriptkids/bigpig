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
void run_server(int listenfd) 
{
    char buf[1024];
    struct sockaddr_in cliaddr;
    socklen_t clilen;
    int nfds, connfd;
    int i, num;
    char *add, *now;
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
        struct epoll_event ev,events[MAX_EVENTS];
        int epfd = epoll_create(20);

        ev.events = EPOLLIN;
        ev.data.fd = listenfd; 
        if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
            perror("epoll_ctl: listenfd"); 
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
                    add = inet_ntoa(cliaddr.sin_addr);
                    now = get_time(); 
                    access_log(access_fp, "%s connected at time %s", add, now);
                } else {
                    if ((num = read(events[i].data.fd, buf, MAXLINE)) <= 0) {
                        NOTICE("pid is %d read error!\n", getpid());
                        /*not tested yet*/

                        // epoll_ctl() del
                        close(events[i].data.fd);
                    } else {
                        NOTICE("begin to handle request");
                        handle_request(events[i].data.fd, buf);
                        NOTICE("pid is %d handle_request DONE!!", getpid());
                    }
                }
            }
            NOTICE("end while");
        }//end while
    }
}
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
    mem_pool = create_pool(1024*1024);
    if (NULL == mem_pool) {
        NOTICE("no memory to malloc");
        exit(1);
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

    return 0;
}

