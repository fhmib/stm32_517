#include "cmd.h"
#include "dac.h"
#include "adc.h"
#include "cal.h"
#include "pd.h"
#include "eeprom.h"
#include "cmnlib.h"

#define LED_PORT PB12
#define VER "0.1.8"
#define LED_HOLD_TIME 500 // ms

uint32_t led_count;
String cmd = "";
char c;
int32_t board_type = -1;
extern int32_t eeprom_addr;

void setup()
{
  led_count = 0;

  Wire.begin();
  Serial.begin(9600);

  SPI.begin(); // Initialize the SPI_1 port.
  SPI.setBitOrder(MSBFIRST); // Set the SPI_1 bit order
  SPI.setDataMode(SPI_MODE0); // Set the  SPI_2 data mode 0
  SPI.setClockDivider(SPI_CLOCK_DIV4); // Slow speed (18 / 4 = 4.5 MHz SPI_1 speed)

  pinMode(LED_PORT, OUTPUT);
  pinMode(SPI1_NSS_PIN, OUTPUT);
  pinMode(EEPROM_WP, OUTPUT);
  pinMode(SW_CTL_1, OUTPUT);
  pinMode(SW_CTL_2, OUTPUT);
  pinMode(SW_STAT_1, INPUT);
  pinMode(SW_STAT_2, INPUT);

  Serial.println("");
  Serial.print("Version: ");
  Serial.println(VER);
  Serial.println("COM is ready");
  Serial.println(SUPPORT_BOARD);
  Serial.println();
  Serial.print("Please input board type > ");
}

void loop()
{
  while (Serial.available() > 0)
  {
    c = char(Serial.read());

    if ((c == '\n') || (c == '\r')) {
      Serial.println("");
      if (board_type == -1) {
        get_board_type(cmd.c_str());
        if (board_type == -1) {
          Serial.println("");
          Serial.println(SUPPORT_BOARD);
          Serial.print("Please input board type > ");
        } else {
          Serial.print("Init board for ");
          Serial.print(board_type, DEC);
          Serial.println("...");
          board_init();
          Serial.println("Success. Entering command line");
          Serial.print('>');
        }
      } else {
        process_cmd(cmd.c_str());
        Serial.print('>');
      }
      cmd = "";
    } else if (c == '\b') {
      if (cmd.length() > 0) {
        Serial.print(c);
        Serial.print(" ");
        Serial.print(c);
        cmd.remove(cmd.length() - 1);
      } 
    } else {
      cmd += c;
      Serial.print(c);
    }
  }

  ++led_count;
  if (led_count == LED_HOLD_TIME) {
    digitalWrite(LED_PORT, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else if (led_count == LED_HOLD_TIME * 2) {
    digitalWrite(LED_PORT, LOW);    // turn the LED off by making the voltage LOW
    led_count = 0;
  }
  delay(1);

}

console_cmd cmdlist[25];

console_cmd cmdlist_517[] =
{
  {"tag_pn", cmd_tag_pn, "tag_pn write <content>, tag_pn read", "tag_pn write 123456, tag_pn read"},
  {"tag_vendor", cmd_tag_vendor, "tag_vendor write <content>, tag_vendor read", "tag_vendor write o-net, tag_vendor read"},
  {"tag_model", cmd_tag_model, "tag_model write <content>, tag_model read", "tag_model write model-1, tag_model read"},
  {"tag_sn", cmd_tag_sn, "tag_sn write <content>, tag_sn read", "tag_sn write 123456, tag_sn read"},
  {"tag_date", cmd_tag_date, "tag_date write <content>, tag_date read", "tag_date write 20200202, tag_date read"},
  {"table_init", cmd_table_init, "table_init", "table_init"},
  {"file_name", cmd_file_name, "file_name write <name>", "file_name write M+3.PAR"},
  {"file_version", cmd_file_version, "file_version write <year> <> ...", "file_version write 2020 5 24 16 34 0 0"},
  {"cal", cmd_cal, "cal voa<num>/pd<num>/... <point_num> <adc> <value>, cal voa<num>/pd<num>/... dump", "cal voa3 2 2034 4.5, cal voa3 dump"},
  {"table_cplt", cmd_table_cplt, "table_cplt", "table_cplt"},
  {"table_backup", cmd_table_backup, "table_backup", "table_backup"},

  {"dac", cmd_dac, "dac <channel> write <value>, dac read", "dac 1 write 1300, dac read"},
  {"adc", cmd_adc, "adc <channel> read", "adc 0 read"},
  {"eeprom", cmd_eeprom, "eeprom read <addr> <length>, eeprom write <addr> <val>", "eeprom read 0x0 10, eeprom write 0x0 0x10"},
  {"pd", cmd_pd, "pd <pd_num> read", "pd 1 read"},
  {"voa", cmd_voa, "voa <voa_num> read, voa <voa_num> write", "voa 1 read, voa 1 write 12.3"},

  {"pin", cmd_pin, "INTERNAL", "INTERNAL"},

  {"ver", cmd_ver, "ver", "ver"},
  {"help", cmd_help, "help", "help"},
};

console_cmd cmdlist_503[] =
{
  {"tag_model", cmd_tag_model, "tag_model write <content>, tag_model read", "tag_model write model-1, tag_model read"},
  {"tag_pn", cmd_tag_pn, "tag_pn write <content>, tag_pn read", "tag_pn write 123456, tag_pn read"},
  {"tag_sn", cmd_tag_sn, "tag_sn write <content>, tag_sn read", "tag_sn write 123456, tag_sn read"},
  {"tag_desc", cmd_tag_desc, "tag_desc write <content>, tag_desc read", "tag_desc write 11112222, tag_desc read"},
  {"tag_date", cmd_tag_date, "tag_date write <content>, tag_date read", "tag_date write 20200202, tag_date read"},
  {"tag_vendor", cmd_tag_vendor, "tag_vendor write <content>, tag_vendor read", "tag_vendor write o-net, tag_vendor read"},
  {"tag_ext_model", cmd_tag_ext_model, "tag_ext_model write <content>, tag_ext_model read", "tag_ext_model write Emodel-1, tag_ext_model read"},
  {"file_version", cmd_file_version, "file_version write <year> <> ...", "file_version write 2020 5 24 16 34 0 0"},
  {"table_init", cmd_table_init, "table_init", "table_init"},
  {"cal", cmd_cal, "cal pd<num>/il <point_num> [adc] <value>, cal pd<num>/il dump", "cal pd3 2 2034 4.5, cal pd3 dump"},
  {"table_cplt", cmd_table_cplt, "table_cplt", "table_cplt"},

  {"adc", cmd_adc_7828, "adc <channel> read", "adc 0 read"},
  {"eeprom", cmd_eeprom, "eeprom read <addr> <length>, eeprom write <addr> <val>", "eeprom read 0x0 10, eeprom write 0x0 0x10"},
  {"pd", cmd_pd, "pd <pd_num> read", "pd 1 read"},

  {"pin", cmd_pin, "INTERNAL", "INTERNAL"},

  {"ver", cmd_ver, "ver", "ver"},
  {"help", cmd_help, "help", "help"},
};

console_cmd cmdlist_364[] =
{
  {"tag_pn", cmd_tag_pn, "tag_pn write <content>, tag_pn read", "tag_pn write 123456, tag_pn read"},
  {"tag_vendor", cmd_tag_vendor, "tag_vendor write <content>, tag_vendor read", "tag_vendor write o-net, tag_vendor read"},
  {"tag_model", cmd_tag_model, "tag_model write <content>, tag_model read", "tag_model write model-1, tag_model read"},
  {"tag_sn", cmd_tag_sn, "tag_sn write <content>, tag_sn read", "tag_sn write 123456, tag_sn read"},
  {"tag_date", cmd_tag_date, "tag_date write <content>, tag_date read", "tag_date write 20200202, tag_date read"},
  {"table_init", cmd_table_init, "table_init", "table_init"},
  {"file_name", cmd_file_name, "file_name write <name>", "file_name write M+3.PAR"},
  {"file_version", cmd_file_version, "file_version write <year> <> ...", "file_version write 2020 5 24 16 34 0 0"},
  {"cal", cmd_cal, "cal voa<num>/pd<num>/... <point_num> <adc> <value>, cal voa<num>/pd<num>/... dump", "cal voa3 2 2034 4.5, cal voa3 dump"},
  {"table_cplt", cmd_table_cplt, "table_cplt", "table_cplt"},
  {"table_backup", cmd_table_backup, "table_backup", "table_backup"},

  {"dac", cmd_dac, "dac <channel> write <value>, dac read", "dac 1 write 1300, dac read"},
  {"adc", cmd_adc, "adc <channel> read", "adc 0 read"},
  {"eeprom", cmd_eeprom, "eeprom read <addr> <length>, eeprom write <addr> <val>", "eeprom read 0x0 10, eeprom write 0x0 0x10"},
  {"pd", cmd_pd, "pd <pd_num> read", "pd 1 read"},
  {"voa", cmd_voa, "voa <voa_num> read, voa <voa_num> write <val>", "voa 1 read, voa 1 write 12.3"},
  {"switch", cmd_switch, "switch <sw_num> read, switch <sw_num> write <val>", "switch 1 read, switch 1 write 2"},

  {"pin", cmd_pin, "INTERNAL", "INTERNAL"},

  {"ver", cmd_ver, "ver", "ver"},
  {"help", cmd_help, "help", "help"},
};

void get_board_type(const char *buf)
{
  if (!strcmp("517", buf)) {
    board_type = 517;
  } else if (!strcmp("419", buf)) {
    board_type = 419;
  } else if (!strcmp("573", buf)) {
    board_type = 573;
  } else if (!strcmp("503", buf)) {
    board_type = 503;
  } else if (!strcmp("364", buf)) {
    board_type = 364;
  }
}

void board_init()
{
  if (board_type == 517 || board_type == 573) {
    // Same with 364
    tag_init_for_364();
    memcpy(cmdlist, cmdlist_517, sizeof(cmdlist_517));
    eeprom_addr = EEPROM_ADDR_517;
  } else if (board_type == 503) {
    tag_init_for_503();
    memcpy(cmdlist, cmdlist_503, sizeof(cmdlist_503));
    eeprom_addr = EEPROM_ADDR_503;
  } else if (board_type == 364 || board_type == 419) {
    tag_init_for_364();
    memcpy(cmdlist, cmdlist_364, sizeof(cmdlist_364));
    eeprom_addr = EEPROM_ADDR_364;
  }
}

int32_t cmd_ver(int32_t argc, char **argv)
{
  Serial.print("Version: ");
  Serial.println(VER);
  return 0;
}

int32_t cmd_pin(int32_t argc, char **argv)
{
  uint32_t value, pin_addr;
  char *p;

  if (argc < 3 || strlen(argv[1]) < 3) {
    Serial.println(ARG_ERROR);
    return -1;
  }
  p = argv[1];
  if (*p == 'P') {
    if (*(p+1) == 'A') {
      pin_addr = 0;
    } else if (*(p+1) == 'B') {
      pin_addr = 16;
    } else if (*(p+1) == 'C') {
      pin_addr = 32;
    } else {
      Serial.println(ARG_ERROR);
      return -1;
    }
  } else {
    Serial.println(ARG_ERROR);
    return -1;
  }

  value = strtoul(p + 2, NULL, 10);
  if (value > 15 || (*(p + 1) == 'C' && value < 13)) {
    Serial.println(ARG_ERROR);
    return -1;
  }
  pin_addr += *(p + 1) == 'C'? value - 13 : value;
  Serial.print("Pin address = ");
  Serial.print(pin_addr);
  Serial.println("");
  if (argc == 3 && !strcmp(argv[2], "read")) {
    pinMode(pin_addr, INPUT);
    value = digitalRead(pin_addr);
    if (value) {
      Serial.println("Pin is HIGH");
    } else {
      Serial.println("Pin is LOW");
    }
    return 0;
  } else if (argc == 4 && !strcmp(argv[2], "write")) {
    pinMode(pin_addr, OUTPUT);
    value = strtoul(argv[3], NULL, 10);
    if (value) {
      digitalWrite(pin_addr, HIGH);
    } else {
      digitalWrite(pin_addr, LOW);
    }
    Serial.println("OK");
    return 0;
  }

  Serial.println(ARG_ERROR);
  return -1;
}
