import json
import boto3

dynamodb = boto3.resource('dynamodb')
connections_table = dynamodb.Table('websocket-connections')

apigateway = boto3.client('apigatewaymanagementapi',
    endpoint_url='https://v5rvshh3g7.execute-api.us-west-2.amazonaws.com/prod')

def lambda_handler(event, context):
    # Parse SNS message
    message = json.loads(event['Records'][0]['Sns']['Message'])
    
    # Get all connections
    response = connections_table.scan()
    connections = response.get('Items', [])
    
    # Broadcast to all
    for connection in connections:
        try:
            apigateway.post_to_connection(
                ConnectionId=connection['connectionId'],
                Data=json.dumps(message).encode('utf-8')
            )
        except:
            # Connection stale, delete it
            connections_table.delete_item(Key={'connectionId': connection['connectionId']})
    
    return {'statusCode': 200}
