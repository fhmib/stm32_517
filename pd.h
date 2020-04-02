#ifndef _PD_H_
#define _PD_H_

int cmd_pd(int argc, char **argv);
double get_pd(unsigned int pd_num);
int cmd_voa(int argc, char **argv);
double get_voa(unsigned int voa_num);
void set_voa(unsigned int voa_num, double power);

#endif
