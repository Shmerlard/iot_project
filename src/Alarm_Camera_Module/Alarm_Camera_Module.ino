#include "auth.h"
#include "Comms.h"
#include "Camera.h"
#include "AWS.h"

void setup() {
    // 1. Debug Serial
    Serial.begin(115200);
    Serial.println("\n--- Camera Unit Booting ---");

    initComms();  // Listener (Pins 32/33)
    initCamera(); // Hardware
    initAWS();    // WiFi + SSL Tunnel (Pre-warm)

    Serial.println("System Ready. Waiting for Command...");
}

void loop() {
    // 1. Check if Alarm Unit sent 'C'
    if (checkForTrigger()) {
        Serial.println("\n[Event] Trigger Received!");

        // 2. Capture Photo
        camera_fb_t* fb = capturePhoto();

        if (fb) {
            // 3. Upload & Verify
            bool authorized = checkFaceAuth(fb);

            // 4. Send Verdict back to Alarm Unit
            sendVerdict(authorized);

            // 5. Cleanup
            esp_camera_fb_return(fb);
        }
        else {
            Serial.println("[Error] Camera failed to capture");
            sendVerdict(false); // Default to Alarm if camera fails
        }
    }
}
