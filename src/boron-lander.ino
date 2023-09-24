/*
 * Project Boron Lander
 * Description:
 * Author: Mohit Bhoite
 * Date: 01 Sept 2023
 */
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Orbitron_Medium_40.h> 
#include <Orbitron_Bold_20.h>
#include <Roboto_Condensed_Light_Italic_24.h>
#include <SHT31.h>
#include <weather_icons.h>


//SYSTEM_MODE(MANUAL);
//SYSTEM_THREAD(ENABLED);
//SerialLogHandler logHandler(LOG_LEVEL_TRACE);


//RGB565 custom colors
#define COLOR_PINK 0xf816
#define COLOR_LIME 0x87e0
#define COLOR_PEACH 0xfd89

/****************
*****************
Pin assignment
*****************
Display   Boron
SDA       MOSI
SCL       SCK
RES       A5
DC        A4
*****************
*****************/
#define TFT_CS  -1  // The display I'm using does not have a CS pin, if yours does, assign a pin to it here
#define TFT_RST A5 
#define TFT_DC  A4

#define LED_BLUE  D7
#define LED_RED A0

// For 1.14", 1.3", 1.54", 1.69", and 2.0" TFT with ST7789:
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
SHT31 sht31 = SHT31();
//If you are using the Photon 2, please comment out the following since Photon 2 does not have a fuel gauge
FuelGauge fuel;

char num = 0;
char number[8];

const float Pi = 3.14159265359; 

GFXcanvas16 timeCanvas(240,110);
GFXcanvas16 temperatureCanvas(240,40);

String todayHigh,todayLow,dayoneHigh,dayoneLow,daytwoHigh,daytwoLow,daythreeHigh,daythreeLow;
String todayForecast,dayoneForecast,daytwoForecast,daythreeForecast;

unsigned long previousMillis = 0;  // will store last time LED was updated
// constants won't change:
const long interval = 1000;  // milliiseconds
const long pubinterval = 600; //interval at which to publish in seconds. this is done by counting number of intervals
unsigned long intervalcount = 0;

void setup() 
{
  //Subscribe to the Particle webhook response
  Particle.subscribe("hook-response/get_night_weather", getNightWeather);
  Particle.subscribe("hook-response/get_day_weather", getDayWeather);
  Particle.subscribe("hook-response/get_suntime", getSunriseSunset);

  //set time zone to pacific standard time
  Time.zone(-7);

  pinMode(LED_RED,OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  Serial.begin(9600);
  //enable power to the sht31 sensor
  //I've connected the power pin of the SHT31 to D3 but you can directly connect it to 3.3V
  pinMode(D3,OUTPUT);
  digitalWrite(D3,HIGH);

  //remember to modify SPI setting as necessary for your MCU board in Adafruit_ST77xx.cpp

  tft.init(240,240,SPI_MODE3);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);

  sht31.begin();

  // draw the static icons
  tft.drawRGBBitmap(15,135,sunrise,40,31);
  tft.drawRGBBitmap(85,135,sunset,40,31);
  tft.drawRGBBitmap(160,128,isro,68,66);

  // draw the horizontal console dividing lines
  tft.drawLine(0,120,240,120,0x8c71);//gray
  tft.drawLine(0,199,240,199,0x8c71);//gray

  // display the time and temperature and then update it regularly in the loop function
  drawTimeConsole();
  drawTemperatureConsole();

  String data = String(10); //dummy data
  // publish to trigger the webhooks
  Particle.publish("get_night_weather", data, PRIVATE);
  Particle.publish("get_day_weather", data, PRIVATE);
  Particle.publish("get_suntime", data, PRIVATE);
}

void loop(void) 
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
    drawTimeConsole();
    drawTemperatureConsole();

    ledBlink();
    intervalcount++;
  }
  if(intervalcount>pubinterval)
  {
    intervalcount = 0;
    String data = String(10);
    Particle.publish("get_day_weather", data, PRIVATE);
    Particle.publish("get_night_weather", data, PRIVATE);
  }
 }

//The webhook response data will contain the sunrise and sunset time together as a string
void getSunriseSunset(const char *event, const char *data) 
{
  tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
  tft.setFont();
  tft.setTextSize(2);
  tft.setCursor(5,175);
  tft.print(data);
}

String numtoWeekday(int num)
{
  switch (num)
  {
  case 1:
    return ("SUN");
  case 2:
    return("MON");
  case 3:
    return("TUE");
  case 4:
    return("WED");
  case 5:
   return("THU");
  case 6:
    return("FRI");
  case 7:
    return("SAT");
  }
  return "NULL";
}

void drawTimeConsole(void)
{

    uint8_t hour = Time.hourFormat12();
    uint8_t minute = Time.minute();
    uint8_t second = Time.second();
    uint8_t weekday = Time.weekday();
    uint8_t day = Time.day();
    uint16_t year = Time.year();
    uint8_t month = Time.month();
    char timestring[10];

  timeCanvas.fillScreen(ST77XX_BLACK);

  timeCanvas.setFont(&Roboto_Condensed_Light_Italic_24);
  timeCanvas.setTextSize(1);
  timeCanvas.setTextColor(ST77XX_CYAN);
  timeCanvas.setCursor(0,30);
  //timeCanvas.print("WED,19 APR,2023"); //sample string for testing
  timeCanvas.print(numtoWeekday(weekday));
  timeCanvas.print(",");
  switch (month)
  {
  case 1:
    timeCanvas.print("JAN ");
    break;
  case 2:
    timeCanvas.print("FEB ");
    break;
  case 3:
    timeCanvas.print("MAR ");
    break;
  case 4:
    timeCanvas.print("APR ");
    break;
  case 5:
    timeCanvas.print("MAY ");
    break;
  case 6:
    timeCanvas.print("JUN ");
    break;
  case 7:
    timeCanvas.print("JUL ");
    break;
  case 8:
    timeCanvas.print("AUG ");
    break;
  case 9:
    timeCanvas.print("SEPT ");
    break;
  case 10:
    timeCanvas.print("OCT ");
    break;
  case 11:
    timeCanvas.print("NOV ");
    break;
  case 12:
    timeCanvas.print("DEC ");
    break;
  }
  timeCanvas.print(day);
  timeCanvas.print(",");
  timeCanvas.print(year);
  
  timeCanvas.setFont();
  timeCanvas.setTextSize(1);
  timeCanvas.setCursor(205,14);

  timeCanvas.fillRect(210,0,25,10,ST77XX_BLUE);
  timeCanvas.fillRect(207,3,3,4,ST77XX_BLUE);
  timeCanvas.setTextColor(ST77XX_WHITE);
  //If you are using the Photon 2, please comment out the following since Photon 2 does not have a fuel gauge
  timeCanvas.print(fuel.getVCell());timeCanvas.print("V");

  timeCanvas.setFont(&Orbitron_Medium_40);
  timeCanvas.setTextSize(1);
  timeCanvas.setTextColor(ST77XX_GREEN);

  timeCanvas.setCursor(0,75);
  sprintf(timestring, "%02d:%02d ", hour, minute);
  //sprintf(timestring, "%02d:%02d ", 2, 2); //test string that is the widest when displayed
  timeCanvas.print(timestring);
  //calculate the width of the time string so that you can appent the AM/PM at the end of it
  //this is required because the font width is not the same for all digits
  int16_t  x1, y1;
  uint16_t w, h;
  timeCanvas.getTextBounds(timestring,0,75,&x1,&y1,&w,&h);


  timeCanvas.setFont(&Orbitron_Bold_20);
  timeCanvas.setTextSize(1);
  timeCanvas.setTextColor(0xe469);//orange
  timeCanvas.setCursor(w,60);
  if (Time.isPM())  timeCanvas.print(" PM");
  else  timeCanvas.print(" AM");

  timeCanvas.setFont(&Roboto_Condensed_Light_Italic_24);
  timeCanvas.setTextSize(1);
  timeCanvas.setTextColor(COLOR_PINK);
  timeCanvas.setCursor(0,110);
  timeCanvas.print("SAN FRANCISCO");

  drawWeatherBitmap(192,45,todayForecast);
  
  timeCanvas.setFont();
  timeCanvas.setTextSize(2);
  timeCanvas.setTextColor(ST77XX_WHITE);
  
  if (todayHigh == "  ") 
  {
    timeCanvas.setCursor(205,95);
    timeCanvas.print(todayLow);
  }
  else
  {
    timeCanvas.setCursor(180,95);
    timeCanvas.print(todayHigh); timeCanvas.print("|");timeCanvas.print(todayLow);
  }
  
  tft.drawRGBBitmap(0, 0, timeCanvas.getBuffer(), timeCanvas.width(), timeCanvas.height());

}

void drawTemperatureConsole(void)
{
  float temp = ((sht31.getTemperature())*1.8)+32;
  float hum = sht31.getHumidity();

  temperatureCanvas.fillScreen(ST77XX_BLACK);
  temperatureCanvas.drawRGBBitmap(5,10,thermometer,15,30);

  temperatureCanvas.setTextSize(1);
  temperatureCanvas.setFont(&Roboto_Condensed_Light_Italic_24);
  temperatureCanvas.setTextColor(ST77XX_WHITE,ST77XX_BLACK);
  temperatureCanvas.setCursor(30,30);
  temperatureCanvas.print(temp);temperatureCanvas.print("F");
  temperatureCanvas.setCursor(140,30);
  temperatureCanvas.print(hum);temperatureCanvas.print("%rH");

  tft.drawRGBBitmap(0, 200, temperatureCanvas.getBuffer(), temperatureCanvas.width(), temperatureCanvas.height());

}

void getNightWeather(const char *event, const char *night_data)
{
  int index0,index1,index2,index3,index4,index6,index5,index7,index8,index9,index10,index11,index12;
  String name;
  String weatherString = String(night_data);
  Serial.println(night_data);

  index0 = weatherString.indexOf('~');
  name = weatherString.substring(0,index0);
  Serial.println(name);
  if(name == "Tonight") 
  {
    Serial.println(night_data);
    todayHigh = "  ";
    index1 = weatherString.indexOf('~',index0+1);
    todayLow = weatherString.substring(index0+1,index1);

    index2 = weatherString.indexOf('~',index1+1);
    todayForecast = weatherString.substring(index1+1,index2);

    index3 = weatherString.indexOf('~',index2+1);
    dayoneHigh = weatherString.substring(index2+1,index3);

    index4 = weatherString.indexOf('~',index3+1);
    dayoneForecast = weatherString.substring(index3+1,index4);

    index5 = weatherString.indexOf('~',index4+1);
    dayoneLow = weatherString.substring(index4+1,index5);

    index6 = weatherString.indexOf('~',index5+1);
    daytwoHigh = weatherString.substring(index5+1,index6);

    index7 = weatherString.indexOf('~',index6+1);
    daytwoForecast = weatherString.substring(index6+1,index7);

    index8 = weatherString.indexOf('~',index7+1);
    daytwoLow = weatherString.substring(index7+1,index8);

    index9 = weatherString.indexOf('~',index8+1);
    daythreeHigh = weatherString.substring(index8+1,index9);

    index10 = weatherString.indexOf('~',index9+1);
    daythreeForecast = weatherString.substring(index9+1,index10);

    index11 = weatherString.indexOf('~',index10+1);
    daythreeLow = weatherString.substring(index10+1,index11);
  }
  else Serial.println("Not tonight");

 
}

void getDayWeather(const char *event, const char *day_data)
{
  int index0,index1,index2,index3,index4,index6,index5,index7,index8,index9,index10,index11,index12;
  String name;
  String weatherString = String(day_data);
  Serial.println(day_data);

  index0 = weatherString.indexOf('~');
  name = weatherString.substring(0,index0);
  Serial.println(name);
  if((name == "Today") || (name == "This Afternoon"))
  {
    Serial.println(day_data);
    index1 = weatherString.indexOf('~',index0+1);
    todayHigh = weatherString.substring(index0+1,index1);

    index2 = weatherString.indexOf('~',index1+1);
    todayForecast = weatherString.substring(index1+1,index2);

    index3 = weatherString.indexOf('~',index2+1);
    todayLow = weatherString.substring(index2+1,index3);

    index4 = weatherString.indexOf('~',index3+1);
    dayoneHigh = weatherString.substring(index3+1,index4);

    index5 = weatherString.indexOf('~',index4+1);
    dayoneForecast = weatherString.substring(index4+1,index5);

    index6 = weatherString.indexOf('~',index5+1);
    dayoneLow = weatherString.substring(index5+1,index6);

    index7 = weatherString.indexOf('~',index6+1);
    daytwoHigh = weatherString.substring(index6+1,index7);

    index8 = weatherString.indexOf('~',index7+1);
    daytwoForecast= weatherString.substring(index7+1,index8);

    index9 = weatherString.indexOf('~',index8+1);
    daytwoLow  = weatherString.substring(index8+1,index9);

    index10 = weatherString.indexOf('~',index9+1);
    daythreeHigh = weatherString.substring(index9+1,index10);

    index11 = weatherString.indexOf('~',index10+1);
    daythreeForecast = weatherString.substring(index10+1,index11);

    index12 = weatherString.indexOf('~',index11+1);
    daythreeLow = weatherString.substring(index11+1,index12);
  }
  else Serial.println("Not today");

 
}

void drawWeatherBitmap(int x,int y,String forecast)
{
  if(!strcmp(forecast,"Sunny"))
  timeCanvas.drawRGBBitmap(x,y,sunny,40,40);

  if(!strcmp(forecast,"Mostly Sunny"))
  timeCanvas.drawRGBBitmap(x,y,partial_cloudy,40,40);

  else if(!strcmp(forecast,"Clear"))
  timeCanvas.drawRGBBitmap(x,y,moon,40,40);

  else if(!strcmp(forecast,"Mostly Clear"))
  timeCanvas.drawRGBBitmap(x,y,moon,40,40);

  else if(!strcmp(forecast,"Partly Cloudy"))
  timeCanvas.drawRGBBitmap(x,y,cloudy,40,40);

  else if(forecast.indexOf("Cloudy") > 0)
  timeCanvas.drawRGBBitmap(x,y,cloud,40,40);

  else if(!strcmp(forecast,"Showers"))
  timeCanvas.drawRGBBitmap(x,y,rain,40,40);

  else if(!strcmp(forecast,"Windy"))
  timeCanvas.drawRGBBitmap(x,y,windy,40,40);

  else if(forecast.indexOf("Rain") > 0)
  timeCanvas.drawRGBBitmap(x,y,rain,40,40);

  else if(forecast.indexOf("thunder") > 0)
  timeCanvas.drawRGBBitmap(x,y,rain_thunder,40,40);

  else if(forecast.indexOf("Snow") > 0)
  timeCanvas.drawRGBBitmap(x,y,snow,40,40);

  else if(forecast.indexOf("Overcast") > 0)
  timeCanvas.drawRGBBitmap(x,y,cloud,40,40);

}

void ledBlink(void)
{
  digitalWrite(LED_BLUE,LOW);
  digitalWrite(LED_RED,LOW);

  for(int r=0;r<4;r++)
  {
    digitalWrite(LED_RED,HIGH);
    delay(30);
    digitalWrite(LED_RED,LOW);
    delay(30);
  }

  for(int b=0;b<4;b++)
  {
    digitalWrite(LED_BLUE,HIGH);
    delay(30);
    digitalWrite(LED_BLUE,LOW);
    delay(30);
  }
}
