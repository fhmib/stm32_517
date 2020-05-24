#include "cmnlib.h"
#include "eeprom.h"

const char *TECH_ERROR = "SERIOUS ERROR: Invalid board type! Please inform the technician";

uint32_t switch_endian(uint32_t i)
{
  return (((i>>24)&0xff) | ((i>>8)&0xff00) | ((i<<8)&0xff0000) | ((i<<24)&0xff000000));
}

uint32_t get_32_from_eeprom(int32_t bus_addr, uint32_t addr)
{
  uint32_t val;
  i2c_eeprom_read_buffer(bus_addr, addr, (unsigned char*)&val, 4);
  return switch_endian(val);
}

void write_32_to_eeprom(int32_t bus_addr, uint32_t addr, uint32_t data)
{
  uint32_t val = switch_endian(data);
  i2c_eeprom_write_buffer(bus_addr, addr, (unsigned char*)&val, 4);
}

void write_byte_to_eeprom(int32_t bus_addr, uint32_t addr, unsigned char byte, uint32_t count)
{
  for (int i = 0; i < count; ++i) {
    i2c_eeprom_write_byte(bus_addr, addr + i, byte);
    delay(10);
  }
}
