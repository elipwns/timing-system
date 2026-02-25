# SNS Topic
resource "aws_sns_topic" "timing_events" {
  name = "timing-events"
  
  lifecycle {
    ignore_changes = [tags_all]
  }
}
