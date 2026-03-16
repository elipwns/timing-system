# Roadmap & Future Ideas

Ideas that are too big or too speculative for the README checklist, but worth preserving.

---

## 🚨 IR Sensor — TC102 Selected, Alignment TBD

**Status:** TC102 ordered. Alignment tolerance is the open question — test before finalizing enclosure design.

### Commercial benchmark — FarmTek electric eyes

The FarmTek wireless timing system (sold via Solo Performance Specialties) represents the commercial standard for autocross/hillclimb electric eyes:

- **200 foot optical range**
- **"Just roughly eyeball the alignment"** — wide acceptance angle by design
- Purpose-engineered for motorsport lighting conditions over 25+ years
- **$330/pair** (spare eyes only, full system much more)

This is what serious clubs use. The key differentiator is alignment tolerance — their optics are designed to accept rough aim.

### TC102 alignment test plan

The TC102 has ±10° beam angle. Only real-world testing will tell if this is acceptable for field use.

**Test procedure (do when TC102 arrives):**
1. Set up both units on tripods at 15 feet apart (autocross lane width)
2. Deliberately misalign by a few degrees in each axis
3. Note how much misalignment kills the beam
4. Try setting up on uneven ground / simulated hillclimb slope
5. Time how long alignment takes from scratch

**Pass criteria:** Alignment achievable in under 2 minutes by one person on uneven ground.

**If TC102 fails the test:**
- Look at wider-acceptance industrial through-beam sensors (Sick, Banner, Omron)
- These are $50-100/pair but designed for factory floor use — wide acceptance, reliable
- Or accept that alignment takes longer and build a better bracket/sight system to compensate

### What your system has that FarmTek doesn't
- Solar powered — no battery swaps
- LoRa → cloud in real time
- Historical data, environmental logging, sector timing
- ~$100 total hardware cost vs $1200+ full FarmTek system
- Open source — you can add features

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
