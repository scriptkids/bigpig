/*使用select io复用
 */
#include "bp.h"
#define MAXLINE 1024
int main(void)
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];
    ssize_t n;
    fd_set rset, allset;
    char buf[MAXLINE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
/*
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == listenfd) {
        perror("socket");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8888);

    bind(listenfd, (struct sockaddr*) &servaddr, \
            sizeof(servaddr));
    
    listen(listenfd, 20);
 */  
    listenfd = tcp_listen(&servaddr); 
    maxfd = listenfd;
    maxi = -1;
    for(i = 0; i< FD_SETSIZE; i++)
        client[i] = -1;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for( ; ; ) {
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);

        if(FD_ISSET(listenfd, &rset)) {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd,(struct sockaddr*) &cliaddr, \
                    &clilen);
//#ifdef NOTDEF
            printf("new client: %s,port %d\n",inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),ntohs(cliaddr.sin_port));
//#endif
            for(i = 0; i< FD_SETSIZE; i++)
                if(client[i] < 0) {
                    client[i] = connfd;
                    break;
                }
            if(i == FD_SETSIZE)
                perror("too many clients");

            FD_SET(connfd, &allset);

            if(connfd > maxfd)
                maxfd = connfd;
            if(i > maxi)
                maxi = i;

            if(--nready <= 0)
                continue;
        }

        for(i = 0; i <= maxi; i++) {
            if( (sockfd = client[i]) < 0)
                continue;
            if(FD_ISSET(sockfd, &rset)) {
                if( (n = read(sockfd, buf, MAXLINE)) == 0) {
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


