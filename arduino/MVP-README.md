# MVP Drag Strip Timing System

## Simple, One-Lane Timing

**What it does:**
1. Press button on START board → sends START signal via LoRa
2. FINISH board receives START → starts internal timer
3. Press button on FINISH board → stops timer, calculates elapsed time
4. FINISH board sends result back to START board
5. Both boards display the time

**No sensors, no dashboard, no complexity. Just timing.**

---

## Upload Instructions

### Board 1: Start Line
1. Open `timing_start/timing_start.ino`
2. Upload to first Heltec board
3. This board can run on battery or USB

### Board 2: Finish Line
1. Open `timing_finish/timing_finish.ino`
2. Upload to second Heltec board
3. Keep this one connected to USB for Serial Monitor

---

## How to Use

1. **Power on both boards**
   - START board shows "START LINE - Press to START"
   - FINISH board shows "FINISH LINE - Waiting..."

2. **Press button on START board**
   - START board sends LoRa message
   - FINISH board receives it and starts timer
   - Both displays update

3. **Press button on FINISH board**
   - Timer stops
   - Elapsed time calculated
   - Result sent back to START board via LoRa
   - Both boards display the time

4. **Repeat**
   - Press START button again for next run
   - Run numbers increment automatically

---

## Message Protocol

### START Message
```
Format: "START:runNumber"
Example: "START:1"
```

### RESULT Message
```
Format: "RESULT:runNumber:time"
Example: "RESULT:1:12.347"
```

---

## Display States

### START Board:
- **Ready:** "START LINE - Press to START"
- **Sending:** "SENDING START - Run #X"
- **Waiting:** "RUN IN PROGRESS - Waiting..."
- **Result:** "RUN COMPLETE! - 12.347s"

### FINISH Board:
- **Ready:** "FINISH LINE - Waiting..."
- **Running:** "RUN IN PROGRESS - Press to FINISH"
- **Finished:** "FINISHED! - 12.347s"
- **Sending:** "Sending result..."

---

## Testing

### Test 1: Basic Timing
1. Place boards on table
2. Press START
3. Wait 5 seconds
4. Press FINISH
5. Should show ~5.000 seconds

### Test 2: Multiple Runs
1. Do 3 runs in a row
2. Run numbers should increment (1, 2, 3)
3. Times should be accurate

### Test 3: Range
1. Move boards apart
2. Test at 1m, 5m, 10m, 20m
3. Verify LoRa communication works

---

## Serial Monitor Output

### START Board:
```
START LINE Ready
Sending: START:1
Result: 12.347s
```

### FINISH Board:
```
FINISH LINE Ready
START received! Run #1
FINISH! Time: 12.347 seconds
Result sent!
```

---

## Next Steps

Once this works:
1. Add car number input (via Serial or button cycling)
2. Add dashboard to display results
3. Add sensors (IR beam, laser) to replace buttons
4. Add split timing (60ft, 1/8 mile)
5. Add second lane

But for now: **Keep it simple. Make it work.**

---

## Troubleshooting

**No START signal received:**
- Check both boards are on same frequency (905.2 MHz)
- Check antennas are attached
- Move boards closer together
- Check Serial Monitor for errors

**Time seems wrong:**
- ESP32 millis() is accurate to ~1ms
- Check you're not holding button too long
- Verify timer starts when START is received

**Result not received at START:**
- FINISH board must finish transmitting before START can receive
- Check Serial Monitor on FINISH board
- Verify LoRa communication is working both ways

---

## Future Enhancements

**Hardware:**
- IR beam sensors instead of buttons
- Weatherproof enclosures
- Solar power for remote operation

**Software:**
- Car number association
- Split timing at 60ft, 1/8 mile
- Data logging to SD card
- Dashboard web interface

**Features:**
- Multi-lane support
- Reaction time measurement
- Best time tracking
- Export to CSV

---

This is the foundation. Build on it step by step.
