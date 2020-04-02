#ifndef _CMD_H_
#define _CMD_H_

typedef struct _console_cmd
{
   char *cmd;
   int (*callfn)(int argc, char** argv);
   char *info;
   char *example;
}console_cmd;

extern console_cmd cmdlist[];

int cmd_help(int argc, char **argv);
int process_cmd(const char *buf);

#endif
