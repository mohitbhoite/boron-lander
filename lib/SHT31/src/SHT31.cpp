#include "SHT31.h"

SHT31::SHT31() {
}

boolean SHT31::begin(uint8_t i2caddr) {
  Wire.begin();
  _i2caddr = i2caddr;
  reset();
  //return (readStatus() == 0x40);
  return true;
}

float SHT31::getTemperature(void) {
  if (! getTempHum()) return NAN;
  return temp;
}


float SHT31::getHumidity(void) {
  if (! getTempHum()) return NAN;
  return humidity;
}

uint16_t SHT31::readStatus(void) {  
  return 0;
}

void SHT31::reset(void) {
  writeCommand(SHT31_SOFTRESET);
  delay(10);
}

void SHT31::heater(boolean h) {
  if (h)
    writeCommand(SHT31_HEATEREN);
  else
    writeCommand(SHT31_HEATERDIS);
}

uint8_t SHT31::crc8(const uint8_t *data, int len) {
  const uint8_t POLYNOMIAL(0x31);
  uint8_t crc(0xFF);
  
  for ( int j = len; j; --j ) {
      crc ^= *data++;

      for ( int i = 8; i; --i ) {
	crc = ( crc & 0x80 )
	  ? (crc << 1) ^ POLYNOMIAL
	  : (crc << 1);
      }
  }
  return crc; 
}


boolean SHT31::getTempHum(void) {
  uint8_t readbuffer[6];

  writeCommand(SHT31_MEAS_HIGHREP);
  
  delay(50);
  Wire.requestFrom(_i2caddr, (uint8_t)6);
  if (Wire.available() != 6) 
    return false;
  for (uint8_t i=0; i<6; i++) {
    readbuffer[i] = Wire.read();
  }
  uint16_t ST, SRH;
  ST = readbuffer[0];
  ST <<= 8;
  ST |= readbuffer[1];

  if (readbuffer[2] != crc8(readbuffer, 2)) return false;

  SRH = readbuffer[3];
  SRH <<= 8;
  SRH |= readbuffer[4];

  if (readbuffer[5] != crc8(readbuffer+3, 2)) return false;
 
  double stemp = ST;
  stemp *= 175;
  stemp /= 0xffff;
  stemp = -45 + stemp;
  temp = stemp;
  
  double shum = SRH;
  shum *= 100;
  shum /= 0xFFFF;
  
  humidity = shum;
  
  return true;
}

void SHT31::writeCommand(uint16_t cmd) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(cmd >> 8);
  Wire.write(cmd & 0xFF);
  Wire.endTransmission();      
}
