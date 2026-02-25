# Terraform Import Progress

## ✅ Completed

- [x] DynamoDB tables (2)
  - timing-runs
  - websocket-connections
- [x] SNS topic
  - timing-events
- [x] IAM roles (4)
  - timing-store-role-xdcnkbwl
  - websocket-connect-role-gdd4pflw
  - websocket-disconnect-role-0jyle23z
  - websocket-broadcast-role-liu4zeei
- [x] Lambda functions (4) with actual code
  - timing-store
  - websocket-connect
  - websocket-disconnect
  - websocket-broadcast

## 🚧 Remaining

### API Gateway REST API
- [ ] Import REST API: `ip8ds6zguc`
- [ ] Import stage: `prd`
- [ ] Import resource: `/timing`
- [ ] Import method: `POST /timing`
- [ ] Import integration: Lambda (timing-store)
- [ ] CORS configuration

### API Gateway WebSocket API
- [ ] Import WebSocket API: `v5rvshh3g7`
- [ ] Import stage: `prod`
- [ ] Import routes:
  - `$connect` → websocket-connect
  - `$disconnect` → websocket-disconnect
  - `$default`
- [ ] Import integrations (with Lambda proxy enabled!)
- [ ] Import Lambda permissions for API Gateway

### SNS Subscriptions
- [ ] Import SNS subscription: timing-events → websocket-broadcast Lambda

### CloudWatch Log Groups (Optional)
- [ ] /aws/lambda/timing-store
- [ ] /aws/lambda/websocket-connect
- [ ] /aws/lambda/websocket-disconnect
- [ ] /aws/lambda/websocket-broadcast

## 📝 Notes

### IAM Permissions Needed
User `amplify-dev` needs these policies:
- ✅ AmazonDynamoDBReadOnlyAccess
- ✅ AmazonSNSReadOnlyAccess
- ✅ AWSLambda_ReadOnlyAccess (or lambda:GetFunction)
- ⚠️ IAM read permissions (for listing roles)
- ⚠️ API Gateway read permissions (for importing APIs)

### Key Learnings
1. **Import blocks** (Terraform 1.5+) are amazing - just specify resource type and ID
2. **Lifecycle ignore_changes** needed for Lambda code during import (but we switched to actual code)
3. **Path attribute** matters for IAM roles created via Console (service-role/)
4. **Archive provider** auto-zips Lambda code from source directories
5. Always check what Terraform wants to change before applying!

### API Gateway Complexity
- REST API has nested resources (API → Stage → Resource → Method → Integration)
- WebSocket API needs Lambda proxy integration enabled (critical!)
- Lambda permissions needed for API Gateway to invoke functions
- CORS configuration on REST API

## 🎯 Next Session Plan

1. Add API Gateway read permissions to `amplify-dev` user
2. Import REST API (ip8ds6zguc) with stage and resources
3. Import WebSocket API (v5rvshh3g7) with routes
4. Import Lambda permissions (aws_lambda_permission resources)
5. Import SNS subscription
6. Verify `terraform plan` shows no changes
7. Merge PR!

## 🔗 References

- AWS Infrastructure docs: `docs/AWS-INFRASTRUCTURE.md`
- Terraform migration plan: `TERRAFORM-MIGRATION-PLAN.md`
- API Gateway REST ID: ip8ds6zguc
- API Gateway WebSocket ID: v5rvshh3g7
