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

int main(void)
{
    struct sockaddr_in servaddr, cliaddr;
    int listenfd, epfd,connfd,n,nfds;
    socklen_t clilen;
    struct epoll_event ev,events[MAX_EVENTS];
    char buf[1024];
    bzero(buf,sizeof(buf));
    
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

    listenfd = tcp_listen(&servaddr);
    /*need fix*/
    access_log(server_fp, "server started at prort:%d\n", ntohs(servaddr.sin_port));
    //NOTICE("server started at PORT %d\n",ntohs(servaddr.sin_port));
    epfd = epoll_create(20);
    ev.events = EPOLLIN;
    ev.data.fd = listenfd; 
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("epoll_ctl: listenfd"); 
    }

    while(1) { 
//        DEBUG("begin while");  
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        //printf("nfds == %d\n",nfds);
        NOTICE("epoll_wait nfds == %d\n", nfds);
        for (n = 0; n < nfds; n++) {
            if (events[n].data.fd == listenfd) {
                connfd = accept(listenfd, (struct sockaddr *)\
                        &cliaddr, &clilen);
                NOTICE("connfd is %d", connfd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = connfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev); 
                char *add;
                char *now;
                add = inet_ntoa(cliaddr.sin_addr);
                now = get_time(); 
                access_log(access_fp, "%s connected at time %s", add, now);
            } else {
                int num;
                if ((num = read(events[n].data.fd, buf, MAXLINE)) <= 0) {
                    NOTICE("read error!\n");
                    /*not tested yet*/
                    close(events[n].data.fd);
                } else {
                    //NOTICE("begin handle_request");
                    mem_pool = create_pool(1024*1024);
                    handle_request(events[n].data.fd, buf);
                    destory_pool(mem_pool);
                    close(events[n].data.fd);
                    DEBUG("handle_request DONE!!");
                }
            }
        }
    NOTICE("end while");
    }//end while
}
