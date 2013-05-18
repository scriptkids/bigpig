#include "bp.h"

int tcp_listen(struct sockaddr_in *servaddr)
{
    int serfd,reuse;
    serfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == serfd) {
        perror("tcp_listen, socket");
    }

    servaddr->sin_family        = AF_INET;
    servaddr->sin_addr.s_addr   = htonl(INADDR_ANY);
    servaddr->sin_port          = htons(PORT);

    reuse = 1;
    if(setsockopt(serfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <0 ) {
        perror("setopt");
        close(serfd);
    }

    if(-1 == bind(serfd, (struct sockaddr*) servaddr,\
                sizeof(*servaddr))) {
        perror("tcp_listen bind");
        close(serfd);
    }

    if(-1 == listen(serfd, 1024)) {
        perror("tcp_listen listen");
        close(serfd);
    }
    return serfd;
}

