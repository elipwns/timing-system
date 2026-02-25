# DynamoDB Tables
resource "aws_dynamodb_table" "timing_runs" {
  name         = "timing-runs"
  billing_mode = "PAY_PER_REQUEST"
  hash_key     = "date"
  range_key    = "timestamp"
  
  attribute {
    name = "date"
    type = "S"
  }
  
  attribute {
    name = "timestamp"
    type = "S"
  }
}

resource "aws_dynamodb_table" "websocket_connections" {
  name         = "websocket-connections"
  billing_mode = "PAY_PER_REQUEST"
  hash_key     = "connectionId"
  
  attribute {
    name = "connectionId"
    type = "S"
  }
}
