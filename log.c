#include "bp.h"
void err_log(char *buf)
{
    write(STDERR_FILENO, buf, strlen(buf));
}

void debug(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}
