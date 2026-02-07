import json
import boto3
import base64
import uuid
import os
import datetime

# --- INITIALIZE CLIENTS ---
s3_client = boto3.client('s3')
rekognition_client = boto3.client('rekognition', region_name='eu-west-1')
sns_client = boto3.client('sns', region_name='eu-north-1')

# --- CONFIGURATION ---
# "Captured" bucket (Where we save the new photo)
BUCKET_NAME = os.environ.get('BUCKET_NAME')

# "Authorized" bucket (Where we have the faces of people who are allowed to enter known faces are stored)
AUTHORIZED_BUCKET_NAME = os.environ.get('AUTHORIZED_BUCKET_NAME')

# Security Token (Must match the one in auth.h)
SECRET_ACCESS_CODE = os.environ.get('ESP_AUTH_TOKEN')

# SNS Topic ARN (Set this in Lambda Environment Variables)
SNS_TOPIC_ARN = os.environ.get('SNS_TOPIC_ARN')

def lambda_handler(event, context):
    print("Received new upload request...")

    try:
        # 1. SECURITY CHECK (Access Token)
        headers = event.get('headers', {})
        incoming_token = headers.get('x-auth-token') or headers.get('X-Auth-Token')

        if not incoming_token or incoming_token != SECRET_ACCESS_CODE:
            print(f"Auth Failed. Received: {incoming_token}")
            return {
                'statusCode': 403,
                'body': json.dumps("Access Denied: Invalid Token")
            }

        # 2. DECODE IMAGE DATA
        image_data = base64.b64decode(event['body'])

        # 3. GENERATE UNIQUE FILENAME & UPLOAD TO S3
        file_name = f"capture_{uuid.uuid4()}.jpg"

        s3_client.put_object(
            Bucket=BUCKET_NAME,
            Key=file_name,
            Body=image_data,
            ContentType='image/jpeg'
        )
        print(f"Image saved to S3: {file_name}")

        # --- FACE RECOGNITION CHECK ---
        print("Starting Face Check...")

        auth_list = s3_client.list_objects_v2(Bucket=AUTHORIZED_BUCKET_NAME)

        match_found = False
        matched_user = "Unknown"

        if 'Contents' in auth_list:
            for obj in auth_list['Contents']:
                auth_key = obj['Key']
                try:
                    target_obj = s3_client.get_object(Bucket=AUTHORIZED_BUCKET_NAME, Key=auth_key)
                    target_bytes = target_obj['Body'].read()

                    response = rekognition_client.compare_faces(
                        SourceImage={'Bytes': image_data},
                        TargetImage={'Bytes': target_bytes},
                        SimilarityThreshold=85
                    )

                    if response['FaceMatches']:
                        print(f"MATCH FOUND: {auth_key}")
                        match_found = True
                        matched_user = auth_key
                        break

                except Exception as loop_e:
                    print(f"Skipping {auth_key}: {str(loop_e)}")
                    continue
        else:
            print("Warning: Authorized bucket is empty.")

        # 4. RETURN VERDICT & NOTIFY IF INTRUDER
        print(f"DEBUG CHECK: SNS_TOPIC_ARN is '{SNS_TOPIC_ARN}'")
        if match_found:
            # AUTHORIZED
            return {
                'statusCode': 200,
                'body': json.dumps(f"Welcome {matched_user}")
            }
        else:
            # --- INTRUDER DETECTED! SEND ALERT ---
            print("Intruder detected. Sending SNS alert...")

            if SNS_TOPIC_ARN:
                try:
                    # Construct the message
                    message_body = (
                        f"⚠️ INTRUDER ALERT ⚠️\n\n"
                        f"An unauthorized person was detected at your door.\n"
                        f"Time: {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n"
                        f"Image ID: {file_name}\n"
                        f"Bucket: {BUCKET_NAME}\n\n"
                        f"Check the S3 console to view the image."
                    )

                    sns_client.publish(
                        TopicArn=SNS_TOPIC_ARN,
                        Message=message_body,
                        Subject="[Security] Intruder Detected at Door"
                    )
                    print("SNS Notification sent successfully.")
                except Exception as sns_error:
                    # Don't crash the door lock just because email failed
                    print(f"Failed to send SNS: {str(sns_error)}")
            else:
                print("SNS_TOPIC_ARN not set. Skipping notification.")

            # Still return 403 to ESP32 so it keeps the door locked
            return {
                'statusCode': 403,
                'body': json.dumps("Intruder Detected")
            }

    except Exception as e:
        print(f"Critical Error: {str(e)}")
        return {
            'statusCode': 500,
            'body': json.dumps(f"Server Error: {str(e)}")
        }
