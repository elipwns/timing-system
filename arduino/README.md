# LoRa Communication Test Programs

## Files
- `lora_transmitter/lora_transmitter.ino` - Transmitter (Start Line Board)
- `lora_receiver/lora_receiver.ino` - Receiver (Finish Line Board)

## Setup Instructions

### Step 1: Program Transmitter (Board 1)
1. Open `lora_transmitter/lora_transmitter.ino` in Arduino IDE
2. Select Board: "Heltec WiFi LoRa 32(V3)"
3. Select Port: Your board's COM port
4. Upload
5. Disconnect from USB
6. Power with battery bank

### Step 2: Program Receiver (Board 2)
1. Open `lora_receiver/lora_receiver.ino` in Arduino IDE
2. Select Board: "Heltec WiFi LoRa 32(V3)"
3. Select Port: Your board's COM port
4. Upload
5. Keep connected to USB
6. Open Serial Monitor (115200 baud)

### Step 3: Test
1. Press PRG button on transmitter board
2. Receiver should display "RECEIVED!" with message
3. Serial Monitor shows RSSI (signal strength)

## Troubleshooting

### No messages received
- Check both antennas are attached
- Verify both boards show same frequency in Serial Monitor
- Try moving boards closer (start at 1 meter)
- Check LoRa region setting: Tools → LoRa Region

### RSSI Values
- -30 to -50 dBm: Excellent (very close)
- -50 to -70 dBm: Good (normal range)
- -70 to -90 dBm: Fair (getting far)
- -90 to -120 dBm: Weak (at limit)

## Key Changes from Previous Code
- Added `radio.startReceive()` in receiver setup
- Added `radio.startReceive()` after displaying received message
- This puts the radio in continuous receive mode
- Without this, the radio only checks once and stops listening

## Next Steps
Once LoRa communication works:
1. Add timing logic (start/finish)
2. Calculate elapsed time
3. Send results to dashboard
4. Build presentation demo
