#ifndef __LOG__
#define __LOG__
void err_log(char *buf);
void debug(const char* format, ...);
void access_log(FILE* fp, const char* format, ...);
void notice(const char* format, ...);
#endif
