# IoT Biometric Security System
A dual-core ESP32 home security system featuring facial recognition, cloud integration, and real-time AWS alerts.

## Overview
This project is a DIY security system that bridges hardware sensors with cloud AI. It uses two separate ESP32 microcontrollers communicating via UART to separate low-power sensor monitoring from high-power image processing.

When a threat is detected (Motion or Door Open), the system captures an image, uploads it to AWS, and uses Amazon Rekognition to determine if the person is an authorized user or an intruder.

```bash
.
├── images/                  # Test dataset for facial recognition
│   ├── authorized/          # Known faces
│   └── unauthorized/        # Intruder samples
├── src/
│   ├── Alarm_Peripherals/   # The "Brain" (ESP32 DevKit V1)
│   │   ├── FSM.h            # Finite State Machine logic
│   │   ├── Peripherals.h    # Hardware definitions (Sensors, LCD)
│   │   ├── Comms.h          # UART Communication logic
│   │   └── Power.h          # Light Sleep & Power management
│   ├── Alarm_Camera_Module/ # The "Eye" (ESP32-S3 Camera)
│   │   ├── AWS.h            # Lambda upload & WiFi logic
│   │   ├── Camera.h         # Sensor config & Image Flip
│   │   └── auth.h           # Secrets (Excluded from Git)
│   └── AWS/                 # Cloud Backend
│       └── lambda_function.py # Python script for Rekognition & SNS
├── videos/                  # Demo recordings of the system in action
└── README.md
```
## Demos & Media

System Arming & Disarming: [example](./videos/output.mp4)

Intruder Detection (Instant Alert): [example](./videos/output2.mp4)

Authorized Face Unlock: [example](./videos/output3.mp4)

## Hardware Architecture
1. The Peripheral Unit (The "Brain")
Device: ESP32 DevKit V1

Role: Manages system state, user input, and sensors.

Pinout:

```
GPIO 4: Door Sensor (Magnetic Reed Switch)
GPIO 2: Motion Sensor (PIR Simulator)
GPIO 23: Siren / Alarm LED
GPIO 18/5: UART (RX/TX) to Camera
I2C (21/22): 16x2 LCD Display
```

2. The Vision Unit (The "Eye")
Device: ESP32-S3 Camera

Role: High-res image capture and secure HTTPS upload.

Pinout:

```
UART: Connected to Peripheral Unit
Camera Pins: Standard S3 Camera Config
```

3. Cloud Backend (AWS)
S3 Buckets: Stores captured images (incoming) and authorized faces (database).

Lambda (Python): Handles uploads, calls Rekognition, and manages logic.

Amazon Rekognition: Performs facial comparison (95% confidence threshold).

Amazon SNS: Sends email/SMS alerts for intruders.

<img src="./images/sns_example.jpg" width="400" alt="SNS Example">

## System Logic (Finite State Machine)
The system operates on a robust FSM with the following states:

IDLE: System disarmed. Low power mode (Light Sleep) enabled after 20s inactivity.

EXIT DELAY: 10-second countdown to leave the house.

ARMED: Monitoring sensors.

Motion (Pin 2): INSTANT ALARM. Triggers siren + Camera Capture.

Door (Pin 4): ENTRY DELAY. Starts 15s countdown for password.

ALARM: Siren active. Owner notified.

ENTRY DELAY: Waiting for valid Password OR Face ID.

Communication Protocol (UART)
The two units speak a simple custom protocol at 9600 baud:

'C' (Check): Alarm Unit tells Camera to snap a photo.

'K' (OK/Known): Camera tells Alarm Unit the face is authorized.

'A' (Alert/Unknown): Camera tells Alarm Unit an intruder was detected.

## Getting Started
Prerequisites  
Hardware: 2x ESP32s, Keypad, LCD, Jumpers.

Software: Arduino IDE, AWS Account.

Libraries: LiquidCrystal_I2C, Keypad, WiFiClientSecure.

### AWS Setup
Create two S3 buckets: one for uploads, one for known faces.

Create an SNS Topic for email alerts.

Deploy the Python script in /AWS to a function with a Function URL.

Add the function URL to auth.h in the Camera code.
create the following auth.h file
```
#define WIFI_SSID "YOUR SSID"
#define WIFI_PASS "YOUR_PASSWORD"

#define LAMBDA_URL "https://your_lambda_url.lambda-url.eu-north-1.on.aws/"
#define LAMBDA_ACCESS_TOKEN "your_lambda_access_token"
```

Flashing
Peripheral: Upload /Peripheral_Unit code to the standard ESP32.

Camera: Select "ESP32S3 Dev Module" and upload /Camera_Unit.

Wiring: Connect TX of one unit to RX of the other (Common Ground required!).
