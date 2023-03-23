#include <SPI.>
#include <Ethernet.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

String tempVarId = "575475df7625423fd9da9c36";
String humVarId = "575475f1762542406cb10c43";
String lightVarId = "575475fc762542410358a0c3";
String soilVarId = "5754760576254241593d4d47";
String token = "xxxxx";

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

char server[]="things.ubidots.com";

EthernetClient client;
IPAddress ip(192, 168, 1, 40); // Arduino IP Add
IPAddress myDns(8,8,8,8);
IPAddress myGateway(192,168,1,1);

int moisturePin = 0;
int lightPin = 3;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.print("Starting...");
  // Net connection...
}

void loop() {
  float soilHum = analogRead(moisturePin);
  soilHum = (1023 - soilHum) * 100 /1023;
  Serial.println("Soil Humidty: " + String(soilHum));

  // Read light
  float volts = analogRead(lightPin) * 5.0 / 1024.0;
  float amps = volts /10000.0;
  float microamps = amps * 1000000;
  float lux = microamps * 2.0;

  Serial.println("Lux: " + String(lux));

  float h = dht.readHumidity();
  float temp = dht.readTemperature();

  Serial.println("Temp: " + String(temp,2));
  Serial.println("Hum: " + String(h,2));
  sendValue(temp, h, lux, soilHum);
  delay (60000);
}

void sendValue(float tempValue, float humValue, float lux, float soil)
{
  Serial.println("Sending data...");
  // if you get a connection, report back via serial:
  int bodySize = 0;
  delay(2000);

  // Post single value to single var
  String varString = "[{\"variable\": \"" + tempVarId + "\", \"value\":"
         + String(tempValue) + "}";

  // Add other variables
  Serial.println("Connecting...");

  if (client.connect(server,80)) {
   client.println("POST /api/v1.6/collections/values HTTP/1.1");
   Serial.println("POST /api/v1.6/collections/values HTTP/1.1");

   client.println("Content-Type: application/json");
   Serial.println("Content-Type: application/json");
   client.println("Content-Length: "+String(bodySize));
   Serial.println("Content-Length: "+String(bodySize));
   client.println("X-Auth-Token: "+token);
   Serial.println("X-Auth-Token: "+token);
   client.println("Host: things.ubidots.com\n");
   Serial.println("Host: things.ubidots.com\n");
   client.print(varString);
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

   boolean sta = client.connected();
   Serial.println("Connection ["+String(sta)+"]");
   if (!client.connected()) {
     Serial.println();
     Serial.println("disconnecting.");
     client.stop();
   }

  Serial.println("Reading..");
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  client.flush();
  client.stop();
}
