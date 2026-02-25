/*
 * MVP Drag Strip Timing - FINISH LINE with AWS IoT Core (HTTPS)
 * Uses HTTPS POST instead of MQTT (port 443 instead of 8883)
 */

#include <heltec_unofficial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// AWS API Gateway endpoint
#define API_ENDPOINT "YOUR_API_GATEWAY_ENDPOINT"

// LoRa settings
#define BUTTON_PIN 0
#define FREQUENCY 905.2
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 7
#define TRANSMIT_POWER 14

// Timing variables - support multiple concurrent runs
struct Run {
  int number;
  unsigned long startTime;
};

Run pendingRuns[10];
int pendingCount = 0;
unsigned long lastPress = 0;
String rxdata;
volatile bool rxFlag = false;

void setup() {
  heltec_setup();
  
  both.println("FINISH LINE with HTTP");
  
  // Setup LoRa
  RADIOLIB_OR_HALT(radio.begin());
  radio.setDio1Action(rx);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  both.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    both.print(".");
  }
  
  both.println("\nWiFi connected!");
  both.print("IP: ");
  both.println(WiFi.localIP());
  
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "FINISH LINE");
  display.drawString(0, 20, "HTTP Ready");
  display.display();
}

void loop() {
  heltec_loop();
  
  // Check for START signal via LoRa
  if (rxFlag) {
    rxFlag = false;
    radio.readData(rxdata);
    
    if (_radiolib_status == RADIOLIB_ERR_NONE && rxdata.startsWith("START:")) {
      int colon = rxdata.indexOf(':');
      int runNum = rxdata.substring(colon + 1).toInt();
      
      if (pendingCount < 10) {
        pendingRuns[pendingCount].number = runNum;
        pendingRuns[pendingCount].startTime = millis();
        pendingCount++;
        
        both.printf("START received! Run #%d (%d pending)\n", runNum, pendingCount);
        
        display.clear();
        display.setFont(ArialMT_Plain_16);
        display.drawString(0, 0, "RUN #" + String(runNum));
        display.drawString(0, 20, String(pendingCount) + " PENDING");
        display.display();
        
        postToCloud("START", runNum, 0);
      }
    }
    
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
  
  // Check for button press to finish oldest pending run
  if (pendingCount > 0 && digitalRead(BUTTON_PIN) == LOW && millis() - lastPress > 500) {
    lastPress = millis();
    
    // Finish oldest run (FIFO)
    Run finishedRun = pendingRuns[0];
    unsigned long finishTime = millis();
    float elapsedTime = (finishTime - finishedRun.startTime) / 1000.0;
    
    // Remove from queue
    for (int i = 0; i < pendingCount - 1; i++) {
      pendingRuns[i] = pendingRuns[i + 1];
    }
    pendingCount--;
    
    both.printf("FINISH! Run #%d: %.3f seconds\n", finishedRun.number, elapsedTime);
    
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Run #" + String(finishedRun.number));
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 15, String(elapsedTime, 3) + "s");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 45, String(pendingCount) + " pending");
    display.display();
    
    // Send result via LoRa
    radio.standby();
    String result = "RESULT:" + String(finishedRun.number) + ":" + String(elapsedTime, 3);
    radio.transmit(result.c_str());
    
    postToCloud("RESULT", finishedRun.number, elapsedTime);
    
    delay(1000);
    
    if (pendingCount == 0) {
      display.clear();
      display.setFont(ArialMT_Plain_16);
      display.drawString(0, 0, "FINISH LINE");
      display.drawString(0, 20, "HTTP Ready");
      display.display();
    }
    
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
}

void rx() {
  rxFlag = true;
}

void postToCloud(const char* event, int runNum, float time) {
  if (WiFi.status() != WL_CONNECTED) {
    both.println("WiFi not connected!");
    return;
  }
  
  HTTPClient http;
  http.begin(API_ENDPOINT);
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<200> doc;
  doc["event"] = event;
  doc["run"] = runNum;
  doc["time"] = time;
  doc["timestamp"] = millis();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpCode = http.POST(jsonString);
  
  if (httpCode > 0) {
    both.printf("HTTP POST: %d\n", httpCode);
    if (httpCode == 200) {
      both.println("✓ Data sent successfully!");
    }
  } else {
    both.printf("HTTP POST failed: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
}
