# Lambda Functions

data "archive_file" "timing_store" {
  type        = "zip"
  source_dir  = "${path.module}/../lambda/timing-store"
  output_path = "${path.module}/timing-store.zip"
}

data "archive_file" "websocket_connect" {
  type        = "zip"
  source_dir  = "${path.module}/../lambda/websocket-connect"
  output_path = "${path.module}/websocket-connect.zip"
}

data "archive_file" "websocket_disconnect" {
  type        = "zip"
  source_dir  = "${path.module}/../lambda/websocket-disconnect"
  output_path = "${path.module}/websocket-disconnect.zip"
}

data "archive_file" "websocket_broadcast" {
  type        = "zip"
  source_dir  = "${path.module}/../lambda/websocket-broadcast"
  output_path = "${path.module}/websocket-broadcast.zip"
}

resource "aws_lambda_function" "timing_store" {
  function_name    = "timing-store"
  role             = aws_iam_role.timing_store.arn
  runtime          = "python3.12"
  handler          = "lambda_function.lambda_handler"
  filename         = data.archive_file.timing_store.output_path
  source_code_hash = data.archive_file.timing_store.output_base64sha256
}

resource "aws_lambda_function" "websocket_connect" {
  function_name    = "websocket-connect"
  role             = aws_iam_role.websocket_connect.arn
  runtime          = "python3.12"
  handler          = "lambda_function.lambda_handler"
  filename         = data.archive_file.websocket_connect.output_path
  source_code_hash = data.archive_file.websocket_connect.output_base64sha256
}

resource "aws_lambda_function" "websocket_disconnect" {
  function_name    = "websocket-disconnect"
  role             = aws_iam_role.websocket_disconnect.arn
  runtime          = "python3.12"
  handler          = "lambda_function.lambda_handler"
  filename         = data.archive_file.websocket_disconnect.output_path
  source_code_hash = data.archive_file.websocket_disconnect.output_base64sha256
}

resource "aws_lambda_function" "websocket_broadcast" {
  function_name    = "websocket-broadcast"
  role             = aws_iam_role.websocket_broadcast.arn
  runtime          = "python3.12"
  handler          = "lambda_function.lambda_handler"
  filename         = data.archive_file.websocket_broadcast.output_path
  source_code_hash = data.archive_file.websocket_broadcast.output_base64sha256
}
