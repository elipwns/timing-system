/*
 * MVP Drag Strip Timing - START LINE
 * Press button to send START signal
 * Receives and displays final time
 */

#include <heltec_unofficial.h>

#define BUTTON_PIN 0
#define FREQUENCY 905.2
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 7
#define TRANSMIT_POWER 14

int runNumber = 0;
unsigned long lastPress = 0;
String rxdata;
volatile bool rxFlag = false;

void setup() {
  heltec_setup();
  
  both.println("START LINE Ready");
  RADIOLIB_OR_HALT(radio.begin());
  radio.setDio1Action(rx);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "START LINE");
  display.drawString(0, 20, "Press to START");
  display.display();
  delay(100);
}

void loop() {
  heltec_loop();
  
  if (digitalRead(BUTTON_PIN) == LOW && millis() - lastPress > 1000) {
    lastPress = millis();
    runNumber++;
    
    radio.standby();
    String msg = "START:" + String(runNumber);
    both.println("Sending: " + msg);
    
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "SENDING START");
    display.drawString(0, 20, "Run #" + String(runNumber));
    display.display();
    delay(50);
    
    heltec_led(50);
    radio.transmit(msg.c_str());
    heltec_led(0);
    
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "RUN IN PROGRESS");
    display.drawString(0, 20, "Waiting...");
    display.display();
    delay(50);
    
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
  
  if (rxFlag) {
    rxFlag = false;
    radio.readData(rxdata);
    
    if (_radiolib_status == RADIOLIB_ERR_NONE && rxdata.startsWith("RESULT:")) {
      int firstColon = rxdata.indexOf(':');
      int secondColon = rxdata.indexOf(':', firstColon + 1);
      String timeStr = rxdata.substring(secondColon + 1);
      
      both.println("Result: " + timeStr + "s");
      
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 0, "RUN COMPLETE!");
      display.setFont(ArialMT_Plain_24);
      display.drawString(0, 20, timeStr + "s");
      display.display();
      delay(50);
      
      delay(5000);
      
      display.clear();
      display.setFont(ArialMT_Plain_16);
      display.drawString(0, 0, "START LINE");
      display.drawString(0, 20, "Press to START");
      display.drawString(0, 40, "Last: " + timeStr + "s");
      display.display();
      delay(50);
    }
    
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
}

void rx() {
  rxFlag = true;
}
