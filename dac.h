#ifndef _DAC_H_
#define _DAC_H_

#include <Wire.h>

#define DAC_ADDR 0xC

#define DAC_CHANNEL_A   0x0
#define DAC_CHANNEL_B   0x1
#define DAC_CHANNEL_C   0x2
#define DAC_CHANNEL_D   0x3
#define DAC_CHANNEL_ALL 0x4

int cmd_dac(int argc, char **argv);
int dac_write(byte addr, byte chan, unsigned int val);

#endif
