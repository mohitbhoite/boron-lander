#include "SHT31.h"

SHT31 sht31 = SHT31();

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("begin...");
  sht31.begin();
}

void loop()
{
  float temp = sht31.getTemperature();
  float hum = sht31.getHumidity();
  Serial.print("Temp = ");
  Serial.print(temp);
  Serial.println(" C"); //The unit for  Celsius because original arduino don't support speical symbols
  Serial.print("Hum = ");
  Serial.print(hum);
  Serial.println("%");
  Serial.println();
  delay(1000);
}
