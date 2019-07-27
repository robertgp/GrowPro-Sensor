#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_Si7021.h"
#include "Adafruit_TSL2591.h"


SoftwareSerial mySerial(10, 11);
Adafruit_Si7021 sensor = Adafruit_Si7021(); //  identifier for temp/humidity sensor
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

void configureSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  //tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  // tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
   //tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */  
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
  switch(gain)
  {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print  (F("Timing:       "));
  Serial.print((tsl.getTiming() + 1) * 100, DEC); 
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}


void setup() {

  Serial.begin(115200);
  mySerial.begin(57600);
  sensor.begin();
  configureSensor();

  
  // put your setup code here, to run once:

}

void loop() {
  
  sensor.begin();
  configureSensor(); 
  delay(1000);
  int moisture;
  moisture = analogRead(0);

  delay(1000);
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  delay(1000);
  String temperature = String(sensor.readTemperature());
  String humidity = String(sensor.readHumidity());
  String light = String(tsl.calculateLux(full, ir));
  delay(1000);
  mySerial.print(temperature);
  mySerial.print("&");
  mySerial.print(humidity);
  mySerial.print("%");
  mySerial.print(light); 
  mySerial.print("$");
  mySerial.print(moisture);
  mySerial.print("#");
  //Serial.println("TESTING ARDUINO");
  //delay(1000);
  while (mySerial.available()) {
    Serial.write(mySerial.read());

    
  }
  delay(120000);
  
  
}

