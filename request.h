#ifndef  __REQUEST__
#define  __REQUEST__

extern char http_header[][22];
enum type {
    FILE_REG,
    FILE_FOLD,
    FILE_OTHER,
};

enum http_version {
    V0,
    V1,
    VOTHER,
};

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
int  analysis_request(char buf[], struct http_request* request);
void show_info(struct http_request*);
void handle_request(int fd, char buf[]);
void header(int fd, char *buf);
void http200(struct http_request*);
void http404(struct http_request*);
void set_cgi_env(struct http_request*);
int  do_script_file(struct http_request*);
void init_request(struct http_request*);
void do_static_file(char *file_name, int fd);
void do_folder(char *dir_name, int fd);
char *analysis_uri(struct http_request* request);
#endif
