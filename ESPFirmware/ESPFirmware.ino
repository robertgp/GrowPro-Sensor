/*
 *  HTTP over TLS (HTTPS) example sketch
 *
 *  This example demonstrates how to use
 *  WiFiClientSecure class to access HTTPS API.
 *  We fetch and display the status of
 *  esp8266/Arduino project continuous integration
 *  build.
 *
 *  Limitations:
 *    only RSA certificates
 *    no support of Perfect Forward Secrecy (PFS)
 *    TLSv1.2 is supported since version 2.4.0-rc1
 *
 *  Created by Ivan Grokhotkov, 2015.
 *  This example is in public domain.
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "traphouse3";
const char* password = "liquorstore420";

const char* host = "growpro.herokuapp.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C";

void setup() {

  Serial.begin(57600);
  
}

void loop() {
  
  String temperature;
  String humidity; 
  String light; 
  boolean tempFound = false;
  boolean humidityFound = false;
  boolean lightFound = false;
  boolean z = false;
   while (Serial.available()) {
   if (Serial.available() >0) {
     char c = Serial.read();  //gets one byte from serial buffer
     if (c == '&') {
        tempFound = true;
     }
     else if (c == '%') {
        humidityFound = true;
         
     }

     else if ( c == '$') {
      lightFound = true; 
     }
   

     else if ( !tempFound ) {
      temperature +=c;
     }
     else if ( !humidityFound && tempFound ) {
      humidity +=c;
     }

     else if (!lightFound &&  humidityFound) {
      light +=c;
      
     }
     
 
   } 
 }


  
  
 


  delay(1000);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  String url = "/api/metrics/temperature&humidity&light";
  String body;
  
  body = "name=Francis&light="+light+ "&" + "temperature=" + temperature + "&"+"humidity=" + humidity;
  Serial.print("requesting URL: ");
  Serial.println(url);
  
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Content-Type: application/x-www-form-urlencoded" + "\r\n" + 
               "Cache-Control: no-cache\r\n" + 
               "Content-Length: " + String(body.length()) + "\r\n" +
               "Connection: close\r\n\r\n");

  client.print(body);
  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
  
 

}

