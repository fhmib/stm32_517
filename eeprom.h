#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <Wire.h>
#include <stdint.h>

#define EEPROM_WP PB1

// 517, 364
#define EEPROM_ADDR_517 0x57
#define EEPROM_ADDR_364 0x57

// 503
#define EEPROM_ADDR_503 0x54

void i2c_eeprom_write_byte(int32_t bus_addr, uint32_t addr, unsigned char data);
void i2c_eeprom_write_buffer(int32_t bus_addr, uint32_t addr, unsigned char *buf, int32_t length);
unsigned char i2c_eeprom_read_byte(int bus_addr, uint32_t addr);
void i2c_eeprom_read_buffer(int32_t bus_addr, uint32_t addr, unsigned char *buf, int32_t length);
void i2c_eeprom_read_buffer2(int32_t bus_addr, uint32_t addr, unsigned char *buf, int32_t length);
int32_t cmd_eeprom(int32_t argc, char **argv);

#endif
