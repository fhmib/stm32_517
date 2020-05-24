#ifndef _DAC_H_
#define _DAC_H_

#include <Wire.h>
#include <stdint.h>

// for 517
#define DAC_ADDR_517 0xC

// for 364
#define DAC_ADDR_364_1 0xC
#define DAC_ADDR_364_2 0xF

// Channel index definition for KTA5625
#define DAC_CHANNEL_A   0x0
#define DAC_CHANNEL_B   0x1
#define DAC_CHANNEL_C   0x2
#define DAC_CHANNEL_D   0x3
#define DAC_CHANNEL_ALL 0x4

int32_t cmd_dac(int32_t argc, char **argv);
int32_t dac_write(byte addr, byte chan, uint32_t val);
int32_t dac_read(byte addr, byte *buf);
int32_t dac_read2(byte addr, byte chan, uint32_t &val);

#endif
