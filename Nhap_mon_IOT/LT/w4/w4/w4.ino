#include "DHT.h"

// // const int debounceTime = 50; // ms
// // bool motionDetected = false;
// // int motionCount = 0;
// // unsigned long lastDebounceTime = 0;
int pir_pin = 9;
int led = 8;

int dht_pin = A0;
DHT dht(dht_pin, DHT11);

void setup()
{
  Serial.begin(9600);
  pinMode(pir_pin, INPUT);
  pinMode(led, OUTPUT);

  dht.begin();
}


void loop()
{
  int sensorValue = digitalRead(pir_pin);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);


  if(sensorValue == HIGH)
  {
    if (millis() - lastDebounceTime > debounceTime)
    {
      motionCount++;
      lastDebounceTime = millis();
    }
  }

  else motionCount = 0;

  if (motionCount >= 5)
  {
    motionDetected = true;
  }

  else
  {
    motionDetected = false;
  }

  Serial.println(motionCount);

  if(motionDetected) 
    digitalWrite(led, HIGH);
  else 
    digitalWrite(led, LOW);


  Serial.println("--------------------------");
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Temperature(C): ");
  Serial.println(t);
  Serial.print("Temperature(F): ");
  Serial.println(f);

  delay(500);


}


// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// #include "DHT.h"

// LiquidCrystal_I2C lcd(0x27, 16, 2);

// void setup()
// {
//   lcd.init();
//   dht.begin();
// }

// void loop()
// { 
//   float h = dht.readHumidity();
//   float t = dht.readTemperature();

//   lcd.setCursor(0, 0);
//   lcd.print("Humid: ");
//   lcd.print(h);

//   lcd.setCursor(0, 1);
//   lcd.print("Temp: ");
//   lcd.print(t);

//   lcd.backlight();
//   // delay(1000);
//   // lcd.noBacklight();
//   // delay(1000);
// }

// int relay_pin = 2;
// int led = 9;

// void setup()
// {
//   pinMode(relay_pin, OUTPUT);
//   pinMode(led, OUTPUT);
// }


// void loop()
// {
//   digitalWrite(relay_pin, LOW);
//   digitalWrite(led, LOW);
//   delay(2000);
//   digitalWrite(relay_pin, HIGH);
//   digitalWrite(led, HIGH);
//   delay(2000);
// }