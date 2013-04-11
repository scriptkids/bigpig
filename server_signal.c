#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
int main(void)
{
    void sig_chld(int signo);
    void str_echo(int clifd);
    struct sockaddr_in server_addr, client_addr;
    int serfd,clifd;
    int cli_len,childpid;
    serfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == serfd) {
        perror("socket");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8888);

    if(-1 == bind(serfd, (struct sockaddr*) &server_addr, \
                sizeof(server_addr))) {
        perror("bind");
        close(serfd);    
    }

    listen(serfd, 20);
    signal(SIGCHLD,sig_chld);
    for( ; ; ) {
        cli_len = sizeof(client_addr);
        clifd = accept(serfd, (struct sockaddr*) &client_addr, \
                &cli_len); 
        if(-1 == clifd) {
            perror("accept");
        }

        if((childpid = fork()) == 0) { //child
            close(serfd);
            str_echo(clifd);
            exit(0);
        }
        printf("childpid == %d\n",childpid);
        close(clifd);
    }

}
void str_echo(int clifd) 
{
    int n;
    char recv[1024];
    bzero(recv,sizeof(recv));
    while( (n = read(clifd,recv,1024)) > 0) {
       write(clifd, recv, n); 
       write(STDOUT_FILENO,recv,n);
    }


}
void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    pid = wait(&stat);
    pritnf("child %d terminated\n",pid);
    return;


}


