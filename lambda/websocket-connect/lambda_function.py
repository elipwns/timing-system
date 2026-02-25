import json
import boto3
from datetime import datetime

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('websocket-connections')

def lambda_handler(event, context):
    try:
        print(f"Event: {json.dumps(event)}")
        
        connection_id = event['requestContext']['connectionId']
        
        table.put_item(Item={
            'connectionId': connection_id,
            'timestamp': datetime.utcnow().isoformat()
        })
        
        print(f"Connected: {connection_id}")
        
        return {
            'statusCode': 200,
            'body': 'Connected'
        }
    except Exception as e:
        print(f"ERROR: {str(e)}")
        return {
            'statusCode': 500,
            'body': json.dumps({'error': str(e)})
        }
