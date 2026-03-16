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
- TC102 has built-in alignment LED on receiver — OFF LED extinguishes when beam is aligned

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
| Mounting | Two screw holes — top and bottom of oval body |
| Alignment indicator | OFF LED on receiver — **extinguishes** when beam is aligned, **illuminates** when beam is broken or missing |

### Why TC102 vs Adafruit #2168
- Adafruit #2168: 50cm max range — cannot span 10-15 foot autocross gate
- TC102: 65 foot range, outdoor rated, modulated IR (rejects sunlight), built-in alignment LED
- TC102 is purpose-built for exactly this use case (outdoor gate/barrier detection)

### ⚠️ Relay wiring — NC/COM, not NO/COM

**User review correction:** The NC (Normally Closed) to COM terminal behavior is counterintuitive and opposite to what the label implies:
- NC/COM = **OPEN** when there is no beam (beam broken or missing)
- NC/COM = **CLOSED** when beam is present

This means use **NC/COM** (not NO/COM) for the ESP32 interrupt:

```
TC102 relay NC terminal → ESP32 GPIO pin
TC102 relay COM terminal → GND
ESP32 GPIO → 10kΩ pull-up resistor → 3.3V

Beam present = NC closed = GPIO pulled LOW   (normal operating state)
Beam broken  = NC open   = GPIO pulled HIGH  (interrupt fires → record timestamp)
```

Standard active-high interrupt. No additional components beyond the pull-up resistor.

### Alignment procedure (from user experience)
Alignment requires both units powered on. The ±10° beam angle means 2-dimensional angle alignment is needed — not just left/right but also vertical tilt. Tips from field use:

1. Power both emitter and receiver from 12V supply before mounting permanently
2. Watch the OFF indicator on the receiver — it **extinguishes** when beam is aligned
3. Use a straight-edge or sight line to get rough horizontal aim first, then fine-tune with the LED
4. **Bracket sight line feature** — design a notch or groove on top of bracket to sight along for rough aim before LED fine-tuning
5. Once aligned, lock down tripod and bracket — don't bump it

**At events:** Power on both nodes first, align before starting runs. The alignment indicator is on the receiver (detector) side.

### Power architecture
TC102 requires 12V. bq25185 outputs 5V. Solution: MT3608 boost converter module (5V → 12V).
- MT3608 module: ~$1-2, tiny, 85% efficient
- Pre-adjust to 12V output before installing (trim pot on module)
- One per enclosure

---

## TC102 Mounting Bracket (printed, ASA)

The TC102 mounts on a dedicated printed bracket attached to the front face of the enclosure. Serves three functions: mechanical mounting, sun shielding, and alignment assistance.

### TC102 physical details
- Oval body, 75 × 48 × 30mm
- Two mounting screw holes — top and bottom of the oval
- Wires exit from bottom
- Receiver should be shielded from direct sunlight (per spec)

### Bracket design requirements
- Mounts to enclosure front face via M3 screws
- Two holes matching TC102 top/bottom mounting hole spacing (measure on arrival 🔲)
- **Sun visor** — printed overhang ~15-20mm above lens, blocks high-angle sun while allowing horizontal beam
- **Sight line feature** — groove or notch along top of bracket for rough visual aim before LED alignment
- Cable channel routes TC102 wires through front face of enclosure (cable gland)
- Same bracket design for both emitter and detector (identical TC102 body)
- Print in ASA — fully exposed to UV and weather

### TODO
- [ ] Measure TC102 mounting hole spacing top-to-bottom on arrival
- [ ] Measure TC102 mounting hole diameter
- [ ] Design bracket in Onshape (separate Part Studio)
- [ ] PLA test print for fit, final in ASA

---

## Shopping List — To Order

| Item | Qty | Purpose | Notes |
|---|---|---|---|
| **TOPENS TC102 photocell sensor** | 2 pairs | 2 deployed gates | ~$15-20/pair on Amazon |
| **MT3608 boost converter module** | 4+ | 5V→12V per enclosure | ~$1-2 each. Pre-adjust to 12V output. |
| **¼-20 brass heat set inserts** | 8+ | Tripod mount boss | Measure OD before designing boss hole |
| **ASA filament** | 1-2 spools | Final enclosures + brackets | Bambu ASA recommended |
| **TPU filament** | 1 spool | Gaskets | Bambu TPU 95A |
| **Waterproof USB-C panel mount** | 4+ | Charging port on enclosure | ~12-14mm cutout. Measure OD before cutting. |
| **M3 screws** | 20+ | Lid fastening | Button/flat head, ~8-12mm |
| **10kΩ resistors** | 10+ | GPIO pull-up for relay | Verify on hand |

---

## Full BOM

| Qty | Component | Source | Price | Notes |
|---|---|---|---|---|
| 2 pairs | TOPENS TC102 photocell | Amazon | ~$15-20/pair | 2 gates |
| 4 | MT3608 boost converter | Amazon | ~$1-2 ea | 5V→12V |
| 5 | bq25185 5V Boost Charger | Adafruit #6106 | $8.95 | 2x emitter, 2x detector, 1x hub |
| 2 | Voltaic P123 0.6W panel | Adafruit #5856 | $8.95 | Emitter lid |
| 2 | Voltaic P124 1.2W panel | Adafruit #5368 | $14.95 | Detector lid |
| 4 | JLJLUP 2000mAh LiPo | Amazon LP103450 | ~$6.00 | 2x emitter, 2x detector |
| 4 | spare JLJLUP 2000mAh LiPo | Amazon LP103450 | ~$6.00 | Spares |

**Already on hand:**
- Heltec V3 x2, Heltec V4 x2, HTIT-Tracker x1
- M3 heat set inserts (OD 4.53mm caliper-verified)
- Schottky diodes, 10kΩ resistors (verify qty)
- Soldering iron, wire strippers, Dremel, multimeter, digital calipers
- 3000mAh LiPo batteries (66mm — reserved for detector or Meshtastic)
- Double-sided tape pads (2.3 × 11.7 × 11.7mm)
- Waterproof USB connector (26mm OD — too large, reserved for other project)
- Adafruit #2168 break beam sensors (50cm — cannot use for gate, repurpose for short-range projects)

---

## Power Budget

All calculations assume 12hr event day, MT3608 boost at 85% efficiency.

### Emitter Node

| Component | Draw |
|---|---|
| TC102 emitter @ 12V via MT3608 | ~84mA from 5V rail |
| bq25185 + MT3608 quiescent | ~7mA |
| **Total** | **~91mA** |

**2000mAh → ~22hr without solar** | **P123 sunny day: net positive** ✅

### Detector Node

| Component | Draw |
|---|---|
| TC102 receiver @ 12V via MT3608 | ~42mA from 5V rail |
| ESP32 Heltec V4 active avg | ~80mA |
| bq25185 + MT3608 quiescent | ~7mA |
| **Total** | **~129mA** |

**2000mAh → ~15.5hr without solar** | **P124 sunny day: net positive** ✅

Both nodes cover a full 12hr event day on battery alone with margin. Solar extends to multi-day.

---

## Charge Controller — bq25185 (#6106)

Solar → VIN/G solder pads | LiPo → JST PH 2-pin | 5V out → MT3608 → 12V → TC102

---

## Batteries

**JLJLUP 2000mAh** (ordered): 34 × 52 × 10mm spec, JST PH 2.0mm. Verify on arrival 🔲
**3000mAh on hand**: 10.3 × 36 × 66mm. Reserved for detector or Meshtastic.

---

## Solar Panels

**Emitter — P123**: 65.5 × 65.5 × 3.1mm, 0.6W ✅ in stock
**Detector — P124**: 66 × 113 × 2.6mm, 1.2W ✅ in stock

---

## Enclosure 1 — Emitter Housing

Power supply box only. TC102 emitter on external bracket.

**Electronics:** bq25185, 2000mAh LiPo, P123 lid, MT3608 boost, power switch, red LED
**Mechanical:** 80×80×30mm, 5mm walls, corner block inserts, TPU gasket, ¼-20 tripod boss, USB-C back face, switch+LED back face, cable gland front face, TC102 bracket front face

**TODO:** USB-C cutout, ¼-20 boss, switch pocket (geometry ready), cable gland, bracket holes
**Power:** ~91mA → 22hr battery, net positive sunny day ✅

---

## Enclosure 2 — Detector Housing

ESP32 + LoRa + power. TC102 receiver on bracket, relay → GPIO.

**Electronics:** Heltec V3/V4, bq25185, LiPo, P124 lid, MT3608, TC102 relay → 10kΩ → GPIO, SMA bulkhead
**Mechanical:** Same architecture as emitter, SMA on side, cable gland front face, TC102 bracket

**TODO:** Measure Heltec V4 PCB (may resize enclosure), USB-C, ¼-20 boss, SMA cutout, bracket holes
**Power:** ~129mA → 15.5hr battery, net positive sunny day ✅

---

## Enclosure 3 — Hub / Base Station

Deferred. USB powered. HTIT-Tracker + environmental sensors TBD.

---

## Heat Set Insert Notes

- M3: OD 4.53mm → print hole 4.3mm, 230°C PETG
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

### ⚠️ Measure before finalizing — do not guess dimensions

### Confirmed ✅
| Component | Dimensions |
|---|---|
| Slide switch | 8.64 × 3.74 × 3.60mm — pocket geometry test printed, fits well |
| 3000mAh LiPo | 10.3 × 36 × 66mm |
| Double-sided tape pads | 2.3 × 11.7 × 11.7mm |
| M3 heat set insert OD | 4.53mm → hole 4.3mm, 12×12mm corner block |

### Verify on arrival 🔲
| Component | Spec | Needed for |
|---|---|---|
| bq25185 board | 32 × 26.3 × 7.2mm | All enclosures |
| JLJLUP 2000mAh LiPo | 34 × 52 × 10mm | All enclosures |
| Voltaic P123 | 65.5 × 65.5 × 3.1mm | Emitter lid |
| Voltaic P124 | 66 × 113 × 2.6mm | Detector lid |
| MT3608 module | TBD | Internal layout |
| TC102 mounting hole spacing | TBD | Bracket |
| TC102 mounting hole diameter | TBD | Bracket |
| ¼-20 insert OD | TBD | Tripod boss |
| USB-C panel mount OD | TBD | Side cutout |
| Heltec V3/V4 PCB | TBD | Detector sizing |
| SMA bulkhead OD | TBD | Detector side |

### Onshape state
- `timing-emitter-housing-v2` — body complete (80×80×30mm, corner blocks, gasket, insert holes)
- Remaining: switch pocket, cable gland, USB-C cutout, tripod boss, lid, TC102 bracket

---

## MakerWorld Notes

- ¼-20 tripod mount enclosures (photography accessories category)
- TC102 photocell mounting brackets
