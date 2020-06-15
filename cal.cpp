#include "cal.h"
#include "cmnlib.h"
#include "eeprom.h"

tag_addr_stu tag_addr;

extern int32_t eeprom_addr;
extern int32_t board_type;

#define EEPROM_HEADER_OFFSET           0x1000
#define FILE_HEADER_OFFSET1            0x1040
#define FILE_HEADER_OFFSET2            0x1070
#define CAL_HEADER_OFFSET1             0x10c0
#define CAL_HEADER_OFFSET2             0x10e0
#define EEPROM_BACKUP_OFFSET           0x4000

#define FILE_NAME_OFFSET               0x1050
#define FILE_PURE_DATA_CHKSUM          0x1070
#define FILE_HEADER_DATE               0x1074
#define FILE_HEADER_CHKSUM             0x10BC


// fixed content
unsigned char eeprom_header[24] = {0x3c,0x45,0x45,0x50,0x52,0x4f,0x4d,0x5f,0x4e,0x45,0x57,0x49,0x4e,0x50,0x3e,0x10,
                                   0x10,0x0, 0x10, 0x0, 0x0, 0x0, 0x1, 0x0 };

unsigned char file_header1[16] =  {0x0, 0x1, 0x0, 0x0, 0x50,0x44,0x2e,0x50,0x41,0x52,0x0, 0x1, 0,    0,   0,  0};

unsigned char cal_header[32] =    {0x3c,0x5f,0x54,0x48,0x49,0x53,0x5f,0x49,0x53,0x5f,0x50,0x41,0x52,0x41,0x46,0x49,
                                   0x4c,0x45,0x5f,0x43,0x48,0x45,0x43,0x4b,0x5f,0x48,0x45,0x41,0x44,0x5f,0x3e, 0x0};

// different content for each board
// for 364
#define TAG_MAX_LENGTH_364 16

unsigned char cal_header2_for_364[] =      {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,0x01,0x00,0x10,0x01,
                                            0x0A,0x01,0x02,0x0A,0x01,0x03,0x0A,0x01,0x04,0x0A,0x01,0x05,0x0A,0x01,0x06,0x18,
                                            0x00,0x07,0x10,0x01,0x08,0x10,0x01,0x09,0x33,0x01,0x0A,0x33,0x01,0x0B,0x33,0x01,
                                            0x0C,0x33,0x01,0x0D,0x33,0x01,0x0E,0x33,0x01,0x0F,0x33,0x01,0x10,0x33,0x01};
uint32_t cal_header2_length_364 = 63;

uint32_t file_length_for_364 = 0x1A2D;
uint32_t eeprom_length_for_364 = 10989;

char pd_header_name_for_364[5][8] = {"LIN1\n", "LIN2\n", "LOUT1\n", "LOUT2\n", "AM1\n"};
uint32_t pd_power_max_for_364[] = {0xE6, 0xE6, 0xF0, 0xF0, 0xF0};
uint32_t pd_power_min_for_364[] = {0xFFFFFEA2, 0xFFFFFEA2, 0xFFFFFF9C, 0xFFFFFF9C, 0xFFFFFEA2};

char voa_header_name_for_364[8][6] = {"VOA7\n", "VOA8\n", "VOA2\n", "VOA4\n", "VOA5\n", "VOA6\n", "VOA1\n", "VOA3\n"};

char others_header_name_for_364[3][20] = {"INSERTION LOSS\n", "VOLTAGE_K_B\n", "VOLTAGE_THRESHOLD\n"};
uint32_t others_addr_364[] = { 0x168D, 0x178D, 0x18AD };
uint32_t others_cali_count_364[] = { 0xC, 0x10, 0x10 }; // 1 = 8byte

extern uint32_t pd_addr_364[];
extern uint32_t pd_cali_count_364;
extern uint32_t pd_count_364;

extern uint32_t voa_addr_364[];
extern uint32_t voa_cali_count_364;
extern uint32_t voa_count_364;

void table_init_for_364()
{
  uint32_t sum, i, offset = 0;

  // 0x100 ~ 0x104
  write_byte_to_eeprom(eeprom_addr, 0x100, 0x3, 1);
  write_32_to_eeprom(eeprom_addr, 0x101, 0x6E);

  // EEPROM header 0x1000 ~ 0x103F
  i2c_eeprom_write_buffer(eeprom_addr, EEPROM_HEADER_OFFSET, eeprom_header, 24);
  write_byte_to_eeprom(eeprom_addr, EEPROM_HEADER_OFFSET + 24, 0, 60 - 24);
  sum = checksum(eeprom_header, 24);
  Serial.print("EEPROM header checksum = 0x");
  Serial.println(sum, HEX);
  write_32_to_eeprom(eeprom_addr, EEPROM_HEADER_OFFSET + 60, sum);
  Serial.println("EEPROM header completed");

  // file header 1 0x1040 ~ 0x106F
  i2c_eeprom_write_buffer(eeprom_addr, FILE_HEADER_OFFSET1, file_header1, 16);
  write_byte_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET1 + 16, 0, 24);
  write_32_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET1 + 16 + 24, 0x06);
  write_32_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET1 + 16 + 24 + 4, file_length_for_364);

  // file header 2 0x1070 ~ 0x10BF
  write_byte_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET2, 0, 16 * 5);
  Serial.println("file header completed");

  // cal header 1 0x10C0 ~ 0x10DF
  i2c_eeprom_write_buffer(eeprom_addr, CAL_HEADER_OFFSET1, cal_header, 32);

  // cal header 2 0x10E0 ~ 0x113C
  i2c_eeprom_write_buffer(eeprom_addr, CAL_HEADER_OFFSET2, cal_header2_for_364, cal_header2_length_364);
  write_byte_to_eeprom(eeprom_addr, CAL_HEADER_OFFSET2 + cal_header2_length_364, 0, 0x113C - 0x10E0 + 1 - cal_header2_length_364);
  Serial.println("cal header completed");

  // pd table header
  for (i = 0; i < pd_count_364; ++i) {
    offset = pd_addr_364[i] - 0xA0;
    write_byte_to_eeprom(eeprom_addr, offset, 0, 8);
    offset += 8;
    i2c_eeprom_write_buffer(eeprom_addr, offset, (unsigned char*)pd_header_name_for_364[i], strlen(pd_header_name_for_364[i]));
    offset += strlen(pd_header_name_for_364[i]);
    write_byte_to_eeprom(eeprom_addr, offset, 0, pd_addr_364[i] - offset + pd_cali_count_364 * 8);
    offset = pd_addr_364[i] - 4 * 5;
    write_32_to_eeprom(eeprom_addr, offset, pd_power_max_for_364[i]);
    offset = pd_addr_364[i] - 4 * 2;
    write_32_to_eeprom(eeprom_addr, offset, pd_power_min_for_364[i]);
  }
  Serial.println("pd header completed");

  // voa table header
  for (i = 0; i < voa_count_364; ++i) {
    offset = voa_addr_364[i] - 0xA0;
    write_byte_to_eeprom(eeprom_addr, offset, 0, 8);
    offset += 8;
    i2c_eeprom_write_buffer(eeprom_addr, offset, (unsigned char*)voa_header_name_for_364[i], strlen(voa_header_name_for_364[i]));
    offset += strlen(voa_header_name_for_364[i]);
    write_byte_to_eeprom(eeprom_addr, offset, 0, voa_addr_364[i] - offset + voa_cali_count_364 * 8);
  }
  Serial.println("voa header completed");

  // other tables
  for (i = 0; i < 3; ++i) {
    offset = others_addr_364[i] - 0xA0;
    write_byte_to_eeprom(eeprom_addr, offset, 0, 8);
    offset += 8;
    i2c_eeprom_write_buffer(eeprom_addr, offset, (unsigned char*)others_header_name_for_364[i], strlen(others_header_name_for_364[i]));
    offset += strlen(others_header_name_for_364[i]);
    write_byte_to_eeprom(eeprom_addr, offset, 0, others_addr_364[i] - offset + others_cali_count_364[i] * 8);
  }
  Serial.println("others header completed");

}

// for 517
unsigned char cal_header2_for_517[] =      {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,0x01,0x00,0x06,0x04,
                                            0x0A,0x01,0x05,0x0E,0x00,0x06,0x10,0x01,0x07,0x10,0x01,0x08,0x33,0x01,0x09,0x33,
                                            0x01 };
uint32_t cal_header2_length_517 = 33;

uint32_t file_length_for_517 = 0x8F5;
uint32_t eeprom_length_for_517 = 6581;

char pd_header_name_for_517[1][8] = {"AM1\n"};
uint32_t pd_power_max_for_517[] = {0xF0};
uint32_t pd_power_min_for_517[] = {0xFFFFFEA2};

char voa_header_name_for_517[2][6] = {"VOA1\n", "VOA2\n"};

char others_header_name_for_517[3][20] = {"INSERTION LOSS\n", "VOLTAGE_K_B\n", "VOLTAGE_THRESHOLD\n"};
uint32_t others_addr_517[] = { 0x12CD, 0x13A5, 0x14C5 };
uint32_t others_cali_count_517[] = { 0x7, 0x10, 0x10 }; // 1 = 8byte

extern uint32_t pd_addr_517[];
extern uint32_t pd_cali_count_517;
extern uint32_t pd_count_517;

extern uint32_t voa_addr_517[];
extern uint32_t voa_cali_count_517;
extern uint32_t voa_count_517;

void table_init_for_517()
{
  uint32_t sum, i, offset = 0;

  // 0x100 ~ 0x104
  write_byte_to_eeprom(eeprom_addr, 0x100, 0x3, 1);
  write_32_to_eeprom(eeprom_addr, 0x101, 0x6E);

  // EEPROM header 0x1000 ~ 0x103F
  i2c_eeprom_write_buffer(eeprom_addr, EEPROM_HEADER_OFFSET, eeprom_header, 24);
  write_byte_to_eeprom(eeprom_addr, EEPROM_HEADER_OFFSET + 24, 0, 60 - 24);
  sum = checksum(eeprom_header, 24);
  Serial.print("EEPROM header checksum = 0x");
  Serial.println(sum, HEX);
  write_32_to_eeprom(eeprom_addr, EEPROM_HEADER_OFFSET + 60, sum);
  Serial.println("EEPROM header completed");

  // file header 1 0x1040 ~ 0x106F
  i2c_eeprom_write_buffer(eeprom_addr, FILE_HEADER_OFFSET1, file_header1, 16);
  write_byte_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET1 + 16, 0, 24);
  write_32_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET1 + 16 + 24, 0x06);
  write_32_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET1 + 16 + 24 + 4, file_length_for_517);

  // file header 2 0x1070 ~ 0x10BF
  write_byte_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET2, 0, 16 * 5);
  Serial.println("file header completed");

  // cal header 1 0x10C0 ~ 0x10DF
  i2c_eeprom_write_buffer(eeprom_addr, CAL_HEADER_OFFSET1, cal_header, 32);

  // cal header 2 0x10E0 ~ 0x113C
  i2c_eeprom_write_buffer(eeprom_addr, CAL_HEADER_OFFSET2, cal_header2_for_517, cal_header2_length_517);
  write_byte_to_eeprom(eeprom_addr, CAL_HEADER_OFFSET2 + cal_header2_length_517, 0, 0x113C - 0x10E0 + 1 - cal_header2_length_517);
  Serial.println("cal header completed");

  // pd table header
  for (i = 0; i < pd_count_517; ++i) {
    offset = pd_addr_517[i] - 0xA0;
    write_byte_to_eeprom(eeprom_addr, offset, 0, 8);
    offset += 8;
    i2c_eeprom_write_buffer(eeprom_addr, offset, (unsigned char*)pd_header_name_for_517[i], strlen(pd_header_name_for_517[i]));
    offset += strlen(pd_header_name_for_517[i]);
    write_byte_to_eeprom(eeprom_addr, offset, 0, pd_addr_517[i] - offset + pd_cali_count_517 * 8);
    offset = pd_addr_517[i] - 4 * 5;
    write_32_to_eeprom(eeprom_addr, offset, pd_power_max_for_517[i]);
    offset = pd_addr_517[i] - 4 * 2;
    write_32_to_eeprom(eeprom_addr, offset, pd_power_min_for_517[i]);
  }
  Serial.println("pd header completed");

  // voa table header
  for (i = 0; i < voa_count_517; ++i) {
    offset = voa_addr_517[i] - 0xA0;
    write_byte_to_eeprom(eeprom_addr, offset, 0, 8);
    offset += 8;
    i2c_eeprom_write_buffer(eeprom_addr, offset, (unsigned char*)voa_header_name_for_517[i], strlen(voa_header_name_for_517[i]));
    offset += strlen(voa_header_name_for_517[i]);
    write_byte_to_eeprom(eeprom_addr, offset, 0, voa_addr_517[i] - offset + voa_cali_count_517 * 8);
  }
  Serial.println("voa header completed");

  // other tables
  for (i = 0; i < 3; ++i) {
    offset = others_addr_517[i] - 0xA0;
    write_byte_to_eeprom(eeprom_addr, offset, 0, 8);
    offset += 8;
    i2c_eeprom_write_buffer(eeprom_addr, offset, (unsigned char*)others_header_name_for_517[i], strlen(others_header_name_for_517[i]));
    offset += strlen(others_header_name_for_517[i]);
    write_byte_to_eeprom(eeprom_addr, offset, 0, others_addr_517[i] - offset + others_cali_count_517[i] * 8);
  }
  Serial.println("others header completed");

}

// for 503
unsigned char file_name_for_503[] =        {0x52,0x58,0x46,0x49,0x55,0x2E,0x50,0x41,0x52,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,
                                            0x20,0x20,0x20,0x20 }; // RXFIU.PAR

unsigned char cal_header2_for_503[] =      {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,0x01,0x00,0x05,0x01,
                                            0x10,0x01,0x02,0x10,0x01,0x03,0x10,0x01,0x04,0x10,0x01,0x05,0x06,0x01 };
uint32_t cal_header2_length_503 = 30;

uint32_t file_length_for_503 = 0x59D;
uint32_t eeprom_length_for_503 = 5725;

char pd_header_name_for_503[3][8] = {"RM2\n", "RMON\n", "RC\n"};

char others_header_name_for_503[3][20] = {"INSERTION LOSS\n", "RM1\n"};
uint32_t others_addr_503[] = { 0x162D, 0x13ED };
uint32_t others_cali_count_503[] = { 0x6, 0x10 }; // 1 = 8byte

extern uint32_t pd_addr_503[];
extern uint32_t pd_cali_count_503;
extern uint32_t pd_count_503;

void table_init_for_503()
{
  uint32_t sum, i, offset = 0;

  // 0x0 ~ 0x4
  write_byte_to_eeprom(eeprom_addr, 0x0, 0x1, 1);
  write_32_to_eeprom(eeprom_addr, 0x1, 0x0);

  // EEPROM header 0x1000 ~ 0x103F
  i2c_eeprom_write_buffer(eeprom_addr, EEPROM_HEADER_OFFSET, eeprom_header, 24);
  write_byte_to_eeprom(eeprom_addr, EEPROM_HEADER_OFFSET + 24, 0, 60 - 24);
  sum = checksum(eeprom_header, 24);
  Serial.print("EEPROM header checksum = 0x");
  Serial.println(sum, HEX);
  write_32_to_eeprom(eeprom_addr, EEPROM_HEADER_OFFSET + 60, sum);
  Serial.println("EEPROM header completed");

  // file header 1 0x1040 ~ 0x106F
  i2c_eeprom_write_buffer(eeprom_addr, FILE_HEADER_OFFSET1, file_header1, 16);
  i2c_eeprom_write_buffer(eeprom_addr, FILE_HEADER_OFFSET1 + 16, file_name_for_503, 20);
  write_32_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET1 + 16 + 20, 0x0);
  write_32_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET1 + 16 + 24, 0x06);
  write_32_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET1 + 16 + 24 + 4, file_length_for_503);

  // file header 2 0x1070 ~ 0x10BF
  write_byte_to_eeprom(eeprom_addr, FILE_HEADER_OFFSET2, 0, 16 * 5);
  Serial.println("file header completed");

  // cal header 1 0x10C0 ~ 0x10DF
  i2c_eeprom_write_buffer(eeprom_addr, CAL_HEADER_OFFSET1, cal_header, 32);

  // cal header 2 0x10E0 ~ 0x110C
  i2c_eeprom_write_buffer(eeprom_addr, CAL_HEADER_OFFSET2, cal_header2_for_503, cal_header2_length_503);
  write_byte_to_eeprom(eeprom_addr, CAL_HEADER_OFFSET2 + cal_header2_length_503, 0, 0x110C - 0x10E0 + 1 - cal_header2_length_503);
  Serial.println("cal header completed");

  // pd table header
  for (i = 0; i < pd_count_503; ++i) {
    offset = pd_addr_503[i] - 0xA0;
    write_byte_to_eeprom(eeprom_addr, offset, 0, 8);
    offset += 8;
    i2c_eeprom_write_buffer(eeprom_addr, offset, (unsigned char*)pd_header_name_for_503[i], strlen(pd_header_name_for_503[i]));
    offset += strlen(pd_header_name_for_503[i]);
    write_byte_to_eeprom(eeprom_addr, offset, 0, pd_addr_503[i] - offset + pd_cali_count_503 * 8);
  }
  Serial.println("pd header completed");

  // other tables
  for (i = 0; i < 2; ++i) {
    offset = others_addr_503[i] - 0xA0;
    write_byte_to_eeprom(eeprom_addr, offset, 0, 8);
    offset += 8;
    i2c_eeprom_write_buffer(eeprom_addr, offset, (unsigned char*)others_header_name_for_503[i], strlen(others_header_name_for_503[i]));
    offset += strlen(others_header_name_for_503[i]);
    write_byte_to_eeprom(eeprom_addr, offset, 0, others_addr_503[i] - offset + others_cali_count_503[i] * 8);
  }
  Serial.println("others header completed");
}






int32_t cmd_table_init(int32_t argc, char **argv)
{
  if(argc == 1) {
    if (board_type == 364) {
      table_init_for_364();
      return 0;
    } else if (board_type == 517) {
      table_init_for_517();
      return 0;
    } else if (board_type == 503) {
      table_init_for_503();
      return 0;
    } else {
      Serial.println(TECH_ERROR);
      return -1;
    }
  } else {
    Serial.println(ARG_ERROR);
    return -1;
  }
}

void table_cplt()
{
  uint32_t sum, i, j, offset = 0, file_length;
  int32_t value, value1_max, value1_min, value2_max, value2_min, value2_sub;
  byte skip_voa = 0;

  uint32_t obj_count;
  uint32_t *obj_addr_array = NULL;
  uint32_t obj_cali_count;

  // pd table header
  if (board_type == 364) {
    obj_count = pd_count_364;
    obj_addr_array = pd_addr_364;
    obj_cali_count = pd_cali_count_364;
  } else if (board_type == 517) {
    obj_count = pd_count_517;
    obj_addr_array = pd_addr_517;
    obj_cali_count = pd_cali_count_517;
  } else if (board_type == 503) {
    obj_count = pd_count_503;
    obj_addr_array = pd_addr_503;
    obj_cali_count = pd_cali_count_503;
  } else {
    Serial.println(TECH_ERROR);
    return;
  }

  for (i = 0; i < obj_count; ++i) {
    value1_max = 0; value1_min = 0; value2_max = 0; value2_min = 0; j = 0;
    offset = obj_addr_array[i];

    value1_max = value1_min = (int32_t)get_32_from_eeprom(eeprom_addr, offset);
    value2_max = value2_min = (int32_t)get_32_from_eeprom(eeprom_addr, offset + 4);
    offset += 8;
    ++j;

    do {
      value = (int32_t)get_32_from_eeprom(eeprom_addr, offset);
      if (value > value1_max) value1_max = value;
      if (value < value1_min) value1_min = value;
      value = (int32_t)get_32_from_eeprom(eeprom_addr, offset + 4);
      if (value > value2_max) value2_max = value;
      if (value < value2_min) value2_min = value;
      offset += 8;
    } while (++j < obj_cali_count);
    write_32_to_eeprom(eeprom_addr, obj_addr_array[i] - 4 * 6, value1_max);
    write_32_to_eeprom(eeprom_addr, obj_addr_array[i] - 4 * 3, value1_min);
    if (board_type == 503) {
      write_32_to_eeprom(eeprom_addr, obj_addr_array[i] - 4 * 5, value2_max);
      write_32_to_eeprom(eeprom_addr, obj_addr_array[i] - 4 * 2, value2_min);
    }
  }
  Serial.println("pd table completed");

  // voa table header
  if (board_type == 364) {
    obj_count = voa_count_364;
    obj_addr_array = voa_addr_364;
    obj_cali_count = voa_cali_count_364;
  } else if (board_type == 517) {
    obj_count = voa_count_517;
    obj_addr_array = voa_addr_517;
    obj_cali_count = voa_cali_count_517;
  } else {
    skip_voa = 1;
  }

  if (!skip_voa) {
    for (i = 0; i < obj_count; ++i) {
      value1_max = 0; value1_min = 0; value2_max = 0; value2_min = 0; j = 0;
      offset = obj_addr_array[i];
      value2_sub = MY_SUB((int32_t)get_32_from_eeprom(eeprom_addr, offset + 4), (int32_t)get_32_from_eeprom(eeprom_addr, offset + 4 + 8));
  
      value1_max = value1_min = (int32_t)get_32_from_eeprom(eeprom_addr, offset);
      value2_max = value2_min = (int32_t)get_32_from_eeprom(eeprom_addr, offset + 4);
      offset += 8;
      ++j;
  
      do {
        value = (int32_t)get_32_from_eeprom(eeprom_addr, offset);
        if (value > value1_max) value1_max = value;
        if (value < value1_min) value1_min = value;
        value = (int32_t)get_32_from_eeprom(eeprom_addr, offset + 4);
        if (value > value2_max) value2_max = value;
        if (value < value2_min) value2_min = value;
        offset += 8;
      } while (++j < obj_cali_count);
      write_32_to_eeprom(eeprom_addr, obj_addr_array[i] - 4 * 6, value1_max);
      write_32_to_eeprom(eeprom_addr, obj_addr_array[i] - 4 * 3, value1_min);
      write_32_to_eeprom(eeprom_addr, obj_addr_array[i] - 4 * 5, value2_max);
      write_32_to_eeprom(eeprom_addr, obj_addr_array[i] - 4 * 2, value2_min);
      write_32_to_eeprom(eeprom_addr, obj_addr_array[i] - 4 * 1, value2_sub);
    }
    Serial.println("voa table completed");
  }

  // calculate checksum
  if (board_type == 364) {
    file_length = file_length_for_364;
  } else if (board_type == 517) {
    file_length = file_length_for_517;
  } else if (board_type == 503) {
    file_length = file_length_for_503;
  } else {
    Serial.println(TECH_ERROR);
    return;
  }

  if (board_type == 503) {
    sum = cal_checksum_8(CAL_HEADER_OFFSET1, file_length);
  } else {
    sum = cal_checksum_32(CAL_HEADER_OFFSET1, file_length);
  }
  write_32_to_eeprom(eeprom_addr, FILE_PURE_DATA_CHKSUM, sum);
  sum = cal_checksum_32(FILE_HEADER_OFFSET1, 0x10 * 8 - 4);
  write_32_to_eeprom(eeprom_addr, FILE_HEADER_CHKSUM, sum);
  Serial.println("Calculate sum completed");
}

int32_t cmd_table_cplt(int32_t argc, char **argv)
{
  if(argc == 1) {
    table_cplt();
    return 0;
  } else {
    Serial.println(ARG_ERROR);
    return -1;
  }
}

void table_backup()
{
  uint32_t file_length, offset, every_len;
  unsigned char buf[512];

  if (board_type == 364) {
    file_length = file_length_for_364 + 0xC0;
  } else if (board_type == 517) {
    file_length = file_length_for_517 + 0xC0;
  } else if (board_type == 503) {
    file_length = file_length_for_503 + 0xC0;
  } else {
    Serial.println(TECH_ERROR);
    return;
  }

#if 1
  if (file_length % 4 != 0) {
    file_length += 4 - (file_length % 4);
  }
#endif

  offset = 0;
  while (offset < file_length) {
    every_len = (offset + 512 < file_length) ? 512 : file_length - offset;
    Serial.print("Copy ");
    Serial.print(every_len, DEC);
    Serial.print(" data from 0x");
    Serial.print(EEPROM_HEADER_OFFSET + offset, HEX);
    Serial.print(" to 0x");
    Serial.println(EEPROM_BACKUP_OFFSET + offset, HEX);
    i2c_eeprom_read_buffer(eeprom_addr, EEPROM_HEADER_OFFSET + offset, buf, every_len);
    i2c_eeprom_write_buffer(eeprom_addr, EEPROM_BACKUP_OFFSET + offset, buf, every_len);
    offset += every_len;
  }

  Serial.println("Done.");
}

int32_t cmd_table_backup(int32_t argc, char **argv)
{
  if(argc == 1) {
    table_backup();
    return 0;
  } else {
    Serial.println(ARG_ERROR);
    return -1;
  }
}

int32_t cmd_file_name(int32_t argc, char **argv)
{
  char buf[21];
  if(argc == 3 && !strcmp(argv[1], "write")) {
    if(strlen(argv[2]) > 20) {
      Serial.println("file name exceed 20");
      return -1;
    }
    i2c_eeprom_write_buffer(eeprom_addr, FILE_NAME_OFFSET, (unsigned char*)argv[2], strlen(argv[2]));
    return 0;
  } else if (argc == 2 && !strcmp(argv[1], "read")) {
    i2c_eeprom_read_buffer(eeprom_addr, FILE_NAME_OFFSET, (unsigned char*)buf, 20);
    buf[20] = 0;
    Serial.println(buf);
  } else {
    Serial.println(ARG_ERROR);
    return -1;
  }
}

int32_t cmd_file_version(int32_t argc, char **argv)
{
  unsigned char buf[9];
  unsigned int i;

  if(argc == 9 && !strcmp(argv[1], "write")) {
    memset(buf,0,sizeof(buf));
    i = strtoul(argv[2], NULL, 10);
    buf[0] = (i >> 8) & 0xff;
    buf[1] = i & 0xff;
    for(i = 3; i < 9; i++)
    {
      buf[i-1] = strtoul(argv[i], NULL, 10);
    }
    i2c_eeprom_write_buffer(eeprom_addr, FILE_HEADER_DATE, buf, 8);
  } else if (argc == 2 && !strcmp(argv[1], "read")) {
    i2c_eeprom_read_buffer(eeprom_addr, FILE_HEADER_DATE, buf, 8);
    buf[8] = 0;
    Serial.print((unsigned int)((buf[0] & 0xff) << 8) | (buf[1] & 0xff), DEC);
    Serial.print(" ");
    for (i = 0; i < 6; ++i) {
      Serial.print(buf[i + 2], DEC);
      Serial.print(" ");
    }
    Serial.println("");
  } else {
    Serial.println(ARG_ERROR);
    return -1;
  }
}

void tag_init_for_364()
{
  tag_addr.tag_pn.addr = TAG_PN_364;
  tag_addr.tag_vendor.addr = TAG_VENDOR_364;
  tag_addr.tag_model.addr = TAG_MODEL_364;
  tag_addr.tag_sn.addr = TAG_SN_364;
  tag_addr.tag_date.addr = TAG_DATE_364;

  tag_addr.tag_pn.length = TAG_MAX_LENGTH_364;
  tag_addr.tag_vendor.length = TAG_MAX_LENGTH_364;
  tag_addr.tag_model.length = TAG_MAX_LENGTH_364;
  tag_addr.tag_sn.length = TAG_MAX_LENGTH_364;
  tag_addr.tag_date.length = TAG_MAX_LENGTH_364;

  tag_addr.tag_desc.length = 0;
  tag_addr.tag_ext_model.length = 0;
}

void tag_init_for_503()
{
  tag_addr.tag_model.addr = TAG_MODEL_503;
  tag_addr.tag_pn.addr = TAG_PN_503;
  tag_addr.tag_sn.addr = TAG_SN_503;
  tag_addr.tag_desc.addr = TAG_DESC_503;
  tag_addr.tag_date.addr = TAG_DATE_503;
  tag_addr.tag_vendor.addr = TAG_VENDOR_503;
  tag_addr.tag_ext_model.addr = TAG_EXT_MODEL_503;

  tag_addr.tag_model.length = 20;
  tag_addr.tag_pn.length = 20;
  tag_addr.tag_sn.length = 20;
  tag_addr.tag_desc.length = 200;
  tag_addr.tag_date.length = 12;
  tag_addr.tag_vendor.length = 20;
  tag_addr.tag_ext_model.length = 20;
}

int32_t tag_write(uint32_t addr, char* buf, int32_t length)
{
  byte buf_temp[256];
  byte pad;

  if (length == 0) {
    Serial.println("Unsupport tag type");
    return -1;
  }

  if (strlen(buf) > length - 1) {
    Serial.println("Failed, it's too long to write\n");
    return -1;
  }

  if (board_type == 364 || board_type == 517) {
    pad = 0;
  } else if (board_type == 503) {
    pad = ' ';
  } else {
    Serial.println(TECH_ERROR);
    return -1;
  }

  memset(buf_temp, pad, length);
  strcpy((char*)buf_temp, buf);
  buf_temp[strlen(buf)] = 0x0A;

  i2c_eeprom_write_buffer(eeprom_addr, addr, buf_temp, length);

  return 0;
}

int32_t tag_read(uint32_t addr, int32_t length)
{
  int32_t i = 0; 
  byte buf_temp[256];

  if (length == 0 || length >= 256) {
    Serial.println("Length invalid");
    return -1;
  }

  i2c_eeprom_read_buffer(eeprom_addr, addr, buf_temp, length);
  buf_temp[length] = 0;

  Serial.println((char*)buf_temp);

  return 0;  
}

int32_t tag_process(int32_t argc, char **argv, uint32_t addr, int32_t size)
{
  if(strcmp(argv[1], "write")==0) {
    if(argc!=3) {
      Serial.println(ARG_ERROR);
      return -1;
    } else
      return tag_write(addr, argv[2], size);
  } else if(strcmp(argv[1], "read")==0) {
    if(argc!=2) {
      Serial.println(ARG_ERROR);
      return -1;
    } else
      return tag_read(addr,size);
  } else
      Serial.println(ARG_ERROR);
      return -1;
}


int32_t cmd_tag_pn(int32_t argc, char **argv)
{
  return tag_process(argc, argv, tag_addr.tag_pn.addr, tag_addr.tag_pn.length);
}

int32_t cmd_tag_vendor(int32_t argc, char **argv)
{
  return tag_process(argc, argv, tag_addr.tag_vendor.addr, tag_addr.tag_vendor.length);
}

int32_t cmd_tag_model(int32_t argc, char **argv)
{
  return tag_process(argc, argv, tag_addr.tag_model.addr, tag_addr.tag_model.length);
}

int32_t cmd_tag_sn(int32_t argc, char **argv)
{
  return tag_process(argc, argv, tag_addr.tag_sn.addr, tag_addr.tag_sn.length);
}

int32_t cmd_tag_date(int32_t argc, char **argv)
{
  return tag_process(argc, argv, tag_addr.tag_date.addr, tag_addr.tag_date.length);
}

int32_t cmd_tag_desc(int32_t argc, char **argv)
{
  return tag_process(argc, argv, tag_addr.tag_desc.addr, tag_addr.tag_desc.length);
}

int32_t cmd_tag_ext_model(int32_t argc, char **argv)
{
  return tag_process(argc, argv, tag_addr.tag_ext_model.addr, tag_addr.tag_ext_model.length);
}

int32_t cal_voa(int32_t argc, char **argv)
{
  uint32_t num, cal_num;
  uint32_t voa_count = 0;
  uint32_t *voa_addr_array = NULL;
  uint32_t voa_cali_count;
  uint32_t adc, addr;
  int32_t atten;
  double atten_raw;

  if (board_type == 364) {
    voa_addr_array = voa_addr_364;
    voa_cali_count = voa_cali_count_364;
    voa_count = voa_count_364;
  } else if (board_type == 517) {
    voa_addr_array = voa_addr_517;
    voa_cali_count = voa_cali_count_517;
    voa_count = voa_count_517;
  } else {
    Serial.println(TECH_ERROR);
    return -1;
  }

  num = strtoul(argv[1] + 3, NULL, 10);
  if (num < 1 || num > voa_count) {
    Serial.println("Wrong voa number");
    return -1;
  }

  if (!strcmp(argv[2], "dump")) {
    addr = voa_addr_array[num - 1];
    dump_cali(addr, voa_cali_count, DUMP_MODE_VOA);
    return 0;
  }

  if (argc != 5) {
    Serial.println(ARG_ERROR);
    return -1;
  }
  
  cal_num = strtoul(argv[2], NULL, 10);
  if (cal_num < 1 || cal_num > voa_cali_count) {
    Serial.println("Wrong voa cali number");
    return -1;
  }
  adc = strtoul(argv[3], NULL, 10);
  atten_raw = atof(argv[4]) * 10;
  if (atten_raw > 0)
    atten = (int32_t)(atten_raw + 0.5);
  else
    atten = (int32_t)(atten_raw - 0.5);

  addr = voa_addr_array[num - 1] + (cal_num - 1) * 8;
  write_32_to_eeprom(eeprom_addr, addr, adc);
  write_32_to_eeprom(eeprom_addr, addr + 4, atten);

  return 0;
}

int32_t cal_pd(int32_t argc, char **argv)
{
  uint32_t num, cal_num;
  uint32_t pd_count = 0;
  uint32_t *pd_addr_array = NULL;
  uint32_t pd_cali_count;
  uint32_t adc, addr;
  int32_t power;
  double power_raw;

  if (board_type == 364) {
    pd_addr_array = pd_addr_364;
    pd_cali_count = pd_cali_count_364;
    pd_count = pd_count_364;
  } else if (board_type == 517) {
    pd_addr_array = pd_addr_517;
    pd_cali_count = pd_cali_count_517;
    pd_count = pd_count_517;
  } else if (board_type == 503) {
    pd_addr_array = pd_addr_503;
    pd_cali_count = pd_cali_count_503;
    pd_count = pd_count_503;
  } else {
    Serial.println(TECH_ERROR);
    return -1;
  }

  num = strtoul(argv[1] + 2, NULL, 10);
  if (num < 1 || num > pd_count) {
    Serial.println("Wrong pd number");
    return -1;
  }

  if (!strcmp(argv[2], "dump")) {
    addr = pd_addr_array[num - 1];
    dump_cali(addr, pd_cali_count, DUMP_MODE_PD);
    return 0;
  }

  if (argc != 5) {
    Serial.println(ARG_ERROR);
    return -1;
  }

  cal_num = strtoul(argv[2], NULL, 10);
  if (cal_num < 1 || cal_num > pd_cali_count) {
    Serial.println("Wrong pd cali number");
    return -1;
  }
  adc = strtoul(argv[3], NULL, 10);
  power_raw = atof(argv[4]) * 10;
  if (power_raw > 0)
    power = (int32_t)(power_raw + 0.5);
  else
    power = (int32_t)(power_raw - 0.5);

  addr = pd_addr_array[num - 1] + (cal_num - 1) * 8;
  write_32_to_eeprom(eeprom_addr, addr, adc);
  write_32_to_eeprom(eeprom_addr, addr + 4, power);

  return 0;
}

int32_t cal_il(int32_t argc, char **argv)
{
  uint32_t cal_num;
  uint32_t il_addr;
  uint32_t il_cali_count;
  uint32_t addr;
  int32_t power;
  double power_raw;

  if (board_type == 364) {
    il_addr = others_addr_364[0];
    il_cali_count = others_cali_count_364[0] * 2;
  } else if (board_type == 517) {
    il_addr = others_addr_517[0];
    il_cali_count = others_cali_count_517[0] * 2;
  } else if (board_type == 503) {
    il_addr = others_addr_503[0];
    il_cali_count = others_cali_count_503[0];
  } else {
    Serial.println(TECH_ERROR);
    return -1;
  }

  if (!strcmp(argv[2], "dump")) {
    addr = il_addr;
    dump_cali(addr, il_cali_count, DUMP_MODE_IL);
    return 0;
  }

  if (argc != 4) {
    Serial.println(ARG_ERROR);
    return -1;
  }

  cal_num = strtoul(argv[2], NULL, 10);
  if (cal_num < 1 || cal_num > il_cali_count) {
    Serial.println("Wrong cali number");
    return -1;
  }
  power_raw = atof(argv[3]) * 10;
  if (power_raw > 0)
    power = (int32_t)(power_raw + 0.5);
  else
    power = (int32_t)(power_raw - 0.5);

  if (board_type == 503) {
    addr = il_addr + (cal_num - 1) * 8 + 4;
  } else {
    addr = il_addr + (cal_num - 1) * 4;
  }

  write_32_to_eeprom(eeprom_addr, addr, power);

  return 0;
}

int32_t cal_vkb(int32_t argc, char **argv)
{
  uint32_t cal_num;
  uint32_t vkb_addr;
  uint32_t vkb_cali_count;
  uint32_t addr;
  unsigned char buf[4];
  float raw;
  uint32_t *pp = (uint32_t*)&raw;

  if (board_type == 364) {
    vkb_addr = others_addr_364[1];
    vkb_cali_count = others_cali_count_364[1];
  } else if (board_type == 517) {
    vkb_addr = others_addr_517[1];
    vkb_cali_count = others_cali_count_517[1];
  } else {
    Serial.println(TECH_ERROR);
    return -1;
  }

  if (!strcmp(argv[2], "dump")) {
    addr = vkb_addr;
    dump_cali(addr, vkb_cali_count, DUMP_MODE_VKB);
    return 0;
  }

  if (argc != 5) {
    Serial.println(ARG_ERROR);
    return -1;
  }

  cal_num = strtoul(argv[2], NULL, 10);
  if (cal_num < 1 || cal_num > vkb_cali_count) {
    Serial.println("Wrong cali number");
    return -1;
  }

  addr = vkb_addr + (cal_num - 1) * 8;

  raw = atof(argv[3]);  
  buf[0] = (*pp >> 24) & 0xFF;
  buf[1] = (*pp >> 16) & 0xFF;
  buf[2] = (*pp >> 8) & 0xFF;
  buf[3] = (*pp >> 0) & 0xFF;
  i2c_eeprom_write_buffer(eeprom_addr, addr, buf, 4);

  raw = atof(argv[4]);
  buf[0] = (*pp >> 24) & 0xFF;
  buf[1] = (*pp >> 16) & 0xFF;
  buf[2] = (*pp >> 8) & 0xFF;
  buf[3] = (*pp >> 0) & 0xFF;
  i2c_eeprom_write_buffer(eeprom_addr, addr + 4, buf, 4);

  return 0;
}

int32_t cal_vt(int32_t argc, char **argv)
{
  uint32_t cal_num;
  uint32_t vt_addr;
  uint32_t vt_cali_count;
  uint32_t addr;
#if 0
  int32_t voltage;
  float voltage_raw;
#else
  int32_t adc1, adc2;
#endif

  if (board_type == 364) {
    vt_addr = others_addr_364[2];
    vt_cali_count = others_cali_count_364[2];
  } else if (board_type == 517) {
    vt_addr = others_addr_517[2];
    vt_cali_count = others_cali_count_517[2];
  } else {
    Serial.println(TECH_ERROR);
    return -1;
  }

  if (!strcmp(argv[2], "dump")) {
    addr = vt_addr;
    dump_cali(addr, vt_cali_count, DUMP_MODE_VT);
    return 0;
  }

  if (argc != 5) {
    Serial.println(ARG_ERROR);
    return -1;
  }

  cal_num = strtoul(argv[2], NULL, 10);
  if (cal_num < 1 || cal_num > vt_cali_count) {
    Serial.println("Wrong cali number");
    return -1;
  }

  addr = vt_addr + (cal_num - 1) * 8;

#if 0
  voltage_raw = atof(argv[3]) * 100;  
  if (voltage_raw > 0)
    voltage = (int32_t)(voltage_raw + 0.5);
  else
    voltage = (int32_t)(voltage_raw - 0.5);
  write_32_to_eeprom(eeprom_addr, addr, voltage);

  voltage_raw = atof(argv[4]) * 100;  
  if (voltage_raw > 0)
    voltage = (int32_t)(voltage_raw + 0.5);
  else
    voltage = (int32_t)(voltage_raw - 0.5);
  write_32_to_eeprom(eeprom_addr, addr + 4, voltage);
#else
  adc1 = atoi(argv[3]);
  adc2 = atoi(argv[4]);
  write_32_to_eeprom(eeprom_addr, addr, (uint32_t)adc1);
  write_32_to_eeprom(eeprom_addr, addr + 4, (uint32_t)adc2);
#endif
  return 0;
}

int32_t cmd_cal(int32_t argc, char **argv)
{
  unsigned char cvoa = 0, cpd = 0, cil = 0, cvkb = 0, cvt = 0;
  if (board_type == 364) {
    cvoa = 1; cpd = 1; cil = 1; cvkb = 1; cvt = 1;
  } else if (board_type == 517) {
    cvoa = 1; cpd = 1; cil = 1; cvkb = 1; cvt = 1;
  } else if (board_type == 503) {
    cvoa = 0; cpd = 1; cil = 1; cvkb = 0; cvt = 0;
  } else {
    Serial.println(TECH_ERROR);
    return -1;
  }

  if (argc < 3) {
    Serial.println(ARG_ERROR);
    return -1;
  }

  if (cvoa && strncmp(argv[1], "voa", 3) == 0 && strlen(argv[1]) > 3) {
    return cal_voa(argc, argv);
  } else if (cpd && strncmp(argv[1], "pd", 2) == 0 && strlen(argv[1]) > 2) {
    return cal_pd(argc, argv);
  } else if (cil && strncmp(argv[1], "il", 2) == 0) {
    return cal_il(argc, argv);
  } else if (cvkb && strncmp(argv[1], "vkb", 3) == 0) {
    return cal_vkb(argc, argv);
  } else if (cvt && strncmp(argv[1], "vt", 2) == 0) {
    return cal_vt(argc, argv);
  } else {
    Serial.println(ARG_ERROR);
    return -1;
  }
}

void dump_cali(uint32_t addr, uint32_t count, unsigned char mode)
{
  uint32_t i;
  int32_t val;
  float val_f, *pf = (float*)&val;

  Serial.print("Dump from addr 0x");
  Serial.println(addr, HEX);
  if (mode == DUMP_MODE_IL) {
    for (i = 0; i < count; ++i) {
      Serial.print(i + 1);
      Serial.print(",");
      if (board_type == 503) {
        val = (int32_t)get_32_from_eeprom(eeprom_addr, addr + i * 8 + 4);
      } else {
        val = (int32_t)get_32_from_eeprom(eeprom_addr, addr + i * 4);
      }
      val_f = (float)((double)val / 10);
      Serial.println(val_f);
    }
  } else if (mode == DUMP_MODE_VKB) {
    for (i = 0; i < count; ++i) {
      Serial.print(i + 1);
      Serial.print(",");
      val = (int32_t)get_32_from_eeprom(eeprom_addr, addr + i * 8);
      val_f = *pf;
      Serial.print(val_f);
      Serial.print(",");
      val = (int32_t)get_32_from_eeprom(eeprom_addr, addr + i * 8 + 4);
      val_f = *pf;
      Serial.println(val_f);
    }
  } else if (mode == DUMP_MODE_VT) {
    for (i = 0; i < count; ++i) {
      Serial.print(i + 1);
      Serial.print(",");
      val = (int32_t)get_32_from_eeprom(eeprom_addr, addr + i * 8);
#if 0
      val_f = (float)((float)val / 100);
      Serial.print(val_f);
#else
      Serial.print(val, DEC);
#endif
      Serial.print(",");
      val = (int32_t)get_32_from_eeprom(eeprom_addr, addr + i * 8 + 4);
#if 0
      val_f = (float)((float)val / 100);
      Serial.println(val_f);
#else
      Serial.println(val, DEC);
#endif
    }
  } else {
    // PD and VOA
    for (i = 0; i < count; ++i) {
      Serial.print(i + 1);
      Serial.print(",");
      val = (int32_t)get_32_from_eeprom(eeprom_addr, addr + i * 8);
      Serial.print(val, DEC);
      Serial.print(",");
      val = (int32_t)get_32_from_eeprom(eeprom_addr, addr + i * 8 + 4);
      val_f = (float)((float)val / 10);
      Serial.println(val_f);
    }
  }
}

uint32_t checksum(uint8_t *pdata, uint32_t length)
{
  uint32_t res = 0, offset = 0;

  while(offset < length)
  {
    res += *pdata;
    ++pdata;
    ++offset;
  }
  return res;
}

uint32_t cal_checksum_8(uint32_t addr, uint32_t length)
{
  uint32_t offset = addr, every_len, res = 0, i;
  unsigned char buf[128];

  while (offset < addr + length) {
    every_len = (offset + 128 < addr + length) ? 128 : addr + length - offset;
    i2c_eeprom_read_buffer2(eeprom_addr, offset, buf, every_len);
    for (i = 0; i < every_len; ++i) {
      res += buf[i];
    }
    offset += every_len;
  }
  Serial.print("checksum_8: 0x");
  Serial.println(res, HEX);

  return res;
}

uint32_t cal_checksum_32(uint32_t addr, uint32_t length)
{
  uint32_t res = 0, offset = addr, exceed, val;

  while(offset < addr + length)
  {
    val = get_32_from_eeprom(eeprom_addr, offset);
#if 1
    if (offset == addr) {
      Serial.print("First data: 0x");
      Serial.println(val, HEX);
    }
#endif
    if (offset >= addr + length - 4) {
      exceed = offset + 4 - (addr + length);
      if (exceed == 1) {
        val &= 0xFFFFFF00;
      } else if (exceed == 2) {
        val &= 0xFFFF0000;
      } else if (exceed == 3) {
        val &= 0xFF000000;
      }

      if (exceed) {
        write_byte_to_eeprom(eeprom_addr, offset + (4 - exceed), 0, exceed);
      }

      Serial.print("Last data: 0x");
      Serial.println(val, HEX);
    }
    res += val;
    offset += 4;
  }
  Serial.print("checksum_32: 0x");
  Serial.println(res, HEX);

  return res;
}
