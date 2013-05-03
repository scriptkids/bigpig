#include "bp.h"
void err_log(char *buf)
{
    write(STDERR_FILENO, buf, strlen(buf));
}

#if ISDEBUG 
/*
inline void debug(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "DEBUG:");
    vfprintf(stderr, format, args);
    va_end(args);
}*/
inline void debug(const char *filename, const char *func, const int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "DEBUG:%d %s [%s] [%d]line\t",getpid(), filename, func, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}
#else
void debug(const char *filename, const char *func, int line, const char* format, ...)
{
    ;
}
#endif

inline void notice(const char *filename, const char *func, const int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "NOTICE:%d %s [%s] [%d]line\t",getpid(), filename, func, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}
/*
void NOTICE(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "NOTICE:");
    vfprintf(stderr, format, args);
    va_end(args);
}
*/
void access_log(FILE* fp, const char *format, ...)
{
    va_list args;
    va_start(args, format);
//    vfprintf(stderr, format, args);
    vfprintf(fp, format, args);
    fflush(fp);
    va_end(args);
}

char *get_time()
{
    time_t now_t;

    time(&now_t);
    return ctime(&now_t);
}
