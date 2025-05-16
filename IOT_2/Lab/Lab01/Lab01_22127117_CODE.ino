byte leds[] = {2, 3, 4, 5, 6, 7};
int button = 8;
byte num_leds;
int mode = 1;
int lastButtonState = LOW;
int lastMillis = 0;
int lastMillis_2 = 0;
int led_index_mode_2 = 3;


// SET UP
void setup(){
  num_leds = sizeof(leds);
  
  for(int i = 0; i < num_leds; i++) {
    pinMode(leds[i], OUTPUT);
  }
  
  pinMode(button, INPUT);
  
  Serial.begin(9600);
  Serial.println("Mode 1");
}

// MODE 1
void activate_mode_1(){
  for(int i = 0; i < 3; i++) {
    digitalWrite(leds[i], HIGH);
  }
}

// MODE 2
void activate_mode_2() {
  if(millis() - lastMillis_2 >= 1000) {
    lastMillis_2 = millis();
    
    turn_off_leds();
    
    digitalWrite(leds[led_index_mode_2], HIGH);
    
    led_index_mode_2++;
    if (led_index_mode_2 >= num_leds) {
      led_index_mode_2 = 3;
    }
  }
}


// OTHER FUNCTIONS
void turn_off_leds(){
  for(int i = 0; i < num_leds; i++) {
    digitalWrite(leds[i], LOW);
  }
}

void activate(int mode){
  if(mode == 1) {
    activate_mode_1();
  }
  
  else {
    activate_mode_2();
  }
}



// LOOP
void loop(){
  turn_off_leds();
  
  int buttonState = digitalRead(button);
  
  if(buttonState != lastButtonState) {
    lastButtonState = buttonState;
    
    if(buttonState == HIGH) {
      lastMillis = millis();
    }
    
    else if (buttonState == LOW) {
      if(millis() - lastMillis >= 4000) {
        mode = 2;
        Serial.println("Mode 2");
      }
      
      else {
        mode = 1;
        Serial.println("Mode 1");
      }
    }
  }
  
  activate(mode);
}