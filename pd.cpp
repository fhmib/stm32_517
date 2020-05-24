#include "pd.h"
#include "adc.h"
#include "dac.h"
#include "eeprom.h"
#include "cmnlib.h"

// for 517
uint32_t pd_addr_517[] = { 0x11DD };
uint32_t pd_cali_count_517 = 10;
uint32_t pd_count_517 = 1;

uint32_t voa_addr_517[] = { 0x15E5, 0x181D };
uint32_t voa_cali_count_517 = 51;
uint32_t voa_count_517 = 2;

// for 503
  // PD1 = 0x150D, PD2 = 0x11AD, PD3 = 0x12CD
uint32_t pd_addr_503[] = { 0x150D, 0x11AD, 0x12CD };
uint32_t pd_cali_count_503 = 16;
uint32_t pd_count_503 = 3;

// for 364
  // LIN1 = 0x11DD, LIN2 = 0x12CD, LOUT1 = 0x13BD, LOUT2 = 0x14AD, AM1 = 0x159D
uint32_t pd_addr_364[] = { 0x11DD, 0x12CD, 0x13BD, 0x14AD, 0x159D};
uint32_t pd_cali_count_364 = 10;
uint32_t pd_count_364 = 5;
  // A_TM2 = 0x19CD, A_TM1 = 0x1C05, B_TM2 = 0x1E3D, B_TM1 = 0x2075, LOUT1 = 0x22AD, LOUT2 = 0x24E5, LIN1 = 0x271D, LIN2 = 0x2955
uint32_t voa_addr_364[] = { 0x271D, 0x2955, 0x1C05, 0x2075, 0x22AD, 0x24E5, 0x19CD, 0x1E3D};
uint32_t voa_cali_count_364 = 51;
uint32_t voa_count_364 = 8;


extern int board_type;
extern int eeprom_addr;

int32_t cmd_pd(int32_t argc, char **argv)
{
  uint32_t pd_num;
  uint32_t pd_count = 0;
  double pd;

  if (board_type == 517) {
    pd_count = pd_count_517;
  } else if (board_type == 503) {
    pd_count = pd_count_503;
  } else if (board_type == 364) {
    pd_count = pd_count_364;
  } else {
    Serial.println(TECH_ERROR);
    return -1;
  }

  if (pd_count == 0) {
    Serial.println("ERROR: There is no pd command for this board! Please inform the technician");
    return -1;
  }

  if (argc == 3 && !strcmp(argv[2], "read")) {
    pd_num = strtoul(argv[1], NULL, 0);
    if (pd_num < 1 || pd_num > pd_count) {
      Serial.println("Invalid pd number");
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

double get_pd(uint32_t pd_num)
{
  int32_t channel, adc1, adc2, offset, pd_raw;
  double power1, power2;
  uint32_t *pd_addr_array = NULL;
  uint32_t pd_cali_count;

  if (board_type == 517) {
    pd_addr_array = pd_addr_517;
    pd_cali_count = pd_cali_count_517;
    switch(pd_num) {
    case 1:
      channel = 0;
      break;
    default:
      Serial.println("Invalid pd_num");
      return 0;
    }
  } else if (board_type == 503) {
    pd_addr_array = pd_addr_503;
    pd_cali_count = pd_cali_count_503;
    switch(pd_num) {
    case 1:
      channel = 0;
      break;
    case 2:
      channel = 1;
      break;
    case 3:
      channel = 2;
      break;
    default:
      Serial.println("Invalid pd_num");
      return 0;
    }
  } else if (board_type == 364) {
    pd_addr_array = pd_addr_364;
    pd_cali_count = pd_cali_count_364;
    switch(pd_num) {
    case 1:
      channel = 0;
      break;
    case 2:
      channel = 1;
      break;
    case 3:
      channel = 2;
      break;
    case 4:
      channel = 3;
      break;
    case 5:
      channel = 4;
      break;
    default:
      Serial.println("Invalid pd_num");
      return 0;
    }
  } else {
    Serial.println(TECH_ERROR);
    return 0;
  }

  if (board_type == 517 || board_type == 364) {
    pd_raw = ReadChannelDigital((byte)channel);
  } else if (board_type == 503) {
    pd_raw = get_adc_7828((byte)channel);
  }
  Serial.print("ADC Channel = ");
  Serial.println(channel, DEC);
  Serial.print("PD raw data = ");
  Serial.println(pd_raw, DEC);

  offset = 0;
  adc1 = get_32_from_eeprom(eeprom_addr, pd_addr_array[pd_num - 1]);

  ++offset;
  while ((adc2 = get_32_from_eeprom(eeprom_addr, pd_addr_array[pd_num - 1] + offset * 8)) < pd_raw) {
    if (++offset > pd_cali_count - 1)
      break;
    adc1 = adc2;
  }
  power1 = (double)((int32_t)get_32_from_eeprom(eeprom_addr, pd_addr_array[pd_num - 1] + 4 + (offset - 1) * 8)) / 10;
  power2 = (double)((int32_t)get_32_from_eeprom(eeprom_addr, pd_addr_array[pd_num - 1] + 4 + offset * 8)) / 10;

  if (adc1 == adc2 || power1 == power2) {
    Serial.println("It needs calibration data in eeprom, read failed");
    return 0;
  }

  return (pd_raw - adc1) * (power2 - power1) / (adc2 - adc1) + power1;
}

int32_t cmd_voa(int32_t argc, char **argv)
{
  uint32_t voa_num;
  uint32_t voa_count = 0;
  double atten;

  if (board_type == 517) {
    voa_count = voa_count_517;
  } else if (board_type == 364) {
    voa_count = voa_count_364;
  } else {
    Serial.println(TECH_ERROR);
    return -1;
  }

  if (argc == 3 && !strcmp(argv[2], "read")) {
    voa_num = strtoul(argv[1], NULL, 0);
    if (voa_num < 1 || voa_num > voa_count) {
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

double get_voa(uint32_t voa_num)
{
  int32_t channel, adc1, adc2, offset, voa_raw;
  double atten1, atten2;
  uint32_t *voa_addr_array = NULL;
  uint32_t voa_cali_count;

  if (board_type == 517) {
    voa_addr_array = voa_addr_517;
    voa_cali_count = voa_cali_count_517;
    switch(voa_num) {
    case 1:
      channel = 1;
      break;
    case 2:
      channel = 2;
      break;
    default:
      Serial.println("Invalid voa_num");
      return 0;
    }
  } else if (board_type == 364) {
    voa_addr_array = voa_addr_364;
    voa_cali_count = voa_cali_count_364;
    switch(voa_num) {
    case 1:
      channel = 11;
      break;
    case 2:
      channel = 12;
      break;
    case 3:
      channel = 6;
      break;
    case 4:
      channel = 8;
      break;
    case 5:
      channel = 9;
      break;
    case 6:
      channel = 10;
      break;
    case 7:
      channel = 5;
      break;
    case 8:
      channel = 7;
      break;
    default:
      Serial.println("Invalid voa_num");
      return 0;
    }
  } else {
    Serial.println(TECH_ERROR);
    return 0;
  }

  if (board_type == 517 || board_type == 364) {
    voa_raw = ReadChannelDigital((byte)channel);
  }
  Serial.print("VOA raw data = ");
  Serial.println(voa_raw, DEC);

  adc1 = get_32_from_eeprom(eeprom_addr, voa_addr_array[voa_num - 1]);
  offset = 1;
  
  while ((adc2 = get_32_from_eeprom(eeprom_addr, voa_addr_array[voa_num - 1] + offset * 8)) > voa_raw) {
    if (++offset > voa_cali_count - 1)
      break;
    adc1 = adc2;
  }
  atten1 = (double)((int32_t)get_32_from_eeprom(eeprom_addr, voa_addr_array[voa_num - 1] + 4 + (offset - 1) * 8)) / 10;
  atten2 = (double)((int32_t)get_32_from_eeprom(eeprom_addr, voa_addr_array[voa_num - 1] + 4 + offset * 8)) / 10;

  if (adc1 == adc2 || atten1 == atten2) {
    Serial.println("It needs calibration data in eeprom, read failed");
    return 0;
  }

  Serial.print("adc1 = ");
  Serial.print(adc1);
  Serial.print(" adc2 = ");
  Serial.println(adc2);
  Serial.print("atten1 = ");
  Serial.print(atten1);
  Serial.print(" atten2 = ");
  Serial.println(atten2);
  Serial.print("voa_raw = ");
  Serial.println(voa_raw);

  return (double)(voa_raw - adc1) * (atten2 - atten1) / (double)(adc2 - adc1) + atten1;
}

void set_voa(uint32_t voa_num, double atten)
{
  int32_t channel, adc1, adc2, adc_act, offset;
  double atten1, atten2;
  uint32_t *voa_addr_array = NULL;
  uint32_t voa_cali_count;
  byte dac_addr;

  if (board_type == 517) {
    voa_addr_array = voa_addr_517;
    voa_cali_count = voa_cali_count_517;
    switch(voa_num) {
    case 1:
      channel = DAC_CHANNEL_A;
      break;
    case 2:
      channel = DAC_CHANNEL_B;
      break;
    default:
      Serial.println("Invalid voa_num");
      return;
    }
    dac_addr = DAC_ADDR_517;
  } else if (board_type == 364) {
    voa_addr_array = voa_addr_364;
    voa_cali_count = voa_cali_count_364;
    switch(voa_num) {
    case 1:
      channel = DAC_CHANNEL_A;
      dac_addr = DAC_ADDR_364_1;
      break;
    case 2:
      channel = DAC_CHANNEL_B;
      dac_addr = DAC_ADDR_364_1;
      break;
    case 3:
      channel = DAC_CHANNEL_C;
      dac_addr = DAC_ADDR_364_1;
      break;
    case 4:
      channel = DAC_CHANNEL_D;
      dac_addr = DAC_ADDR_364_1;
      break;
    case 5:
      channel = DAC_CHANNEL_A;
      dac_addr = DAC_ADDR_364_2;
      break;
    case 6:
      channel = DAC_CHANNEL_B;
      dac_addr = DAC_ADDR_364_2;
      break;
    case 7:
      channel = DAC_CHANNEL_C;
      dac_addr = DAC_ADDR_364_2;
      break;
    case 8:
      channel = DAC_CHANNEL_D;
      dac_addr = DAC_ADDR_364_2;
      break;
    default:
      Serial.println("Invalid voa_num");
      return;
    }
  } else {
    Serial.println(TECH_ERROR);
    return;
  }

  atten1 = (double)((int32_t)get_32_from_eeprom(eeprom_addr, voa_addr_array[voa_num - 1] + 4)) / 10;
  offset = 1;
  
  while ((atten2 = (double)((int32_t)get_32_from_eeprom(eeprom_addr, voa_addr_array[voa_num - 1] + 4 + offset * 8)) / 10) < atten) {
    if (++offset > voa_cali_count - 1)
      break;
    atten1 = atten2;
  }
  adc1 = get_32_from_eeprom(eeprom_addr, voa_addr_array[voa_num - 1] + (offset - 1) * 8);
  adc2 = get_32_from_eeprom(eeprom_addr, voa_addr_array[voa_num - 1] + offset * 8);

  Serial.print("adc1 = ");
  Serial.print(adc1);
  Serial.print(" adc2 = ");
  Serial.println(adc2);
  Serial.print("atten1 = ");
  Serial.print(atten1);
  Serial.print(" atten2 = ");
  Serial.println(atten2);
  Serial.print("atten = ");
  Serial.println(atten);

  if (adc1 == adc2 || atten1 == atten2) {
    Serial.println("It needs calibration data in eeprom, write failed");
    return;
  }

  if (atten > atten2) {
    Serial.println("Attenuation value exceeds limitation"); 
    adc_act = adc2;
    atten = atten2;
  } else {
    adc_act = (atten - atten1) * (adc2 - adc1) / (atten2 - atten1) + adc1;
  }
  Serial.print("Actual write adc: ");
  Serial.print(adc_act, DEC);
  Serial.print(" attenuation: ");
  Serial.println(atten);
  dac_write(dac_addr, channel, adc_act);
  Serial.println("OK");

  return ;
}

int32_t cmd_switch(int32_t argc, char **argv)
{
  uint32_t switch_num;
  uint32_t switch_state;
  byte out_pin, in_pin;

  if (argc < 3) {
    Serial.println("Wrong arg");
    return -1;
  }

  if (board_type == 364) {
    switch_num = strtoul(argv[1], NULL, 0);
    if (switch_num != 1) {
      Serial.println("Invalid switch number");
      return -1;
    }
    out_pin = PB13;
    pinMode(out_pin, OUTPUT);
    in_pin = PB9;
    pinMode(in_pin, INPUT);
  } else {
    Serial.println(TECH_ERROR);
    return -1;
  }

  if (argc == 3 && !strcmp(argv[2], "read")) {
    switch_state = digitalRead(in_pin);
    if (switch_state == 0) {
      Serial.println("Switch is set to LIN2");
    } else {
      Serial.println("Switch is set to LIN1");
    }
    return 0;
  } else if (argc == 4 && !strcmp(argv[2], "write")) {
    switch_state = strtoul(argv[3], NULL, 0);
    if (switch_state == 0 || switch_state > 2) {
      Serial.println("Invalid switch state");
      return -1;
    }

    if (switch_state == 1) {
      digitalWrite(out_pin, HIGH); //LIN1
    } else {
      digitalWrite(out_pin, LOW); //LIN2
    }

    Serial.print("Switch to ");
    Serial.print(switch_state);
    Serial.println(" OK");

    return 0;
  }

  Serial.println("Wrong arg");
  return -1;
}
