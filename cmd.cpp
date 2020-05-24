#include "cmd.h"
#include <HardwareSerial.h>

int32_t cmd_help(int32_t argc, char **argv)
{
  int32_t i;
  for (i=0;;i++)
  {
    Serial.print("cmd: ");
    Serial.println(cmdlist[i].cmd);
    Serial.print("     usage:   ");
    Serial.println(cmdlist[i].info); 
    Serial.print("     example: ");
    Serial.println(cmdlist[i].example); 

    if(strcmp(cmdlist[i].cmd,"help")==0) 
      break;     
  }
  return 0;
}

int32_t process_cmd(const char *buf)
{
  int32_t i, argc = 0;
  char *_argv[32];
  char **argv = &_argv[0];

  char* p = strtok((char*)buf," \t");
  while (p) {
    argv[argc++] = p;
    p = strtok(NULL, " \t");
  }

  if (argc == 0)
    return 1;

  for (i=0;;i++) {
    if (strcmp(cmdlist[i].cmd, argv[0]) == 0)
      return cmdlist[i].callfn(argc, argv); 
    if(strcmp(cmdlist[i].cmd, "help") == 0) 
      return -1;   
  }

  return 0;
}
