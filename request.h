#ifndef  __REQUEST__
#define  __REQUEST__

#define CONTENT_HTML  "Content-Type: text/html"
#define CONTENT_CSS   "Content-Type: text/css"
#define CONTENT_JPG   "Content-Type: image/jpeg"
#define CONTENT_PNG   "Content-Type: image/png"
#define CONTENT_GIF   "Content-Type: image/gif"
#define CONTENT_JS    "Content-Type: text/javascript"

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
void handle_request1(int fd);
void header(int fd, char *buf);
void http200(struct http_request*);
void http404(struct http_request*);
void set_cgi_env(struct http_request*);
int  do_script_file(struct http_request*);
void init_request(struct http_request*);
void do_static_file(char *file_name, struct http_request* request);
void do_folder(char *dir_name, struct http_request* request);
char *analysis_uri(struct http_request* request);
int  read_request(int fd, char buf[]);
#endif
