/******************************************************************************
 * Copyright 2018 Google
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "esp32-mqtt.h"
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include <ThingsBoard.h>
#include <SPI.h> 



// Update these with values suitable for your network.
#define ONE_WIRE_BUS 26                         //water temperature data pin to esp pin
#define DHTTYPE DHT22
#define DHTPIN 33                               //DHT22 data pin to esp pin
#define PH_PIN 35                               //pH data pin to esp pin
float voltage;
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float Celcius=0;
float Fahrenheit=0;
DynamicJsonDocument doc(1024) ;
DynamicJsonDocument ht(512);
char charBuf[150] ;
int i=0,j=0;
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
float phValue;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  setupCloudIoT();
  dht.begin();
  sensors.begin();
}

unsigned long lastMillis = 0;
void loop() {
  mqtt->loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqttClient->connected()) {
    connect();
  }
// put your main code here, to run repeatedly:
//client.loop();
   doc["humiditya"]=dht22().substring(0,5);
   doc["tempa"] = dht22().substring(6,11);
   doc["watertemp"] =  water_temp();//== NULL ?float(0):water_temp() ;
   doc["ph"] = pH_value();//== NULL ?float(0):pH_value() ; 
   doc["EC"] = (water_temp()+1224);
   serializeJson(doc,charBuf);
    serializeJson(doc,Serial);
  Serial.println(charBuf);
  delay(1000);
  //Serial.println("Sent");
  // TODO: replace with your code
  // publish a message roughly every second.
  if (millis() - lastMillis > 60000) {
    lastMillis = millis();
    //publishTelemetry(mqttClient, "/sensors", getDefaultSensor());
    publishTelemetry(charBuf);
  }
}


String dht22()
{
 
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) )
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return "None";
  }

  Serial.print(F("Humidity: "));
 Serial.println(h);
 Serial.print(F("Temperature: "));
 Serial.println(t);
 

 return String(String(h)+"-"+String(t)+"-");
}

float water_temp()
{
   sensors.requestTemperatures();
  Celcius=sensors.getTempCByIndex(0);
  Fahrenheit=sensors.toFahrenheit(Celcius);
  return float(Celcius);
}

float pH_value()
{
  voltage = analogRead(PH_PIN);
  delay(1000);
  phValue = (0.00363*voltage - 0.08625)/2;
  Serial.println(phValue);
  delay(3000);
  return float(phValue);
}
