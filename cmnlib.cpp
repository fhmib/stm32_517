#include "cmnlib.h"
#include "eeprom.h"

const char *SUPPORT_BOARD = "Supported boards: 503 517 364 573 419";
const char *TECH_ERROR = "SERIOUS ERROR: Invalid board type! Please inform the technician";
const char *ARG_ERROR = "Wrong arg";

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
  uint32_t offset, every_len;
  unsigned char buf[128];

  memset(buf, byte, 128);
  offset = 0;
  while (offset < count) {
    every_len = (offset + 128 < count) ? 128 : count - offset;
    /*
    Serial.print("Write ");
    Serial.print(every_len, DEC);
    Serial.print(" 0x");
    Serial.print(byte, HEX);
    Serial.print(" to 0x");
    Serial.println(addr + offset, HEX);
    */
    i2c_eeprom_write_buffer(bus_addr, addr + offset, buf, every_len);
    offset += every_len;
  }
}
