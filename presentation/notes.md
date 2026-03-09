# Motorsports Timing System — Speaker Notes
**Target: 5 minutes | 7 slides**

> 🗣️ **SAY THIS** = actual words to say out loud
> 🧠 **KNOW THIS** = background context, keep in mind while practicing, Q&A prep

---

## Slide 1 — Title (~15s)

🗣️ **SAY THIS**
> "This is a drag strip and track day timing system I built from scratch — two ESP32 boards talking over LoRa radio, with a serverless AWS backend and a live browser dashboard."

---

## Slide 2 — Why I Built This (~30s)

🗣️ **SAY THIS**
> "Commercial systems start at $1,280 and are proprietary black boxes. I wanted something open, cheap, and extensible — ~$50 in the two boards so far — trip sensors still on the roadmap — and the AWS bill is literally zero."

---

## Slide 3 — System Architecture (~60s)

🗣️ **SAY THIS**
> "Start board fires a LoRa packet, finish board receives it and starts a timer. When the run finishes, it POSTs over HTTPS to API Gateway, Lambda writes to DynamoDB and fans out over SNS — that's Simple Notification Service — and the dashboard gets a WebSocket push."

🧠 **KNOW THIS**
- The LoRa packet is just a run number (`START:1`) — no timestamp. The finish board starts its own local `millis()` timer on packet receipt.
- The ~75ms offset is time-on-air (not distance-based) so it's constant across all runs — relative timing is perfectly accurate.

---

## Slide 4 — Hardware (~45s)

🗣️ **SAY THIS**
> "Both boards are Heltec LoRa 32 V3 — ESP32 dual-core with an SX1262 LoRa chip built in. $25 each, no soldering. The board even has solar charging input, which is wild for $25."

> "My setup gets around 500m — more than enough for any track. Worth noting though: the LoRa world record is 830 miles, set with a high-altitude balloon. So we're leaving a little range on the table."

---

## Slide 5 — Cloud Stack (~45s)

🗣️ **SAY THIS**
> "Four Lambda functions in Python 3.12. All infra is Terraform — one `terraform apply` stands up both API Gateways, SNS, DynamoDB, everything."

> "Big thing I want to call out: I used import blocks — a TF 1.5+ feature. You declare what you want to import right in the `.tf` file, run `terraform plan -generate-config-out`, and it writes the resource config for you directly from live state. Completely different experience from the old CLI import. This is actually one of the reasons we've been pushing to upgrade our Terraform version at work."

> "The other thing I found really interesting is how the live push to every browser works — there's no persistent server. When a browser opens the dashboard, API Gateway WebSocket assigns it a unique connection ID and fires a Lambda that writes it to DynamoDB. That's the live connection registry. When a result comes in, the broadcast Lambda reads that list and pushes to every connected client. DynamoDB is doing double duty — run history and connection registry."

🧠 **KNOW THIS**
- Import blocks are idempotent and version-controlled — the old `terraform import` CLI was a one-shot side effect with no record in code.
- Stale connections (crashed tabs) are handled by catching `GoneException` from the Management API and cleaning up the dead ID on the fly.
- SNS = Simple Notification Service — the pub/sub fan-out between `timing-store` and `websocket-broadcast`.

---

## Slide 6 — Demo (~60s)

🗣️ **SAY THIS**
> "Here's the flow live — [hit start, hit finish, show dashboard updating]. End-to-end under 500ms."

> "If you want to see it yourself, scan the QR or hit elikloft.com/timing — open it on your phone now and you'll see it update in real time when I press the boards."

🧠 **KNOW THIS**
- Boards just need power — they're already flashed and connect to WiFi automatically.
- The OLED screens are broken (sun damage) but that's fine — the dashboard is the wow moment.
- If the demo fails, the video on the slide is the fallback.

---

## Slide 7 — Takeaways (~30s)

🗣️ **SAY THIS**
> "Full IoT stack — firmware, cloud, frontend — for about $50. Biggest learns: the SNS fan-out pattern — one publish, multiple subscribers — and using DynamoDB for double duty as both run history and live WebSocket connection registry. Next up: leaderboard with run history and hardware trip sensors to replace the button."

---

## Timing Guide
| Slide | Target |
|-------|--------|
| 1 Title | ~15s |
| 2 Why I Built This | ~30s |
| 3 Architecture | ~60s |
| 4 Hardware | ~45s |
| 5 Cloud Stack | ~45s |
| 6 Demo | ~60s |
| 7 Takeaways | ~30s |
| **Total** | **~5:05** |
