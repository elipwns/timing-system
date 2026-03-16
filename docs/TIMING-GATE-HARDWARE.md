# Timing Gate Hardware Design Spec

Self-contained, solar-powered IR beam break timing gates. Two gates total (start + finish), each gate consisting of two independent housings on tripods — one emitter side, one receiver side. No wires cross the track. A separate hub/base station sits at the timing table handles all data aggregation and cloud upload.

---

## Three Enclosure Types

| Enclosure | Qty | Lives | Key constraint |
|---|---|---|---|
| **Emitter** | 2 | Field, tripod, weatherproof | Square solar lid, TC102 emitter mounted externally, power supply box |
| **Detector** | 2 | Field, tripod, weatherproof | Rectangular solar lid, TC102 receiver + ESP32 + LoRa, relay interface |
| **Hub / base station** | 1 | Timing table, semi-protected | GPS, WiFi, environmental sensors, display |

---

## Physical Setup

```
[Emitter enclosure]                        [Detector enclosure]
  bq25185 + LiPo + solar                    bq25185 + LiPo + solar + ESP32
  MT3608 boost → 12V                        MT3608 boost → 12V
  TC102 emitter (external mount)            TC102 receiver (external mount)
  IR beam ──────────────────────────────→   relay → ESP32 GPIO interrupt

  ¼-20 tripod mount                         ¼-20 tripod mount

                    ESP32 fires LoRa → [Hub at timing table] → WiFi → AWS
```

- Gate width: up to 65 feet (TC102 spec), well beyond 10-15 foot requirement
- Each housing is self-contained: solar + LiPo, no cables across track
- TC102 sensors mount externally on the enclosure or tripod head, wired to power box
- TC102 has built-in alignment LED on receiver — status LED turns off when aligned

---

## IR Sensor — TOPENS TC102 Photocell Beam Sensor ✅ SELECTED

**Source**: Amazon ~$15-20/pair
**Replaces**: Adafruit #2168 (max 50cm range — insufficient for 10-15 foot gate)

### Specifications
| Spec | Value |
|---|---|
| Max range | 20m / 65 feet |
| Operating voltage | 12-24V DC/AC |
| Emitter current | 30mA @ 12V |
| Receiver current | 15mA @ 12V |
| Wavelength | 940nm |
| Frequency | 1.92kHz modulated |
| Beam angle | <±10° |
| Response time | 10ms max |
| Output | Relay SPDT (NO/NC dry contact) |
| Weather rating | Outdoor rated |
| Dimensions | 75 × 48 × 30mm (both emitter and receiver) |
| Alignment | Built-in status LED — turns off when beam is aligned |

### Why TC102 vs Adafruit #2168
- Adafruit #2168: 50cm max range — cannot span 10-15 foot autocross gate
- TC102: 65 foot range, outdoor rated, modulated IR (rejects sunlight), built-in alignment LED
- TC102 is purpose-built for exactly this use case (outdoor gate/barrier detection)

### Relay to ESP32 GPIO interface
The TC102 receiver outputs a relay contact (NO/NC), not a direct GPIO signal. Interface is simple:
```
TC102 relay NO terminal → ESP32 GPIO pin
TC102 relay COM terminal → GND
ESP32 GPIO → 10kΩ pull-up resistor → 3.3V

Beam intact  = relay closed = GPIO pulled LOW
Beam broken  = relay open  = GPIO pulled HIGH (interrupt fires)
```
Standard active-high interrupt. No additional components beyond the pull-up resistor.

### Power architecture change
TC102 requires 12V. bq25185 outputs 5V. Solution: MT3608 boost converter module (5V → 12V).
- MT3608 module: ~$1-2, tiny, 85% efficient
- Sits inside enclosure between bq25185 5V output and TC102 power wires
- Both emitter and detector enclosures need one MT3608 each

---

## Shopping List — To Order

| Item | Qty | Purpose | Notes |
|---|---|---|---|
| **TOPENS TC102 photocell sensor** | 2 pairs (4 total) | 2 deployed gates + 2 spare pairs | Search Amazon "TOPENS TC102". ~$15-20/pair. |
| **MT3608 boost converter module** | 4+ | 5V→12V for TC102 in each enclosure | Pre-adjust to 12V output before installing. Search "MT3608 boost module". ~$1-2 each. |
| **¼-20 brass heat set inserts** | 8+ | Tripod mount boss, bottom of each enclosure | Measure OD on arrival before designing boss hole. |
| **ASA filament** | 1-2 spools | Final field enclosure prints | UV stable, high heat tolerance. Bambu ASA recommended. |
| **TPU filament** | 1 spool | Gaskets for all enclosures | 95A Shore hardness. Bambu TPU 95A recommended. |
| **Waterproof USB-C panel mount** | 4+ | USB-C charging access on enclosure side | Need ~12-14mm cutout. Measure OD before designing cutout. 26mm unit on hand is too large. |
| **M3 screws** | 20+ | Lid to body fastening (4 per enclosure) | Button or flat head, length TBD once lid thickness confirmed. ~8-12mm. |
| **10kΩ resistors** | 10+ | Pull-up on ESP32 GPIO for relay interface | Standard 1/4W. Probably already on hand. |

---

## Full BOM

| Qty | Component | Source | Part # | Price | Notes |
|---|---|---|---|---|---|
| 4 | TOPENS TC102 photocell | Amazon | TC102 | ~$15-20/pair | 2 pairs — 2 gates deployed |
| 4 | MT3608 boost converter | Amazon | — | ~$1-2 ea | 5V→12V, one per enclosure |
| 5 | bq25185 5V Boost Charger | Adafruit | 6106 | $8.95 | 2x emitter, 2x detector, 1x hub |
| 2 | Voltaic P123 0.6W panel | Adafruit | 5856 | $8.95 | Emitter lid, 65.5×65.5mm square |
| 2 | Voltaic P124 1.2W panel | Adafruit | 5368 | $14.95 | Detector lid, 66×113mm rectangular |
| 4 | JLJLUP 2000mAh LiPo | Amazon | LP103450 | ~$6.00 | 2x emitter, 2x detector |
| 4 | spare JLJLUP 2000mAh LiPo | Amazon | LP103450 | ~$6.00 | Spares |

**Already on hand:**
- Heltec V3 x2, Heltec V4 x2, HTIT-Tracker x1
- M3 heat set inserts (OD 4.53mm caliper-verified)
- Schottky diodes, 10kΩ resistors (verify qty)
- Soldering iron, wire strippers, Dremel, multimeter, digital calipers
- 3000mAh LiPo batteries (66mm — reserved for detector or Meshtastic)
- Double-sided tape pads (2.3 × 11.7 × 11.7mm)
- Waterproof USB connector (26mm OD — too large, reserved for other project)
- Adafruit #2168 break beam sensors (50cm range — cannot use for gate, repurpose for short-range projects)

---

## Power Budget (Updated for TC102)

All calculations assume 12hr event day, MT3608 boost at 85% efficiency.

### Emitter Node (TC102 emitter + bq25185 + MT3608 boost)

| Component | Current Draw |
|---|---|
| TC102 emitter @ 12V | 30mA |
| MT3608 efficiency loss (5V side) | 30mA × 12V / (5V × 0.85) = **84mA from 5V** |
| bq25185 quiescent | ~5mA |
| MT3608 quiescent | ~2mA |
| **Total from battery** | **~91mA continuous** |

**Battery:** 91mA × 12hr = 1092mAh required → 2000mAh covers with **83% headroom** ✅
**Solar (P123, 30% cloud efficiency):** 100mA peak × 30% = 30mA average → net drain 61mA in cloud
**Worst case (no solar, 3 days):** 91mA × 72hr = 6552mAh — battery needs recharge after ~22hrs without solar. With any sun: net positive.
**Sunny event day:** P123 at peak (100mA) > draw (91mA) → net positive, battery charges ✅

### Detector Node (TC102 receiver + ESP32 + bq25185 + MT3608)

| Component | Current Draw |
|---|---|
| TC102 receiver @ 12V | 15mA |
| MT3608 efficiency loss (5V side) | 15mA × 12V / (5V × 0.85) = **42mA from 5V** |
| ESP32 active (Heltec V4, avg) | ~80mA |
| bq25185 quiescent | ~5mA |
| MT3608 quiescent | ~2mA |
| **Total from battery** | **~129mA continuous** |

**Battery:** 129mA × 12hr = 1548mAh required → 2000mAh covers with **29% headroom** ✅
**Solar (P124, 30% cloud efficiency):** 200mA peak × 30% = 60mA average → net drain 69mA in cloud
**Sunny event day:** P124 at peak (200mA) > draw (129mA) → net positive, battery charges ✅
**Multi-day without sun:** drains ~1656mAh/day — replace or recharge battery after each event day in prolonged cloud.

### Panel Sizing Verdict
P123 (emitter) and P124 (detector) remain the correct panel selections even with TC102 power draw. Both are net positive on a sunny event day. Battery alone covers a full 12hr event with margin on both nodes.

---

## Charge Controller — bq25185 (#6106)

**URL**: https://www.adafruit.com/product/6106 — $8.95

**Key features:**
- Solar + USB-C simultaneously, no external diode needed
- Near-MPPT solar optimization
- Regulated 5V boost output regardless of battery state
- Onboard LEDs: orange (charging), red (fault), green (3.3V good)
- JST PH 2-pin battery, screw terminal 5V out, USB-C in, solar solder pads

**Connections:**
- Solar → VIN/G solder pads
- LiPo → JST PH 2-pin BATT
- bq25185 5V out → MT3608 boost input → 12V → TC102

---

## Battery — JLJLUP 2000mAh LiPo (ordered)

**Dimensions**: 34 × 52 × 10mm (spec — verify on arrival 🔲)
**Connector**: JST PH 2.0mm — matches bq25185 natively

### Polarity — verify before first connect
Verify with multimeter before first plug-in. If reversed: pop JST pins and swap.

---

## Battery — 3000mAh LiPo (on hand) ✅

**Dimensions**: 10.3 × 36 × 66mm (caliper-verified)
Reserved for detector node or Meshtastic — 66mm length doesn't fit emitter footprint.

---

## Double-Sided Tape Pads (on hand) ✅

**Dimensions**: 2.3 × 11.7 × 11.7mm (caliper-verified)

---

## Solar Panels

### Emitter — Voltaic P123 (#5856) ✅
- 65.5 × 65.5 × 3.1mm square, 0.6W, ~100mA peak
- https://www.adafruit.com/product/5856

### Detector — Voltaic P124 (#5368) ✅
- 66 × 113 × 2.6mm rectangular, 1.2W, ~200mA peak
- https://www.adafruit.com/product/5368

Both verified in stock as of March 2026 at Adafruit.

---

## Enclosure 1 — Emitter Housing

**Role:** Power supply box only. TC102 emitter mounts externally on enclosure face or tripod head.

### Electronics
- bq25185 (#6106) — solar charging, 5V regulated output
- JLJLUP 2000mAh LiPo
- Voltaic P123 lid
- MT3608 boost module (5V → 12V)
- Power switch (micro mini slide switch, geometry verified)
- Red power indicator LED

### Mechanical
- 80×80×30mm outer body, 5mm walls
- ¼-20 heat set insert, bottom — tripod mount (insert on order)
- Voltaic P123 panel recessed in lid tray
- TPU gasket, M3 corner screws outside gasket line
- Waterproof USB-C panel mount on back face (connector on order)
- Power switch + red LED on back face
- TC102 emitter mounts on front face externally — cable gland or weatherproof pass-through for 12V wires
- 5mm exterior fillets

### TODO before final print
- [ ] **USB-C cutout** — measure connector OD on arrival
- [ ] **¼-20 tripod boss** — measure insert OD on arrival
- [ ] **Switch pocket** — use verified geometry from switch-pocket-test
- [ ] **Cable gland / pass-through** — for TC102 emitter wiring, front face
- [ ] **TC102 external mount bracket** — printed bracket to hold TC102 on front face or tripod head

### Power budget
- **~91mA continuous draw**
- 2000mAh battery: ~22hr runtime without solar
- P123 sunny day: net positive ✅
- Full 12hr event day: 1092mAh used, 908mAh remaining ✅

---

## Enclosure 2 — Detector Housing

**Role:** ESP32 + LoRa + power supply. TC102 receiver mounts externally, relay wired to ESP32 GPIO.

### Electronics
- Heltec V3 or V4 ESP32
- bq25185 (#6106)
- JLJLUP 2000mAh LiPo (or 3000mAh on hand)
- Voltaic P124 lid
- MT3608 boost module (5V → 12V)
- TC102 receiver relay → 10kΩ pull-up → ESP32 GPIO interrupt
- SMA bulkhead for LoRa antenna

### Status indicators
- bq25185 onboard LEDs
- TC102 built-in alignment LED (on receiver itself)
- LoRa activity LED: blinks on transmit (ESP32 GPIO)

### Mechanical
- 80×80×30mm outer body (or sized to fit Heltec V4 — measure first 🔲)
- ¼-20 heat set insert, bottom — tripod mount
- Voltaic P124 panel recessed in lid tray
- TPU gasket, M3 corner screws outside gasket line
- SMA bulkhead on side
- Waterproof USB-C panel mount on back face
- Cable gland / pass-through for TC102 receiver wiring
- TC102 external mount bracket on front face or tripod head

### TODO before final print
- [ ] **Measure Heltec V4 PCB** — may need to resize enclosure footprint
- [ ] **USB-C cutout** — measure connector OD
- [ ] **¼-20 tripod boss** — measure insert OD
- [ ] **SMA bulkhead cutout** — measure bulkhead OD
- [ ] **Cable gland** — for TC102 receiver wiring

### Power budget
- **~129mA continuous draw**
- 2000mAh battery: ~15.5hr runtime without solar
- P124 sunny day: net positive ✅
- Full 12hr event day: 1548mAh used, 452mAh remaining ✅

---

## Enclosure 3 — Hub / Base Station

Deferred. Lives at timing table, USB powered. HTIT-Tracker. Environmental sensors TBD.

---

## Heat Set Insert Notes

- M3 insert OD: 4.53mm (caliper-verified) → print hole 4.3mm for PETG
- Iron temp: 230°C for PETG
- ¼-20 insert OD: TBD — measure on arrival

---

## Print Material Plan

| Use | Material | Notes |
|---|---|---|
| Test/geometry prints | PLA | Fitment checks only |
| Intermediate tests | PETG | Insert fitment, tolerance validation |
| Final field enclosures | ASA | UV stable, outdoor rated — on order |
| Gaskets | TPU | 95A, on order |

---

## CAD Notes (Onshape)

### ⚠️ Parts Verification Rule
Do not finalize enclosure dimensions until parts are in hand and measured with calipers.

### Confirmed dimensions (caliper-verified ✅)
| Component | Dimensions |
|---|---|
| Micro mini slide switch | 8.64 × 3.74 × 3.60mm body, 1.5mm actuator, ~2mm travel |
| Switch pocket geometry | 8.84 × 3.94mm pocket, 3.7 × 1.7mm slot, 1.4mm roof — test printed, fits well |
| 3000mAh LiPo (on hand) | 10.3 × 36 × 66mm |
| Double-sided tape pads | 2.3 × 11.7 × 11.7mm |
| M3 heat set insert OD | 4.53mm — print hole 4.3mm, corner block 12×12mm |

### Verify on arrival 🔲
| Component | Spec | Needed for |
|---|---|---|
| bq25185 board | 32 × 26.3 × 7.2mm | All enclosures |
| JLJLUP 2000mAh LiPo | 34 × 52 × 10mm | All enclosures |
| Voltaic P123 panel | 65.5 × 65.5 × 3.1mm | Emitter lid |
| Voltaic P124 panel | 66 × 113 × 2.6mm | Detector lid |
| MT3608 boost module | TBD | Internal layout |
| TC102 emitter/receiver | 75 × 48 × 30mm (spec) | External mount bracket |
| ¼-20 heat set insert OD | TBD | Tripod boss hole |
| Waterproof USB-C panel mount OD | TBD | Side wall cutout |
| Heltec V3/V4 PCB | TBD | Detector enclosure sizing |
| SMA bulkhead cutout OD | TBD | Detector side wall |

### Current Onshape state
- `timing-emitter-housing-v2` — active
- Body: 80×80×30mm, 5mm walls, 12×12mm corner blocks, gasket channel, insert holes
- Remaining: switch pocket, USB-C cutout, cable gland, tripod boss, lid

---

## MakerWorld Search Notes

- Electronics enclosures with ¼-20 tripod mounts (photography accessory category)
- Gate opener photocell mounting brackets — search for TC102 or similar
