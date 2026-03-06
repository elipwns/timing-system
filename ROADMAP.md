# Roadmap & Future Ideas

Ideas that are too big or too speculative for the README checklist, but worth preserving.

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
