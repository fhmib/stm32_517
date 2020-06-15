#ifndef _CMNLIB_H_
#define _CMNLIB_H_

#include <stdint.h>

#define MY_SUB(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))

//typedef uint32_t uint32_t;
//typedef unsigned short uint16_t;
//typedef unsigned char uint8_t;

extern const char *SUPPORT_BOARD;
extern const char *TECH_ERROR;
extern const char *ARG_ERROR;

uint32_t switch_endian(uint32_t i);
uint32_t get_32_from_eeprom(int32_t bus_addr, uint32_t addr);
void write_32_to_eeprom(int32_t bus_addr, uint32_t addr, uint32_t data);
void write_byte_to_eeprom(int32_t bus_addr, uint32_t addr, unsigned char byte, uint32_t count);

#endif
