int blue_led = 2;
int red_led = 3;
int green_led = 4;

int trig = 5;
int echo = 6;

int buzzer = 10;

float get_distance(){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  int duration = pulseIn(echo, HIGH);
  
  float distance = duration * 0.034 / 2;
  
  return distance;
}
  
void activate_zone_1(){
  digitalWrite(blue_led, HIGH);
  digitalWrite(red_led, LOW);
  digitalWrite(green_led, LOW);
  noTone(buzzer);
}

void activate_zone_2(){
  digitalWrite(blue_led, LOW);
  digitalWrite(green_led, LOW);
  
  digitalWrite(red_led, HIGH);
  tone(buzzer, 1000);
  delay(500);
  
  digitalWrite(red_led, LOW);
  noTone(buzzer);
  delay(500);
}

void activate_zone_3(float distance){
  int delay_time = map(distance, 0, 50, 20, 200);
  
  digitalWrite(blue_led, HIGH);
  digitalWrite(red_led, HIGH);
  digitalWrite(green_led, HIGH);
  tone(buzzer, 1000);
  delay(delay_time);
  
  digitalWrite(blue_led, LOW);
  digitalWrite(red_led, LOW);
  digitalWrite(green_led, LOW);
  noTone(buzzer);
  delay(delay_time);
}

void setup(){
  pinMode(blue_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  
  pinMode(buzzer, OUTPUT);
  
  digitalWrite(blue_led, LOW);
  digitalWrite(red_led, LOW);
  digitalWrite(green_led, LOW);
  noTone(buzzer);
  
  Serial.begin(9600);
}

void loop(){
  float distance = get_distance();
  Serial.println(distance);
  
  if(distance >= 150)
  {
    activate_zone_1();
  }
  
  else if(50 < distance && distance < 150)
  {
    activate_zone_2();
  }
  
  else
  {
    activate_zone_3(distance);
  }
}