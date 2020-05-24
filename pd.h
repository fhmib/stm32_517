#ifndef _PD_H_
#define _PD_H_

#include <stdint.h>

int32_t cmd_pd(int32_t argc, char **argv);
double get_pd(uint32_t pd_num);
int32_t cmd_voa(int32_t argc, char **argv);
double get_voa(uint32_t voa_num);
void set_voa(uint32_t voa_num, double power);
int32_t cmd_switch(int32_t argc, char **argv);

#endif
