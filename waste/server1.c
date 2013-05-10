/*使用阻塞IO单进程的web服务器,版本1
 * 1.socket
 * 2.bind
 * 3,listen
 * 4,accept
 * */
#include "bp.h"
int main(void)
{
    void str_echo(int clifd);
    struct sockaddr_in server_addr, client_addr;
    int serfd, clifd;
    int client_len = sizeof(client_addr);
/*    serfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == serfd) {
        perror("socket");
        close(serfd);
    }

    server_addr.sin_family      =       AF_INET;
    server_addr.sin_addr.s_addr =       htonl(INADDR_ANY);
    server_addr.sin_port        =       htons(8888);
    
    if(-1 == bind(serfd, (struct sockaddr*) &server_addr,\
                sizeof(server_addr))) {
        perror("bind");
        close(serfd);
    }

    if(-1 == listen(serfd, 20)) {
        perror("listen");
        close(serfd);
    }
*/
    serfd = tcp_listen(&server_addr);

    clifd = accept(serfd, (struct sockaddr*) &client_addr,\
            &client_len);
    if(-1 == clifd) {
        perror("accept");
        close(clifd);
    }
    
    str_echo(clifd);
    close(clifd);
    close(serfd);
    return 0;
}
void str_echo(int clifd)
{
    ssize_t n;
    char buf[1024];
again:
    while((n = read(clifd,buf,1024)) > 0)
        write(clifd,buf,1024);

    if(n < 0 && errno == EINTR)
        goto again;
    else if(n < 0)
        perror("str_echo");

}
