# API Gateway REST API

resource "aws_api_gateway_rest_api" "timing_api" {
  name = "timing-api"
}


# Lambda permission for API Gateway
resource "aws_lambda_permission" "api_gateway_timing_store" {
  statement_id  = "63e89568-9228-5f41-8167-fa4521b6c949"
  action        = "lambda:InvokeFunction"
  function_name = aws_lambda_function.timing_store.function_name
  principal     = "apigateway.amazonaws.com"
  source_arn    = "${aws_api_gateway_rest_api.timing_api.execution_arn}/*/POST/timing"
}
