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
/*
int file_len(FILE *fp)
{
    int len = 0;
    if(NULL == fp) {
        return 0;
    }
    int pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, pos, SEEK_SET);

    return len;

}
int file_content(FILE *fp, char *f_content)
{
    int len = 0;
    if(fp == NULL || f_content == NULL) {
        return 0;
    }
    len = file_len(fp);
    while(!feof(fp)) {
        *(f_content++) = getc(fp);
    }
    return len ;
}
*/
/*
struct http_request {
    char method[100];
    char path[100];
    int version;  // HTTP_1_0  or HTTP_1_1
    char client_ip[100];
    char client_port;
    char UA[1024];
};
struct http_request* get_info(char buf[])
{
    struct http_request* request ;
    request = (struct http_request*)malloc(sizeof(struct http_request));
    char *tmp;
    printf("time to strtok\n");
    tmp = strtok(buf," ");
    strcpy(request->method, tmp);
    //printf("tmp = %s request_method = %s\n",tmp,request->method);
    tmp = strtok(NULL, " ");
    strcpy(request->path, tmp);
    tmp = strtok(NULL, " ");
    if(0 == strcmp(tmp, "HTPP/1.1"))
        request->version = 1;
    else 
        request->version = 0;

   return request; 


}
void show_info(char buf[])
{
    
    struct http_request* request;
    request = get_info(buf);
    if(NULL != request) {
        printf("Method is %s\n",request->method);
        printf("Path is %s\n",request->path);
        printf("HTTP version is 1.%d\n",request->version);
    }

    printf("show info");
}
*/
int main(void)
{
    struct sockaddr_in servaddr, cliaddr;
    int listenfd, epfd,connfd,n,nfds,clilen;
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
                    //printf("read == 0\n");
                } else {
                   // printf("%s\n",buf);
                    /* dosomething analysis buf*/
                    //show_info(events[n].data.fd, buf);
                    handle_request(events[n].data.fd, buf);
                }
            }
        }
    }
}
