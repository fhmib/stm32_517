#include "eeprom.h"
#include "cal.h"
#include "cmnlib.h"
#include <HardwareSerial.h>

#define PAGESIZE 16

byte eeprom_buf[512];
int32_t eeprom_addr;

extern int32_t board_type;

void i2c_eeprom_write_byte(int32_t bus_addr, uint32_t addr, unsigned char data)
{
  int32_t rdata = data, err;
  
  digitalWrite(EEPROM_WP, LOW);

  Wire.beginTransmission((int)bus_addr);
  Wire.write((int)(addr >> 8));
  Wire.write((int)(addr & 0xff));
  Wire.write((int)rdata);
  err = Wire.endTransmission();
  if (err) {
    Serial.print("I2C communication failed, error code = ");
    Serial.println(err);
  }
  delay(10);

  digitalWrite(EEPROM_WP, HIGH);
}

void i2c_eeprom_write_buffer(int32_t bus_addr, uint32_t addr, unsigned char *buf, int32_t length)
{
  int32_t offset = 0, write_length = 0, byte_addr = 0, i, err;

  digitalWrite(EEPROM_WP, LOW);

  while (length > 0) {
    addr += write_length;
    byte_addr = addr % PAGESIZE;
    write_length = min(PAGESIZE - byte_addr, length);
    Wire.beginTransmission((int)bus_addr);
    Wire.write((int)(addr >> 8));
    Wire.write((int)(addr & 0xff));
    for (i = 0; i < write_length; ++i) {
      Wire.write(buf[i + offset]);
    }
    err = Wire.endTransmission();
    if (err) {
      Serial.print("I2C communication failed, error code = ");
      Serial.println(err);
      digitalWrite(EEPROM_WP, HIGH);
      return;
    }
    length -= write_length;  
    offset += write_length;
    delay(10);
  }

  digitalWrite(EEPROM_WP, HIGH);
}

unsigned char i2c_eeprom_read_byte(int32_t bus_addr, uint32_t addr)
{
  unsigned char rdata = 0xee;
  int32_t err;

  Wire.beginTransmission((int)bus_addr);
  Wire.write((int)(addr >> 8));
  Wire.write((int)(addr & 0xff));
  err = Wire.endTransmission();
  if (err) {
    Serial.print("I2C communication failed, error code = ");
    Serial.println(err);
    return 0;
  }

  Wire.requestFrom((int)bus_addr, 1);
  if (Wire.available()) 
    rdata = Wire.read();
  delay(10);

  return rdata;
}

void i2c_eeprom_read_buffer(int32_t bus_addr, uint32_t addr, unsigned char *buf, int32_t length)
{
  int32_t read_length = 0, offset = 0, byte_addr = 0, i, err;
  
  while(length > 0) { 
    offset += read_length;
    byte_addr = offset % PAGESIZE;
    read_length = min(PAGESIZE - byte_addr, length);
    Wire.beginTransmission((int)bus_addr);
    Wire.write((int)(addr >> 8));
    Wire.write((int)(addr & 0xff));
    err = Wire.endTransmission();
    if (err) {
      Serial.print("I2C communication failed, error code = ");
      Serial.println(err);
      return;
    }

   Wire.requestFrom((int)bus_addr, read_length);
    for(i = 0; i < read_length; i++) {
      if(Wire.available())
        buf[i + offset] = Wire.read();
    }
    length -= read_length;
    addr += read_length;
    delay(10);
  }
}

void i2c_eeprom_read_buffer2(int32_t bus_addr, uint32_t addr, unsigned char *buf, int32_t length)
{
  int32_t read_length = 0, offset = 0, byte_addr = 0, i, err;
  
  while(length > 0) { 
    offset += read_length;
    byte_addr = offset % PAGESIZE;
    read_length = min(PAGESIZE - byte_addr, length);

    err = Wire.CombinedTrans((int)bus_addr, addr, 2, read_length);
    if (err) {
      Serial.print("Error occured while i2c combined transmission, error = ");
      Serial.println(err);
      return;
    }

    for(i = 0; i < read_length; i++) {
      if(Wire.available())
        buf[i + offset] = Wire.read();
    }
    length -= read_length;
    addr += read_length;
    delay(10);
  }
}

int32_t cmd_eeprom(int32_t argc, char **argv)
{
  uint32_t addr, n, i;
  unsigned char c;

  if (argc == 4 && !strcmp(argv[1], "read")) {
    addr = strtoul(argv[2], NULL, 0);
    n = strtoul(argv[3], NULL, 0);
    i2c_eeprom_read_buffer2(eeprom_addr, addr, eeprom_buf, n);
    Serial.print("Read from address: 0x");
    Serial.print(addr, HEX);
    for (i = 0; i < n; ++i) {
      if (i % 16 == 0) {
        Serial.println("");
        Serial.print("0x");
        Serial.print(i, HEX);
        Serial.print(": ");
      }
      Serial.print(eeprom_buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
    return 1;
  } else if (argc == 4 && !strcmp(argv[1], "write")) {
    addr = strtoul(argv[2], NULL, 0);
    c = strtoul(argv[3], NULL, 0);
    i2c_eeprom_write_byte(eeprom_addr, addr, c);
    Serial.println("OK");
  } else if (argc == 2 && !strcmp(argv[1], "dump")) {
    if (board_type == 364) {
      n = eeprom_length_for_364;
    } else if (board_type == 419) {
      n = eeprom_length_for_419;
    } else if (board_type == 517) {
      n = eeprom_length_for_517;
    } else if (board_type == 573) {
      n = eeprom_length_for_573;
    } else if (board_type == 503) {
      n = eeprom_length_for_503;
    } else {
      Serial.println(TECH_ERROR);
      return -1;
    }
    for (i = 0; i < n; ++i) {
      c = i2c_eeprom_read_byte(eeprom_addr, i);
      Serial.print(i,DEC); Serial.print(","); Serial.print(i,HEX); Serial.print(",");
      Serial.print(c, DEC); Serial.print(","); Serial.println(c, HEX);
      if (board_type == 503) {
        if (i == 0x4) i = 0x100 - 1;
        else if (i == TAG_EXT_MODEL_503 + 20 - 1) i = 0x1000 - 1;
      } else {
        if (i == 0x4F) i = 0x100 - 1;
        else if (i == 0x104) i = 0x1000 - 1;
      }
      delay(10);
    }
  } else {
    Serial.println(ARG_ERROR);
    return -1;
  }
}
