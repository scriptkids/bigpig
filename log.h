#ifndef __LOG__
#define __LOG__
void err_log(char *buf);
inline void debug(char *filename, int line, const char* format, ...);
//inline void debug(const char* format, ...);
void access_log(FILE* fp, const char* format, ...);
void notice(const char* format, ...);
char *get_time();
#define DEBUG(fmt,...) \
    debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif
