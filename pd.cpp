#include "pd.h"
#include "adc.h"
#include "dac.h"
#include "eeprom.h"
#include "cmnlib.h"

static unsigned int pd_addr_517[] = { 0x11DD };
static unsigned int voa_addr_517[] = { 0x15E5, 0x181D };

int cmd_pd(int argc, char **argv)
{
  unsigned int pd_num;
  double pd;

  if (argc == 3 && !strcmp(argv[2], "read")) {
    pd_num = strtoul(argv[1], NULL, 0);
    if (pd_num < 1 || pd_num > 1) {
      Serial.println("Wrong pd number");
      return -1;
    }

    pd = get_pd(pd_num);
    Serial.print("PD: ");
    Serial.print(pd);
    Serial.println("dbm");

    return 0;
  }

  Serial.println("Wrong arg");
  return -1;
}

double get_pd(unsigned int pd_num)
{
  int channel, adc1, adc2, offset, pd_raw;
  double power1, power2;

  switch(pd_num) {
  case 1:
    channel = 0;
    break;
  default:
    Serial.println("Wrong pd_num");
    return 0;
  }

  pd_raw = ReadChannelDigital((byte)channel);
  Serial.print("PD raw data = ");
  Serial.println(pd_raw, DEC);
  
  adc1 = get_32_from_eeprom(EEPROM_ADDR, pd_addr_517[pd_num - 1]);
  offset = 1;
  
  while ((adc2 = get_32_from_eeprom(EEPROM_ADDR, pd_addr_517[pd_num - 1] + offset * 8)) < pd_raw) {
    if (offset > 8) break;
    ++offset;
    adc1 = adc2;
  }
  power1 = (double)((int)get_32_from_eeprom(EEPROM_ADDR, pd_addr_517[pd_num - 1] + 4 + (offset - 1) * 8)) / 10;
  power2 = (double)((int)get_32_from_eeprom(EEPROM_ADDR, pd_addr_517[pd_num - 1] + 4 + offset * 8)) / 10;

  if (adc1 == adc2 || power1 == power2) {
    Serial.println("It needs calibration data in eeprom, read failed");
    return 0;
  }

  return (pd_raw - adc1) * (power2 - power1) / (adc2 - adc1) + power1;
}

int cmd_voa(int argc, char **argv)
{
  unsigned int voa_num;
  double atten;

  if (argc == 3 && !strcmp(argv[2], "read")) {
    voa_num = strtoul(argv[1], NULL, 0);
    if (voa_num < 1 || voa_num > 2) {
      Serial.println("Wrong voa number");
      return -1;
    }

    atten = get_voa(voa_num);
    Serial.print("VOA: ");
    Serial.print(atten);
    Serial.println("db");

    return 0;
  } else if (argc == 4 && !strcmp(argv[2], "write")) {
    voa_num = strtoul(argv[1], NULL, 0);
    atten = atof(argv[3]);

    set_voa(voa_num, atten);

    return 0;
  }

  Serial.println("Wrong arg");
  return -1;
}

double get_voa(unsigned int voa_num)
{
  int channel, adc1, adc2, offset, voa_raw;
  double atten1, atten2;

  switch(voa_num) {
  case 1:
    channel = 1;
    break;
  case 2:
    channel = 2;
    break;
  default:
    Serial.println("Wrong voa_num");
    return 0;
  }

  voa_raw = ReadChannelDigital((byte)channel);
  Serial.print("VOA raw data = ");
  Serial.println(voa_raw, DEC);
  
  adc1 = get_32_from_eeprom(EEPROM_ADDR, voa_addr_517[voa_num - 1]);
  offset = 1;
  
  while ((adc2 = get_32_from_eeprom(EEPROM_ADDR, voa_addr_517[voa_num - 1] + offset * 8)) > voa_raw) {
    if (offset > 49) break;
    ++offset;
    adc1 = adc2;
  }
  atten1 = (double)((int)get_32_from_eeprom(EEPROM_ADDR, voa_addr_517[voa_num - 1] + 4 + (offset - 1) * 8)) / 10;
  atten2 = (double)((int)get_32_from_eeprom(EEPROM_ADDR, voa_addr_517[voa_num - 1] + 4 + offset * 8)) / 10;

  if (adc1 == adc2 || atten1 == atten2) {
    Serial.println("It needs calibration data in eeprom, read failed");
    return 0;
  }

  return (double)(voa_raw - adc1) * (atten2 - atten1) / (double)(adc2 - adc1) + atten1;
}

void set_voa(unsigned int voa_num, double atten)
{
  int channel, adc1, adc2, adc_act, offset;
  double atten1, atten2;

  switch(voa_num) {
  case 1:
    channel = DAC_CHANNEL_A;
    break;
  case 2:
    channel = DAC_CHANNEL_B;
    break;
  default:
    Serial.println("Wrong voa_num");
    return;
  }

  atten1 = (double)((int)get_32_from_eeprom(EEPROM_ADDR, voa_addr_517[voa_num - 1] + 4)) / 10;
  offset = 1;
  
  while ((atten2 = (double)((int)get_32_from_eeprom(EEPROM_ADDR, voa_addr_517[voa_num - 1] + 4 + offset * 8)) / 10) < atten) {
    if (offset > 49) break;
    ++offset;
    atten1 = atten2;
  }
  adc1 = get_32_from_eeprom(EEPROM_ADDR, voa_addr_517[voa_num - 1] + (offset - 1) * 8);
  adc2 = get_32_from_eeprom(EEPROM_ADDR, voa_addr_517[voa_num - 1] + offset * 8);

  if (adc1 == adc2 || atten1 == atten2) {
    Serial.println("It needs calibration data in eeprom, write failed");
    return;
  }

  if (atten > atten2) {
    Serial.println("Attenuation value exceeds limit"); 
    adc_act = adc2;
    atten = atten2;
  } else {
    adc_act = (atten - atten1) * (adc2 - adc1) / (atten2 - atten1) + adc1;
  }
  Serial.print("Actual write adc: ");
  Serial.print(adc_act, DEC);
  Serial.print(" attenuation: ");
  Serial.println(atten);
  dac_write(DAC_ADDR, channel, adc_act);
  Serial.println("OK");

  return ;
}
