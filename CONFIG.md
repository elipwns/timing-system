# Configuration Guide

## WiFi Setup

Edit `arduino/timing_finish_http/timing_finish_http.ino`:

```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
```

## AWS API Gateway

After deploying AWS infrastructure with Terraform, update:

```cpp
#define API_ENDPOINT "YOUR_API_GATEWAY_ENDPOINT"
```

Example: `https://xxxxxxxxxx.execute-api.us-west-2.amazonaws.com/prd/timing`
