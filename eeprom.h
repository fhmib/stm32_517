#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <Wire.h>

#define EEPROM_WP PB1
#define EEPROM_ADDR 0x57

void i2c_eeprom_write_byte(int bus_addr, unsigned int addr, unsigned char data);
unsigned char i2c_eeprom_read_byte(int bus_addr, unsigned int addr);
void i2c_eeprom_write_buffer(int bus_addr, unsigned int addr, unsigned char *buf, int length);
void i2c_eeprom_read_buffer(int bus_addr, unsigned int addr, unsigned char *buf, int length);
int cmd_eeprom(int argc, char **argv);

#endif
