#include "bp.h"

char http_header[][22]={"Host:","Accept:",\
    "User-Agent:","Connection:","Accept-Language:",\
        "Accept-Encoding:", "Referer:", "Content-Type:", \
        "Content-Length:",};

void handle_request(int fd, char buf[])
{
    /*for DEBUG*/
    DEBUG("========the request is ===================================");
    DEBUG("%s",buf);
    DEBUG("===========request end ===================================");

    char *base_dir = BASE_DIR;
    char *file_name;
    struct http_request* request;
   
    request = (struct http_request*)get_memory(mem_pool, sizeof(struct http_request)); 
    //request = (struct http_request*)malloc(sizeof(struct http_request));
    init_request(request); 

    analysis_request(buf, request);
    request->fd = fd;

    /*参考apache通过判断是否是cgi-bin文件夹中的内容来判断是否是cgi程序*/
    if ( NULL == strstr(request->uri, "/cgi-bin")) { // static file
        file_name = (char*)get_memory(mem_pool, (strlen(request->uri) + strlen(base_dir)+1) * sizeof(char));
        //file_name = (char *)malloc((strlen(request->uri) + strlen(base_dir))*sizeof(char));
        if ( !strcmp(request->uri, "/") ) {
            DEBUG("the uri is / requset index page");
            sprintf(file_name, "%s%s", base_dir, "/index.html");
        } else {
            DEBUG("uri is %s", request->uri);
            char *name = analysis_uri(request);
            sprintf(file_name, "%s%s", base_dir, name);
        }

        enum type f_type;
        char tmp[BUF_SIZE];
        DEBUG("filename=%s", file_name);
        f_type = file_type(file_name);
        if (FILE_FOLD == f_type) { 
            //如果是文件夹，那么先查看index.html是否存在
            //存在则打开index.html不存在，打开文件夹。
            DEBUG("文件夹");
            strcpy(tmp, file_name);
            strcat(tmp, "/index.html");
            if (FILE_OTHER == file_type(tmp)) { //index.html 文件不存在,返回文件夹内容
                DEBUG("文件夹下不存在index.html");
                http200(request);
                do_folder(file_name, fd);
            }else {//index.html 文件存在，打开index.html
                DEBUG("文件夹下存在index.html");
                http200(request);
                do_static_file(tmp, fd);
            } 
        }else if(FILE_REG == f_type) { 
            //请求是正常文件
            DEBUG("正常文件");
            http200(request);
            DEBUG("http200done\n");
            do_static_file(file_name, fd);
            DEBUG("正常文件done");
           // return;
        }else if(FILE_OTHER == f_type) {
            DEBUG("file_name is %s",file_name);
            http404(request);
            sprintf(tmp, "%s/404.html", base_dir);
            do_static_file(tmp, fd);
            //return;
        }

    }else if( NULL != strstr(request->uri, "/cgi-bin") ) {  //for script
        set_cgi_env(request);
        do_script_file(request); 
    }

}
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
    request->uri = (char*)get_memory(mem_pool, (strlen(tmp) + 1) * sizeof(char));
    //request->uri = (char *)malloc(strlen(tmp) * sizeof(char));
    strcpy(request->uri, tmp);
    DEBUG("request->uri is %s\n", request->uri);    

    tmp = strtok(NULL, " ");

    if(0 == strcmp(tmp, "HTPP/1.1"))
        request->version = V1;
    else 
        request->version = V0;

    tmp = strtok(buf, "\r\n");
    if(tmp == NULL) {
        perror("tmp = NULL");
    }
    /*need fix*/
    request->query = (char*)get_memory(mem_pool, BUF_SIZE);
    //request->query = malloc(BUF_SIZE);
    while( (tmp = strtok(NULL, "\r\n")) != NULL) {
        for(i=0; i<9; i++) {
            if(strstr(tmp, http_header[i]) != NULL) {
                DEBUG("%s %s",tmp, http_header[i]);
                tmp += strlen(http_header[i]);
                break;
            }
        }
        //a dirty workround need fix
        //如果是post方法的话请求的query位于请求头后。
        if (i >= 9) {
            strcpy(request->query, tmp); 
            DEBUG(request->query);
        }
        switch(i) {
            case 0: 
                    strcpy(request->host, tmp); break;
            case 1:
                    strcpy(request->accept, tmp); break;
            case 2: 
                    request->UA = (char*)get_memory(mem_pool, (strlen(tmp)+1)*sizeof(char));
                    //request->UA = (char*)malloc(strlen(tmp) * sizeof(char));
                    strcpy(request->UA, tmp);
                    break;
            case 3: //connetion
                    break;
            case 4: //acept-language
                    break;
            case 5: //Accept-encoding
                    break;
            case 6: 
                    request->refer = (char*)get_memory(mem_pool, (strlen(tmp)+1)*sizeof(char));
                    //request->refer = (char*)malloc(strlen(tmp)*sizeof(char));
                    strcpy(request->refer, tmp);
                    break;
            case 7:
                    request->type = (char *)get_memory(mem_pool, (strlen(tmp)+1)*sizeof(char));
                    //request->type = (char*)malloc(strlen(tmp)*sizeof(char));
                    strcpy(request->type, tmp);
                    break;
            case 8:
                    strcpy(request->length, tmp);
                    break;
            default:
                    break;
        }
    }
    show_info(request);
    return 0; 
}
char *analysis_uri(struct http_request* request) 
{
    char *ptr = request->uri;
    char *tmp, *result; 
    tmp = (char*)get_memory(mem_pool, strlen(request->uri)+1); 
    //tmp = malloc(strlen(request->uri));
    result   = tmp;
    DEBUG("%s", request->uri);
    while( *ptr != '?' && *ptr != '\0') {
        *(tmp++) = *(ptr++);
    }
    *tmp = '\0';
    if('?' == *ptr) {
        ptr++;
        request->query = (char*)get_memory(mem_pool, strlen(request->uri)+1); 
        //request->query = malloc(strlen(request->uri));
        strcpy(request->query, ptr);
        DEBUG("request->query is %s",request->query);
    }
    return result;
}
/*only for DEBUG*/
void show_info(struct http_request* request)
{
    DEBUG("in the show_info");
    DEBUG("Method is %s ", request->method);
    DEBUG("Uri is %s ", request->uri);
    DEBUG("http_version is %d ",request->version);
    DEBUG("User-Agent is %s ",request->UA);
    DEBUG("cookie is %s ",request->cookie);
    DEBUG("query is %s ",request->query);
    DEBUG("refer is %s ",request->refer);
    DEBUG("length is %s ",request->length);
    DEBUG("type is %s ",request->type);
    DEBUG("accept_type is %s ",request->accept);
    DEBUG("host is %s ",request->host);
    DEBUG("http->query is %s ",request->query); 
    DEBUG("show_info end ");
}
void init_request(struct http_request* request)
{
    if(NULL == request) {
        DEBUG("init_request: request is NULL");
        return;
    }
    request->UA     = NULL;
    request->cookie = NULL;
    request->query  = NULL;
    request->refer  = NULL;
    request->type   = NULL;
    request->uri    = NULL;

}
void do_static_file(char *file_name, int fd) {
   char *buf, *tmp;
   FILE *fp;
   int  len;

   DEBUG("in do_static_file");
   fp = fopen(file_name, "r");
   if(NULL == fp) {
       NOTICE("do_static_file , fopen , fp is NULL %s\n",file_name);
   }
   len = file_len(fp);
   buf = get_memory(mem_pool, len*sizeof(char));
   //buf = (char*)malloc(len*sizeof(char));
   
   file_content(fp, buf);
   tmp = (char*)get_memory(mem_pool, (len+30)*sizeof(char));
   // tmp = (char*)malloc((len+30)*sizeof(char));
   
   NOTICE("lenth=%d\n", len); 
  
   sprintf(tmp, "Content-Length: %d\r\n\r\n", len);

   header(fd, tmp);
   header(fd, buf);
   fclose(fp);
} 
void do_folder(char *dir_name, int fd) {
    DIR             *dir;
    struct dirent   *dir_content;
    char            *buf;
    char            *result;
    int             len;
    
    DEBUG("in do_folder"); 
    dir = opendir(dir_name);
    if(NULL == dir) {
        NOTICE("open dir %s failed\n", dir_name);
    }

    buf     = (char*)get_memory(mem_pool, BUF_SIZE*sizeof(char));
    result  = (char*)get_memory(mem_pool, BUF_SIZE*sizeof(char));
    //buf     = (char*)malloc(BUF_SIZE*sizeof(char));
    //result  = (char*)malloc(BUF_SIZE*sizeof(char));
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
    
    file_name = (char*)get_memory(mem_pool, strlen(request->uri) + 1); 
    //file_name = (char*)malloc(strlen(request->uri));
    request->uri = analysis_uri(request);
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
        write(pipe_fd[1], request->query, strlen(request->query));
        DEBUG(request->query);
        close(pipe_fd[1]); 
    }
    return 0;
}
/*need fix*/
void set_cgi_env(struct http_request* request) 
{
    if(request == NULL) {
        DEBUG("request is NULL");
        return ;
    }

    setenv("REQUEST_METHOD","POST", 1);
    setenv("REQUEST_TYPE", "text/html", 1);
    setenv("CONTENT_LENGTH", request->length, 1);    
}
void header(int fd, char *buf)
{
    DEBUG("begin header %d", fd);
    int left, num;

    left = strlen(buf);
   
    while(left > 0) {
        num = write(fd, buf, left); 
        left = left - num; 
    }
    DEBUG("end header");
    /*for DEBUG*/
    //DEBUG(buf);
}
void http200(struct http_request* request)
{
    //access_log(access_fp, "%s %s %d 200\n", request->method, request->uri, request->version);
    DEBUG("begind http200");
    header(request->fd, "HTTP/1.1 200 OK\r\n");
    header(request->fd, "Content-Type:text/html\r\n");
    DEBUG("end http200");
}
void http404(struct http_request* request)
{
    access_log(access_fp, "%s %s %d 404\n", request->method, request->uri, request->version);
    header(request->fd, "HTTP/1.1 404 NOT FOUND\r\n");
    header(request->fd, "Content-Type:text/html\r\n"); 
}
