#ifndef _CMD_H_
#define _CMD_H_

#include <stdint.h>

typedef struct _console_cmd
{
   char *cmd;
   int32_t (*callfn)(int32_t argc, char **argv);
   char *info;
   char *example;
}console_cmd;

extern console_cmd cmdlist[];

int32_t cmd_help(int32_t argc, char **argv);
int32_t process_cmd(const char *buf);

#endif
