#include "bp.h"

char http_header[][22]={"Host:","Accept:",\
    "User-Agent:","Connection:","Accept-Language:",\
        "Accept-Encoding:", "Referer:", "Content-Type:", \
        "Content-Length:",};

int read_request(int fd, char *buf)
{
    int pos, num;
    pos = 0;
    while (1) {
        num = read(fd, buf+pos, MAXLINE*2);
        if( -1 == num) {
            if(errno != EAGAIN) {
                NOTICE("some error happen in recv loop");
                return -1;
            } else {
                // errno == EAGAIN  read over 
                NOTICE("receive over !");
                break;
            }
        } else if (0 == num){
            NOTICE("receive a EOF fd is %d num =0;\n", fd);
            return 0;
            //close(fd);
            //epoll return 0;
            //need fix epoll_ctl del
        } else if (num > 0) {
            pos = num;
        }
    }
   return 1; 

}

void handle_request(int fd, char buf[], struct pool_node *mem_pool)
{
    /*for DEBUG*/
    DEBUG("========the request is ===================================");
    DEBUG("%s",buf);
    DEBUG("===========request end ===================================");

    char *base_dir = BASE_DIR;
    char *file_name;
    struct http_request* request;
    
    request = (struct http_request*)get_memory(mem_pool, sizeof(struct http_request)); 
    request->pool = mem_pool;
    //request = (struct http_request*)malloc(sizeof(struct http_request));
    init_request(request); 

    analysis_request(buf, request);
    request->fd = fd;

    /*参考apache通过判断是否是cgi-bin文件夹中的内容来判断是否是cgi程序*/
    if ( NULL == strstr(request->uri, "/cgi-bin")) { // static file
        file_name = (char*)get_memory(mem_pool, (strlen(request->uri) + strlen(base_dir)+1) * sizeof(char));
        //file_name = (char *)malloc((strlen(request->uri) + strlen(base_dir))*sizeof(char));
        if ( !strcmp(request->uri, "/") ) {
            //DEBUG("the uri is / requset index page");
            sprintf(file_name, "%s%s", base_dir, "/index.html");
        } else {
            //DEBUG("uri is %s", request->uri);
            char *name = analysis_uri(request);
            sprintf(file_name, "%s%s", base_dir, name);
        }

        enum type f_type;
        char tmp[BUF_SIZE];
        //DEBUG("filename=%s", file_name);
        f_type = file_type(file_name);
        if (FILE_FOLD == f_type) { 
            //如果是文件夹，那么先查看index.html是否存在
            //存在则打开index.html不存在，打开文件夹。
            DEBUG("文件夹");
            strcpy(tmp, file_name);
            strcat(tmp, "/index.html");
            
            http200(tmp, request);
            if (FILE_OTHER == file_type(tmp)) { //index.html 文件不存在,返回文件夹内容
                DEBUG("文件夹下不存在index.html");
                do_folder(file_name, request);
            }else {//index.html 文件存在，打开index.html
                DEBUG("文件夹下存在index.html");
                do_static_file(tmp, request);
            } 
        }else if(FILE_REG == f_type) { 
            //请求是正常文件
            DEBUG("正常文件");
            http200(file_name, request);
            do_static_file(file_name, request);
            DEBUG("正常文件done");
           // return;
        }else if(FILE_OTHER == f_type) {
            DEBUG("file_name is %s",file_name);
            http404(request);
            sprintf(tmp, "%s/404.html", base_dir);
            do_static_file(tmp, request);
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
    if (0 == strcmp(tmp, "GET")) {
        request->method = GET;
    } else if (0 == strcmp(tmp, "POST")) {
        request->method = POST;
    } else {
        request->method = M_OTHER;
    }

    tmp = strtok(NULL, " ");
    request->uri = (char*)get_memory(request->pool, (strlen(tmp) + 1) * sizeof(char));
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
    request->query = (char*)get_memory(request->pool, BUF_SIZE);
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
                    request->UA = (char*)get_memory(request->pool, (strlen(tmp)+1)*sizeof(char));
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
                    request->refer = (char*)get_memory(request->pool, (strlen(tmp)+1)*sizeof(char));
                    //request->refer = (char*)malloc(strlen(tmp)*sizeof(char));
                    strcpy(request->refer, tmp);
                    break;
            case 7:
                    request->type = (char *)get_memory(request->pool, (strlen(tmp)+1)*sizeof(char));
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
   // show_info(request);
    return 0; 
}
char *analysis_uri(struct http_request* request) 
{
    char *ptr = request->uri;
    char *tmp, *result; 
    tmp = (char*)get_memory(request->pool, strlen(request->uri)+1); 
    //tmp = malloc(strlen(request->uri));
    result   = tmp;
    //DEBUG("%s", request->uri);
    while( *ptr != '?' && *ptr != '\0') {
        *(tmp++) = *(ptr++);
    }
    *tmp = '\0';
    if('?' == *ptr) {
        ptr++;
        request->query = (char*)get_memory(request->pool, strlen(request->uri)+1); 
        //request->query = malloc(strlen(request->uri));
        strcpy(request->query, ptr);
        DEBUG("request->query is %s",request->query);
    }
    return result;
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
/*need fix*/
void set_cgi_env(struct http_request* request) 
{
    if(request == NULL) {
        DEBUG("request is NULL");
        return ;
    }
    if (GET == request->method) {
        setenv("REQUEST_METHOD","GET", 1);
    } else if (POST == request->method) {
        setenv("REQUEST_METHOD","POST", 1);
    }
    setenv("REQUEST_TYPE", "text/html", 1);
    setenv("CONTENT_LENGTH", request->length, 1);    
}
/*only for DEBUG*/
void show_info(struct http_request* request)
{
    DEBUG("in the show_info");
    DEBUG("Method is %d ", request->method);
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
