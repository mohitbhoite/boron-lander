#ifndef _SHT31_H_
#define _SHT31_H_

#include "Arduino.h"
#include "Wire.h"

#define SHT31_ADDR    0x44
#define SHT31_MEAS_HIGHREP_STRETCH 0x2C06
#define SHT31_MEAS_MEDREP_STRETCH  0x2C0D
#define SHT31_MEAS_LOWREP_STRETCH  0x2C10
#define SHT31_MEAS_HIGHREP         0x2400
#define SHT31_MEAS_MEDREP          0x240B
#define SHT31_MEAS_LOWREP          0x2416
#define SHT31_READSTATUS           0xF32D
#define SHT31_CLEARSTATUS          0x3041
#define SHT31_SOFTRESET            0x30A2
#define SHT31_HEATEREN             0x306D
#define SHT31_HEATERDIS            0x3066

class SHT31 {
  public:
    SHT31();    
    boolean begin(uint8_t i2caddr = SHT31_ADDR);
    float getTemperature(void);
    float getHumidity(void);
    uint16_t readStatus(void);
    void reset(void);
    void heater(boolean);
    uint8_t crc8(const uint8_t *data, int len);

  private:
    boolean getTempHum(void);
    void writeCommand(uint16_t cmd);

    uint8_t _i2caddr;
    float humidity, temp;
    
};

#endif