/*
 * MVP Drag Strip Timing - FINISH LINE with AWS IoT Core
 * Receives START signal via LoRa, starts timer
 * Press button to finish, calculates time, sends to AWS IoT Core
 */

#include <heltec_unofficial.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "aws_root_ca.h"
#include "aws_cert.h"
#include "aws_private_key.h"

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

// WiFi and MQTT
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  heltec_setup();
  
  both.println("FINISH LINE with AWS IoT");
  
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
  connectWiFi();
  
  // Sync time (required for TLS)
  both.println("Syncing time...");
  configTime(-8 * 3600, 0, "pool.ntp.org", "time.nist.gov");  // PST timezone
  
  time_t now = time(nullptr);
  int timeAttempts = 0;
  while (now < 1000000000 && timeAttempts < 20) {
    delay(500);
    both.print(".");
    now = time(nullptr);
    timeAttempts++;
  }
  both.println("");
  
  if (now > 1000000000) {
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    both.printf("Time synced: %s", asctime(&timeinfo));
  } else {
    both.println("Time sync failed! Using insecure mode...");
    wifiClient.setInsecure();  // Skip cert validation if time fails
  }
  
  // Setup MQTT
  // Temporarily skip certificate validation to test connection
  wifiClient.setInsecure();
  
  // Comment these out for now
  // wifiClient.setCACert(AWS_ROOT_CA);
  // wifiClient.setCertificate(AWS_CERT);
  // wifiClient.setPrivateKey(AWS_PRIVATE_KEY);
  
  mqttClient.setServer(AWS_IOT_ENDPOINT, 8883);
  mqttClient.setBufferSize(512);
  
  connectAWS();
  
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "FINISH LINE");
  display.drawString(0, 20, "AWS Connected");
  display.display();
  delay(100);
}

void loop() {
  heltec_loop();
  
  // Keep MQTT connected
  if (!mqttClient.connected()) {
    connectAWS();
  }
  mqttClient.loop();
  
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
        display.drawString(0, 40, "Press to FINISH");
        display.display();
        delay(50);
        
        publishToAWS("START", runNum, 0);
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
    delay(50);
    
    // Send result back to start line via LoRa
    radio.standby();
    String result = "RESULT:" + String(finishedRun.number) + ":" + String(elapsedTime, 3);
    
    heltec_led(50);
    radio.transmit(result.c_str());
    heltec_led(0);
    
    publishToAWS("RESULT", finishedRun.number, elapsedTime);
    
    delay(1000);
    
    if (pendingCount == 0) {
      display.clear();
      display.setFont(ArialMT_Plain_16);
      display.drawString(0, 0, "FINISH LINE");
      display.drawString(0, 20, "AWS Connected");
      display.drawString(0, 40, "Last: " + String(elapsedTime, 3) + "s");
      display.display();
      delay(50);
    }
    
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
}

void rx() {
  rxFlag = true;
}

void connectWiFi() {
  both.print("Connecting to WiFi");
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Connecting WiFi");
  display.display();
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    both.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    both.println("\nWiFi connected!");
    both.print("IP: ");
    both.println(WiFi.localIP());
    
    display.clear();
    display.drawString(0, 0, "WiFi Connected");
    display.drawString(0, 20, WiFi.localIP().toString());
    display.display();
    delay(2000);
  } else {
    both.println("\nWiFi failed!");
    display.clear();
    display.drawString(0, 0, "WiFi FAILED");
    display.display();
    delay(2000);
  }
}

void connectAWS() {
  both.print("Connecting to AWS IoT");
  
  int attempts = 0;
  while (!mqttClient.connected() && attempts < 5) {
    if (mqttClient.connect(THING_NAME)) {
      both.println("\nAWS IoT connected!");
      return;
    }
    
    int state = mqttClient.state();
    both.printf("\nMQTT error: %d\n", state);
    both.print(".");
    delay(1000);
    attempts++;
  }
  
  both.println("\nAWS IoT connection failed!");
  both.printf("Final MQTT state: %d\n", mqttClient.state());
}

void publishToAWS(const char* eventType, int runNum, float time) {
  StaticJsonDocument<200> doc;
  doc["event"] = eventType;
  doc["run"] = runNum;
  doc["time"] = time;
  doc["timestamp"] = millis();
  
  char jsonBuffer[200];
  serializeJson(doc, jsonBuffer);
  
  if (mqttClient.publish(MQTT_TOPIC_PUBLISH, jsonBuffer)) {
    both.printf("Published to AWS: %s\n", jsonBuffer);
  } else {
    both.println("AWS publish failed!");
  }
}
