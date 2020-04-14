#include "eeprom.h"
#include <HardwareSerial.h>

#define PAGESIZE 16

byte eeprom_buf[512];

extern int board_type;

void i2c_eeprom_write_byte(int bus_addr, unsigned int addr, unsigned char data)
{
  int rdata = data, err;
  
  digitalWrite(EEPROM_WP, LOW);

  Wire.beginTransmission(bus_addr);
  Wire.write((int)(addr >> 8));
  Wire.write((int)(addr & 0xff));
  Wire.write(rdata);
  err = Wire.endTransmission();
  if (err) {
    Serial.print("I2C communication failed, error code = ");
    Serial.println(err);
  }

  digitalWrite(EEPROM_WP, HIGH);
}

void i2c_eeprom_write_buffer(int bus_addr, unsigned int addr, unsigned char *buf, int length)
{
  int offset = 0, write_length = 0, byte_addr = 0, i, err;

  digitalWrite(EEPROM_WP, LOW);

  while (length > 0) {
    addr += write_length;
    byte_addr = addr % PAGESIZE;
    write_length = min(PAGESIZE - byte_addr, length);
    Wire.beginTransmission(bus_addr);
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

unsigned char i2c_eeprom_read_byte(int bus_addr, unsigned int addr)
{
  unsigned char rdata = 0xee;
  int err;

  Wire.beginTransmission(bus_addr);
  Wire.write((int)(addr >> 8));
  Wire.write((int)(addr & 0xff));
  err = Wire.endTransmission();
  if (err) {
    Serial.print("I2C communication failed, error code = ");
    Serial.println(err);
    return 0;
  }

  Wire.requestFrom(bus_addr, 1);
  if (Wire.available()) 
    rdata = Wire.read();

  return rdata;
}

void i2c_eeprom_read_buffer(int bus_addr, unsigned int addr, unsigned char *buf, int length)
{
  int read_length = 0, offset = 0, byte_addr = 0, i, err;
  
  while(length > 0) { 
    offset += read_length;
    byte_addr = offset % PAGESIZE;
    read_length = min(PAGESIZE - byte_addr, length);
    Wire.beginTransmission(bus_addr);
    Wire.write((int)(addr >> 8));
    Wire.write((int)(addr & 0xff));
    err = Wire.endTransmission();
    if (err) {
      Serial.print("I2C communication failed, error code = ");
      Serial.println(err);
      return;
    }

   Wire.requestFrom(bus_addr, read_length);
    for(i = 0; i < read_length; i++) {
      if(Wire.available())
        buf[i + offset] = Wire.read();
    }
    length -= read_length;
    addr += read_length;
    delay(10);
  }
}

void i2c_eeprom_read_buffer2(int bus_addr, unsigned int addr, unsigned char *buf, int length)
{
  int read_length = 0, offset = 0, byte_addr = 0, i, err;
  
  while(length > 0) { 
    offset += read_length;
    byte_addr = offset % PAGESIZE;
    read_length = min(PAGESIZE - byte_addr, length);

    err = Wire.CombinedTrans(bus_addr, addr, 2, read_length);
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

int cmd_eeprom(int argc, char **argv)
{
  unsigned int addr, n, i;
  unsigned char c;
  int eeprom_addr;

  if (argc != 4) {
    Serial.println("Wrong arg");
    return -1;
  }

  if (board_type == 517) {
    eeprom_addr = EEPROM_ADDR_517;
  } else if (board_type == 503) {
    eeprom_addr = EEPROM_ADDR_503;
  }

  if (strcmp(argv[1], "read") == 0) {
    addr = strtoul(argv[2], NULL, 0);
    n = atoi(argv[3]);
    i2c_eeprom_read_buffer2(eeprom_addr, addr, eeprom_buf, n);
    for (i = 0; i < n; ++i) {
      Serial.print(eeprom_buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
    return 1;
  } else if (strcmp(argv[1], "write") == 0) {
    addr = strtoul(argv[2], NULL, 0);
    c = strtoul(argv[3], NULL, 0);
    i2c_eeprom_write_byte(eeprom_addr, addr, c);
    Serial.println("OK");
  } else {
    Serial.println("Wrong arg");
    return -1;
  }
}
