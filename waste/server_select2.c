#include "bp.h"
int main(void)
{
    struct sockaddr_in servaddr, cliaddr;
    fd_set rset,allset;
    int listenfd, maxfd;
    int maxi, i, connfd, sockfd;
    char buf[1024];
    socklen_t clilen;
    ssize_t n;
    int nready;
    int client[FD_SETSIZE];
    listenfd = tcp_listen(&servaddr);
    
    maxfd = listenfd;
    maxi = -1;
    /*初始化client数组*/
    for(i = 0; i < FD_SETSIZE; i++)
       client[i] = -1; 

    FD_ZERO(&allset);  /*initial allset for select*/
    FD_SET(listenfd, &allset); /*bind for select*/
    
    for( ; ; ) {
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        /*检查listenfd，如果已经就绪就开始进行accept*/
        if(FD_ISSET(listenfd, &rset)) {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr*)&cliaddr,\
                   &clilen );
            printf("new client:%d\n",ntohs(cliaddr.sin_port));

            /*找到第一个没有使用的client,将connfd放入其中*/
            for(i = 0; i < FD_SETSIZE; i++)
                if(client[i] < 0) {
                    client[i] = connfd; 
                    break;
                }

            if(i == FD_SETSIZE)
                perror("Too many clients");
            /*设置connfd监听状态*/
            FD_SET(connfd, &allset);
            if(connfd > maxfd)
                maxfd = connfd;
            if(i > maxi)
                maxi = i;

            if(--nready <= 0)
                continue;
        }
        printf("maxi == %d\n",maxi);
        /*逐个检查client[i]看是否可以读取*/
        for(i = 0; i <= maxi; i++) {
            if((sockfd = client[i]) < 0)
                continue;
            if(FD_ISSET(sockfd, &rset)) {
                if((n = read(sockfd, buf, MAXLINE)) == 0) {
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } else
                    write(sockfd, buf, n);
                if(--nready <= 0)
                    break;
            }
        
        }
    
    }
}
