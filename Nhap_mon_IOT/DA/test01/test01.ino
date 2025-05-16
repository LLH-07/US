#include <Servo.h>


// Điều khiển servo 180, quay liên tục từ góc 0 đến 180 độ và ngược lại.

Servo myservo;
int pos = 0; // lưu vị trí của servo

void setup() {
  myservo.attach(2);  // kết nối servo vào chân P4
}

void loop() {
  myservo.write(0);
  delay(1000);
  myservo.write(180);
  delay(1000);
}