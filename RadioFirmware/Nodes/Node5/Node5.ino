// rf24_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF24 class. RH_RF24 class does not provide for addressing or
// reliability, so you should only use RH_RF24 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf24_server.
// Tested on Anarduino Mini http://www.anarduino.com/mini/ with RFM24W and RFM26W

#include <SPI.h>
#include <RH_NRF24.h>
//#include "Adafruit_TSL2591.h"
#include "Si7021.h" 
Si7021 si7021;

// Singleton instance of the radio driver
RH_NRF24 rf24(7, 8);
int soilPin = 0;
//Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

char incomingData = 0;
struct packet {
  float temperature;
  float humidity;
  float soilMoisture;
  float nodeNumber = 5.0;
};

typedef struct packet Packet;

Packet data;

/*
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

  // Display the gain and integration time for reference sake  
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
*/

void setup()
{
  uint64_t serialNumber = 0ULL;
  
  Serial.begin(9600);

  si7021.begin(); // These two lines are for the temp/hum sensor
  serialNumber = si7021.getSerialNumber(); // temp/hum
  
  pinMode(3, OUTPUT);
  if (!rf24.init())
    Serial.println("init failed");
}


void loop()
{

  if (Serial.available()) {
     incomingData = Serial.read();
     digitalWrite(3, HIGH);
  }
 
  //Serial.print(incomingData);
  
//************Measure************************************************************
  
  data.humidity = si7021.measureHumidity();
  data.soilMoisture = analogRead(soilPin);
  data.temperature = si7021.getTemperatureFromPreviousHumidityMeasurement();
  delay(500);
  
  
//***************Send*********************************************
  Serial.println("Sending to rf24_server");
  float temp = data.temperature;
  char tempSend[32];
  dtostrf(temp,3,2,tempSend);

  float humidity = data.humidity;
  char humSend[32];
  dtostrf(humidity,3,2,humSend);

  float soil = data.soilMoisture;
  char soilSend[32];
  dtostrf(soil,4,2,soilSend);

  float node = data.nodeNumber;
  char nodeSend[32];
  dtostrf(node,3,2,nodeSend);


  char spaceArray[5] = "NTHS"; // to separate data for readability


  // Populate a the string that will be sent using the previously obtained strings
  int sizeSending = strlen(tempSend) + strlen(humSend) + strlen(soilSend) + strlen(nodeSend) + strlen(spaceArray) + 1;
  uint8_t sending[sizeSending];
  sending[sizeSending - 1] = '\0'; 

  int currentSpot = 0;
  sending[currentSpot] = spaceArray[0];
  currentSpot++;

  for (int i = 0; i < strlen(nodeSend); i++){
    sending[currentSpot] = nodeSend[i];
    currentSpot++;
  }
  sending[currentSpot] = spaceArray[1];
  currentSpot++;

  for (int i = 0; i < strlen(tempSend); i++){
    sending[currentSpot] = tempSend[i];
    currentSpot++;
  }
  sending[currentSpot] = spaceArray[2];
  currentSpot++;

  for (int i = 0; i < strlen(humSend); i++){
    sending[currentSpot] = humSend[i];
    currentSpot++;
  }
  sending[currentSpot] = spaceArray[3];
  currentSpot++;

  for (int i = 0; i < strlen(soilSend); i++){
    sending[currentSpot] = soilSend[i];
    currentSpot++;
  }
  
  rf24.send(sending, sizeof(sending));
  rf24.waitPacketSent();

//************RECEIVE************************************************
  
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf24.waitAvailableTimeout(500))
  {
    // Should be a reply message for us now
    if (rf24.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf24_server running?");
  }
  delay(5000);
  digitalWrite(3, LOW);
  delay(400);
}

