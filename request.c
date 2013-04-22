#include "bp.h"

char http_header[][22]={"Host:","Accept:",\
    "User-Agent:","Connection:","Accept-Language:",\
        "Accept-Encoding:", "Referer:", "Content-Type:", \
        "Content-Length:",};

int analysis_request(char buf[], struct http_request* request)
{
    char req[MAXLINE];
    char *tmp;
    int i;

    //读取buf中的第一行，请求头
    sscanf(buf, "%[^\n]", req);

    tmp = strtok(req, " ");
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
        //a dirty workround need fix
        //请求的value位于请求头后。
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
char *analysis_uri(struct http_request* request) 
{
    char *ptr = request->uri;
    char *tmp, *result; 
    
    tmp = malloc(strlen(request->uri));
    result   = tmp;
    while( *ptr != '?' && *ptr != '\0') {
        *(tmp++) = *(ptr++);
    }
    *tmp = '\0';
    if('?' == *ptr) {
        ptr++;
        request->query = malloc(strlen(request->uri));
        strcpy(request->query, ptr);
        debug("request->query is %s\n",request->query);
    }
    return result;
}
/*only for debug*/
void show_info(struct http_request* request)
{
    debug("in the show_info");
    debug("Method is %s\n", request->method);
    debug("Uri is %s\n", request->uri);
    debug("http_version is %d\n",request->version);
    debug("User-Agent is %s\n",request->UA);
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
    debug("========the request is ===================================\n");
    debug("%s\n",buf);
    debug("===========request end ===================================\n");

    char *base_dir = BASE_DIR;
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

    if( !strcmp(request->method, "GET") ) {
        //FILE *fp;
        file_name = (char *)malloc((strlen(request->uri) + strlen(base_dir))*sizeof(char));
        if( !strcmp(request->uri, "/") ) {
            sprintf(file_name, "%s%s", base_dir, "/index.html");
        }else{
            char *name = analysis_uri(request);
            sprintf(file_name, "%s%s", base_dir, name);
        }

        enum type f_type;
        char tmp[BUFF_SIZE];

        f_type = file_type(file_name);
        if(FILE_FOLD == f_type) { 
            //如果是文件夹，那么先查看index.html是否存在
            //存在则打开index.html不存在，打开文件夹。
            strcpy(tmp, file_name);
            strcat(file_name, "/index.html");
            if(FILE_OTHER == file_type(file_name)) { //index.html 文件不存在,返回文件夹内容
                http200(request);
                do_folder(tmp, fd);
            }else {
                http200(request);
                do_static_file(file_name, fd);
            } 
        }else if(FILE_REG == f_type) {
            http200(request);
            do_static_file(file_name, fd);      
        }else if(FILE_OTHER == f_type) {
            http404(request);
            sprintf(tmp, "%s/404.html", base_dir);
            do_static_file(tmp, fd);
        }

    }else if( !strcmp(request->method, "POST") ) {  //for script
        set_cgi_env(request);
        do_script_file(request); 
    }
    /*for debug*/
    show_info(request);
    /*need to do access_log  foramt is GET / HTTP/1.1  200 */

}
void do_static_file(char *file_name, int fd) {
   char *buf, *tmp;
   FILE *fp;
   int  len;

   fp = fopen(file_name, "r");
   if(NULL == fp) {
       notice("do_static_file , fopen , fp is NULL %s\n",file_name);
   }
   len = file_len(fp);
   buf = (char*)malloc(len*sizeof(char));
   
   file_content(fp, buf);
   tmp = (char*)malloc((len+30)*sizeof(char));
   notice("lenth=%d\n", len); 
   sprintf(tmp, "Content-Length: %d\r\n\r\n", len);

   header(fd, tmp);
   header(fd, buf);
} 
void do_folder(char *dir_name, int fd) {
    DIR             *dir;
    struct dirent   *dir_content;
    char            *buf;
    char            *result;
    int             len;
    
    dir = opendir(dir_name);
    if(NULL == dir) {
        notice("open dir %s failed\n", dir_name);
    }

    buf     = (char*)malloc(BUFF_SIZE*sizeof(char));
    result  = (char*)malloc(BUFF_SIZE*sizeof(char));
    *result = '\0';
    dir_name+= strlen(BASE_DIR);
    while((dir_content = readdir(dir)) != NULL) {
        sprintf(buf, "<p><a href=\"%s/%s\">%s</a></p>\r\n", dir_name, dir_content->d_name, dir_content->d_name);
        strcat(result, buf);  
    }

    len = strlen(result);
    sprintf(buf, "Content-Length: %d\r\n\r\n", len);
    
    header(fd, buf);
    header(fd, result); 
}
int do_script_file(struct http_request* request)
{
    char *file_name;
    char *base_dir = BASE_DIR;
    pid_t pid;
    int pipe_fd[2];
    
    file_name = (char*)malloc(strlen(request->uri));

    sprintf(file_name, "%s%s", base_dir, request->uri);
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
        close(pipe_fd[0]);
        
        http200(request);
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
    //debug(buf);
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
