import json
import boto3

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('websocket-connections')

def lambda_handler(event, context):
    try:
        print(f"Event: {json.dumps(event)}")
        
        connection_id = event['requestContext']['connectionId']
        
        table.delete_item(Key={'connectionId': connection_id})
        
        print(f"Disconnected: {connection_id}")
        
        return {
            'statusCode': 200,
            'body': 'Disconnected'
        }
    except Exception as e:
        print(f"ERROR: {str(e)}")
        return {
            'statusCode': 200,  # Return 200 even on error for disconnect
            'body': json.dumps({'error': str(e)})
        }
