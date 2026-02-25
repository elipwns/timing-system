# AWS Infrastructure Inventory

**Region:** us-west-2  
**Account ID:** 772255980793  
**Created:** Manually via AWS Console  
**Status:** Production (working system)

---

## API Gateway - REST API

**Name:** timing-api  
**ID:** ip8ds6zguc  
**Endpoint:** https://ip8ds6zguc.execute-api.us-west-2.amazonaws.com/prd  
**Stage:** prd

### Resources & Methods
- **POST /timing** → Lambda: timing-store
  - CORS enabled
  - No authentication
  - Accepts JSON: `{event, run, time, timestamp}`

### Configuration
- Binary media types: None
- Throttling: Default AWS limits
- Caching: Disabled
- Logging: CloudWatch (if enabled)

---

## API Gateway - WebSocket API

**Name:** timing-websocket  
**ID:** v5rvshh3g7  
**Endpoint:** wss://v5rvshh3g7.execute-api.us-west-2.amazonaws.com/prod  
**Stage:** prod

### Routes
- **$connect** → Lambda: websocket-connect
  - Stores connectionId in DynamoDB
  - Lambda proxy integration: ENABLED (critical!)
  
- **$disconnect** → Lambda: websocket-disconnect
  - Removes connectionId from DynamoDB
  - Lambda proxy integration: ENABLED
  
- **$default** → None (not used)

### Configuration
- Route selection: $request.body.action
- Integration timeout: 29 seconds (AWS max)
- Logging: CloudWatch (if enabled)

---

## Lambda Functions

### 1. timing-store
**Runtime:** Python 3.12  
**Handler:** lambda_function.lambda_handler  
**Memory:** 128 MB  
**Timeout:** 30 seconds  
**Trigger:** API Gateway REST (POST /timing)

**Environment Variables:** None

**IAM Role Permissions:**
- DynamoDB: PutItem on timing-runs table
- SNS: Publish to timing-events topic
- CloudWatch Logs: CreateLogGroup, CreateLogStream, PutLogEvents

**Code Logic:**
1. Receives timing data from ESP32 via HTTP POST
2. Converts float values to Decimal (DynamoDB requirement)
3. Writes to DynamoDB with date as partition key
4. Publishes to SNS topic for broadcasting
5. Returns 200 OK

**Dependencies:**
- boto3 (AWS SDK)
- json
- datetime
- decimal

---

### 2. websocket-connect
**Runtime:** Python 3.12  
**Handler:** lambda_function.lambda_handler  
**Memory:** 128 MB  
**Timeout:** 30 seconds  
**Trigger:** API Gateway WebSocket ($connect route)

**Environment Variables:** None

**IAM Role Permissions:**
- DynamoDB: PutItem on websocket-connections table
- CloudWatch Logs: CreateLogGroup, CreateLogStream, PutLogEvents

**Code Logic:**
1. Extracts connectionId from event.requestContext
2. Stores connectionId in DynamoDB with timestamp
3. Returns 200 OK

---

### 3. websocket-disconnect
**Runtime:** Python 3.12  
**Handler:** lambda_function.lambda_handler  
**Memory:** 128 MB  
**Timeout:** 30 seconds  
**Trigger:** API Gateway WebSocket ($disconnect route)

**Environment Variables:** None

**IAM Role Permissions:**
- DynamoDB: DeleteItem on websocket-connections table
- CloudWatch Logs: CreateLogGroup, CreateLogStream, PutLogEvents

**Code Logic:**
1. Extracts connectionId from event.requestContext
2. Deletes connectionId from DynamoDB
3. Returns 200 OK

---

### 4. websocket-broadcast
**Runtime:** Python 3.12  
**Handler:** lambda_function.lambda_handler  
**Memory:** 128 MB  
**Timeout:** 30 seconds  
**Trigger:** SNS topic (timing-events)

**Environment Variables:**
- WEBSOCKET_API_ENDPOINT: v5rvshh3g7.execute-api.us-west-2.amazonaws.com/prod

**IAM Role Permissions:**
- DynamoDB: Scan on websocket-connections table
- API Gateway: POST to @connections (WebSocket callback)
- CloudWatch Logs: CreateLogGroup, CreateLogStream, PutLogEvents

**Code Logic:**
1. Triggered by SNS when new timing data arrives
2. Queries all active connectionIds from DynamoDB
3. Posts timing data to each connection via API Gateway Management API
4. Handles stale connections (410 Gone) by removing from DynamoDB
5. Returns success count

**Dependencies:**
- boto3 (AWS SDK)
- json
- os

---

## DynamoDB Tables

### 1. timing-runs
**Purpose:** Store all timing events (START and RESULT)

**Partition Key:** date (String) - Format: YYYY-MM-DD  
**Sort Key:** timestamp (String) - ISO 8601 format

**Attributes:**
- date (String) - Partition key
- timestamp (String) - Sort key
- event (String) - "START" or "RESULT"
- run (Number) - Run number
- time (Number) - Elapsed time in seconds (0 for START events)

**Capacity:**
- Billing mode: On-demand (pay per request)
- Read/Write capacity: Auto-scaled

**Indexes:** None

**TTL:** Disabled (keep all data)

**Point-in-time recovery:** Disabled (can enable for production)

**Encryption:** AWS managed key

---

### 2. websocket-connections
**Purpose:** Track active WebSocket connections

**Partition Key:** connectionId (String)

**Attributes:**
- connectionId (String) - Partition key
- timestamp (String) - Connection time

**Capacity:**
- Billing mode: On-demand
- Read/Write capacity: Auto-scaled

**Indexes:** None

**TTL:** Could enable (e.g., 1 hour) to auto-cleanup stale connections

**Point-in-time recovery:** Disabled

**Encryption:** AWS managed key

---

## SNS Topic

**Name:** timing-events  
**ARN:** arn:aws:sns:us-west-2:772255980793:timing-events  
**Type:** Standard (not FIFO)

**Subscriptions:**
- Lambda: websocket-broadcast (protocol: lambda)

**Access Policy:**
- Allow Lambda (timing-store) to publish

**Delivery Policy:** Default retry policy

**Purpose:** Decouples timing data storage from WebSocket broadcasting

---

## IAM Roles

### timing-store-role
**Trusted Entity:** lambda.amazonaws.com

**Policies:**
- AWSLambdaBasicExecutionRole (managed)
- Custom inline policy:
  ```json
  {
    "Version": "2012-10-17",
    "Statement": [
      {
        "Effect": "Allow",
        "Action": [
          "dynamodb:PutItem"
        ],
        "Resource": "arn:aws:dynamodb:us-west-2:772255980793:table/timing-runs"
      },
      {
        "Effect": "Allow",
        "Action": [
          "sns:Publish"
        ],
        "Resource": "arn:aws:sns:us-west-2:772255980793:timing-events"
      }
    ]
  }
  ```

---

### websocket-connect-role
**Trusted Entity:** lambda.amazonaws.com

**Policies:**
- AWSLambdaBasicExecutionRole (managed)
- Custom inline policy:
  ```json
  {
    "Version": "2012-10-17",
    "Statement": [
      {
        "Effect": "Allow",
        "Action": [
          "dynamodb:PutItem"
        ],
        "Resource": "arn:aws:dynamodb:us-west-2:772255980793:table/websocket-connections"
      }
    ]
  }
  ```

---

### websocket-disconnect-role
**Trusted Entity:** lambda.amazonaws.com

**Policies:**
- AWSLambdaBasicExecutionRole (managed)
- Custom inline policy:
  ```json
  {
    "Version": "2012-10-17",
    "Statement": [
      {
        "Effect": "Allow",
        "Action": [
          "dynamodb:DeleteItem"
        ],
        "Resource": "arn:aws:dynamodb:us-west-2:772255980793:table/websocket-connections"
      }
    ]
  }
  ```

---

### websocket-broadcast-role
**Trusted Entity:** lambda.amazonaws.com

**Policies:**
- AWSLambdaBasicExecutionRole (managed)
- Custom inline policy:
  ```json
  {
    "Version": "2012-10-17",
    "Statement": [
      {
        "Effect": "Allow",
        "Action": [
          "dynamodb:Scan",
          "dynamodb:DeleteItem"
        ],
        "Resource": "arn:aws:dynamodb:us-west-2:772255980793:table/websocket-connections"
      },
      {
        "Effect": "Allow",
        "Action": [
          "execute-api:ManageConnections"
        ],
        "Resource": "arn:aws:execute-api:us-west-2:772255980793:v5rvshh3g7/prod/POST/@connections/*"
      }
    ]
  }
  ```

---

## CloudWatch Logs

**Log Groups:**
- /aws/lambda/timing-store
- /aws/lambda/websocket-connect
- /aws/lambda/websocket-disconnect
- /aws/lambda/websocket-broadcast

**Retention:** 7 days (default, can adjust)

**Metrics:** Lambda duration, errors, invocations

---

## Cost Breakdown (Estimated Monthly)

### Current Usage (Low Volume)
- **API Gateway REST:** ~1,000 requests/month = $0.00
- **API Gateway WebSocket:** ~100 connections/month = $0.00
- **Lambda:** ~2,000 invocations/month = $0.00 (free tier)
- **DynamoDB:** On-demand, ~500 writes/month = $0.00
- **SNS:** ~1,000 publishes/month = $0.00
- **CloudWatch Logs:** ~100 MB/month = $0.00

**Total:** $0.00/month (within free tier)

### Production Scale (1,000 users, 10,000 runs/month)
- **API Gateway REST:** 10,000 requests = $0.04
- **API Gateway WebSocket:** 1,000 connections × 1 hour avg = $0.25
- **Lambda:** 50,000 invocations = $0.00 (free tier covers)
- **DynamoDB:** 20,000 writes = $2.50
- **SNS:** 10,000 publishes = $0.00
- **CloudWatch Logs:** 1 GB = $0.50

**Total:** ~$3.29/month

---

## Data Flow

### Timing Event Flow
1. ESP32 (FINISH board) → HTTP POST → API Gateway REST
2. API Gateway → Lambda (timing-store)
3. Lambda → DynamoDB (timing-runs) - Store event
4. Lambda → SNS (timing-events) - Publish event
5. SNS → Lambda (websocket-broadcast) - Trigger broadcast
6. Lambda → DynamoDB (websocket-connections) - Get all connections
7. Lambda → API Gateway WebSocket → All connected browsers

### WebSocket Connection Flow
1. Browser → WebSocket connect → API Gateway WebSocket
2. API Gateway → Lambda (websocket-connect)
3. Lambda → DynamoDB (websocket-connections) - Store connectionId
4. Browser receives timing updates via WebSocket

### WebSocket Disconnection Flow
1. Browser closes → API Gateway WebSocket
2. API Gateway → Lambda (websocket-disconnect)
3. Lambda → DynamoDB (websocket-connections) - Remove connectionId

---

## Critical Configuration Notes

### Lambda Proxy Integration
- **MUST** be enabled for WebSocket routes ($connect, $disconnect)
- Without it, event.requestContext is empty
- This was a major debugging issue during development

### DynamoDB Decimal Type
- Python floats must be converted to Decimal for DynamoDB
- Use: `Decimal(str(float_value))`
- Direct float assignment causes TypeError

### WebSocket Callback URL
- Format: `https://{api-id}.execute-api.{region}.amazonaws.com/{stage}/@connections/{connectionId}`
- Requires execute-api:ManageConnections permission
- Returns 410 Gone for stale connections (handle gracefully)

### CORS Configuration
- REST API needs CORS enabled for browser access
- WebSocket API doesn't use CORS (different protocol)

---

## Terraform Migration Priority

### Phase 1: Foundation (Start Here)
1. DynamoDB tables (timing-runs, websocket-connections)
2. SNS topic (timing-events)

### Phase 2: Compute
3. IAM roles for Lambda functions
4. Lambda functions (all 4)

### Phase 3: API Gateway (Most Complex)
5. API Gateway REST API
6. API Gateway WebSocket API
7. Integrations and routes

### Phase 4: Monitoring
8. CloudWatch Log Groups
9. CloudWatch Alarms (optional)

---

## Testing Checklist

After Terraform deployment, verify:
- [ ] POST to REST API endpoint returns 200
- [ ] Data appears in timing-runs DynamoDB table
- [ ] WebSocket connection succeeds
- [ ] connectionId stored in websocket-connections table
- [ ] Timing event triggers WebSocket broadcast
- [ ] Browser receives real-time update
- [ ] WebSocket disconnect removes connectionId
- [ ] Lambda logs appear in CloudWatch

---

## Backup & Disaster Recovery

### Current State
- No automated backups
- DynamoDB point-in-time recovery: DISABLED
- Lambda code: Stored in AWS (no version control yet)

### Recommended
- Enable DynamoDB point-in-time recovery
- Store Lambda code in Git (timing-system repo)
- Export DynamoDB data periodically (S3)
- Document manual recreation steps

---

## Security Considerations

### Current Security Posture
- No authentication on REST API (public endpoint)
- No authentication on WebSocket API (public)
- IAM roles follow least privilege
- No sensitive data in timing events

### Production Recommendations
- Add API key or Cognito authentication
- Rate limiting on API Gateway
- WAF rules for DDoS protection
- Enable CloudTrail for audit logging
- Rotate credentials regularly

---

## Known Issues & Limitations

1. **No authentication** - Anyone can POST timing data
2. **No data validation** - Lambda trusts input data
3. **Scan operation** - websocket-broadcast uses Scan (inefficient at scale)
4. **No error handling** - Failed WebSocket posts don't retry
5. **No monitoring** - No CloudWatch alarms for failures
6. **Hardcoded values** - API endpoints in Lambda code

### Future Improvements
- Add input validation in timing-store Lambda
- Use DynamoDB Streams instead of SNS + Scan
- Add CloudWatch alarms for Lambda errors
- Parameterize API endpoints (environment variables)
- Add authentication (API keys or Cognito)

---

## References

- AWS Account: 772255980793
- Region: us-west-2
- Dashboard: https://elikloft.com/timing
- GitHub: https://github.com/elipwns/timing-system
