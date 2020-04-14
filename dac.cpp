#include "dac.h"


int cmd_dac(int argc, char **argv)
{
  byte channel;
  unsigned int val;

  if (argc == 4 && !strcmp(argv[2], "write")) {
    if (!strcmp(argv[1], "1")) {
      channel = DAC_CHANNEL_A;
    } else if (!strcmp(argv[1], "2")) {
      channel = DAC_CHANNEL_B;
    } else if (!strcmp(argv[1], "3")) {
      channel = DAC_CHANNEL_C;
    } else if (!strcmp(argv[1], "4")) {
      channel = DAC_CHANNEL_D;
    } else if (!strcmp(argv[1], "ALL")) {
      channel = DAC_CHANNEL_ALL;
    } else {
      Serial.println("Wrong channel");
      return -1;
    }

    val = strtoul(argv[3], NULL, 0);
    if (val >> 12) {
      Serial.println("Dac value overflow");
      return -1;
    }

    dac_write(DAC_ADDR, channel, val);
    Serial.println("OK");

    return 0;
  }/* else if (argc == 3 && !strcmp(argv[2], "read")) {
    if (!strcmp(argv[1], "1")) {
      channel = DAC_CHANNEL_A;
    } else if (!strcmp(argv[1], "2")) {
      channel = DAC_CHANNEL_B;
    } else if (!strcmp(argv[1], "3")) {
      channel = DAC_CHANNEL_C;
    } else if (!strcmp(argv[1], "4")) {
      channel = DAC_CHANNEL_D;
    } else if (!strcmp(argv[1], "ALL")) {
      channel = DAC_CHANNEL_ALL;
    } else {
      Serial.println("Wrong channel");
      return -1;
    }

    dac_read2(DAC_ADDR, channel, val);
    Serial.print("Value: ");
    Serial.println(val, DEC);

    return 0;
  }*/ else if (argc == 2 && !strcmp(argv[1], "read")) {
    byte buf[3];

    dac_read(DAC_ADDR, buf);
    Serial.print("Read from dac: 0x");
    Serial.print(buf[0], HEX);
    Serial.print(" 0x");
    Serial.print(buf[1], HEX);
    Serial.print(" 0x");
    Serial.println(buf[2], HEX);

    return 0;
  }

  Serial.println("Wrong arg");
  return -1;
}

int dac_write(byte addr, byte chan, unsigned int val)
{
  byte cmd, data1, data2;
  int err;

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

int dac_read(byte addr, byte *buf)
{
    int i;

    Wire.requestFrom(addr, 3);
    for (i = 0; i < 3; i++) {
      if (Wire.available()) {
        buf[i] = Wire.read();
      }
    }

    return 0;
}
#if 0
int dac_read2(byte addr, byte chan, unsigned int &val)
{
    int err, i;
    int cmd = (0x3 << 3) | chan;
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
