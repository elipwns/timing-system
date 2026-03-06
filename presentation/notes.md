# Motorsports Timing System — Speaker Notes
**Target: 5 minutes | 7 slides**

---

## Slide 1 — Title

"This is a drag strip / track day timing system I built from scratch — two ESP32 boards talking over LoRa radio, with a serverless AWS backend and a live browser dashboard."

---

## Slide 2 — Why I Built This

"Commercial systems start at $1,280 and are proprietary black boxes. I wanted something open, cheap, and extensible — came in around $50 in hardware and the AWS bill is literally zero."

---

## Slide 3 — System Architecture

"Start board fires a LoRa packet, finish board receives it and starts a timer. When the run finishes, it POSTs over HTTPS to API Gateway, Lambda writes to DynamoDB and fans out over SNS, and the dashboard gets a WebSocket push."

---

## Slide 4 — Hardware

"Both boards are Heltec LoRa 32 V3 — ESP32 dual-core with an SX1262 LoRa chip built in. $25 each, no soldering. 500m outdoor range at SF7. The board even has solar charging input which is wild for $25."

---

## Slide 5 — Cloud Stack

"Four Lambda functions in Python 3.12. All infra is Terraform — one `terraform apply` stands up both API Gateways, SNS, DynamoDB, everything."

"Big thing I want to call out: I used **import blocks** — a TF 1.5+ feature. You declare what you want to import right in the `.tf` file, run `terraform plan -generate-config-out=generated.tf`, and it writes the resource config for you directly from live state. Completely different experience from the old CLI import where you had to write the config yourself blind.

> **Q&A ready:** If anyone asks — import blocks are idempotent and version-controlled. The old `terraform import` CLI was a one-shot side effect with no record in code. Import blocks live in your `.tf` files, get reviewed in PRs, and can be re-run safely.

### WebSocket connection flow — worth calling out

"The part I found really interesting is how the live push to every browser actually works. There's no persistent server — it's all stateless Lambdas. When a browser opens the dashboard, API Gateway WebSocket assigns it a unique `connectionId` and fires the `ws-connect` Lambda, which just writes that ID into a DynamoDB table. That's your live connection registry.

When a timing result comes in, `ws-broadcast` reads the full list of connectionIds from DynamoDB and calls the API Gateway Management API once per client — `POST /{connectionId}` — and each browser gets the push instantly. When someone closes their tab, `ws-disconnect` fires and removes their ID.

So DynamoDB is doing double duty — it's both the run history store and the live connection registry. And because it's just a table of IDs, you could have 1 browser or 100 browsers watching and the broadcast Lambda doesn't care."

> **Q&A ready:** Stale connections (browser crashed, tab force-closed) are handled by catching the `GoneException` from the Management API and cleaning up the dead connectionId on the fly.

---

## Slide 6 — Demo

"Here's the flow live — [hit start, hit finish, show dashboard updating]. End-to-end under 500ms. If you want to see it yourself, scan the QR or hit elikloft.com/timing — open it on your phone now and you'll see it update in real time when I press the boards."

---

## Slide 7 — Takeaways

"Full IoT stack — firmware, cloud, frontend — for about $50. Biggest learn was Terraform importing live state and the LoRa ISM band regulations. Next up: leaderboard with run history and hardware trip sensors to replace the button."

---

## Timing Guide
| Slide | Target time |
|-------|-------------|
| 1 Title | ~15s |
| 2 Why I Built This | ~30s |
| 3 Architecture | ~60s |
| 4 Hardware | ~45s |
| 5 Cloud Stack | ~45s |
| 6 Demo | ~60s |
| 7 Takeaways | ~30s |
| **Total** | **~5:05** |
