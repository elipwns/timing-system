# Timing Gate Hardware Design Spec

Self-contained, solar-powered IR beam break timing gates. Two gates total (start + finish), each gate consisting of two independent housings on tripods — one emitter side, one receiver side. No wires cross the track.

---

## Physical Setup

```
[Emitter housing]                        [Receiver housing]
  IR LED ──────────────────────────────→  Phototransistor
  (dumb, just needs power)                (smart, ESP32 + LoRa)

  ¼-20 tripod mount                       ¼-20 tripod mount
  weighted tripod on asphalt              weighted tripod on asphalt
```

- Gate width: ~10-15 feet (start line width + clearance for sideways entry)
- Each housing is self-contained: solar panel + LiPo, no cables between sides
- Both housings point at each other — alignment is handled by rotating the tripod head
- Short alignment tube around IR elements narrows acceptance angle, makes alignment positive and deliberate
- Two distinct housing designs: emitter is compact/square (square panel), receiver is longer (rectangular panel + antenna). Immediately visually distinct at event setup.

---

## Charge Controller — bq25185 (both housings)

**Adafruit product ID**: 6106
**URL**: https://www.adafruit.com/product/6106
**Price**: $8.95 (volume breaks at 10+ and 100+)

Used on both emitter and receiver. Standardized BOM across all solar node projects (timing gates + Meshtastic outdoor node).

**Why bq25185 over TP4056:**
- Proper solar + USB-C input handling simultaneously — no diode hack needed, no simultaneous charge/load problem
- Near-MPPT solar optimization built in — adjusts current draw to keep panel at peak voltage
- Power path to load — draws from charger first, remainder goes to battery. Battery isn't constantly cycling.
- Regulated 5V boost output (TPS61023) — clean 5V to the load regardless of battery state
- Three onboard status LEDs (charging, fault, 3.3V output)
- JST PH 2-pin battery connector, terminal block output, USB-C input, DC/solar solder pads
- Mounting holes

**Connections:**
- Solar panel → DC/solar solder pads (5-18V input, no external diode needed)
- LiPo → JST PH 2-pin BATT port
- USB-C → charging input (backup / overnight)
- Load (IR LED or ESP32) → 5V terminal block output

---

## Solar Panels — Sourcing

Both panels are from Voltaic Systems via Adafruit. ETFE coating — superior to epoxy or PET, IP67 waterproof, UV resistant, 5-7 year rated life, 22+% efficiency monocrystalline SunPower cells. 2-year warranty. Solder pads on back (no connector included).

**Note on voltage:** Both panels are nominally 5V but output 6-6.1V at peak power point. The bq25185 accepts 5-18V solar input — both panels are well within range.

### Emitter panel — Voltaic P123
- **Adafruit product ID**: 5856
- **URL**: https://www.adafruit.com/product/5856
- **Price**: $8.95
- **Dimensions**: 65.5 x 65.5 x 3.1mm (square)
- **Output**: 5V nominal, ~6V peak, 120mA peak, 0.6W
- **Stock**: In stock (68 units as of March 2026)
- **Why**: Square footprint drives a compact symmetric emitter housing. 0.6W is more than sufficient for ~45mA emitter draw. Visually distinct from receiver.

### Receiver panel — Voltaic P124
- **Adafruit product ID**: 5368
- **URL**: https://www.adafruit.com/product/5368
- **Price**: $14.95
- **Dimensions**: 66 x 113 x 2.6mm (rectangular)
- **Output**: 5V nominal, ~6.07V peak, 200mA peak, 1.2W
- **Stock**: In stock (as of March 2026)
- **Why**: Higher output suits ESP32 + LoRa draw (~80-100mA avg). Rectangular lid makes receiver housing immediately identifiable vs emitter. Antenna on side is the other visual differentiator.

### Voltaic lineup stock status (as of March 2026)
| Panel | Status |
|---|---|
| Small 6V 1W | Out of stock |
| Medium 6V 2W | Out of stock |
| Large 6V 3.5W | Out of stock |
| Huge 6V 6W | No longer stocked |
| Colossal 6V 9W | Out of stock |
| 2V 0.3W ETFE | $5.50 — in stock |
| **5V 1.2W ETFE (P124)** | **$14.95 — in stock ✅ (receiver)** |
| 5V 0.3W ETFE | Out of stock |
| **5V 0.6W ETFE (P123)** | **$8.95 — in stock ✅ (emitter)** |
| 6V 2W ETFE | $20.95 — in stock |
| 5V 5W ETFE | $34.95 — in stock |
| 5V 10W ETFE | $64.95 — in stock |

---

## Receiver Housing (Smart Side)

This is the "brain" of each gate. One per gate (x2 total for start + finish).

### Electronics
- **ESP32**: Heltec V3 or V4
- **Charge controller**: Adafruit bq25185 (#6106)
- **Battery**: flat LiPo, ~2000mAh, JST PH 2-pin connector
- **Solar panel**: Voltaic P124, 66 x 113mm, rectangular lid face
- **Sensor**: IR phototransistor, forward-facing through alignment tube
- **Radio**: LoRa antenna — SMA bulkhead on enclosure side

### Status LEDs
- **Battery level**: driven from bq25185 status pins — charging (orange), fault (red), 3.3V output (green) onboard. Additional external LEDs for beam status if needed.
- **Beam aligned**: lights up when phototransistor is actively receiving IR signal — primary alignment aid during event setup
- **LoRa activity**: blinks on transmit from ESP32 GPIO

### Behavior
- Beam aligned LED makes setup fast — just rotate tripod until it goes green
- On beam break: ESP32 fires LoRa message → hub node at timing table
- Deep sleep between events to conserve power
- Solar keeps it topped up all day; USB-C for overnight charging

### Mechanical
- ¼-20 threaded insert on bottom for tripod mount
- Solar panel (66 x 113mm) as top lid face
- IR phototransistor mounted in forward-facing tube (10-15mm) to narrow acceptance angle
- SMA bulkhead on side for LoRa antenna
- USB-C port accessible on side/bottom (bq25185 onboard connector)
- Weatherproof — sealed enough for outdoor event use

---

## Emitter Housing (Dumb Side)

Just needs to blast IR light continuously. One per gate (x2 total).

### Electronics
- **IR LED**: forward-facing 940nm, ~40mA continuous
- **Current limiting resistor**: calculate based on 5V bq25185 output
- **Charge controller**: Adafruit bq25185 (#6106) — same board as receiver, standardized BOM
- **Battery**: flat LiPo ~2000mAh, JST PH 2-pin connector
- **Solar panel**: Voltaic P123, 65.5 x 65.5mm, square lid face

### Status LEDs
- **Power on / charging status**: bq25185 onboard LEDs visible or piped out
- **Aim assist**: visible-light LED (red or green) mounted directly next to the IR LED — coarse visual aiming before IR alignment confirmed. IR is invisible; this makes setup fast.

### Mechanical
- ¼-20 threaded insert on bottom for tripod mount
- Solar panel (65.5 x 65.5mm) as square top lid face
- IR LED mounted in forward-facing alignment tube, same spec as receiver side
- Aim assist LED adjacent to IR LED, same forward face
- USB-C port accessible on side/bottom (bq25185 onboard connector)
- NO antenna — key visual differentiator from receiver
- Weatherproof

---

## Alignment Tube Spec

A short tube (10-15mm length, ~8mm ID) around each IR element:
- Narrows the beam/acceptance cone so alignment has to be deliberate
- Reduces false triggers from ambient IR (sun, other lights)
- Makes the "beam aligned" LED on the receiver more meaningful — it only lights when you're actually on axis
- Same tube spec on both emitter and receiver for symmetry

---

## Power Notes

### Both housings — standardized stack
- **Charge controller**: bq25185 — solar + USB-C input, 5V boost output, power path management
- **Battery**: flat LiPo ~2000mAh
- **No external diodes needed** — bq25185 handles solar/USB-C priority internally

### Emitter power budget
- Draw: ~45mA continuous (IR LED + indicator LEDs) from 5V output
- Worst case (3-day event, zero solar): 45mA × 9hr × 3 = 1215mAh
- 2000mAh covers worst case with 65% headroom
- Solar (P123, 0.6W, ~120mA peak): near net-zero even on cloudy days

### Receiver power budget
- Draw: ~80-100mA average (ESP32 light sleep + LoRa activity) from 5V output
- 2000mAh covers a full event day solo; solar extends to multi-day no-charge
- Solar (P124, 1.2W, ~200mA peak): net positive on sunny days

---

## CAD Notes (Onshape)

### Known dimensions
- bq25185 board: need to measure — check Adafruit product page for PCB dimensions
- Flat LiPo 2000mAh: ~60mm × 40mm × 7mm (verify when ordered)
- Emitter panel (P123): 65.5 × 65.5 × 3.1mm — drives emitter housing footprint
- Receiver panel (P124): 66 × 113 × 2.6mm — drives receiver housing footprint
- ¼-20 threaded insert: verify OD for heat-set fit

### Still needed before receiver modeling
- Heltec V3 / V4 PCB footprint and mounting hole pattern
- SMA bulkhead cutout diameter

### Plan
Emitter housing first — good first Onshape project, simpler internals, same bq25185 board as receiver so dimensions transfer. Measure bq25185 board and LiPo physically before modeling. Dial in tube alignment, tripod mount, and panel lid fit on emitter, then carry those lessons into receiver.

---

## MakerWorld Search Notes

Before full custom design, check MakerWorld for:
- Electronics enclosures with ¼-20 tripod mounts (photography accessory category)
- bq25185 project enclosures (Adafruit even sells a snap-on enclosure for this board: product #6126)

Likely still need custom design for full integrated housing, but reference prints may help validate dimensions and fitment approach.
