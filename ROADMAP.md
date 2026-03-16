# Roadmap & Future Ideas

Ideas that are too big or too speculative for the README checklist, but worth preserving.

---

## 🚨 BLOCKER — IR Sensor Selection

**This must be resolved before enclosure CAD can be finalized.**

### The problem

The originally specified Adafruit Break Beam Sensor (product #2168) has a maximum sensing distance of **50cm / 20 inches**. The timing gate needs to span **10-15 feet**. This is a fundamental incompatibility — the sensor selection needs to change.

### Use case requirements

| Requirement | Detail |
|---|---|
| **Range** | 10-15 feet minimum (autocross lane width) |
| **Alignment tolerance** | Must tolerate imperfect setup — hillclimb courses on slopes, uneven ground, fast deployment by one person |
| **Outdoor** | Full sun, rain, dust |
| **Response time** | <10ms — cars are fast |
| **Power** | 5V preferred (matches bq25185 output) or 12V with regulator |
| **Output** | GPIO-compatible signal or relay contact (into ESP32 interrupt) |

### Alignment tolerance is critical

Autocross on flat pavement — easy, time to align carefully. Hillclimb on a road — slope, limited space, faster setup, may not be able to get tripods perfectly level. The sensor needs enough acceptance angle or beam width to tolerate a few degrees of misalignment without losing the beam.

### Options to evaluate

**Option 1 — Gate opener photocell (e.g. TOPENS TC102)**
- Range: 10-46 feet ✅
- Outdoor rated, weather resistant ✅
- Lensed optics — decent alignment tolerance ✅
- Built-in alignment LED on receiver ✅
- Power: 12-24V AC/DC — needs regulator from 5V bq25185 ⚠️
- Output: relay contact (NO/NC) — needs wiring to ESP32 GPIO ⚠️
- Cost: ~$15-20/pair
- Pro: purpose-built for exactly this use case, proven, cheap
- Con: not a direct GPIO sensor, relay adds complexity

**Option 2 — Industrial modulated IR pair (38kHz)**
- Range: up to 7-10 meters with lensed emitter ✅
- Modulated carrier rejects ambient IR noise (sunlight) ✅
- More DIY — requires driver circuit for emitter ⚠️
- TSSP4038 or similar receiver IC
- Better GPIO integration than relay
- Cost: ~$5-10 in parts

**Option 3 — Laser diode + photodiode**
- Range: effectively unlimited ✅
- Very precise, narrow beam
- Alignment is hardest — least forgiving ❌ (bad for hillclimb)
- Visible laser makes alignment easier
- Safety considerations for event use ⚠️

**Option 4 — Retroreflective sensor**
- Single housing, beam bounces off reflector on opposite side
- Only one side needs power — simpler wiring
- Range typically shorter than through-beam
- Reflector still needs alignment
- Good for narrow courses

### Recommended path

**Start with gate opener photocells (Option 1)** for a working v1 — proven range, proven outdoor use, cheap, alignment indicator built in. Wire relay output to ESP32 GPIO interrupt. Accept the 12V power requirement and use a small buck converter from the 5V bq25185 output... or more simply power the photocell from a separate 12V source and keep the ESP32 on 5V.

**Evaluate modulated IR (Option 2) for v2** — cleaner integration, no relay, more control over beam angle and acceptance cone. Better long-term solution once v1 is proven.

### Impact on enclosure design

The enclosure design started around a 5mm LED through a 6mm tube. Gate opener photocells are self-contained units with their own housing — they would mount externally on the enclosure or on the tripod head directly, not inside the printed enclosure at all. This significantly changes the emitter/detector enclosure design:

- Emitter enclosure becomes: **bq25185 + battery + solar + power switch** — power supply box only
- The photocell sensor mounts on the outside, wired to the power box
- The detector enclosure becomes: **ESP32 + bq25185 + battery + solar + relay interface** — with photocell receiver mounted externally

This is actually simpler than trying to build optical alignment into the printed housing.

**CAD work can continue on the enclosure body geometry** (walls, gasket, lid, tripod mount) since that's material-independent. Hold off on the IR tube / LED hole features until sensor selection is confirmed.

---

## 🏗️ Hub-and-Spoke Architecture (Next Major Refactor)

**The core idea:** Separate sensor nodes from the cloud uploader. Every checkpoint node is dumb and cheap — it only needs to detect a beam break and fire a LoRa message. One central "hub" node at the timing table receives all messages, does the math, and posts to AWS.

```
[START node]     ──LoRa──→ ┐
[SECTOR 1 node]  ──LoRa──→ ├→ [HTIT-Tracker at timing table] ──WiFi──→ AWS → Dashboard
[SECTOR 2 node]  ──LoRa──→ ┘
[FINISH node]    ──LoRa──→ ┘
```

### Why this is better than the current architecture

Currently the FINISH node does double duty — it's both a sensor node AND the WiFi/cloud uploader. That means it needs to be physically at the finish line AND near WiFi, which creates placement constraints. Separating these roles is cleaner and more scalable.

### The hub node: HTIT-Tracker

The HTIT-Tracker is the natural fit for the hub role:
- Has built-in GPS — provides a single consistent clock source for timestamping all received LoRa messages. No need to sync clocks across nodes.
- WiFi capable — posts to AWS from wherever the timing table is
- Can sit at the timing tent, trailer, or scoring table — not tied to any physical timing line
- Same hardware can serve dual purpose: hub for timing events, or in-car GPS/telemetry node for track days (different firmware)

### Sensor node design (simplified)

Every checkpoint node becomes identical and minimal:
- ESP32 (Heltec V3 or V4)
- IR beam break sensor on a GPIO interrupt
- LoRa radio — transmits `BEAM_BREAK:checkpoint_id:local_millis` on trigger
- Solar charged LiPo (bq25185 + 18650 + small panel)
- No WiFi needed at sensor nodes
- 3D printed weatherproof enclosure

This makes sensor nodes cheap, replaceable, and easy to add. 6 sector nodes costs less than one fancy unit.

### Message protocol (proposed)

```
Sensor → Hub:   BEAM:checkpoint_id:local_millis
Hub → AWS:      { run_id, checkpoint_id, hub_timestamp, sector_times[] }
```

Hub uses its GPS-derived timestamp when it receives each LoRa message — single clock source, no sync needed across nodes.

---

## 🚦 IR Beam Break Sensor Integration

**Replaces button presses with real detection.** The current firmware uses a physical button to simulate start/finish. IR beam break sensors are a drop-in replacement — the receiver output is just a GPIO that flips when the beam is blocked.

### Hardware
- Emitter side: IR LED + resistor, powered continuously, no logic
- Receiver side: phototransistor output → GPIO input with pullup
- One GPIO interrupt per node, same as the current button pin

### Wiring
```
Emitter: VCC → resistor → IR LED → GND (always on)
Receiver: VCC → phototransistor → GPIO (pullup to VCC)
```

### Firmware change
Swap `digitalRead(BUTTON_PIN)` for an interrupt on the IR receiver GPIO. Beam present = HIGH, beam broken = LOW (or inverted depending on receiver circuit). Debounce logic needed — cars are fast but the signal can chatter.

### Physical considerations
- Emitter and receiver mount on opposite sides of the course
- Long cable runs back to the node enclosure
- Alignment is critical — small misalignment kills the beam at distance
- Consider a small bracket/housing for emitter and receiver that can be staked into the ground or clamped to cones

---

## ☀️ Solar Power

**Goal:** Nodes that run all day without battery swaps. Currently using USB power / 9V batteries which require frequent replacement at events.

### Power budget (per sensor node)
- ESP32 active: ~80-240mA, light sleep between events: ~2-5mA
- IR emitter: ~20-50mA continuous
- Estimated average: ~50-100mA
- All-day (12hr) requirement: ~600mAh-1.2Ah

### Solution
- **18650 LiPo** (2500-3500mAh) — covers a full day with headroom even without solar
- **bq25185 solar charger** — same component already selected for Meshtastic node build. Solar in, LiPo management, regulated 5V out.
- **Small solar panel** (1-2W, 6V) — net-zero or net-positive on a sunny day. Overcast still draws down slowly but battery buffer covers it.
- Standardize this power stack across timing nodes and Meshtastic nodes — same BOM, same enclosure design pattern.

---

## 📊 Historical Performance Dashboard

**The core idea:** *"How did I do compared to last season?"*

The current commercial system (Pronto Timing) only shows results for the active event. Once the event ends, the data is gone — no way to look back. DynamoDB changes that. Every run is stored permanently, timestamped, and queryable.

This opens up a class of features that don't exist in any off-the-shelf timing product:

- **Same venue, same driver** — your times at this course over the last 2 years
- **Same event, different season** — did you improve year-over-year?
- **Against a specific competitor** — head-to-head across multiple events
- **Class trends** — is NS1 getting faster as a class? Are certain cars dominating?

The dashboard should make this *fun* — not just a table, but something you actually want to explore after an event.

---

## 🌦️ Environmental & Surface Condition Logging

**The idea:** attach a small sensor node to the timing system that records conditions at the time of each run. Store this alongside the timing data in DynamoDB so every run has environmental context baked in.

### Why it matters

Surface condition of the asphalt is the biggest variable in autocross times — more so than air density. A freshly laid course on a cold morning runs completely differently than the same course at 2pm after 100 cars have gone through. Rubber laid down, surface temp, moisture — these matter enormously and currently nobody tracks them systematically.

### What to log

**High priority:**
- Asphalt surface temperature (IR thermometer, e.g. MLX90614)
- Ambient air temperature
- Time of day / run number (proxy for rubber laid down / track evolution)

**Good to have:**
- Barometric pressure + humidity → density altitude calculation
- Wind speed/direction (relevant for open-course events)

**Nice to have / future:**
- Traction index (some kind of surface friction proxy — harder but interesting)

### Hardware angle

Already have experience with weather stations. The sensor node could be a third ESP32 board — sits at the start line, logs conditions every few minutes, posts to the same AWS backend. Could even broadcast over LoRa so it doesn't need its own WiFi.

### What this enables

- "You ran a 40.2 here last May — it was 18°C ambient, asphalt was 28°C. Today it's 38°C asphalt and your time is 40.8. The track is slower, not you."
- Separate *driver improvement* from *conditions improvement*
- Useful data for the whole class, not just one driver

---

## 🔗 Mesh / Relay Architecture

For hill climbs and long courses where the base station can't reach the finish line over LoRa, relay nodes can extend the network. Each relay is just another ESP32 board that listens and re-broadcasts.

Alternative for extreme distances: both boards connect to the internet independently and use server-side timestamps to compute the delta. No radio path required at all.

The system isn't locked into one topology — that's the whole point of building on open hardware.

---

## 🏁 Sensor Agnosticism

The timing system doesn't dictate how you detect a car crossing the line. Current options being considered:

- IR beam break (same tech as the commercial systems)
- Pressure mat / hose
- Radar/lidar
- Reuse existing commercial IR gates as dumb sensors feeding into the ESP32

Keeping this flexible is a feature — different events have different requirements and budgets.

---

## 🧰 Hardware Inventory & Role Assignment

| Board | Count | Assigned Role |
|---|---|---|
| Heltec V3 | 2 | Current start/finish nodes — keep deployed, working |
| Heltec V4 | 2 | Sector/checkpoint nodes — newer hardware, better sleep current |
| HTIT-Tracker | 1 | Hub node at timing table (GPS clock source + WiFi uploader) OR in-car telemetry for track days |
| RAK10724 | 1 (incoming) | Meshtastic outdoor node — not part of timing system |
