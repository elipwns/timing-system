/*
 * MVP Drag Strip Timing - FINISH LINE
 * Receives START signal, starts timer
 * Press button to finish, calculates time, sends result
 */

#include <heltec_unofficial.h>

#define BUTTON_PIN 0
#define FREQUENCY 905.2
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 7
#define TRANSMIT_POWER 14

unsigned long startTime = 0;
bool runInProgress = false;
int currentRun = 0;
unsigned long lastPress = 0;
String rxdata;
volatile bool rxFlag = false;

void setup() {
  heltec_setup();
  
  both.println("FINISH LINE Ready");
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
  display.drawString(0, 0, "FINISH LINE");
  display.drawString(0, 20, "Waiting...");
  display.display();
  delay(100);
}

void loop() {
  heltec_loop();
  
  // Check for START signal
  if (rxFlag) {
    rxFlag = false;
    radio.readData(rxdata);
    
    if (_radiolib_status == RADIOLIB_ERR_NONE && rxdata.startsWith("START:")) {
      int colon = rxdata.indexOf(':');
      currentRun = rxdata.substring(colon + 1).toInt();
      
      startTime = millis();
      runInProgress = true;
      
      // Send to serial for dashboard
      Serial.println("START:" + String(currentRun));
      
      both.printf("START received! Run #%d\n", currentRun);
      
      display.clear();
      display.setFont(ArialMT_Plain_16);
      display.drawString(0, 0, "RUN IN PROGRESS");
      display.drawString(0, 20, "Run #" + String(currentRun));
      display.drawString(0, 40, "Press to FINISH");
      display.display();
      delay(50);
    }
    
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
  
  // Check for button press to finish
  if (runInProgress && digitalRead(BUTTON_PIN) == LOW && millis() - lastPress > 500) {
    lastPress = millis();
    
    unsigned long finishTime = millis();
    float elapsedTime = (finishTime - startTime) / 1000.0;
    
    runInProgress = false;
    
    both.printf("FINISH! Time: %.3f seconds\n", elapsedTime);
    
    // Display time
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "FINISHED!");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 15, String(elapsedTime, 3) + "s");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 45, "Sending result...");
    display.display();
    delay(50);
    
    // Send result back to start line
    radio.standby();
    String result = "RESULT:" + String(currentRun) + ":" + String(elapsedTime, 3);
    
    // Send to serial for dashboard
    Serial.println(result);
    
    heltec_led(50);
    radio.transmit(result.c_str());
    heltec_led(0);
    
    both.println("Result sent!");
    
    delay(2000);
    
    // Back to waiting
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "FINISH LINE");
    display.drawString(0, 20, "Waiting...");
    display.drawString(0, 40, "Last: " + String(elapsedTime, 3) + "s");
    display.display();
    delay(50);
    
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
}

void rx() {
  rxFlag = true;
}
