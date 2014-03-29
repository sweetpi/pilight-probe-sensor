#include <OneWire.h> // http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
#include <DallasTemperature.h> // http://download.milesburton.com/Arduino/MaximTemperature/DallasTemperature_LATEST.zip
#include "lib/NewRemoteTransmitter.h"

#define txPin 10     // RF TRANSMITTER PIN
const int ledPin = 8; // LED PIN
#define ONE_WIRE_BUS 9 // DS18B20 PIN

OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature

void setup() {
    // Define outputs
    pinMode(ledPin, OUTPUT);
    pinMode(txPin, OUTPUT);
}

void loop() {
  Blink(ledPin,2);  
  NewRemoteTransmitter transmitter(8934000, txPin, 242,3);
  
  float temperature;

  sensors.begin(); //start up temp sensor
  sensors.requestTemperatures(); // Get the temperature
  temperature = sensors.getTempCByIndex(0); // Get temperature in Celcius

  int NewTemp = temperature * 10;
  transmitter.sendSensor(3,NewTemp);      
  
  delay(60000);
}

void Blink(int led, int times){
  for (int i=0; i< times; i++){
    digitalWrite(ledPin,HIGH);
    delay (50);
    digitalWrite(ledPin,LOW);
    delay (50);
  }
}