#include "dac.h"
#include "cmnlib.h"

extern int board_type;

int32_t cmd_dac(int32_t argc, char **argv)
{
  byte channel, addr;
  uint32_t val;

  if (argc == 4 && !strcmp(argv[2], "write")) {
    if (board_type == 517 || board_type == 573) {
      if (!strcmp(argv[1], "1")) {
        channel = DAC_CHANNEL_A;
      } else if (!strcmp(argv[1], "2")) {
        channel = DAC_CHANNEL_B;
      } else if (!strcmp(argv[1], "3")) {
        channel = DAC_CHANNEL_C;
      } else if (!strcmp(argv[1], "4")) {
        channel = DAC_CHANNEL_D;
      } else {
        Serial.println("Wrong channel");
        return -1;
      }
      addr = DAC_ADDR_517;
    } else if (board_type == 364 || board_type == 419) {
      if (!strcmp(argv[1], "1")) {
        channel = DAC_CHANNEL_A;
        addr = DAC_ADDR_364_1;
      } else if (!strcmp(argv[1], "2")) {
        channel = DAC_CHANNEL_B;
        addr = DAC_ADDR_364_1;
      } else if (!strcmp(argv[1], "3")) {
        channel = DAC_CHANNEL_C;
        addr = DAC_ADDR_364_1;
      } else if (!strcmp(argv[1], "4")) {
        channel = DAC_CHANNEL_D;
        addr = DAC_ADDR_364_1;
      } else if (!strcmp(argv[1], "5")) {
        channel = DAC_CHANNEL_A;
        addr = DAC_ADDR_364_2;
      } else if (!strcmp(argv[1], "6")) {
        channel = DAC_CHANNEL_B;
        addr = DAC_ADDR_364_2;
      } else if (!strcmp(argv[1], "7")) {
        channel = DAC_CHANNEL_C;
        addr = DAC_ADDR_364_2;
      } else if (!strcmp(argv[1], "8")) {
        channel = DAC_CHANNEL_D;
        addr = DAC_ADDR_364_2;
      } else {
        Serial.println("Wrong channel");
        return -1;
      }
    } else {
      Serial.println("SERIOUS ERROR: Invalid board type! Please inform the technician");
      return -1;
    }

    val = strtoul(argv[3], NULL, 0);
    if (val >> 12) {
      Serial.println("Dac value overflow");
      return -1;
    }

    dac_write(addr, channel, val);
    Serial.println("OK");

    return 0;
  } else if (argc == 2 && !strcmp(argv[1], "read")) {
    byte buf[3];

    if (board_type == 517 || board_type == 573) {
      dac_read(DAC_ADDR_517, buf);
      Serial.print("Read from dac: 0x");
      Serial.print(buf[0], HEX);
      Serial.print(" 0x");
      Serial.print(buf[1], HEX);
      Serial.print(" 0x");
      Serial.println(buf[2], HEX);
    } else if (board_type == 364 || board_type == 419) {
      dac_read(DAC_ADDR_364_1, buf);
      Serial.print("Read from dac 0xC: 0x");
      Serial.print(buf[0], HEX);
      Serial.print(" 0x");
      Serial.print(buf[1], HEX);
      Serial.print(" 0x");
      Serial.println(buf[2], HEX);
      dac_read(DAC_ADDR_364_2, buf);
      Serial.print("Read from dac 0xF: 0x");
      Serial.print(buf[0], HEX);
      Serial.print(" 0x");
      Serial.print(buf[1], HEX);
      Serial.print(" 0x");
      Serial.println(buf[2], HEX);
    } else {
      Serial.println("SERIOUS ERROR: Invalid board type! Please inform the technician");
      return -1;
    }

    return 0;
  }

  Serial.println(ARG_ERROR);
  return -1;
}

int32_t dac_write(byte addr, byte chan, uint32_t val)
{
  byte cmd, data1, data2;
  int32_t err;

  cmd = (0x3 << 3) | chan;
  data1 = (byte)(val >> 4); 
  data2 = (((byte)(val & 0xf)) << 4) | 0x0;

  Wire.beginTransmission(addr);
  Wire.write(cmd);
  Wire.write(data1);
  Wire.write(data2);
  err = Wire.endTransmission();
  if (err) {
    Serial.print("I2C communication failed, error code = ");
    Serial.println(err);
    return err;
  }

  return 0;
}

int32_t dac_read(byte addr, byte *buf)
{
  int32_t i;
  
  Wire.requestFrom(addr, 3);
  for (i = 0; i < 3; i++) {
    if (Wire.available()) {
      buf[i] = Wire.read();
    }
  }
  
  return 0;
}
#if 0
int32_t dac_read2(byte addr, byte chan, uint32_t &val)
{
    int32_t err, i;
    int32_t cmd = (0x3 << 3) | chan;
    uint8 buf[3];

    err = Wire.CombinedTrans(addr, cmd, 1, 3);
    if (err) {
      Serial.print("Error occured while i2c combined transmission, error = ");
      Serial.println(err);
      return err;
    }

    for(i = 0; i < 3; i++) {
      if(Wire.available())
        buf[i] = Wire.read();
    }
    Serial.print("buf[0]: 0x");
    Serial.println(buf[0], HEX);
    Serial.print("buf[1]: 0x");
    Serial.println(buf[1], HEX);
    Serial.print("buf[2]: 0x");
    Serial.println(buf[2], HEX);

    val = (buf[1] << 4) | (buf[2] >> 4);

    return 0;
}
#endif
