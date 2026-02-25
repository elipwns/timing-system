/*
 * LoRa Receiver - Based on Heltec Example
 * Uses interrupt for receiving
 */

#include <heltec_unofficial.h>

#define FREQUENCY 905.2  // MHz for US
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 7
#define TRANSMIT_POWER 14

String rxdata;
volatile bool rxFlag = false;
int count = 0;

void setup() {
  heltec_setup();
  
  both.println("Radio init");
  RADIOLIB_OR_HALT(radio.begin());
  
  radio.setDio1Action(rx);
  
  both.printf("Frequency: %.2f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  
  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  
  both.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
  
  both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
  
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  
  display.clear();
  display.drawString(0, 0, "RX Listening");
  display.display();
  
  both.println("RX Listening");
}

void loop() {
  heltec_loop();
  
  if (rxFlag) {
    rxFlag = false;
    radio.readData(rxdata);
    
    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      count++;
      
      int rssi = radio.getRSSI();
      float snr = radio.getSNR();
      
      both.printf("RX [%s]\n", rxdata.c_str());
      both.printf("  RSSI: %.2f dBm\n", rssi);
      both.printf("  SNR: %.2f dB\n", snr);
      
      display.clear();
      display.drawString(0, 0, "RECEIVED!");
      display.drawString(0, 20, rxdata);
      display.drawString(0, 40, "RSSI:" + String(rssi));
      display.display();
      
      delay(2000);
      
      display.clear();
      display.drawString(0, 0, "RX Listening");
      display.drawString(0, 20, "Count:" + String(count));
      display.display();
    }
    
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
}

void rx() {
  rxFlag = true;
}
