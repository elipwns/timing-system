# Universal Motorsports Timing System

A wireless timing system using LoRa radio for drag racing, autocross, track days, and RC racing. Built with ESP32, AWS serverless architecture, and real-time WebSocket updates.

## 🎯 Features

- **LoRa Communication**: 500m-2km range without WiFi infrastructure
- **Multi-Run Queue**: Handle up to 10 concurrent runs (FIFO)
- **Real-Time Dashboard**: WebSocket updates with <100ms latency
- **Cloud Storage**: AWS DynamoDB for historical data
- **Millisecond Precision**: Accurate timing for motorsports
- **Scalable Architecture**: Supports unlimited viewers

## 🏗️ Architecture

```
[START Board]  ←─ LoRa (905.2 MHz) ─→  [FINISH Board]
                                              │
                                           WiFi
                                              ↓
                                    API Gateway (HTTPS)
                                              ↓
                                    Lambda: timing-store
                                         ↓        ↓
                                  DynamoDB    SNS Topic
                                              ↓
                                    Lambda: broadcast
                                              ↓
                                  WebSocket API Gateway
                                              ↓
                                    Browser Dashboard(s)
```

## 🚀 Quick Start

### Hardware Requirements
- 2x Heltec LoRa 32 V3 boards
- USB-C cables
- LoRa antennas (included with boards)

### Software Requirements
- Arduino IDE 2.x
- AWS Account (free tier sufficient)
- Terraform (for infrastructure)

### Setup

1. **Clone the repository**
```bash
git clone git@github.com:elipwns/timing-system.git
cd timing-system
```

2. **Upload Arduino code**
- Open `arduino/timing_start/timing_start.ino` in Arduino IDE
- Upload to START board
- Open `arduino/timing_finish_http/timing_finish_http.ino`
- Update WiFi credentials in code
- Upload to FINISH board

3. **Deploy AWS infrastructure**
```bash
cd terraform
terraform init
terraform plan
terraform apply
```

4. **Access dashboard**
- Open https://elikloft.com/timing
- Press START button on START board
- Press FINISH button on FINISH board
- See results in real-time!

## 📁 Project Structure

```
timing-system/
├── arduino/
│   ├── timing_start/          # START line board code
│   └── timing_finish_http/    # FINISH line board code
├── terraform/
│   ├── main.tf                # Main Terraform config
│   ├── lambda.tf              # Lambda functions
│   ├── api-gateway.tf         # API Gateway config
│   ├── dynamodb.tf            # DynamoDB tables
│   └── sns.tf                 # SNS topic
├── lambda/
│   ├── timing-store/          # Stores timing data
│   ├── websocket-connect/     # WebSocket connection handler
│   ├── websocket-disconnect/  # WebSocket disconnect handler
│   └── websocket-broadcast/   # Broadcasts to all clients
├── docs/
│   ├── PROGRESS.md            # Development progress
│   ├── PRESENTATION-SCRIPT.md # Presentation materials
│   └── TERRAFORM-MIGRATION-PLAN.md
└── README.md
```

## 🔧 Technical Specifications

### LoRa Configuration
- **Frequency**: 905.2 MHz (US ISM band)
- **Bandwidth**: 125.0 kHz
- **Spreading Factor**: 7
- **TX Power**: 14 dBm
- **Range**: 200-500m outdoors

### AWS Resources
- **Region**: us-west-2
- **API Gateway REST**: POST /timing endpoint
- **API Gateway WebSocket**: Real-time updates
- **Lambda Functions**: 4 total (Python 3.12)
- **DynamoDB Tables**: 2 (timing-runs, websocket-connections)
- **SNS Topic**: timing-events

### Performance
- LoRa latency: ~50-100ms
- HTTP POST latency: ~200-300ms
- WebSocket broadcast: ~50-100ms
- **Total end-to-end: <500ms**

## 🎓 Learning Objectives

This project demonstrates:
- **IoT Hardware**: ESP32, LoRa radio communication
- **Embedded Programming**: Arduino/C++ firmware
- **Cloud Architecture**: AWS serverless (Lambda, API Gateway, DynamoDB)
- **Real-Time Systems**: WebSocket implementation
- **Infrastructure as Code**: Terraform for AWS resources
- **Full-Stack Development**: Hardware → Cloud → Frontend

## 📊 Cost

**Hardware**: ~$50 (2x Heltec boards)
**AWS**: $0/month (free tier covers usage)

## 🚧 Future Enhancements

- [ ] Multi-lane support (4-8 simultaneous racers)
- [ ] GPS integration for autocross lap timing
- [ ] Mobile app for drivers
- [ ] Split timing gates with cheaper ESP32-C3 boards
- [ ] "Race your friends" cloud comparison feature
- [ ] In-car telemetry (GPS + IMU)

## 📝 License

MIT License - See LICENSE file for details

## 🤝 Contributing

Contributions welcome! Please open an issue or PR.

## 📧 Contact

- GitHub: [@elipwns](https://github.com/elipwns)
- Dashboard: [elikloft.com/timing](https://elikloft.com/timing)

## 🙏 Acknowledgments

- Heltec Automation for the LoRa 32 V3 boards
- AWS for free tier services
- LoRa Alliance for the protocol specification
