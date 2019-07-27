
// rf24_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF24 class. RH_RF24 class does not provide for addressing or
// reliability, so you should only use RH_RF24  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf24_client
// Tested on Anarduino Mini http://www.anarduino.com/mini/ with RFM24W and RFM26W
#include <LowPower.h>
#include <SPI.h>
#include <RH_NRF24.h>

// Singleton instance of the radio driver
RH_NRF24 rf24(7, 8);



// Plant 1
struct packet {
  uint8_t plantName[32] = "Planty";
  float plantID = 0.0;
};
typedef struct packet Packet;
Packet plant1;

void setup()
{
  Serial.begin(9600);
  if (!rf24.init()) {
    Serial.println("init failed");
  }
  else {
    Serial.println("initialized");
  }

}

void loop()
{

  if (rf24.available())
  {

    //******************************RECEIVE************************************

    // Should be a message for us now
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf24.recv(buf, &len))
    {
      Serial.print("got request: ");
      Serial.println((char*)buf);


      uint8_t nodeInfo[5];
      nodeInfo[4] = '\0';
      for (int i = 0; i < 4; i++) {
        nodeInfo[i] = buf[i + 1];
      }

      uint8_t temp[6];
      uint8_t humidity[6];
      uint8_t soilMoisture[6];
      boolean foundT = false;
      int currentT = 0;
      for (int i = 0; i < sizeof(buf); i++) {
        if (buf[i+1] == 72) {
          foundT = false;
          break;
        }
        if (foundT == true) {
          temp[currentT] = buf[i+1];
          Serial.println((char*)temp);

          currentT++;
        }
        
        if (buf[i] == 84 ) {
          foundT = true;
          temp[currentT] = buf[i+1];
          currentT++;
        }

      
        
       
        if (buf[i] == 0) {
          foundT = false;
        }



      }
      Serial.println("The temperature is");
      Serial.println((char*)temp);


      Serial.print("The node that send that message is node ");
      Serial.println((char*)nodeInfo);
      //*************************SEND**********************************************

      //uint8_t data[] = "And hello back to you";
      //rf24.send(data, sizeof(data));

      char idSend[32];
      dtostrf(plant1.plantID, 3, 2, idSend);

      int sizeAssign = strlen(plant1.plantName) + strlen(idSend) + 2;
      uint8_t assign[sizeAssign];
      assign[sizeAssign - 1] = '\0';
      int currentSpot = 0;

      for (int i = 0; i < strlen(idSend); i++) {
        assign[currentSpot] = idSend[i];
        currentSpot++;
      }
      assign[currentSpot] = '-';
      currentSpot++;

      for (int i = 0; i < strlen(plant1.plantName); i++) {
        assign[currentSpot] = plant1.plantName[i];
        currentSpot++;
      }

      rf24.send(assign, sizeof(assign));
      rf24.waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("recv failed");
    }
      LowPower.powerDown(SLEEP_8S,ADC_OFF,BOD_OFF);

  }



}

