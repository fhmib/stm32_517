#include "cal.h"
#include "eeprom.h"

#define AM1_PIN_EEPROM_ADDR 0x11DD
#define VOA1_TM1_EEPROM_ADDR 0x15E5
#define VOA2_TM2_EEPROM_ADDR 0x181D

//                                          1     2     3     4     5     6     7     8     9     10
static unsigned int am1_pin_adc[10]    = { 711,  941,  1177, 1417, 1658, 1898, 2139, 2380, 3333, 3524 };
static int am1_pin_power[10]           = { -350, -300, -250, -200, -150, -100, -50,  0,    200,  240  };
static unsigned int voa1_tm1_adc[51]   = { 2650, 2490, 2445, 2385, 2343, 2310, 2276, 2244, 2213, 2185,
                                           2158, 2132, 2106, 2082, 2058, 2034, 2012, 1989, 1967, 1946,
                                           1924, 1903, 1882, 1862, 1841, 1821, 1801, 1781, 1761, 1741,
                                           1721, 1701, 1681, 1661, 1641, 1622, 1601, 1581, 1561, 1541,
                                           1520, 1499, 1478, 1456, 1434, 1412, 1389, 1366, 1342, 1318,
                                           1292                                                       };
static int voa1_tm1_IL[51]             = { 0,    5,    10,   15,   20,   25,   30,   35,   40,   45,
                                           50,   55,   60,   65,   70,   75,   80,   85,   90,   95,
                                           100,  105,  110,  115,  120,  125,  130,  135,  140,  145,
                                           150,  155,  160,  165,  170,  175,  180,  185,  190,  195,
                                           200,  205,  210,  215,  220,  225,  230,  235,  240,  245,
                                           250                                                        };
static unsigned int voa2_tm2_adc[51]   = { 2640, 2505, 2470, 2432, 2391, 2357, 2330, 2306, 2282, 2258,
                                           2235, 2213, 2192, 2172, 2153, 2134, 2115, 2097, 2079, 2062,
                                           2044, 2028, 2011, 1995, 1979, 1963, 1947, 1932, 1917, 1902,
                                           1887, 1872, 1857, 1843, 1828, 1814, 1800, 1786, 1772, 1758,
                                           1744, 1730, 1717, 1703, 1690, 1676, 1663, 1649, 1636, 1623,
                                           1609                                                       };
static int voa2_tm2_IL[51]             = { 0,    5,    10,   15,   20,   25,   30,   35,   40,   45,
                                           50,   55,   60,   65,   70,   75,   80,   85,   90,   95,
                                           100,  105,  110,  115,  120,  125,  130,  135,  140,  145,
                                           150,  155,  160,  165,  170,  175,  180,  185,  190,  195,
                                           200,  205,  210,  215,  220,  225,  230,  235,  240,  245,
                                           250                                                        };
#if 1
int cmd_cal_init_517(int argc, char **argv)
{
  byte buf[512];
  int i, count;

  // Write AM1_PIN to EEPROM
  for (i = 0, count = 0; i < 10; ++i) {
    buf[i * 8 + 0] = am1_pin_adc[i] >> 24;
    buf[i * 8 + 1] = am1_pin_adc[i] >> 16;
    buf[i * 8 + 2] = am1_pin_adc[i] >> 8;
    buf[i * 8 + 3] = am1_pin_adc[i] >> 0;
    buf[i * 8 + 4] = am1_pin_power[i] >> 24;
    buf[i * 8 + 5] = am1_pin_power[i] >> 16;
    buf[i * 8 + 6] = am1_pin_power[i] >> 8;
    buf[i * 8 + 7] = am1_pin_power[i] >> 0;
    count += 8;
  }
  i2c_eeprom_write_buffer(EEPROM_ADDR_517, AM1_PIN_EEPROM_ADDR, buf, count);

  // Write VOA1_TM1 to EEPROM
  for (i = 0, count = 0; i < 51; ++i) {
    buf[i * 8 + 0] = voa1_tm1_adc[i] >> 24;
    buf[i * 8 + 1] = voa1_tm1_adc[i] >> 16;
    buf[i * 8 + 2] = voa1_tm1_adc[i] >> 8;
    buf[i * 8 + 3] = voa1_tm1_adc[i] >> 0;
    buf[i * 8 + 4] = voa1_tm1_IL[i] >> 24;
    buf[i * 8 + 5] = voa1_tm1_IL[i] >> 16;
    buf[i * 8 + 6] = voa1_tm1_IL[i] >> 8;
    buf[i * 8 + 7] = voa1_tm1_IL[i] >> 0;
    count += 8;
  }
  i2c_eeprom_write_buffer(EEPROM_ADDR_517, VOA1_TM1_EEPROM_ADDR, buf, count);

  // Write VOA2_TM2 to EEPROM
  for (i = 0, count = 0; i < 51; ++i) {
    buf[i * 8 + 0] = voa2_tm2_adc[i] >> 24;
    buf[i * 8 + 1] = voa2_tm2_adc[i] >> 16;
    buf[i * 8 + 2] = voa2_tm2_adc[i] >> 8;
    buf[i * 8 + 3] = voa2_tm2_adc[i] >> 0;
    buf[i * 8 + 4] = voa2_tm2_IL[i] >> 24;
    buf[i * 8 + 5] = voa2_tm2_IL[i] >> 16;
    buf[i * 8 + 6] = voa2_tm2_IL[i] >> 8;
    buf[i * 8 + 7] = voa2_tm2_IL[i] >> 0;
    count += 8;
  }
  i2c_eeprom_write_buffer(EEPROM_ADDR_517, VOA2_TM2_EEPROM_ADDR, buf, count);

  return 0;
}
static unsigned int pd_addr_364[] = { 0x11DD, 0x12CD, 0x13BD, 0x14AD, 0x159D};
static unsigned int voa_addr_364[] = { 0x271D, 0x2955, 0x1C05, 0x2075, 0x22AD, 0x24E5, 0x19CD, 0x1E3D};

static unsigned int pd_adc_364[10]    = { 711,  941,  1177, 1417, 1658, 1898, 2139, 2380, 3333, 3524 };
static int pd_power_364[10]                  = { -350, -300, -250, -200, -150, -100, -50,  0,    200,  240  };

static unsigned int voa_adc_364[51]    = { 2640, 2505, 2470, 2432, 2391, 2357, 2330, 2306, 2282, 2258,
                                           2235, 2213, 2192, 2172, 2153, 2134, 2115, 2097, 2079, 2062,
                                           2044, 2028, 2011, 1995, 1979, 1963, 1947, 1932, 1917, 1902,
                                           1887, 1872, 1857, 1843, 1828, 1814, 1800, 1786, 1772, 1758,
                                           1744, 1730, 1717, 1703, 1690, 1676, 1663, 1649, 1636, 1623,
                                           1609                                                       };
static int voa_power_364[51]           = { 0,    5,    10,   15,   20,   25,   30,   35,   40,   45,
                                           50,   55,   60,   65,   70,   75,   80,   85,   90,   95,
                                           100,  105,  110,  115,  120,  125,  130,  135,  140,  145,
                                           150,  155,  160,  165,  170,  175,  180,  185,  190,  195,
                                           200,  205,  210,  215,  220,  225,  230,  235,  240,  245,
                                           250                                                        };

int cmd_cal_init_364(int argc, char **argv)
{
  byte buf[512];
  int i, count;

  // Write pd cali to EEPROM
  for (i = 0, count = 0; i < 10; ++i) {
    buf[i * 8 + 0] = pd_adc_364[i] >> 24;
    buf[i * 8 + 1] = pd_adc_364[i] >> 16;
    buf[i * 8 + 2] = pd_adc_364[i] >> 8;
    buf[i * 8 + 3] = pd_adc_364[i] >> 0;
    buf[i * 8 + 4] = pd_power_364[i] >> 24;
    buf[i * 8 + 5] = pd_power_364[i] >> 16;
    buf[i * 8 + 6] = pd_power_364[i] >> 8;
    buf[i * 8 + 7] = pd_power_364[i] >> 0;
    count += 8;
  }
  for (i = 0; i < sizeof(pd_addr_364)/sizeof(pd_addr_364[0]); ++i) {
    i2c_eeprom_write_buffer(EEPROM_ADDR_517, pd_addr_364[i], buf, count);
  }

  // Write voa cali to EEPROM
  for (i = 0, count = 0; i < 51; ++i) {
    buf[i * 8 + 0] = voa_adc_364[i] >> 24;
    buf[i * 8 + 1] = voa_adc_364[i] >> 16;
    buf[i * 8 + 2] = voa_adc_364[i] >> 8;
    buf[i * 8 + 3] = voa_adc_364[i] >> 0;
    buf[i * 8 + 4] = voa_power_364[i] >> 24;
    buf[i * 8 + 5] = voa_power_364[i] >> 16;
    buf[i * 8 + 6] = voa_power_364[i] >> 8;
    buf[i * 8 + 7] = voa_power_364[i] >> 0;
    count += 8;
  }
  for (i = 0; i < sizeof(pd_addr_364)/sizeof(pd_addr_364[0]); ++i) {
    i2c_eeprom_write_buffer(EEPROM_ADDR_517, voa_addr_364[i], buf, count);
  }

  return 0;
}

#endif
