#include "bp.h"

int file_len(FILE *fp)
{
    int len = 0, pos; 
    if(NULL == fp) {
        err_log("in file_len fp == null");
        return 0;
    }
    

    pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, pos, SEEK_SET);
    return len;
}
/*
int file_len(char *filename)
{
    struct stat temp;

    stat(filename, &temp);
    return temp.st_size;
}
*/
int file_content(FILE *fp, char *buf)
{
    int len = 0;
    if( NULL == fp || NULL == buf) {
        err_log("file_content fp == NULL or buf == NULL ");
        return 0;
    }

    len = file_len(fp);
    while(!feof(fp)) {
        *(buf++) = getc(fp);
    }
//    *buf='\0'; 
    return len;
}

enum type file_type(char *file_name)
{// return 1 is dir return 0 is regular file
    struct stat buf;
    if(lstat(file_name, &buf) < 0) {
        notice("%s lstat error", file_name);
        return FILE_OTHER;
    }

    if(S_ISREG(buf.st_mode)) {
        notice("%s is reg file\n", file_name);
        return FILE_REG;
    }

    if(S_ISDIR(buf.st_mode)) {
        notice("%s is dir file\n", file_name);
        return FILE_FOLD;
    }
    /*need to test if here is a soft link*/
    return FILE_OTHER;
}

char *process_dir(char *dir_name)
{
    DIR*            dir;
    struct dirent*  dir_content;
    char            *buf;
    char            *result;
   
    dir = opendir(dir_name);
    if(NULL == dir) {
        notice("open dir %s failed \n", dir_name);
        return NULL;
    }
    buf = (char*)malloc(BUFF_SIZE*sizeof(char));
    result = (char*)malloc(BUFF_SIZE*sizeof(char));
    *result = '\0';
    while((dir_content = readdir(dir)) != NULL) {
        sprintf(buf, "<p><a href=\"%s/%s\">%s</a></p>\r\n", dir_name, dir_content->d_name, dir_content->d_name);
        fprintf(stderr, "<a href=\"%s/%s\">%s</a>\n", dir_name, dir_content->d_name, dir_content->d_name);
        strcat(result, buf);
    }
    fprintf(stderr, "hello %s\n", result);
    return result;
}



