#include "bp.h"
struct http_request* get_info(char buf[])
{
    struct http_request* request ;
    request = (struct http_request*)malloc(sizeof(struct http_request));
    char *tmp;
    printf("time to strtok\n");
    tmp = strtok(buf," ");
    strcpy(request->method, tmp);
    //printf("tmp = %s request_method = %s\n",tmp,request->method);
    tmp = strtok(NULL, " ");
    strcpy(request->path, tmp);
    tmp = strtok(NULL, " ");
    if(0 == strcmp(tmp, "HTPP/1.1"))
        request->version = 1;
    else 
        request->version = 0;

   return request; 
}
void show_info(char buf[])
{
    struct http_request* request;
    request = get_info(buf);
    if(NULL != request) {
        printf("Method is %s\n",request->method);
        printf("Path is %s\n",request->path);
        printf("HTTP version is 1.%d\n",request->version);
    }

    printf("show info");
}

