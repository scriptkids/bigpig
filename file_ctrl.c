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

int file_content(FILE *fp, char *buf)
{
    int len = 0;
    printf("!!!!!!!!!!!!!!!!!!!!11111 %s",http_header[2]);    
    if( NULL == fp || NULL == buf) {
        err_log("file_content fp == NULL or buf == NULL ");
        return 0;
    }

    len = file_len(fp);
    while(!feof(fp)) {
        *(buf++) = getc(fp);
    }
   
    return len;
}
