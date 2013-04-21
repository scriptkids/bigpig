#include "bp.h"

char http_header[][22]={"Host:","Accept:",\
    "User-Agent:","Connection:","Accept-Language:",\
        "Accept-Encoding:", "Referer:", "Content-Type:", \
        "Content-Length:",};

int analysis_request(char buf[], struct http_request* request)
{
    char f_line[100];
    char *tmp;
    int i;

    sscanf(buf, "%[^\n]", f_line);

    tmp = strtok(f_line, " ");
    strcpy(request->method, tmp);

    tmp = strtok(NULL, " ");
    strcpy(request->uri, tmp);

    tmp = strtok(NULL, " ");

    if(0 == strcmp(tmp, "HTPP/1.1"))
        request->version = V1;
    else 
        request->version = V0;

    tmp = strtok(buf, "\r\n");
    if(tmp == NULL) {
        perror("tmp = NULL");
    }
    while( (tmp = strtok(NULL, "\r\n")) != NULL) {
        for(i=0; i<9; i++) {
            if(strstr(tmp, http_header[i]) != NULL) {
                tmp += strlen(http_header[i]);
                break;
            }
        }
        /*a dirty workround need fix*/
       strcpy(request->value, tmp); 
        switch(i) {
            case 0: 
                    strcpy(request->host, tmp); break;
            case 1:
                    strcpy(request->accept, tmp); break;
            case 2: 
                    request->UA = (char*)malloc(strlen(tmp) * sizeof(char));
                    strcpy(request->UA, tmp);
                    break;
            case 3: //connetion
                    break;
            case 4: //acept-language
                    break;
            case 5: //Accept-encoding
                    break;
            case 6: 
                    request->refer = (char*)malloc(strlen(tmp)*sizeof(char));
                    strcpy(request->refer, tmp);
                    break;
            case 7:
                    request->type = (char*)malloc(strlen(tmp)*sizeof(char));
                    strcpy(request->type, tmp);
                    break;
            case 8:
                    strcpy(request->length, tmp);
                    break;
            default:
                    break;
        }
    }

    return 0; 
}
/*only for debug*/
void show_info(struct http_request* request)
{
    debug("in the show_info");
    debug("Method is %s\n", request->method);
    debug("Uri is %s\n", request->uri);
    debug("http_version is %d\n",request->version);
    debug("User-Agent is %s\n",request->UA);
    //if(request->cookie != NULL)
    debug("cookie is %s\n",request->cookie);
    debug("query is %s\n",request->query);
    debug("refer is %s\n",request->refer);
    debug("length is %s\n",request->length);
    debug("type is %s\n",request->type);
    debug("accept_type is %s\n",request->accept);
    debug("host is %s\n",request->host);
    debug("http->value is %s\n",request->value); 
    debug("show_info end\n");
}
void init_request(struct http_request* request)
{
    if(NULL == request) {
        debug("init_request: request is NULL\n");
        return;
    }
    request->UA     = NULL;
    request->cookie = NULL;
    request->query  = NULL;
    request->refer  = NULL;
    request->type   = NULL;

}
void handle_request(int fd, char buf[])
{
    /*for debug*/
    
    debug("into handle_request \n");
    debug("========the request is ===================================\n");
    debug("%s\n",buf);
    debug("===========request end ===================================\n");
    
    char *base_dir = "www";
    char *file_name;
    struct http_request* request;
    request = (struct http_request*)malloc(sizeof(struct http_request));
    init_request(request); 
    
    analysis_request(buf, request);
    /*no more needed*/
    if(NULL == request ) {
        debug("request is NULL\n");
        return;
    }

    request->fd = fd;
    /*for debug*/
    show_info(request);
    
    if( !strcmp(request->method, "GET") ) {
        FILE *fp;
        file_name = (char *)malloc(1000*sizeof(char));
        if( !strcmp(request->uri, "/") ) {
            sprintf(file_name, "%s%s", base_dir, "/index.html");
        }else{
            sprintf(file_name, "%s%s", base_dir, request->uri);
        }
        char *content; 
        if( is_dir(file_name) == 1 ) {
            
            
            
            content = process_dir(file_name);
            http200(request);
            char head_buf[100]; 
           // fprintf(stderr, "strlen=%s\n", content);
            sprintf(head_buf, "Content-Length:%d\r\n\r\n", strlen(content));
            header(fd, head_buf);
            header(fd, "\r\n");
            header(fd, content);
            return;    
        } 
        fp = fopen(file_name,"rb");
        if(NULL == fp) {
            http404(request);
            strcpy(file_name, "www/404.html");
            /*open the 404 page again*/
            fp = fopen(file_name,"rb");
            if(NULL == fp) {
                perror("There must be something wrong with 404page");
                exit(0);
            }
        } else {
            http200(request);
        }

        char *buf;
        int len ;
        len = file_len(fp);
        buf = (char*)malloc(len * (sizeof(char)) );

        sprintf(buf, "Content-Length:%d\r\n\r\n",len);
        header(fd,buf);

        len = file_content(fp, buf);
        fclose(fp);
        header(fd, buf);
    }else if( !strcmp(request->method, "POST") ) {  //for script
        set_cgi_env(request);
        script_file(request); 
    }
    /*need to do access_log GET / HTTP/1.1  200 */

}

int script_file(struct http_request* request)
{
    char *file_name;
    char *base_dir = "www/";
    file_name = (char*)malloc(1000*sizeof(char));

    sprintf(file_name, "%s%s", base_dir, request->uri);
    pid_t pid;
    int pipe_fd[2];
    /*need fix*/
    if( pipe(pipe_fd) < 0) {
        perror("pipe");
    }

    if( (pid=fork()) < 0) {
        perror("fork");
        return -1;
    }
    if(pid == 0) { //child
        close(pipe_fd[1]);
        /*is this needed?*/
        //sleep(1);
        dup2(pipe_fd[0], STDIN_FILENO);
        dup2(request->fd, STDOUT_FILENO);
        execl("/usr/bin/python2", "python2", file_name, (char*)0); 
        exit(0);
    }else if (pid >0) { //father
       // printf("this is the father");
        //int len = atoi(request->length);
        close(pipe_fd[0]);
        //char tmp[100];
        
        http200(request);
        //sprintf(tmp,"Content-Length: %s\r\n\r\n",request->length); 
        write(pipe_fd[1], request->value, strlen(request->value));
        write(STDERR_FILENO, request->value, strlen(request->value));
        close(pipe_fd[1]); 
    }
    return 0;
}
/*need fix*/
void set_cgi_env(struct http_request* request) 
{
    if(request == NULL) {
        debug("request is NULL\n");
        return ;
    }

    setenv("REQUEST_METHOD","POST", 1);
    setenv("REQUEST_TYPE", "text/html", 1);
    setenv("CONTENT_LENGTH", request->length, 1);    
}
void header(int fd, char *buf)
{
    write(fd, buf, strlen(buf) );
    /*for debug*/
    debug(buf);
//    write(STDERR_FILENO, buf, strlen(buf));
}
void http200(struct http_request* request)
{
    access_log(access_fp, "%s %s %d 200\n", request->method, request->uri, request->version);
    header(request->fd, "HTTP/1.1 200 OK\r\n");
    header(request->fd, "Content-Type:text/html\r\n");
}
void http404(struct http_request* request)
{
    access_log(access_fp, "%s %s %d 404\n", request->method, request->uri, request->version);
    header(request->fd, "HTTP/1.1 404 NOT FOUND\r\n");
    header(request->fd, "Content-Type:text/html\r\n");
}
