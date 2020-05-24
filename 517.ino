#include "cmd.h"
#include "dac.h"
#include "adc.h"
#include "cal.h"
#include "pd.h"
#include "eeprom.h"

#define LED_PORT PB12
#define VER "0.0.7"
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

  Serial.println("");
  Serial.print("Version: ");
  Serial.println(VER);
  Serial.println("COM is ready");
  Serial.println("");
  Serial.println("Supported boards: 503 517 364");
 //Serial.print(">");
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
          Serial.println("Supported boards: 503 517 364");
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

console_cmd cmdlist[20];

console_cmd cmdlist_517[] =
{
  {"dac", cmd_dac, "dac <channel> write <value>, dac read", "dac 1 write 1300, dac read"},
  {"adc", cmd_adc, "adc <channel> read", "adc 0 read"},
  {"eeprom", cmd_eeprom, "eeprom read <addr> <length>, eeprom write <addr> <val>", "eeprom read 0x0 10, eeprom write 0x0 0x10"},
  {"pd", cmd_pd, "pd <pd_num> read", "pd 1 read"},
  {"voa", cmd_voa, "voa <voa_num> read, voa <voa_num> write", "voa 1 read, voa 1 write 12.3"},
  {"ver", cmd_ver, "ver", "ver"},
  {"help", cmd_help, "help", "help"},
};

console_cmd cmdlist_503[] =
{
  {"adc", cmd_adc_7828, "adc <channel> read", "adc 0 read"},
  {"eeprom", cmd_eeprom, "eeprom read <addr> <length>, eeprom write <addr> <val>", "eeprom read 0x0 10, eeprom write 0x0 0x10"},
  {"pd", cmd_pd, "pd <pd_num> read", "pd 1 read"},
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
  {"table_init", cmd_table_init, "cmd_table_init", "cmd_table_init"},
  {"file_name", cmd_file_name, "file_name write <name>", "file_name write M+3.PAR"},
  {"file_version", cmd_file_version, "file_version write <year> <> ...", "file_version write 2020 5 24 16 34 0 0"},
  {"cal", cmd_cal, "cal voa<num>/pd<num> <adc> <value>, cal voa<num>/pd<num> dump", "cal voa3 2 2034 4.5, cal voa3 dump"},
  
  {"dac", cmd_dac, "dac <channel> write <value>, dac read", "dac 1 write 1300, dac read"},
  {"adc", cmd_adc, "adc <channel> read", "adc 0 read"},
  {"eeprom", cmd_eeprom, "eeprom read <addr> <length>, eeprom write <addr> <val>", "eeprom read 0x0 10, eeprom write 0x0 0x10"},
  {"pd", cmd_pd, "pd <pd_num> read", "pd 1 read"},
  {"voa", cmd_voa, "voa <voa_num> read, voa <voa_num> write <val>", "voa 1 read, voa 1 write 12.3"},
  {"switch", cmd_switch, "switch <sw_num> read, switch <sw_num> write <val>", "switch 1 read, switch 1 write 2"},
  //{"cali", cmd_cal_init_364, "cali", "cali"},
  {"ver", cmd_ver, "ver", "ver"},
  {"help", cmd_help, "help", "help"},
};

void get_board_type(const char *buf)
{
  if (!strcmp("517", buf)) {
    board_type = 517;
  } else if (!strcmp("503", buf)) {
    board_type = 503;
  } else if (!strcmp("364", buf)) {
    board_type = 364;
  }
}

void board_init()
{
  if (board_type == 517) {
    memcpy(cmdlist, cmdlist_517, sizeof(cmdlist_517));
    eeprom_addr = EEPROM_ADDR_517;
  } else if (board_type == 503) {
    memcpy(cmdlist, cmdlist_503, sizeof(cmdlist_503));
    eeprom_addr = EEPROM_ADDR_503;
  } else if (board_type == 364) {
    memcpy(cmdlist, cmdlist_364, sizeof(cmdlist_364));
    tag_init_for_364();
    eeprom_addr = EEPROM_ADDR_364;
  }
}

int32_t cmd_ver(int32_t argc, char **argv)
{
  Serial.print("Version: ");
  Serial.println(VER);
  return 0;
}
