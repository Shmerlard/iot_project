#ifndef AWS_H
#define AWS_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "auth.h"

// Global objects
WiFiClientSecure *client = new WiFiClientSecure;
HTTPClient http;

void setupWiFi() {
    if (WiFi.status() == WL_CONNECTED) return;

    Serial.print("[WiFi] Connecting to ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[WiFi] Connected!");
}

void connectToSSL() {
    // Check if we need to reconnect
    if (client->connected()) return;

    client->setInsecure(); // Skip certificate check
    client->setTimeout(20); 

    // Parse Host from URL
    String host = String(LAMBDA_URL);
    host.replace("https://", "");
    int pathIndex = host.indexOf("/");
    if (pathIndex != -1) host = host.substring(0, pathIndex);

    Serial.print("[AWS] Handshaking... ");
    if (client->connect(host.c_str(), 443)) {
        Serial.println("Ready.");
    } else {
        Serial.println("Failed.");
    }
}

void initAWS() {
    setupWiFi();
    connectToSSL(); // Pre-warm the connection
}

// Returns: TRUE if Authorized (HTTP 200), FALSE if Intruder/Error
bool checkFaceAuth(camera_fb_t * fb) {
    // Safety Reconnects
    if (WiFi.status() != WL_CONNECTED) setupWiFi();
    if (!client->connected()) connectToSSL();

    Serial.print("[AWS] Uploading... ");

    if (http.begin(*client, LAMBDA_URL)) {
        http.addHeader("Content-Type", "image/jpeg");
        http.addHeader("x-auth-token", LAMBDA_ACCESS_TOKEN); 
        http.setReuse(true); // Keep tunnel open

        unsigned long start = millis();
        int httpCode = http.POST(fb->buf, fb->len);
        unsigned long duration = millis() - start;

        Serial.printf("Done (%lums). Code: %d\n", duration, httpCode);

        // HTTP 200 means Lambda recognized the face
        if (httpCode == 200) {
            return true; 
        }
    } else {
        Serial.println("[AWS] Connection Failed");
        client->stop();
    }

    return false; // Default to unauthorized
}

#endif
