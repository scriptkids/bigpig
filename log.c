#include "bp.h"
void err_log(char *buf)
{
    write(STDOUT_FILENO, buf, strlen(buf));

}
