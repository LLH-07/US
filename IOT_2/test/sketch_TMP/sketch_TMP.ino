#include <ESP8266Wifi.h>
#include <WiFiClient.h>


using namespace std;


// Wifi
const char* ssid = "THREE O'CLOCK FREEWIFI";
const char* password = "";


// Led
int red_led = 2;
int yellow_led = 3;
int green_led = 4;


// Soil moisture sensor
int soil_d_pin = 7;
int soil_a_pin = A0;


// Moisture level
int low_moisture = 300;
int high_moisture = 700;


// CONNECTING
void connect_wifi() {
  WiFI.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }


  Serial.println();
  Serial.print("Connected to Wifi: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}


void connect_led() {
  pinMode(red_led, OUTPUT);
  pinMode(yellow_led, OUTPUT);
  pinMode(green_led, OUTPUT);
}


void connect_soil_moisture_sensor() {
  pinMode(soil_d_pin, OUTPUT);
  digitalWrite(soil_d_pin, LOW);
}


// SOMETHING
void turn_on_led(int led) {
  for (int i = 2; i <= 4; i++)
    digitalWrite(i, LOW);
  
  // led = 0 --> red
  // led = 1 --> yellow
  // led = 2 --> green
  digitalWrite(led + 2, HIGH);
}


double measure_soil_moisture() {
  digitalWrite(soil_d_pin, HIGH);
  delay(10);


  int moisture_value = analogRead(soil_a_pin);
  int moisture_percent = map(moisture_value, 0, 1023, 0, 100);


  if(moisture_value <= low_moisture) {
    turn_on_led(0);
  }
  else if(moisture_value >= high_moisture) {
    turn_on_led(2);
  }
  else {
    turn_on_led(1);
  }


  return moisture_percent;


}


// SET UP & RUN
void setup() {
  Serial.begin(9600);
  // WIFI
  //connect_wifi();


  // LED
  connect_led();


  // Soil moisture sensor
  connect_soil_moisture_sensor()
}


void loop()
{


}