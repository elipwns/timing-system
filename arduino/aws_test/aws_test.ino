/*
 * Minimal AWS IoT Core Connection Test
 * Tests basic connectivity without LoRa interference
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>

#define WIFI_SSID "Kloft"
#define WIFI_PASSWORD "elijah710"
#define AWS_IOT_ENDPOINT "a6wvk5akc45b6-ats.iot.us-west-2.amazonaws.com"

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=== AWS IoT Connection Test ===\n");
  
  // Connect WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // Sync time
  Serial.print("Syncing time...");
  configTime(0, 0, "pool.ntp.org");
  time_t now = time(nullptr);
  int attempts = 0;
  while (now < 1000000000 && attempts < 20) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    attempts++;
  }
  Serial.println();
  
  if (now > 1000000000) {
    Serial.printf("Time synced: %ld\n", now);
  } else {
    Serial.println("Time sync failed!");
  }
  
  // Test 1: Basic TCP connection
  Serial.println("\n--- Test 1: TCP Connection ---");
  Serial.printf("Connecting to %s:8883...\n", AWS_IOT_ENDPOINT);
  
  if (client.connect(AWS_IOT_ENDPOINT, 8883)) {
    Serial.println("✓ TCP connection successful!");
    client.stop();
  } else {
    Serial.println("✗ TCP connection failed!");
  }
  
  // Test 2: TLS connection (insecure)
  Serial.println("\n--- Test 2: TLS Connection (insecure) ---");
  client.setInsecure();
  
  if (client.connect(AWS_IOT_ENDPOINT, 8883)) {
    Serial.println("✓ TLS connection successful!");
    client.stop();
  } else {
    Serial.println("✗ TLS connection failed!");
  }
  
  // Test 3: DNS resolution
  Serial.println("\n--- Test 3: DNS Resolution ---");
  IPAddress ip;
  if (WiFi.hostByName(AWS_IOT_ENDPOINT, ip)) {
    Serial.printf("✓ DNS resolved: %s\n", ip.toString().c_str());
  } else {
    Serial.println("✗ DNS resolution failed!");
  }
  
  Serial.println("\n=== Test Complete ===");
}

void loop() {
  delay(10000);
}
