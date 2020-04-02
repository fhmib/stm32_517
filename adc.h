#ifndef _ADC_H_
#define _ADC_H_

#include <SPI.h>

#define SPI1_NSS_PIN PA4

int cmd_adc(int argc, char **argv);
unsigned int ReadChannelDigital(byte chanIdx);

#endif
