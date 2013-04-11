/*
 * #include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
*/
#include "bp.h"
int main(void)
{
    void  str_cli(FILE*,int);
    struct sockaddr_in server_addr,client_addr;
    int serfd;
    serfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == serfd) {
        perror("socket");
        close(serfd);
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    connect(serfd, (struct sockaddr *) &server_addr, \
            sizeof(server_addr));

    str_cli(stdin,serfd);
    return 0;

}
void str_cli(FILE *fp, int serfd)
{
    char sendline[1024],recvline[1024];
    bzero(recvline,sizeof(recvline));
//    while(fgets(sendline, 1024, fp) != NULL) {
    while(1) { 
        write(serfd, "123\n", strlen(sendline));
        if(read(serfd, recvline, 1024) == 0)
            perror("read");
        fputs(recvline,stdout);
    }
}
