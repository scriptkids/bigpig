#include "bp.h"
void do_static_file(char *file_name, struct http_request* request) {
    char *buf, *tmp;
    FILE *fp;
    int  len, fd;

    DEBUG("in do_static_file");
    fd = request->fd;
    fp = fopen(file_name, "rb");
    if(NULL == fp) {
        NOTICE("do_static_file , fopen , fp is NULL %s\n",file_name);
    }
    len = file_len(fp);
    buf = get_memory(request->pool, len*sizeof(char));

    file_content(fp, buf);
    tmp = (char*)get_memory(request->pool, (len+30)*sizeof(char));

    sprintf(tmp, "Content-Length: %d\r\n\r\n", len);
    header(fd, tmp);
    int left = len, num;

    while(left > 0) {
        num = write(fd, buf, left); 
        NOTICE("num = %d", num);
        if (num == -1) {
            if (errno != EAGAIN ) {
                NOTICE("error happen in write loop");
                break;
            } else {
                NOTICE("EAGAIN in write loop");
                break;
            }
        } else {
            left = left - num;
            buf += num;
        }
    }

    //header(fd, buf);
    fclose(fp);
} 
void do_folder(char *dir_name, struct http_request* request) {
    DIR             *dir;
    struct dirent   *dir_content;
    char            *buf;
    char            *result;
    int             len,fd;
    
    fd = request->fd;
    len = strlen(dir_name);
/*
    if (*(dir_name+len) != '/') {
        *(dir_name+len) = '/';
        *(dir_name+len+1) = '\0';
    }
*/
    dir = opendir(dir_name);
    if(NULL == dir) {
        NOTICE("open dir %s failed\n", dir_name);
    }

    buf     = (char*)get_memory(request->pool, BUF_SIZE*sizeof(char));
    result  = (char*)get_memory(request->pool, BUF_SIZE*sizeof(char));
    *result = '\0';
    //dir_name += strlen(BASE_DIR);
    while ((dir_content = readdir(dir)) != NULL) {
        sprintf(buf, "%s%s", dir_name, dir_content->d_name); 
        NOTICE("file_type buf is %s file_name is %s%s", buf, dir_name+strlen(BASE_DIR), dir_content->d_name); 
        if (FILE_FOLD == file_type(buf)) {
            sprintf(buf, "<p><img src=\"/icons/folder.gif\"/>  <a href=\"%s/%s\">%s</a></p>\r\n", \
                    dir_name+strlen(BASE_DIR), dir_content->d_name, dir_content->d_name);
        } else {
            sprintf(buf, "<p><img src=\"/icons/file.gif\"/>  <a href=\"%s/%s\">%s</a></p>\r\n", \
                    dir_name+strlen(BASE_DIR), dir_content->d_name, dir_content->d_name);
        }
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
    
    file_name = (char*)get_memory(request->pool, strlen(request->uri) + 1); 
    //file_name = (char*)malloc(strlen(request->uri));
    request->uri = analysis_uri(request);
    sprintf(file_name, "%s%s", base_dir, request->uri);
/*
    if(request->method == POST) {
        char buf[MAXLINE];
        read_request(request->fd, buf); 
        request->query = buf;
        printf("!!!%s\n", buf);
    }

*/

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
        char path[100];
        strcpy(path, "/home/zhangjiyang/myProject/bigpig/");
        strcat(path, file_name);

        DEBUG("p=%s f=%s", path, file_name );
        execl(path, file_name, (char*)0);
       // execl("/usr/bin/python2", "python2", file_name, (char*)0); 
        exit(0);
    }else if (pid >0) { //father
        close(pipe_fd[0]);
        
        //http200(request);
        write(pipe_fd[1], request->query, strlen(request->query));
        DEBUG(request->query);
        close(pipe_fd[1]); 
//        close(request->fd);
    }
    return 0;
}
void header(int fd, char *buf)
{
    int left, num;
    left = strlen(buf);
   
    while(left > 0) {
        num = write(fd, buf, left); 
        if (-1 == num) {
            if (errno == EAGAIN) {
               NOTICE("EAGAIN in write loop"); 
               break;
            } else {
                NOTICE("error happen in write loop");
                break;
            }
        } else {
            left = left - num; 
            buf += num;
        }
    //    DEBUG("num = %d", num);
    }
}

void http200(char *file_name, struct http_request* request)
{
    char *dot;
    
    header(request->fd, "HTTP/1.1 200 OK\r\n");
   
    dot = strstr(file_name, ".");
    if (NULL != dot) {
        if ( !strcasecmp(dot, ".jpg") || !strcasecmp(dot, ".jpeg") ) {
            header(request->fd, CONTENT_JPG);
        } else if (!strcasecmp(dot, ".png")) {
            header(request->fd, CONTENT_PNG);
        } else if (!strcasecmp(dot, ".css")) {
            header(request->fd, CONTENT_CSS);
        } else if (!strcasecmp(dot, ".gif")) {
            header(request->fd, CONTENT_GIF);
        } else if (!strcasecmp(dot, "js")) {
            header(request->fd, CONTENT_JS);
        }  
        else {
            header(request->fd, CONTENT_HTML);
        }
    } else {
        header(request->fd, CONTENT_HTML);
    }
    
    //DEBUG("end http200");
}
void http404(struct http_request* request)
{
    access_log(access_fp, "%s %s %d 404\n", request->method, request->uri, request->version);
    header(request->fd, "HTTP/1.1 404 NOT FOUND\r\n");
    header(request->fd, "Content-Type:text/html\r\n"); 
}
