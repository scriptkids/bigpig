#ifndef  __REQUEST__
#define  __REQUEST__
enum http_version {
    V0,
    V1,
    OHTER,
};

extern char http_header[][22];

struct http_request {
    int     fd;
    char    method[16];
    char    uri[100];
    enum    http_version version;
    char    *UA;
    char    *cookie;
    char    *query;
    char    *refer;
    char    length[10];
    char    *type; 
    char    value[100]; //for post
    /*need fix*/
    char host[100];
    char accept[100];
};

struct http_request* get_info(char buf[]);

void show_info(struct http_request*);
void handle_request(int fd, char buf[]);
void header(int fd, char *buf);
void http200(int fd);
void http404(int fd);
void set_cgi_env(struct http_request*);
#endif
