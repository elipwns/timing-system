import json
import boto3
from decimal import Decimal
from datetime import datetime

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('timing-runs')

sns = boto3.client('sns')
SNS_TOPIC_ARN = 'arn:aws:sns:us-west-2:772255980793:timing-events'

def lambda_handler(event, context):
    try:
        body = json.loads(event['body'])
        now = datetime.utcnow()
        
        # Write to DynamoDB
        table.put_item(Item={
            'date': now.strftime('%Y-%m-%d'),
            'timestamp': now.isoformat(),
            'event': body['event'],
            'run': body['run'],
            'time': Decimal(str(body['time']))
        })
        
        # Publish to SNS for WebSocket broadcast
        sns.publish(
            TopicArn=SNS_TOPIC_ARN,
            Message=json.dumps(body)
        )
        
        return {
            'statusCode': 200,
            'body': json.dumps({'status': 'success'})
        }
        
    except Exception as e:
        print(f"ERROR: {str(e)}")
        return {'statusCode': 500, 'body': json.dumps({'error': str(e)})}
