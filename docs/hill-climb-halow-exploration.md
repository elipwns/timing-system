# MM8108 Wi-Fi HaLow for Hill Climb Timing — Exploration

## Actual Current Setup

**Autocross**: Farmtek Polaris console at the venue, standard operation.

**Hill climbs**: Multiple Farmtek consoles are **time-synced together**, then physically placed at each station on the hill. Each runs independently, timestamps events against its synced clock, and results are reconciled manually after the event.

Problems with this:
- **Only 2 beam sets** — need 6+ stations for a proper hill climb
- **Clock drift** — no correction mechanism once the consoles are separated
- **No live view** — results only visible after manually collecting all consoles
- **Manual reconciliation** — matching car numbers to timestamps across units is error-prone
- **Fault tolerance** — if a beam falls over or a console resets, that station's data is gone for that run, and recovery mid-event is awkward

---

## What MM8108 HaLow Would Change

The MM8108 is Morse Micro's Wi-Fi HaLow chip (IEEE 802.11ah):
- Sub-1 GHz (850–950 MHz), 1–2km range at useful data rates
- Full IP networking — HTTP, WebSocket, NTP all work natively
- Mesh networking support
- Low power (runs on battery)
- Supports AP and STA modes

HaLow creates an IP mesh across the entire course. With that in place, the Farmtek consoles don't have to be islands — they can all talk to a coordinator at the paddock in real time.

There are two practical approaches:

---

## Option A — Interface with Existing Farmtek Consoles (Hybrid)

The Farmtek console has a USB port used for timing software integration. A small Pi Zero 2W with a HaLow USB dongle sits alongside each Farmtek console:

```
[Farmtek console + beams]
        ↓ USB (timing data)
[Pi Zero 2W]
        ↓ MM8108 HaLow USB dongle
        ↕ HaLow mesh (~1–2km)
[Coordinator Pi at paddock]
        → live dashboard
        → car tracking
        → results aggregation
```

**Pros**:
- Keeps Farmtek hardware (proven, reliable, already owned)
- Farmtek still does timing — no risk of regression
- Pi just reads and relays; easy to retrofit
- Morse Micro's Linux driver works on Pi — no driver uncertainty

**Cons**:
- Farmtek USB protocol needs to be understood (the Timer Interface Software talks to it — may be documented or can be sniffed)
- Still limited to Farmtek's 4-pair maximum for beams
- Each station needs a Pi + HaLow dongle (~$35) plus power bank

**Recommended as the starting point** — validates the HaLow mesh concept using existing hardware with low implementation risk.

---

## Option B — Standalone HaLow Timing Stations

Each station is a self-contained unit with its own IR trigger:

```
[IR beam trigger]
      ↓ GPIO
[Pi Zero 2W]  ←  timestamp captured in microseconds
      ↓ MM8108 HaLow USB dongle
      ↕ HaLow mesh
[Coordinator Pi at paddock]
```

The Farmtek stays in use for autocross. Hill climbs get a dedicated standalone system.

**Pros**:
- Unlimited stations — identical hardware at each point
- Continuous NTP sync — no pre-event clock ritual, no drift over the day
- Full fault tolerance (local buffering, backfill on reconnect)
- Not capped at Farmtek's 4-pair limit

**Cons**:
- New trigger hardware needed at each station
- More to deploy and maintain

**Natural evolution** once Option A is proven — add extra sector stations beyond what Farmtek covers.

---

## Fault Tolerance Design

Replace the strict state machine with **event streaming + eventual consistency**:

**Each station**:
1. Beam breaks → capture timestamp → write to local file → send event to coordinator
2. If coordinator unreachable → buffer locally → retry when link restores
3. Station can reboot mid-event without losing locally stored data

**Coordinator**:
1. Collects `{station_id, timestamp_us, run_number}` events from all stations
2. Cars tracked by run number — operator logs car number before release
3. If a station misses → that sector is "N/A", run still valid for other sectors
4. Late-arriving buffered events → retroactively fill in the record
5. Operator UI: mark false positive, manually enter a time, cancel a run

| Problem | Outcome |
|---|---|
| Beam knocked over | That sector N/A; rest of run valid |
| Station reboots | Re-syncs NTP, sends buffered events |
| HaLow link drops temporarily | Buffer locally; sync on reconnect |
| Two cars close together | Two events logged; operator assigns manually |
| Wrong car number entered | Correct at coordinator — data already stored safely |

---

## Clock Sync

**Current (Farmtek hill climb)**: Manual pre-event sync, consoles drift independently all day.

**With HaLow coordinator**:
- Coordinator Pi runs an NTP server
- All station Pis sync every ~30 seconds
- Drift stays within ±1ms across all stations for the full event day
- No pre-event sync ritual

For sub-millisecond accuracy (if ever needed): GPS PPS module (~$10) at each station gives ~100µs accuracy independent of NTP.

---

## Trigger Hardware (for Option B)

| Option | Accuracy | Notes |
|---|---|---|
| IR through-beam (new eyes) | ~1ms | Two posts per station; Farmtek-class reliability |
| Retroreflective IR | ~1ms | Single post; reflector strip on opposite side |
| LIDAR TF-Luna (~$25) | ~2–5ms | One unit, no cross-road hardware; adequate for trials |
| Reuse Farmtek spare eyes | ~1ms | Wire detector output to Pi GPIO |

For hill climbs quoted to 0.001s precision, IR through-beam is the right choice. LIDAR is good enough for initial trials while validating the rest of the system.

---

## Car Number Entry

The coordinator needs to know which car just left the start. For a first version, **manual entry** at the coordinator tablet is straightforward:

```
[Car 42] → [RELEASE]  →  coordinator expects triggers at each station within next 5 min
```

Future: RFID transponder on each car (motorsport standard but expensive) or QR code scan.

---

## Frequency Note

Both Farmtek (900 MHz ISM) and MM8108 HaLow (902–928 MHz) operate in the same ISM band. Running both simultaneously at the same event could cause interference. In practice:
- For a comparison trial: space HaLow and Farmtek stations apart on the course
- For full hill climb events: HaLow replaces the Farmtek role; no coexistence issue
- Farmtek continues exclusively for autocross events

---

## Coordinator Hardware

Raspberry Pi 4 (or Pi Zero 2W) at the paddock:
- MM8108 USB HaLow dongle → creates HaLow AP (all station Pis connect as STAs)
- Built-in WiFi → 2.4 GHz hotspot for spectator/official phones and tablets
- Runs: NTP server, event aggregator, WebSocket server, results dashboard
- Power: USB-C power bank (~8W for Pi 4, ~2W for Pi Zero 2W)
- Optional: LTE USB dongle → cloud sync to existing AWS backend when available

---

## Summary Comparison

| | Farmtek (current) | Option A (hybrid) | Option B (standalone) |
|---|---|---|---|
| Range | 300ft eye-to-console | 300ft (Farmtek eyes) | 1–2km |
| Stations | 4 max (2 owned) | 4 max | Unlimited |
| Clock sync | Manual pre-event | NTP continuous | NTP continuous |
| Live results | No | Yes | Yes |
| Car tracking | Manual | Coordinator app | Coordinator app |
| Fault tolerance | Brittle | Event buffering | Event buffering |
| Cost to add station | ~$400 (Farmtek) | ~$35 (Pi + dongle) | ~$95 (Pi + dongle + eyes) |
| Implementation risk | — | Low | Medium |

**Recommended path**: Start with Option A to prove the HaLow mesh and coordinator software with existing Farmtek hardware. Then add Option B stations for the extra sector points Farmtek can't cover.

---

## Files to Build (when ready to implement)

| File | Purpose |
|---|---|
| `coordinator/server.py` | Event aggregator, NTP bridge, WebSocket broadcast, car tracking |
| `coordinator/dashboard/index.html` | Live results dashboard for paddock tablets |
| `coordinator/requirements.txt` | Flask, websockets, etc. |
| `station/station.py` | Station Pi: reads Farmtek USB or GPIO trigger, buffers and sends events |
| `docs/hill-climb-setup.md` | Event day deployment and setup guide |

---

## References

- [SPS Farmtek Wireless AutoCross Timing Package](https://soloperformance.com/products/wireless-auto-cross-timing-package)
- [Farmtek Polaris FCC filing (NWNMI043)](https://fccid.io/NWNMI043)
- [MM8108 Off-Grid Mesh Wi-Fi — Geeky Gadgets](https://www.geeky-gadgets.com/mm8108-long-range-wifi/)
- [Morse Micro MM8108-MF15457 datasheet](https://www.morsemicro.com/resources/datasheets/modules/MM8108-MF15457_Data_Sheet.pdf)
