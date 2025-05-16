// void setup() 
// {
//   Serial.begin(115200); // Mở cổng Serial để xem kết quả
//   pinMode(A0, INPUT);   // Khai báo chân A0 là chân đầu vào để đọc giá trị từ cảm biến
// }
 
// void loop() 
// {
//   int value = analogRead(A0);  // Đọc giá trị analog từ cảm biến Soil Moisture
//   float soilMoisturePercent = map(value, 0, 1023, 100, 0); // Chuyển giá trị từ 0-1023 sang % độ ẩm

//   Serial.print("Soil Moisture Value: ");
//   Serial.print(value);  // Hiển thị giá trị nguyên đọc từ cảm biến
//   Serial.print(" | Soil Moisture Percent: ");
//   Serial.print(soilMoisturePercent);  // Hiển thị độ ẩm tính theo %
//   Serial.println("%");
  
//   delay(1000);  // Chờ 1 giây trước khi đọc lại giá trị
// }









// Gọi thư viện DHT11
#include "DHT.h"            
 
const int DHTPIN = 2;       //Đọc dữ liệu từ DHT11 ở chân 2 trên mạch Arduino
const int DHTTYPE = DHT11;  //Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22
 
DHT dht(DHTPIN, DHTTYPE);
 
void setup() {
  Serial.begin(115200);
  dht.begin();         // Khởi động cảm biến
}
 
void loop() {
  float h = dht.readHumidity();    //Đọc độ ẩm
  float t = dht.readTemperature(); //Đọc nhiệt độ
 
  Serial.print("Nhiet do: ");
  Serial.println(t);               //Xuất nhiệt độ
  Serial.print("Do am: ");
  Serial.println(h);               //Xuất độ ẩm
  
  Serial.println();                //Xuống hàng
  delay(1000);                     //Đợi 1 giây
}