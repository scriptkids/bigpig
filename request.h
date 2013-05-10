#ifndef  __REQUEST__
#define  __REQUEST__

#define CONTENT_HTML  "Content-Type: text/html\r\n"
#define CONTENT_CSS   "Content-Type: text/css\r\n"
#define CONTENT_JPG   "Content-Type: image/jpeg\r\n"
#define CONTENT_PNG   "Content-Type: image/png\r\n"
#define CONTENT_GIF   "Content-Type: image/gif\r\n"
#define CONTENT_JS    "Content-Type: text/javascript\r\n"

extern char http_header[][22];
enum type {
    FILE_REG,
    FILE_FOLD,
    FILE_OTHER,
};

enum http_method {
    GET,
    POST,
    M_OTHER,
};
enum http_version {
    V0,
    V1,
    VOTHER,
};

struct http_request {
    int     fd;
    struct  pool_node *pool;
    enum    http_method  method;
    char    *uri;
    enum    http_version version;
    char    *UA;
    char    *cookie;
    char    *query;
    char    *refer;
    char    length[10];
    char    *type; 
//    char    value[100]; //for post
    /*need fix*/
    char host[200];
    char accept[200];
};

struct http_request* get_info(char buf[]);
int  analysis_request(char buf[], struct http_request* request);
void show_info(struct http_request*);
void handle_request(int fd, char buf[], struct pool_node *mem_pool);
void set_cgi_env(struct http_request*);
int  do_script_file(struct http_request*);
void init_request(struct http_request*);
char *analysis_uri(struct http_request* request);
int  read_request(int fd, char buf[]);
#endif
