#ifndef __FILE_CTRL__
#define __FILE_CTRL_

int file_len(FILE *fp);
int file_content(FILE *fp, char *buf);
enum type file_type(char *file_name);
char *process_dir(char *dir_name);
#endif
