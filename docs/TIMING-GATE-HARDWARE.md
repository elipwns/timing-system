# Timing Gate Hardware Design Spec

Self-contained, solar-powered IR beam break timing gates. Two gates total (start + finish), each gate consisting of two independent housings on tripods — one emitter side, one receiver side. No wires cross the track. A separate hub/base station sits at the timing table and handles all data aggregation and cloud upload.

---

## Three Enclosure Types

| Enclosure | Qty | Lives | Key constraint |
|---|---|---|---|
| **Emitter** | 2 | Field, tripod, weatherproof | Square solar lid, IR LED + aim assist, compact |
| **Detector (receiver)** | 2 | Field, tripod, weatherproof | Rectangular solar lid, IR phototransistor, SMA antenna |
| **Hub / base station** | 1 | Timing table, semi-protected | GPS, WiFi, environmental sensors, display — table-friendly form factor |

---

## Physical Setup

```
[Emitter]                                [Detector]
  IR LED ──────────────────────────────→  Phototransistor
  (dumb, just needs power)                (smart, ESP32 + LoRa)

  ¼-20 tripod mount                       ¼-20 tripod mount
  weighted tripod on asphalt              weighted tripod on asphalt

                    both fire LoRa → [Hub at timing table] → WiFi → AWS
```

- Gate width: ~10-15 feet
- Each field housing is self-contained: solar + LiPo, no cables across track
- Both housings point at each other — alignment via tripod head rotation
- Alignment tube on each narrows beam/acceptance cone

---

## Shopping List — To Order

Items needed before CAD can be finalized or final prints made. Measure everything with calipers on arrival before updating CAD.

| Item | Purpose | Notes |
|---|---|---|
| **¼-20 heat set inserts** | Tripod mount boss in enclosure bottom | Need OD to design boss. Standard brass, M6 length. Search "¼-20 heat set insert brass" on Amazon. |
| **ASA filament** | Final field enclosure prints | UV stable, high heat tolerance. Bambu ASA recommended for compatibility. 1-2 spools. |
| **TPU filament** | Gaskets for all enclosures | 95A Shore hardness. Bambu TPU 95A recommended. |
| **Waterproof USB-C panel mount connector** | USB-C access port on enclosure side | Need ~12-14mm cutout diameter. Search "waterproof USB-C panel mount" — measure OD before designing cutout. On-hand connector is 26mm, too large. |
| **M3 screws** | Lid to body fastening | Flat or button head, length TBD once lid thickness is finalized. ~8-12mm likely. |

---

## Full BOM

| Qty | Component | Source | Part # | Price | Notes |
|---|---|---|---|---|---|
| 5 | bq25185 5V Boost Charger | Adafruit | 6106 | $8.95 | 2x emitter, 2x detector, 1x hub (if solar) |
| 2 | Voltaic P123 0.6W panel | Adafruit | 5856 | $8.95 | Emitter lid, 65.5×65.5mm square |
| 2 | Voltaic P124 1.2W panel | Adafruit | 5368 | $14.95 | Detector lid, 66×113mm rectangular |
| 2 | IR Break Beam 5mm | Adafruit | 2168 | $5.95 | One set per gate, 2 gates = 2 sets |
| 8 | JLJLUP 2000mAh LiPo | Amazon | LP103450 | ~$6.00 | 5 deployed + 3 spares, JST PH 2.0mm |

**Already on hand:**
- Heltec V3 x2, Heltec V4 x2, HTIT-Tracker x1 (hub candidate)
- M3 heat set inserts (caliper-verified OD 4.53mm)
- Schottky diodes
- Soldering iron, wire strippers, Dremel, multimeter, digital calipers
- 3000mAh LiPo batteries (Amazon, caliper-verified — see below)
- Double-sided tape pads (caliper-verified — see below)
- Waterproof USB connector (26mm OD — too large for this enclosure, reserved for other project)

**Hub station sensors — to order (see hub section below)**

---

## Charge Controller — bq25185 (#6106)

**URL**: https://www.adafruit.com/product/6106 — $8.95

Standardized across all three enclosure types and Meshtastic outdoor node.

**Key features:**
- Solar + USB-C simultaneously, no external diode needed
- Near-MPPT solar optimization
- Power path — draws from charger first, battery gets remainder
- Regulated 5V boost output regardless of battery state
- Onboard LEDs: orange (charging), red (fault), green (3.3V good)
- JST PH 2-pin battery, screw terminal 5V out, USB-C in, solar solder pads

**Connections:**
- Solar → VIN/G solder pads (5-18V)
- LiPo → JST PH 2-pin BATT
- Load → 5V screw terminal

---

## Battery — JLJLUP 2000mAh LiPo (ordered)

**Dimensions**: 34 × 52 × 10mm (spec — verify on arrival 🔲)
**Connector**: JST PH 2.0mm — matches bq25185 natively
**Protection**: Built-in PCM

### Polarity — verify before first connect
Standard convention: red = P+, black = P-. bq25185 is wired to match. Verify with multimeter before first plug-in. If reversed: pop JST pins with small flathead and swap — 30 second fix, no soldering.

---

## Battery — 3000mAh LiPo (on hand) ✅

**Dimensions**: 10.3 × 36 × 66mm (caliper-verified)
**Note**: Reserved for detector or Meshtastic node. Emitter will use JLJLUP 2000mAh when it arrives.

---

## Double-Sided Tape Pads (on hand) ✅

**Dimensions**: 2.3 × 11.7 × 11.7mm (caliper-verified)
For component mounting inside enclosures — bq25185 board, battery securing, etc.

---

## Solar Panels

ETFE, IP67, UV resistant, 5-7yr life, 22+% monocrystalline SunPower cells. Solder pads on back.
Both output ~6-6.1V peak — bq25185 accepts 5-18V solar input, both are in spec.

### Emitter panel — Voltaic P123 (#5856)
- 65.5 × 65.5 × 3.1mm — square
- 0.6W, ~120mA peak
- https://www.adafruit.com/product/5856

### Detector panel — Voltaic P124 (#5368)
- 66 × 113 × 2.6mm — rectangular
- 1.2W, ~200mA peak
- https://www.adafruit.com/product/5368

### Voltaic stock status (March 2026)
| Panel | Status |
|---|---|
| Small 6V 1W | Out of stock |
| Medium 6V 2W | Out of stock |
| Large 6V 3.5W | Out of stock |
| Huge 6V 6W | No longer stocked |
| Colossal 6V 9W | Out of stock |
| 2V 0.3W ETFE | $5.50 — in stock |
| **5V 1.2W ETFE (P124)** | **$14.95 — in stock ✅ (detector)** |
| 5V 0.3W ETFE | Out of stock |
| **5V 0.6W ETFE (P123)** | **$8.95 — in stock ✅ (emitter)** |
| 6V 2W ETFE | $20.95 — in stock |
| 5V 5W ETGE | $34.95 — in stock |
| 5V 10W ETFE | $64.95 — in stock |

---

## IR Sensors — Adafruit Break Beam #2168

**URL**: https://www.adafruit.com/product/2168 — $5.95/set
**LED package**: 5mm diameter — sets alignment tube ID at 6mm
**Pre-wired** with header ends — wires run back into housing

2 sets ordered (4 total) — 2 deployed, 2 spares.

---

## Enclosure 1 — Emitter Housing

Two built (one per gate).

### Electronics
- bq25185 (#6106)
- JLJLUP 2000mAh LiPo (on order — fits P123 footprint)
- Voltaic P123 (65.5×65.5mm lid)
- IR LED from break beam set, through alignment tube
- Current limiting resistor (5V supply, ~40mA target)

### Status indicators
- bq25185 onboard LEDs (charging, fault, power good)
- Aim assist LED: visible red or green, adjacent to IR tube on front face

### Mechanical
- ¼-20 heat set insert, bottom — tripod mount (insert on order)
- Voltaic P123 square panel as top lid (recessed in printed lid tray, lip retains panel)
- IR LED in 6mm ID × 10-15mm alignment tube, front face
- Aim assist LED adjacent to tube
- Waterproof USB-C panel mount on side (connector on order — measure OD before designing cutout)
- TPU printed gasket between lid and body (2×2mm cross section, 1mm registration groove)
- M3 heat set inserts × 4 corners outside gasket line, M3 screws lid to body
- 5mm exterior corner fillets
- NO antenna
- Weatherproof

### TODO before final print
- [ ] **Waterproof USB-C cutout** — waiting on correct connector (12-14mm panel mount). Design cutout after measuring part.
- [ ] **¼-20 tripod boss** — waiting on ¼-20 inserts. Design boss after measuring OD.
- [ ] **Switch pocket** — add to side wall using verified geometry from switch-pocket-test
- [ ] **IR LED alignment tube** — 6mm ID × 10-15mm through front face wall
- [ ] **Aim assist LED hole** — adjacent to alignment tube, front face

### Power budget
- ~45mA continuous draw
- Worst case 3-day, no solar: 1215mAh — 2000mAh covers with 65% headroom
- P123 at 30% cloud efficiency: near net-zero

---

## Enclosure 2 — Detector Housing

Two built (one per gate).

### Electronics
- Heltec V3 or V4 ESP32
- bq25185 (#6106)
- JLJLUP 2000mAh LiPo
- Voltaic P124 (66×113mm lid)
- IR phototransistor from break beam set, through alignment tube
- SMA bulkhead for LoRa antenna

### Status indicators
- bq25185 onboard LEDs
- Beam aligned LED: external LED on ESP32 GPIO — lights when phototransistor sees IR. Rotate tripod until green.
- LoRa activity LED: blinks on transmit

### Mechanical
- ¼-20 heat set insert, bottom — tripod mount (insert on order)
- Voltaic P124 rectangular panel as top lid
- IR phototransistor in 6mm ID × 10-15mm alignment tube, front face
- SMA bulkhead on side
- Waterproof USB-C panel mount on side (connector on order)
- TPU printed gasket between lid and body
- M3 heat set inserts × 4 corners outside gasket line, M3 screws lid to body
- Weatherproof

### Power budget
- ~80-100mA average draw
- 2000mAh covers a full event day; solar extends to multi-day
- P124 at peak (~200mA): net positive on sunny days

---

## Enclosure 3 — Hub / Base Station

One built. Lives at the timing table — not a field node. Form factor is table-friendly rather than weatherproof-tripod.

### Board — HTIT-Tracker
Already on hand. Purpose-built for this role:
- Built-in GPS/GNSS — single clock source for all received LoRa timestamps
- WiFi — posts to AWS from wherever the timing table is
- LoRa — receives BEAM messages from all detector nodes
- Display — shows live timing data at the table

### Environmental sensors — wishlist

**Priority 1:** MLX90614 IR thermometer — asphalt surface temperature
**Priority 2:** BME280/BME680 — ambient temp, humidity, barometric pressure → density altitude
**Priority 3:** Anemometer — wind speed/direction

All sensors are I2C — daisy chain off HTIT-Tracker I2C breakout.

### Enclosure design notes
- Deferred until HTIT-Tracker dimensions are measured and sensor selection is finalized

---

## Alignment Tube Spec (field nodes only)

- ID: 6mm (5mm LED package + 1mm clearance)
- Length: 10-15mm
- Printed as part of housing front face
- Same spec on emitter and detector for symmetry

---

## Heat Set Insert Notes

- M3 insert OD: 4.53mm (caliper-verified)
- Print hole at 4.3mm for PETG (verified — seats correctly)
- Iron temp: 230°C for PETG — do NOT use 420°C (causes wall bowing)
- Use dedicated flat-bottom heat set tip, not soldering tip
- Let heat do the work — slow 5-8 second sink, no hard pressure
- ¼-20 insert OD: TBD — on order, measure before designing tripod boss

---

## Print Material Plan

| Use | Material | Reason |
|---|---|---|
| Test/geometry prints | PLA | Fast, cheap, good enough for fitment checks |
| Intermediate tests | PETG | Real material behavior, insert fitment validation |
| Final field enclosures | ASA | UV stable, high heat tolerance (~95°C), outdoor rated — on order |
| Gaskets | TPU | Flexible, compressible, UV stable, reusable — on order |
| ABS on hand | Burn through on current project wrapping up | Not ideal for outdoor UV exposure long term |

---

## CAD Notes (Onshape)

### ⚠️ Parts Verification Rule
**Do not finalize any enclosure dimensions in Onshape until parts are physically in hand and measured with calipers.**

### Confirmed dimensions (caliper-verified ✅)
| Component | Dimensions | Notes |
|---|---|---|
| Micro mini slide switch | 8.64 × 3.74 × 3.60mm body, 1.5mm actuator, ~2mm travel | ✅ |
| Switch pocket geometry | 8.84 × 3.94mm pocket, 3.7 × 1.7mm slot, 1.4mm roof | ✅ test printed, fits well |
| 3000mAh LiPo (on hand) | 10.3 × 36 × 66mm | ✅ reserved for detector/Meshtastic |
| Double-sided tape pads | 2.3 × 11.7 × 11.7mm | ✅ |
| M3 heat set insert OD | 4.53mm | ✅ — print hole 4.3mm, corner block 12×12mm |
| IR LED/receiver package | 5mm diameter | per Adafruit spec — verify on arrival 🔲 |
| Alignment tube ID | 6mm | derived from LED diameter |

### From datasheets / spec pages — verify on arrival 🔲
| Component | Spec dimensions | Needed for |
|---|---|---|
| bq25185 board | 32 × 26.3 × 7.2mm | Emitter + Detector CAD |
| JLJLUP 2000mAh LiPo | 34 × 52 × 10mm | Emitter + Detector CAD |
| Voltaic P123 panel | 65.5 × 65.5 × 3.1mm | Emitter lid design |
| Voltaic P124 panel | 66 × 113 × 2.6mm | Detector lid design |
| ¼-20 heat set insert OD | TBD — on order | Bottom tripod boss |
| Waterproof USB-C panel mount OD | TBD — on order | Side wall cutout |
| Heltec V3/V4 PCB | TBD | Detector CAD |
| SMA bulkhead cutout | TBD | Detector CAD |
| HTIT-Tracker dimensions | TBD | Hub CAD |
| MLX90614 board | TBD | Hub CAD |
| BME280 board | TBD | Hub CAD |

### Current Onshape document state (v2)
- `timing-emitter-housing-v2` — active document
- Body: 80×80×30mm, 5mm walls, 5mm exterior fillets
- Corner blocks: 12×12mm, fully merged into walls, radiused interior transitions
- Gasket channel: 2mm wide × 1mm deep, inside corner blocks
- Insert holes: 4.3mm diameter × 9mm deep, one per corner block
- Remaining features: switch pocket, IR tube, aim assist LED, USB-C cutout, tripod boss

### Build order
1. **Emitter first** ← in progress
2. **Detector second** — carries emitter lessons forward
3. **Hub last** — most complex, most deferred

---

## MakerWorld Search Notes

- Electronics enclosures with ¼-20 tripod mounts (photography accessory category)
- Adafruit snap-on enclosure for bq25185: product #6126 — useful dimension reference
