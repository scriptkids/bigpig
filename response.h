#ifndef __RESPONSE__
#define __RESPONSE__

void header(int fd, char *buf);
void http200(char *file_name, struct http_request*);
void http404(struct http_request*);
void do_static_file(char *file_name, struct http_request* request);
void do_folder(char *dir_name, struct http_request* request);



#endif
