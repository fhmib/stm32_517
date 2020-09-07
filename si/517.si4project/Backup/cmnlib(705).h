#ifndef _CMNLIB_H_
#define _CMNLIB_H_

#include <stdint.h>

//typedef uint32_t uint32_t;
//typedef unsigned short uint16_t;
//typedef unsigned char uint8_t;

extern const char *TECH_ERROR;

uint32_t switch_endian(uint32_t i);
uint32_t get_32_from_eeprom(int32_t bus_addr, uint32_t addr);
void write_32_to_eeprom(int32_t bus_addr, uint32_t addr, uint32_t data);

#endif
