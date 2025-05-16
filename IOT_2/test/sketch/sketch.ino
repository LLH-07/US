#include <ESP8266Wifi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "DHT.h"

using namespace std;

// Wifi 
WiFiClient client;
PubSubClient mqtt_client(client);

const char* ssid = "THREE O'CLOCK FREEWIFI";
const char* password = "";

// MQTT
const char* mqtt_server = "192.168.181.88";
const int mqtt_port = 1883;
const char* mqtt_id = "esp8266";

const char* topic_subscribe = "to-esp8266";
const char* topic_publish = "from-esp8266";

// Led
int red_led = 2;
int yellow_led = 3;
int green_led = 4;

// DHT11
int dht_pin = 12;
DHT dht(dht_pin, DHT11);

// Soil moisture sensor
int soil_d_pin = 7;
int soil_a_pin = A0;

// Relay and pump
int relay_pin = 8;

// Moisture level
int low_moisture = 300;
int high_moisture = 700;

// Auto mode
int auto_mode = 0; // Turn off
int water_status = 0; // -1: need less water, 0: enough water, 1: need more water

// CONNECTING
// Wifi
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Recived data from: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (size_t i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("------------------------------------------");
}

void connect_wifi() {
	WiFi.begin(ssid, password);
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

// MQTT
void connect_mqtt() {
  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(callback);

  Serial.println("Connecting to mqtt... ");
  while (!mqtt_client.connect(mqtt_id))
  {
    delay(500);
  }
  Serial.println("Connected to mqtt ");
  mqtt_client.subscribe(topic_subscribe);
}

// Led
void connect_led() {
  pinMode(red_led, OUTPUT);
	pinMode(yellow_led, OUTPUT);
	pinMode(green_led, OUTPUT);
}

// Soil moisture sensor
void connect_soil_moisture_sensor() {
  pinMode(soil_d_pin, OUTPUT);
  digitalWrite(soil_d_pin, LOW);
}

// Relay
void connect_relay() {
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, LOW);
}


// COMPONENTS' FUNCTION
// led
void turn_on_led(int led) {
  for (int i = 2; i <= 4; i++)
    digitalWrite(i, LOW);
  
  switch(led) {
    case 0:
      digitalWrite(red_led, HIGH);
      break;
    case 1:
      digitalWrite(yellow_led, HIGH);
      break;
    case 2:
      digitalWrite(green_led, HIGH);
      break;
  }
}

// soil moisture
double measure_soil_moisture() {
  digitalWrite(soil_d_pin, HIGH);
  delay(10);

  int moisture_value = analogRead(soil_a_pin);
  int moisture_percent = map(moisture_value, 0, 1023, 0, 100);

  if(moisture_value < low_moisture) {
    turn_on_led(0);
    digitalWrite(relay_pin, HIGH); // Turn on bump
    water_status = 1;
  }
  else if(moisture_value > high_moisture) {
    turn_on_led(2);
    digitalWrite(relay_pin, LOW);
    water_status = -1;
  }
  else {
    turn_on_led(1);
    digitalWrite(relay_pin, LOW);
    water_status = 0;
  }

  delay(10);
  digitalWrite(soil_d_pin, LOW);

  return moisture_percent;

}

// dht11
double measure_humidity() {
  return dht.readHumidity();
}

double measure_temperature_celsius() {
  return dht.readTemperature();
}

double measure_temperature_fahrenheit() {
  return dht.readTemperature(true);
}


// SET UP & RUN
void setup() {
	Serial.begin(9600);
	// WIFI
	//connect_wifi();
  
  // MQTT
  //connect_mqtt();

	// LED
	connect_led();

  // DHT11
  dht.begin();

	// Soil moisture sensor
	connect_soil_moisture_sensor();

  // Relay
  connect_relay();
}

void loop()
{
  mqtt_client.loop();

  if (!mqtt_client.connected()) {
    connect_mqtt();
  }


  double soil_moisture = measure_soil_moisture();
  double humidity = measure_humidity();
  double temperature = measure_temperature_celsius();

  // In ra Serial các thông tin
  Serial.print("Soil Moisture: ");
  Serial.print(soil_moisture);
  Serial.println("%");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("C");

  // Đợi 2 giây trước khi đọc lại
  delay(2000);
}