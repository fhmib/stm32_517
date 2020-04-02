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
  }

  return 0;
}
