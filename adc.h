#ifndef _ADC_H_
#define _ADC_H_

#include <SPI.h>
#include <Wire.h>

// KTA7953
#define SPI1_NSS_PIN PA4

int cmd_adc(int argc, char **argv);
unsigned int ReadChannelDigital(byte chanIdx);

// KTA7828
#define KTA7828_ADDR 0x4B

int cmd_adc_7828(int argc, char **argv);
unsigned int get_adc_7828(byte chanIdx);

#endif
