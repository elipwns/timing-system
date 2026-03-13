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

---

## Receiver Housing (Smart Side)

This is the "brain" of each gate. One per gate (x2 total for start + finish).

### Electronics
- **ESP32**: Heltec V3 or V4
- **Solar charger**: bq25185 (same BOM as Meshtastic outdoor node)
- **Battery**: 18650 LiPo
- **Solar panel**: small panel, top or angled face of enclosure
- **Sensor**: IR phototransistor, forward-facing through alignment tube
- **Radio**: LoRa antenna — SMA bulkhead on enclosure side

### Status LEDs
- **Battery level**: green / yellow / red based on charge state from bq25185
- **Beam aligned**: lights up when phototransistor is actively receiving IR signal — primary alignment aid during event setup
- **LoRa activity**: blinks on transmit

### Behavior
- Beam aligned LED makes setup fast — just rotate tripod until it goes green
- On beam break: ESP32 fires LoRa message → hub node at timing table
- Deep sleep between events to conserve power
- Solar keeps it topped up all day

### Mechanical
- ¼-20 threaded insert on bottom for tripod mount
- Solar panel as top lid face, angled slightly skyward
- IR phototransistor mounted in forward-facing tube (10-15mm) to narrow acceptance angle
- SMA bulkhead on side for LoRa antenna
- Weatherproof — sealed enough for outdoor event use

---

## Emitter Housing (Dumb Side)

Just needs to blast IR light continuously. One per gate (x2 total).

### Electronics
- **IR LED**: forward-facing, matched wavelength to receiver phototransistor
- **Resistor**: current limiting for IR LED
- **Charge board**: TP4056 USB-C (simple, cheap, $1-2) — no need for bq25185 complexity
- **Battery**: small LiPo
- **Solar panel**: same form factor as receiver housing for visual consistency

### Status LEDs
- **Power on**: confirms unit is live
- **Aim assist**: visible-light LED (red or green) mounted directly next to the IR LED — lets you visually aim the housing at the receiver before IR alignment is confirmed. IR is invisible; this makes coarse alignment easy.

### Mechanical
- ¼-20 threaded insert on bottom for tripod mount
- IR LED mounted in forward-facing alignment tube, same spec as receiver side
- Aim assist LED adjacent to IR LED, same forward face
- USB-C port accessible for charging (TP4056)
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

- Emitter draws ~20-50mA continuous — even a 500mAh LiPo lasts 10+ hours. Solar is bonus.
- Receiver ESP32 in light sleep + IR monitoring: ~50-100mA average. 18650 (2500mAh) covers a full day.
- bq25185 on receiver matches Meshtastic node BOM — standardized power stack across both projects.
- TP4056 on emitter is overkill-free — simple USB-C charge, no solar MPPT needed at this power level.
  - Note: if adding solar to emitter, use a TP4056 variant with solar input protection or add a diode.

---

## CAD Notes (Onshape)

Reference dimensions needed before modeling:
- Heltec V3 / V4 PCB footprint and mounting hole pattern
- bq25185 Adafruit board footprint
- 18650 cell diameter (18.5mm) and length (65mm) — standard, but verify fit
- TP4056 board footprint
- ¼-20 threaded insert OD (press-fit or heat-set)
- Solar panel dimensions (TBD based on panel selected)
- SMA bulkhead cutout diameter

Plan: measure physical parts when RAK/Adafruit order arrives, then model both housings. Emitter housing is simpler — good candidate for first print and fit-test.

---

## MakerWorld Search Notes

Before full custom design, check MakerWorld for:
- Electronics enclosures with ¼-20 tripod mounts (photography accessory category)
- 18650 battery holders / enclosures
- TP4056 project boxes

Likely still need custom design for the full integrated housing, but reference prints may help validate dimensions and fitment approach.
