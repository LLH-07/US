int trig_pin = 2;
int echo_pin = 3;
int warnLed = 8;
int led = 7;
int speaker = 9;

void setup()
{
  pinMode(trig_pin, OUTPUT);  
	pinMode(echo_pin, INPUT);
  pinMode(warnLed, OUTPUT);
  pinMode(led, INPUT);
  pinMode(speaker, OUTPUT);  
	Serial.begin(9600);
}

double getDistance()
{
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  double duration = pulseIn(echo_pin, HIGH);
  double distanceCM = duration * 0.034 / 2;

  return distanceCM;
}

void loop()
{
  double distanceCm = getDistance();
  Serial.print("Distance: ");
  Serial.println(distanceCm);
  
  int lightValue = digitalRead(led);
  Serial.print("LED State: ");
  Serial.println(lightValue);

  if(lightValue == 1)
  {
    if(distanceCm <= 5)
    {
      digitalWrite(warnLed, HIGH);
      delay(int(distanceCm)*50);
      digitalWrite(warnLed, LOW);
      delay(int(distanceCm)*50);

      tone(speaker, 50);
    }
    else noTone(speaker);
  }

  else
  {
    digitalWrite(warnLed, LOW);
    noTone(speaker);
  }

  delay(500);
}