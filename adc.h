#ifndef _ADC_H_
#define _ADC_H_

#include <SPI.h>
#include <Wire.h>
#include <stdint.h>

// KTA7953
#define SPI1_NSS_PIN PA4

int32_t cmd_adc(int32_t argc, char **argv);
uint32_t ReadChannelDigital(byte chanIdx);

// KTA7828
#define ADC_ADDR_503 0x4B

int32_t cmd_adc_7828(int32_t argc, char **argv);
uint32_t get_adc_7828(byte chanIdx);

#endif
