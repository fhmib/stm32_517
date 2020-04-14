#include "cmd.h"
#include "dac.h"
#include "adc.h"
#include "cal.h"
#include "pd.h"
#include "eeprom.h"

#define LED_PORT PB12
#define VER "0.0.4"
#define LED_HOLD_TIME 500 // ms

unsigned int led_count;
String cmd = "";
char c;
int board_type = -1;

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
  Serial.println("Supported boards: 503 517 ");
 //Serial.print(">");
  Serial.print("Please input board type > ");
}

#undef PRIVATE_DEBUG
#if defined(PRIVATE_DEBUG)
uint32 counter = 0;
byte buf[32];
unsigned int val;
#endif

void loop()
{
#if !defined(PRIVATE_DEBUG)
  while (Serial.available() > 0)
  {
    c = char(Serial.read());

    if ((c == '\n') || (c == '\r')) {
      Serial.println("");
      if (board_type == -1) {
        get_board_type(cmd.c_str());
        if (board_type == -1) {
          Serial.println("");
          Serial.println("Supported boards: 503 517 ");
          Serial.print("Please input board type > ");
        } else {
          Serial.print("Adding commands for ");
          Serial.print(board_type, DEC);
          Serial.println("...");
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
#else
  ++counter;
  if (counter == 3000) {
    i2c_eeprom_write_byte(EEPROM_ADDR, 0x55, 0xAA);
    Serial.println("OP1 : Writing 0xAA to 0x55 of EEPROM");
    Serial.println("");
  } else if (counter == 6000) {
    i2c_eeprom_read_buffer2(EEPROM_ADDR, 0x55, buf, 1);
    Serial.print("OP2 : Read address 0x55 of EEPROM is 0x");
    Serial.println(buf[0], HEX);
    Serial.println("");
  } else if (counter == 9000) {
    dac_write(DAC_ADDR, DAC_CHANNEL_A, 2020);
    Serial.println("OP3 : Writing 2020(0x7E4) to channel A of EEPROM");
    Serial.println("");
  } else if (counter == 12000) {
    dac_read(DAC_ADDR, buf);
    Serial.print("OP4 : Sequential read 3 bytes from dac: 0x");
    Serial.print(buf[0], HEX);
    Serial.print(" 0x");
    Serial.print(buf[1], HEX);
    Serial.print(" 0x");
    Serial.println(buf[2], HEX);
    Serial.println("");
  } else if (counter == 15000) {
    dac_read2(DAC_ADDR, DAC_CHANNEL_A, val);
    Serial.print("OP5 : Combined read channel A from dac : ");
    Serial.println(val, DEC);
    Serial.println("");
  } else if (counter == 18000) {
    dac_read2(DAC_ADDR, DAC_CHANNEL_B, val);
    Serial.print("OP6 : Combined read channel B from dac : ");
    Serial.println(val, DEC);
    Serial.println("");
  } else if (counter == 21000) {
    Serial.print("OP7 : Sequential read 3 bytes from dac: 0x");
    Serial.print(buf[0], HEX);
    Serial.print(" 0x");
    Serial.print(buf[1], HEX);
    Serial.print(" 0x");
    Serial.println(buf[2], HEX);
    Serial.println("");
    counter = 0;
  }
#endif

  ++led_count;
  if (led_count == LED_HOLD_TIME) {
    digitalWrite(LED_PORT, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else if (led_count == LED_HOLD_TIME * 2) {
    digitalWrite(LED_PORT, LOW);    // turn the LED off by making the voltage LOW
    led_count = 0;
  }
  delay(1);

}

console_cmd cmdlist[16];

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

void get_board_type(const char *buf)
{
  if (!strcmp("517", buf)) {
    board_type = 517;
    memcpy(cmdlist, cmdlist_517, sizeof(cmdlist_517));
  } else if (!strcmp("503", buf)) {
    board_type = 503;
    memcpy(cmdlist, cmdlist_503, sizeof(cmdlist_503));
  }
}

int cmd_ver(int argc, char **argv)
{
  Serial.print("Version: ");
  Serial.println(VER);
  return 0;
}
