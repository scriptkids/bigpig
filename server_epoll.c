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

#define MAX_EVENTS 10

int main(void)
{
    struct sockaddr_in servaddr, cliaddr;
    int listenfd, epfd,connfd,n,nfds;
    socklen_t clilen;
    struct epoll_event ev,events[MAX_EVENTS];
    char buf[1024];
    bzero(buf,sizeof(buf));
    
    listenfd = tcp_listen(&servaddr);

    epfd = epoll_create(20);

    ev.events = EPOLLIN;
    ev.data.fd = listenfd; 
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("epoll_ctl: listenfd"); 
    }

    for(;;) {
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        //printf("nfds == %d\n",nfds);
        debug("epoll_wait nfds == %d\n", nfds);
        for(n = 0; n < nfds; n++) {
            if(events[n].data.fd == listenfd ) {
                connfd = accept(listenfd, (struct sockaddr *)\
                        &cliaddr, &clilen);
          //      printf("connfd == %d\n",connfd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = connfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev); 
            } 
            else {
                int num;
                if( (num = read(events[n].data.fd, buf, MAXLINE)) == 0) {
                    perror("1234read");
                    /*not tested yet*/
                    close(events[n].data.fd);
                } else {
                    handle_request(events[n].data.fd, buf);
                }
            }
        }
    }
}
