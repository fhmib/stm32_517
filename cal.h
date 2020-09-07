#ifndef _CAL_H_
#define _CAL_H_

#include <stdint.h>

extern uint32_t eeprom_length_for_364;
extern uint32_t eeprom_length_for_419;
extern uint32_t eeprom_length_for_517;
extern uint32_t eeprom_length_for_503;
extern uint32_t eeprom_length_for_573;

// for 364 & 517
typedef enum {
  TAG_PN_364 = 0x0,
  TAG_VENDOR_364 = 0x10,
  TAG_MODEL_364 = 0x20,
  TAG_SN_364 = 0x30,
  TAG_DATE_364 = 0x40,
} TAG_ADDR_364;

// for 503
typedef enum {
  TAG_MODEL_503 = 0x100,
  TAG_PN_503 = TAG_MODEL_503 + 20,
  TAG_SN_503 = TAG_PN_503 + 20,
  TAG_DESC_503 = TAG_SN_503 + 20,
  TAG_DATE_503 = TAG_DESC_503 + 200,
  TAG_VENDOR_503 = TAG_DATE_503 + 12,
  TAG_EXT_MODEL_503 = TAG_VENDOR_503 + 20,
} TAG_ADDR_503;

void tag_init_for_364(void);
void tag_init_for_503(void);

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
  tag_attr tag_desc;
  tag_attr tag_ext_model;
} tag_addr_stu;

typedef enum {
  DUMP_MODE_IL,
  DUMP_MODE_VOA,
  DUMP_MODE_PD,
  DUMP_MODE_VKB,
  DUMP_MODE_VT,
} DUMP_MODE;

void table_init_for_364(void);
void table_init_for_419(void);
void table_init_for_517(void);
void table_init_for_573(void);
void table_init_for_503(void);
int32_t cmd_table_init(int32_t argc, char **argv);
void table_cplt(void);
int32_t cmd_table_cplt(int32_t argc, char **argv);
void table_backup(void);
int32_t cmd_table_backup(int32_t argc, char **argv);

int32_t cmd_file_name(int32_t argc, char **argv);
int32_t cmd_file_version(int32_t argc, char **argv);

int32_t tag_write(uint32_t addr, char* buf, int32_t length);
int32_t tag_read(uint32_t addr, int32_t length);
int32_t tag_process(int32_t argc, char **argv, uint32_t addr, int32_t size);

int32_t cmd_tag_pn(int32_t argc, char **argv);
int32_t cmd_tag_vendor(int32_t argc, char **argv);
int32_t cmd_tag_model(int32_t argc, char **argv);
int32_t cmd_tag_sn(int32_t argc, char **argv);
int32_t cmd_tag_date(int32_t argc, char **argv);
int32_t cmd_tag_desc(int32_t argc, char **argv);
int32_t cmd_tag_ext_model(int32_t argc, char **argv);
int32_t cal_voa(int32_t argc, char **argv);
int32_t cal_voa_rb(int32_t argc, char **argv);
int32_t cal_pd(int32_t argc, char **argv);
int32_t cal_il(int32_t argc, char **argv);
int32_t cal_vkb(int32_t argc, char **argv);
int32_t cal_vt(int32_t argc, char **argv);
int32_t cmd_cal(int32_t argc, char **argv);
void dump_cali(uint32_t addr, uint32_t count, unsigned char mode);
uint32_t checksum(uint8_t *pdata, uint32_t length);
uint32_t cal_checksum_8(uint32_t addr, uint32_t length);
uint32_t cal_checksum_32(uint32_t addr, uint32_t length);

#endif
