int led = 11;
int quangTro = A0;


#include <Servo.h>
#include "DHT.h"

Servo myservo;


int dht_pin = 4; // Đọc dữ liệu ở chân 2 là 
DHT dht(dht_pin, DHT11);

// int potentiometer_pin = A0;

int a = 7;
int b = 6;
int c = 11;
int d = 12;
int e = 13;
int f = 8;
int g = 9;


int segments[7] = {7, 6, 11, 12, 13, 8, 9};



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // pinMode(led, OUTPUT);
  
  // myservo.attach(9);

  // pinMode(potentiometer_pin, INPUT); // Dien tro

  // dht.begin();
  for(int i = 0; i < 7; i++)
    pinMode(segments[i], OUTPUT);
}

void display0()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
}

void display1()
{
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
}

void display2()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(g, HIGH);
}

void display3()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(g, HIGH);
}

void display4()
{
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}

void display5()
{
  digitalWrite(a, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}

void display6()
{
  digitalWrite(a, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}

void display7()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
}

void display8()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}

void display9()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}

void clear()
{
  for(int i = 0; i < 7; i++)
    digitalWrite(segments[i], LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  // digitalWrite(led, HIGH);
  // delay(1000);
  // digitalWrite(led, LOW);
  // delay(1000); 

  // int val = analogRead(quangTro);
  // Serial.println(val);
  // delay(500);

  // int value_potentiometer = analogRead(potentiometer_pin); // range in 0 - 1023
  // int goc = map(value_potentiometer, 0, 1023, 0, 180);
  // Serial.println(goc);
  // myservo.write(goc);
  // delay(500);
 
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // 
  float f = dht.readTemperature(true);
  
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Temperature (C): ");
  Serial.println(t);
  Serial.print("Temperature (F): ");
  Serial.println(f);

  delay(100);

  display0();
  delay(1000);
  clear();

  display1();
  delay(1000);
  clear();

  display2();
  delay(1000);
  clear();

  display3();
  delay(1000);
  clear();

  display4();
  delay(1000);
  clear();

  display5();
  delay(1000);
  clear();

  display6();
  delay(1000);
  clear();

  display7();
  delay(1000);
  clear();

  display8();
  delay(1000);
  clear();

  display9();
  delay(1000);
  clear();

}
