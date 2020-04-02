#include "cmnlib.h"
#include "eeprom.h"

unsigned int switch_endian(unsigned int i)
{
  return (((i>>24)&0xff)|((i>>8)&0xff00)|((i<<8)&0xff0000)|((i<<24)&0xff000000));
}

unsigned int get_32_from_eeprom(int bus_addr, unsigned int addr)
{
  unsigned int val;
  i2c_eeprom_read_buffer(bus_addr, addr, (unsigned char*)&val, 4);
  return switch_endian(val);
}
