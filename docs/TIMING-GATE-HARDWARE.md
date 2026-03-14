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

## Full BOM

| Qty | Component | Source | Part # | Price | Notes |
|---|---|---|---|---|---|
| 5 | bq25185 5V Boost Charger | Adafruit | 6106 | $8.95 | 2x emitter, 2x receiver, 1x Meshtastic |
| 2 | Voltaic P123 0.6W panel | Adafruit | 5856 | $8.95 | Emitter lid, 65.5×65.5mm square |
| 2 | Voltaic P124 1.2W panel | Adafruit | 5368 | $14.95 | Receiver lid, 66×113mm rectangular |
| 2 | IR Break Beam 5mm | Adafruit | 2168 | $5.95 | One set per gate, 2 gates total |
| 8 | JLJLUP 2000mAh LiPo | Amazon | JLJLUP LP103450 | ~$6.00 | 5 deployed + 3 spares, PH 2.0mm connector |

**Already on hand:**
- Heltec V3 x2 (current timing nodes), Heltec V4 x2, HTIT-Tracker x1
- Heat set inserts (¼-20)
- Schottky diodes
- Soldering iron, wire strippers, Dremel, multimeter, digital calipers

---

## Charge Controller — bq25185 (#6106)

**URL**: https://www.adafruit.com/product/6106 — $8.95

Used on both emitter and receiver. Same board also used on Meshtastic outdoor node — fully standardized power stack across all solar projects.

**Key features for this use case:**
- Solar + USB-C input simultaneously, no external diode needed
- Near-MPPT solar optimization — adjusts draw to keep panel at peak voltage
- Power path to load — draws from charger first, battery gets remainder. No constant charge/discharge cycling.
- Regulated 5V boost output — clean 5V regardless of battery state
- Three onboard LEDs: orange (charging), red (fault), green (3.3V good)
- JST PH 2-pin battery port, screw terminal 5V output, USB-C input, solar solder pads

**Connections:**
- Solar panel → VIN/G solder pads (5-18V, no diode needed)
- LiPo → JST PH 2-pin BATT port
- USB-C → onboard connector, backup/overnight charge
- Load → 5V screw terminal output

---

## Battery — JLJLUP 2000mAh LiPo

**Source**: Amazon — JLJLUP LP103450 4-pack ~$24
**Dimensions**: 34 × 52 × 10mm
**Connector**: JST PH 2.0mm (matches bq25185 natively)
**Capacity**: 2000mAh @ 3.7V
**Protection**: Built-in PCM (over/under charge, over current, short circuit)

### Connector polarity — verify before first connect
Most hobby LiPos including this one follow the Adafruit/JST standard: **red = positive, black = negative**. The bq25185 is wired to match. However polarity is not guaranteed universal across all manufacturers.

**Before plugging in for the first time:**
1. Probe bq25185 JST socket with multimeter — confirm which pin is positive
2. Confirm red wire on battery = P+
3. If reversed: pop pins from JST housing with a small flathead, swap them. 30 second fix, no soldering.

---

## Solar Panels

Both from Voltaic Systems via Adafruit. ETFE coating, IP67, UV resistant, 5-7 year rated life, 22+% monocrystalline SunPower cells, 2-year warranty. Solder pads on back — solder wires directly, no connector included.

**Voltage note:** Both panels output ~6-6.1V at peak. bq25185 accepts 5-18V solar input — both panels are well within spec. No voltage regulator needed.

### Emitter panel — Voltaic P123 (#5856)
- 65.5 × 65.5 × 3.1mm — square, drives compact emitter housing footprint
- 0.6W, ~120mA peak — more than sufficient for ~45mA emitter draw
- https://www.adafruit.com/product/5856

### Receiver panel — Voltaic P124 (#5368)
- 66 × 113 × 2.6mm — rectangular, drives longer receiver housing footprint
- 1.2W, ~200mA peak — suits ESP32 + LoRa draw (~80-100mA avg)
- https://www.adafruit.com/product/5368

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

## IR Sensors — Adafruit Break Beam #2168

**URL**: https://www.adafruit.com/product/2168 — $5.95/set
**LED package**: 5mm diameter
**Comes with**: pre-wired header ends on both emitter and receiver — wires run back into housing, no bare LED soldering needed

One set per gate = 2 sets total for start + finish. 2 sets ordered (4 total) for spares.

---

## Receiver Housing (Smart Side)

One per gate (x2 total).

### Electronics
- Heltec V3 or V4 ESP32
- bq25185 (#6106)
- JLJLUP 2000mAh LiPo
- Voltaic P124 panel (66×113mm lid)
- IR phototransistor from break beam set, forward-facing through alignment tube
- SMA bulkhead for LoRa antenna

### Status indicators
- bq25185 onboard LEDs: charging, fault, power good
- **Beam aligned LED**: external LED on GPIO — lights when phototransistor sees IR beam. Primary setup aid — rotate tripod until it goes green.
- **LoRa activity LED**: blinks on transmit

### Mechanical
- ¼-20 heat set insert on bottom — tripod mount
- Voltaic P124 (66×113mm) as top lid face
- IR phototransistor in forward-facing 6mm ID alignment tube (10-15mm long)
- SMA bulkhead on side
- USB-C port accessible on side/bottom
- Weatherproof

---

## Emitter Housing (Dumb Side)

One per gate (x2 total).

### Electronics
- bq25185 (#6106)
- JLJLUP 2000mAh LiPo
- Voltaic P123 panel (65.5×65.5mm lid)
- IR LED from break beam set, forward-facing through alignment tube
- Current limiting resistor (calculate for 5V supply, ~40mA target)

### Status indicators
- bq25185 onboard LEDs visible through housing or piped out
- **Aim assist LED**: visible red or green LED adjacent to IR LED on front face — coarse visual aiming before IR beam alignment confirmed

### Mechanical
- ¼-20 heat set insert on bottom — tripod mount
- Voltaic P123 (65.5×65.5mm) as square top lid face
- IR LED in forward-facing 6mm ID alignment tube (10-15mm long)
- Aim assist LED adjacent to tube on front face
- USB-C port accessible on side/bottom
- NO antenna — key visual differentiator from receiver
- Weatherproof

---

## Alignment Tube Spec

- **ID**: 6mm (5mm LED package + 1mm clearance)
- **Length**: 10-15mm
- Narrows beam/acceptance cone — alignment must be deliberate
- Reduces false triggers from ambient IR (sun, stadium lights)
- Same spec on both emitter and receiver for symmetry
- Tube is printed as part of the housing front face

---

## Power Budget

### Emitter
- Draw: ~45mA continuous (IR LED + LEDs) from 5V
- Worst case 3-day, zero solar: 45mA × 9hr × 3 = 1215mAh
- 2000mAh battery: 65% headroom over worst case
- P123 solar at 30% cloud efficiency (~36mA): near net-zero draw

### Receiver
- Draw: ~80-100mA average (ESP32 + LoRa) from 5V
- 2000mAh covers a full event day; solar extends to multi-day
- P124 solar at peak (~200mA): net positive on sunny days

---

## CAD Notes (Onshape)

### Confirmed dimensions
| Component | Dimensions |
|---|---|
| bq25185 board | 32 × 26.3 × 7.2mm (from Adafruit listing) |
| JLJLUP LiPo | 34 × 52 × 10mm (verify with calipers on arrival) |
| IR LED/receiver package | 5mm diameter |
| Alignment tube ID | 6mm |
| Emitter panel (P123) | 65.5 × 65.5 × 3.1mm |
| Receiver panel (P124) | 66 × 113 × 2.6mm |
| Heat set insert OD | Measure with calipers before modeling |

### Still needed before receiver modeling
- Heltec V3/V4 PCB footprint and mounting hole pattern
- SMA bulkhead cutout diameter

### Plan
Emitter housing first — simpler internals, no Heltec measurements needed. Verify all dimensions with calipers when parts arrive before starting sketch. Dial in tube, tripod mount, and panel lid fit on emitter, then carry those lessons to receiver.

---

## MakerWorld Search Notes

- Electronics enclosures with ¼-20 tripod mounts (photography accessory category)
- Adafruit sells a snap-on enclosure for bq25185: product #6126 — useful for dimension reference

Full custom design likely needed for integrated housing, but reference prints help validate fitment.
