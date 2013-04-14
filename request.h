
struct http_request {
    char method[100];
    char path[100];
    int version;  // HTTP_1_0  or HTTP_1_1
    char client_ip[100];
    char client_port;
    char UA[1024];
};

struct http_request* get_info(char buf[]);

void show_info(char buf[]);
