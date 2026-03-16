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
  TC102 emitter (external bracket)          TC102 receiver (external bracket)
  IR beam ──────────────────────────────→   relay → ESP32 GPIO interrupt

  ¼-20 tripod mount                         ¼-20 tripod mount

                    ESP32 fires LoRa → [Hub at timing table] → WiFi → AWS
```

- Gate width: up to 65 feet (TC102 spec), well beyond 10-15 foot requirement
- Each housing is self-contained: solar + LiPo, no cables across track
- TC102 sensors mount on printed bracket attached to front face of enclosure
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
| Mounting | Two screw holes — top and bottom |
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

## TC102 Mounting Bracket (printed, ASA)

The TC102 mounts on a dedicated printed bracket that attaches to the front face of the enclosure. The bracket serves three functions: mechanical mounting, sun shielding, and cable management.

### TC102 physical details
- Oval body, 75 × 48 × 30mm
- Two mounting screw holes — top and bottom of the oval
- Wires exit from bottom wire hole
- Spec notes receiver should be shielded from direct sunlight

### Bracket design requirements
- Mounts to front face of enclosure (M3 screws into enclosure wall)
- Two holes matching TC102 top/bottom mounting hole spacing
- **Sun visor / hood** — printed overhang above the lens face, ~15-20mm projection
  - Receiver spec specifically calls for shielding from direct sunlight
  - Visor blocks high-angle sun (midday) while allowing horizontal beam path
- Cable channel or gland routes TC102 wires through bracket and into enclosure front face
- Same bracket design works for both emitter and detector (identical TC102 units)
- Print in ASA for UV resistance — this part is fully exposed

### TODO
- [ ] Measure TC102 mounting hole spacing (top to bottom) when unit arrives
- [ ] Measure mounting hole diameter
- [ ] Design bracket in Onshape as separate Part Studio
- [ ] Print test fit in PLA first, final in ASA

---

## Shopping List — To Order

| Item | Qty | Purpose | Notes |
|---|---|---|---|
| **TOPENS TC102 photocell sensor** | 2 pairs (4 total) | 2 deployed gates + 2 spare pairs | Search Amazon "TOPENS TC102". ~$15-20/pair. |
| **MT3608 boost converter module** | 4+ | 5V→12V for TC102 in each enclosure | Pre-adjust to 12V output before installing. Search "MT3608 boost module". ~$1-2 each. |
| **¼-20 brass heat set inserts** | 8+ | Tripod mount boss, bottom of each enclosure | Measure OD on arrival before designing boss hole. |
| **ASA filament** | 1-2 spools | Final field enclosure prints + TC102 brackets | UV stable, high heat tolerance. Bambu ASA recommended. |
| **TPU filament** | 1 spool | Gaskets for all enclosures | 95A Shore hardness. Bambu TPU 95A recommended. |
| **Waterproof USB-C panel mount** | 4+ | USB-C charging access on enclosure side | Need ~12-14mm cutout. Measure OD before designing cutout. 26mm unit on hand is too large. |
| **M3 screws** | 20+ | Lid to body fastening (4 per enclosure) | Button or flat head, length TBD once lid thickness confirmed. ~8-12mm. |
| **10kΩ resistors** | 10+ | Pull-up on ESP32 GPIO for relay interface | Standard 1/4W. Probably already on hand — verify. |

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
**Sunny event day:** P123 at peak (100mA) > draw (91mA) → net positive ✅

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
**Sunny event day:** P124 at peak (200mA) > draw (129mA) → net positive ✅

### Panel Sizing Verdict
P123 (emitter) and P124 (detector) remain correct. Both net positive on sunny days. Battery alone covers full 12hr event on both nodes.

---

## Charge Controller — bq25185 (#6106)

**URL**: https://www.adafruit.com/product/6106 — $8.95

**Connections:**
- Solar → VIN/G solder pads
- LiPo → JST PH 2-pin BATT
- bq25185 5V out → MT3608 boost input → 12V → TC102

---

## Battery — JLJLUP 2000mAh LiPo (ordered)

**Dimensions**: 34 × 52 × 10mm (spec — verify on arrival 🔲)
**Connector**: JST PH 2.0mm

### Polarity — verify before first connect
Verify with multimeter. If reversed: pop JST pins and swap.

---

## Battery — 3000mAh LiPo (on hand) ✅

**Dimensions**: 10.3 × 36 × 66mm (caliper-verified)
Reserved for detector node or Meshtastic.

---

## Double-Sided Tape Pads (on hand) ✅

**Dimensions**: 2.3 × 11.7 × 11.7mm (caliper-verified)

---

## Solar Panels

### Emitter — Voltaic P123 (#5856) ✅
- 65.5 × 65.5 × 3.1mm square, 0.6W, ~100mA peak

### Detector — Voltaic P124 (#5368) ✅
- 66 × 113 × 2.6mm rectangular, 1.2W, ~200mA peak

Both in stock at Adafruit as of March 2026.

---

## Enclosure 1 — Emitter Housing

**Role:** Power supply box only. TC102 emitter on external printed bracket.

### Electronics
- bq25185, JLJLUP 2000mAh LiPo, Voltaic P123 lid
- MT3608 boost (5V → 12V)
- Power switch + red power indicator LED
- TC102 emitter wired via cable gland on front face

### Mechanical
- 80×80×30mm outer body, 5mm walls, 5mm exterior fillets
- ¼-20 heat set insert bottom — tripod mount
- TPU gasket, M3 corner screws outside gasket line
- Waterproof USB-C panel mount on back face
- Power switch + LED on back face
- Cable gland on front face for TC102 wiring
- TC102 printed bracket on front face (see TC102 bracket section)

### TODO before final print
- [ ] USB-C cutout — measure connector OD
- [ ] ¼-20 tripod boss — measure insert OD
- [ ] Switch pocket — verified geometry ready
- [ ] Cable gland hole — front face, size TBD
- [ ] TC102 bracket screw holes — measure TC102 mounting hole spacing on arrival

### Power budget
- ~91mA draw → 2000mAh = ~22hr without solar, net positive on sunny day ✅

---

## Enclosure 2 — Detector Housing

**Role:** ESP32 + LoRa + power. TC102 receiver on external bracket, relay → ESP32 GPIO.

### Electronics
- Heltec V3/V4, bq25185, LiPo, Voltaic P124 lid
- MT3608 boost (5V → 12V)
- TC102 relay → 10kΩ pull-up → ESP32 GPIO interrupt
- SMA bulkhead for LoRa antenna

### Mechanical
- 80×80×30mm body (verify fits Heltec V4 — measure PCB first 🔲)
- Same gasket/lid/corner/tripod architecture as emitter
- SMA bulkhead on side
- Cable gland on front face for TC102 wiring
- TC102 bracket on front face

### TODO before final print
- [ ] Measure Heltec V4 PCB dimensions
- [ ] USB-C cutout, ¼-20 boss, SMA bulkhead — all measure on arrival
- [ ] TC102 bracket screw holes

### Power budget
- ~129mA draw → 2000mAh = ~15.5hr without solar, net positive on sunny day ✅

---

## Enclosure 3 — Hub / Base Station

Deferred. USB powered at timing table. HTIT-Tracker + environmental sensors TBD.

---

## Heat Set Insert Notes

- M3: OD 4.53mm → print hole 4.3mm, iron at 230°C for PETG
- ¼-20: TBD — measure on arrival

---

## Print Material Plan

| Use | Material |
|---|---|
| Test prints | PLA |
| Fitment validation | PETG |
| Final enclosures + TC102 brackets | ASA |
| Gaskets | TPU 95A |

---

## CAD Notes (Onshape)

### ⚠️ Parts Verification Rule
Do not finalize dimensions until parts are in hand and caliper-measured.

### Confirmed dimensions ✅
| Component | Dimensions |
|---|---|
| Micro mini slide switch | 8.64 × 3.74 × 3.60mm, pocket geometry verified and test printed |
| 3000mAh LiPo | 10.3 × 36 × 66mm |
| Double-sided tape pads | 2.3 × 11.7 × 11.7mm |
| M3 heat set insert OD | 4.53mm — hole 4.3mm, corner block 12×12mm |

### Verify on arrival 🔲
| Component | Spec | Needed for |
|---|---|---|
| bq25185 board | 32 × 26.3 × 7.2mm | All enclosures |
| JLJLUP 2000mAh LiPo | 34 × 52 × 10mm | All enclosures |
| Voltaic P123 panel | 65.5 × 65.5 × 3.1mm | Emitter lid |
| Voltaic P124 panel | 66 × 113 × 2.6mm | Detector lid |
| MT3608 boost module | TBD | Internal layout |
| TC102 mounting hole spacing | TBD | Bracket design |
| TC102 mounting hole diameter | TBD | Bracket design |
| ¼-20 heat set insert OD | TBD | Tripod boss |
| Waterproof USB-C panel mount OD | TBD | Side wall cutout |
| Heltec V3/V4 PCB | TBD | Detector sizing |
| SMA bulkhead OD | TBD | Detector side wall |

### Current Onshape state
- `timing-emitter-housing-v2` — active
- Body complete: 80×80×30mm, 5mm walls, corner blocks, gasket channel, insert holes
- Remaining: switch pocket, USB-C cutout, cable gland, tripod boss, lid, TC102 bracket

---

## MakerWorld Search Notes

- Electronics enclosures with ¼-20 tripod mounts
- Gate opener photocell mounting brackets — TC102 style
