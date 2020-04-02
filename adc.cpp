#include "adc.h"

int cmd_adc(int argc, char **argv)
{
  unsigned int channel, result;

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

unsigned int ReadChannelDigital(byte chanIdx)
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
//  Serial.print("stage3 : 0x");
//  Serial.print(r1, HEX);
//  Serial.print(" 0x");
//  Serial.println(r2, HEX);

  rChanIdx = r1 >> 4;
  if (rChanIdx != chanIdx) {
    Serial.println("rChanIdx != chanIdx, communication failed");
    return 0;
  }
  return ((r1 & 0xf) << 8) | r2;
}
