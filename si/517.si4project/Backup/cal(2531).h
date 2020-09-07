#ifndef _CAL_H_
#define _CAL_H_

#include <stdint.h>

extern uint32_t eeprom_length_for_364;

// for 364
typedef enum {
  TAG_PN_364 = 0x0,
  TAG_VENDOR_364 = 0x10,
  TAG_MODEL_364 = 0x20,
  TAG_SN_364 = 0x30,
  TAG_DATE_364 =0x40,
} TAG_ADDR_364;
void tag_init_for_364(void);



// for all
typedef struct {
  uint32_t addr;
  uint32_t length;
} tag_attr;

typedef struct {
  tag_attr tag_pn;
  tag_attr tag_vendor;
  tag_attr tag_model;
  tag_attr tag_sn;
  tag_attr tag_date;
} tag_addr_stu;

typedef enum {
  DUMP_MODE_IL,
  DUMP_MODE_VOA,
  DUMP_MODE_PD,
  DUMP_MODE_VKB,
  DUMP_MODE_VT,
} DUMP_MODE;

void table_init_for_364();
int32_t cmd_table_init(int32_t argc, char **argv);
int32_t cmd_file_name(int32_t argc, char **argv);
int32_t cmd_file_version(int32_t argc, char **argv);

int32_t tag_write(uint32_t addr, char* buf, int32_t length);
int32_t tag_read(uint32_t addr, int32_t lenth);
int32_t tag_process(int32_t argc, char **argv, uint32_t addr, int32_t size);

int32_t cmd_tag_pn(int32_t argc, char **argv);
int32_t cmd_tag_vendor(int32_t argc, char **argv);
int32_t cmd_tag_model(int32_t argc, char **argv);
int32_t cmd_tag_sn(int32_t argc, char **argv);
int32_t cmd_tag_date(int32_t argc, char **argv);
int32_t cal_voa(int32_t argc, char **argv);
int32_t cal_pd(int32_t argc, char **argv);
int32_t cmd_cal(int32_t argc, char **argv);
void dump_cali(uint32_t addr, uint32_t count, unsigned char mode);
uint32_t checksum(uint8_t *pdata, uint32_t length);




#if 0
int cmd_cal_init_517(int argc, char **argv);
int cmd_cal_init_364(int argc, char **argv);
#endif

#endif
