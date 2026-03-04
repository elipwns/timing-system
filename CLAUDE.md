# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Universal Motorsports Timing System — ESP32/LoRa hardware boards communicate over radio, and the FINISH board posts timing data to AWS via HTTPS. A serverless AWS backend stores data and broadcasts results to browser dashboards in real time via WebSocket.

Data flow:
```
[START Board] --LoRa--> [FINISH Board] --HTTPS POST--> API Gateway REST
                                                              |
                                                      Lambda: timing-store
                                                         |          |
                                                     DynamoDB   SNS Topic
                                                                    |
                                                      Lambda: websocket-broadcast
                                                                    |
                                                      API Gateway WebSocket
                                                                    |
                                                          Browser Dashboard
```

## AWS Infrastructure Commands

```bash
cd terraform
terraform init       # first time only
terraform plan
terraform apply      # also re-zips and deploys Lambda code changes
```

Lambda functions are auto-zipped from `lambda/` subdirectories by Terraform's `archive_file` data sources — no manual packaging needed.

## Hardcoded AWS Values in Lambda Code

These values are account/deployment-specific and embedded directly in Lambda source:

- `lambda/timing-store/lambda_function.py`: SNS topic ARN (`arn:aws:sns:us-west-2:772255980793:timing-events`)
- `lambda/websocket-broadcast/lambda_function.py`: WebSocket management endpoint URL (`https://v5rvshh3g7.execute-api.us-west-2.amazonaws.com/prod`)

Update these if deploying to a new account or if API Gateway IDs change.

## Arduino / Firmware

**Active sketches** (upload these):
- `arduino/timing_start/timing_start.ino` → START board
- `arduino/timing_finish_http/timing_finish_http.ino` → FINISH board

Before uploading `timing_finish_http`, set in the `.ino` file:
- `WIFI_SSID` / `WIFI_PASSWORD`
- `API_ENDPOINT` — REST API URL from Terraform output (e.g. `https://xxxxxxxxxx.execute-api.us-west-2.amazonaws.com/prd/timing`)

The `arduino/timing_finish/` and `arduino/timing_finish_aws/` directories are older iterations using MQTT/AWS IoT Core — not the active path.

Board: Heltec LoRa 32 V3 (ESP32). Library: `heltec_unofficial`. LoRa: 905.2 MHz, SF7, 125 kHz BW.

## Lambda Functions (Python 3.12)

| Function | Trigger | Purpose |
|---|---|---|
| `timing-store` | REST API POST `/timing` | Writes run to DynamoDB, publishes to SNS |
| `websocket-connect` | WebSocket `$connect` | Saves connectionId to DynamoDB |
| `websocket-disconnect` | WebSocket `$disconnect` | Removes connectionId from DynamoDB |
| `websocket-broadcast` | SNS subscription | Broadcasts timing event to all WS clients |

DynamoDB tables: `timing-runs` (partition: `date`, sort: `timestamp`), `websocket-connections` (partition: `connectionId`).

## Terraform Structure

All infrastructure is in `terraform/` as a flat set of `.tf` files — no modules. Resources were imported from existing AWS state. The `terraform` AWS profile is used for IaC (set via `AWS_DEFAULT_PROFILE` or `--profile terraform`).
