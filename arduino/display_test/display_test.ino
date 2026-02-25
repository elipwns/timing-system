#include <heltec_unofficial.h>

#define OLED_RST 21  // V3 display reset pin

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting...");
  
  // Power cycle the display
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  
  heltec_setup();
  Serial.println("heltec_setup done");
  
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "TEST");
  display.display();
  
  Serial.println("Display updated");
  
  pinMode(35, OUTPUT);
}

void loop() {
  digitalWrite(35, HIGH);
  delay(500);
  digitalWrite(35, LOW);
  delay(500);
  Serial.println("Loop running...");
}
