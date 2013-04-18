#include "bp.h"
void err_log(char *buf)
{
    write(STDERR_FILENO, buf, strlen(buf));
}

#if DEBUG 
void debug(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "DEBUG:");
    vfprintf(stderr, format, args);
    va_end(args);
}
#else
void debug(const char* format, ...)
{
    ;
}
#endif

void notice(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "NOTICE:");
    vfprintf(stderr, format, args);
    va_end(args);
}

void access_log(FILE* fp, const char *format, ...)
{
    va_list args;
    va_start(args, format);
//    vfprintf(stderr, format, args);
    vfprintf(fp, format, args);
    fflush(fp);
    va_end(args);
}
