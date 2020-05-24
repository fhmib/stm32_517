#include "adc.h"

int32_t cmd_adc(int32_t argc, char **argv)
{
  uint32_t channel, result;

  if (argc == 3 && !strcmp(argv[2], "read")) {
    channel = strtoul(argv[1], NULL, 0);
    if (channel > 15) {
      Serial.println("Wrong channel");
      return -1;
    }

    result = ReadChannelDigital((byte)channel);
    Serial.print("Channel ");
    Serial.print(channel, DEC);
    Serial.print(" value: ");
    Serial.println(result, DEC);

    return 0;
  }

  Serial.println("Wrong arg");
  return -1;
}

uint32_t ReadChannelDigital(byte chanIdx)
{
  byte s1, s2;
  byte r1, r2, rChanIdx;

  s1 = (0x1 << 4) | (0x0 << 3) | (chanIdx >> 1);
  s2 = chanIdx << 7;

  digitalWrite(SPI1_NSS_PIN, LOW); // manually take CSN low for SPI_1 transmission
  r1 = SPI.transfer(s1); //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  r2 = SPI.transfer(s2); //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  digitalWrite(SPI1_NSS_PIN, HIGH); // manually take CSN high between spi transmissions

  digitalWrite(SPI1_NSS_PIN, LOW); // manually take CSN low for SPI_1 transmission
  r1 = SPI.transfer(s1); //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  r2 = SPI.transfer(s2); //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  digitalWrite(SPI1_NSS_PIN, HIGH); // manually take CSN high between spi transmissions

  digitalWrite(SPI1_NSS_PIN, LOW); // manually take CSN low for SPI_1 transmission
  r1 = SPI.transfer(s1); //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  r2 = SPI.transfer(s2); //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  digitalWrite(SPI1_NSS_PIN, HIGH); // manually take CSN high between spi transmissions

  rChanIdx = r1 >> 4;
  if (rChanIdx != chanIdx) {
    Serial.println("rChanIdx != chanIdx, communication failed");
    return 0;
  }
  return ((r1 & 0xf) << 8) | r2;
}

int32_t cmd_adc_7828(int32_t argc, char **argv)
{
  uint32_t channel, result;

  if (argc == 3 && !strcmp(argv[2], "read")) {
    channel = strtoul(argv[1], NULL, 0);
    if (channel > 7) {
      Serial.println("Wrong channel");
      return -1;
    }

    result = get_adc_7828((byte)channel);
    Serial.print("Channel ");
    Serial.print(channel, DEC);
    Serial.print(" value: ");
    Serial.println(result, DEC);

    return 0;
  }

  Serial.println("Wrong arg");
  return -1;
}

uint32_t get_adc_7828(byte chanIdx)
{
  int32_t offset, error, i = 0;
  char adc[2];

  offset = 4 * (chanIdx % 2) + chanIdx / 2;
  offset = 0x84 | (offset << 4);
  //Serial.print("offset = 0x");
  //Serial.println(offset, HEX);
#if 0
  Wire.beginTransmission(KTA7828_ADDR);
  Wire.write(offset);
  error = Wire.endTransmission();
  
  delay(5);
  Wire.requestFrom(KTA7828_ADDR, 2);
#else
  error = Wire.CombinedTrans(KTA7828_ADDR, offset, 1, 2);
  if (error) {
    Serial.print("Error occured while i2c combined transmission, error = ");
    Serial.println(error);
    return 0;
  }
#endif
  i = 0;
  while(Wire.available())
  {
    adc[i] = Wire.read();
    i++;
  }
  return ((adc[0] << 8) | adc[1]);
}
