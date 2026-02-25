/*
 * LoRa Transmitter - Based on Heltec Example
 * Press button to send message
 */

#include <heltec_unofficial.h>

#define BUTTON_PIN 0
#define FREQUENCY 905.2  // MHz for US
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 7
#define TRANSMIT_POWER 14

int counter = 0;
unsigned long lastPress = 0;

void setup() {
  heltec_setup();
  
  both.println("Radio init");
  RADIOLIB_OR_HALT(radio.begin());
  
  both.printf("Frequency: %.2f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  
  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  
  both.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
  
  both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  display.clear();
  display.drawString(0, 0, "TX Ready");
  display.drawString(0, 20, "Press button");
  display.display();
  
  both.println("TX Ready");
}

void loop() {
  heltec_loop();
  
  if (digitalRead(BUTTON_PIN) == LOW && millis() - lastPress > 500) {
    lastPress = millis();
    counter++;
    
    String msg = "MSG#" + String(counter);
    both.printf("TX [%s] ", msg.c_str());
    
    heltec_led(50);
    int state = radio.transmit(msg.c_str());
    heltec_led(0);
    
    if (state == RADIOLIB_ERR_NONE) {
      both.println("OK");
      
      display.clear();
      display.drawString(0, 0, "SENT!");
      display.drawString(0, 20, msg);
      display.display();
      
      delay(500);
      
      display.clear();
      display.drawString(0, 0, "TX Ready");
      display.drawString(0, 20, "Count: " + String(counter));
      display.display();
    } else {
      both.printf("FAIL (%i)\n", state);
      
      display.clear();
      display.drawString(0, 0, "TX FAILED!");
      display.drawString(0, 20, "Error: " + String(state));
      display.display();
      delay(1000);
    }
  }
}
