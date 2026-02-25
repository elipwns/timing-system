#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\nI2C Scanner");
  
  // Power on Vext (display power)
  pinMode(36, OUTPUT);
  digitalWrite(36, LOW); // LOW = ON for Vext
  delay(100);
  
  Wire.begin(17, 18); // SDA=17, SCL=18 for Heltec V3
  
  Serial.println("Scanning...");
  byte count = 0;
  
  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      if (i < 16) Serial.print("0");
      Serial.println(i, HEX);
      count++;
    }
  }
  
  Serial.println(count > 0 ? "Scan complete" : "No I2C devices found!");
  Serial.println("Display should be at 0x3C");
}

void loop() {
  delay(5000);
}
