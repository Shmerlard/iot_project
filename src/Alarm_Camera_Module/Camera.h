#ifndef CAMERA_H
#define CAMERA_H

#include "esp_camera.h"
#include "board_config.h"

void initCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG; 
    
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 12; 
    config.fb_count = 1;

    if(psramFound()){
        config.jpeg_quality = 10;
        config.fb_count = 2;
        config.grab_mode = CAMERA_GRAB_LATEST;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("[Camera] Init Failed! Error: 0x%x\n", err);
        return;
    }

    // --- NEW: FLIP THE IMAGE ---
    sensor_t * s = esp_camera_sensor_get();
    
    // 1. Vertical Flip (Fixes "Upside Down")
    s->set_vflip(s, 1);  
    
    // 2. Horizontal Mirror (Uncomment if left/right are swapped too)
    // s->set_hmirror(s, 1); 

    Serial.println("[Camera] Initialized (S3 Mode) with Flip");
}

camera_fb_t* capturePhoto() {
    camera_fb_t * fb = esp_camera_fb_get();
    if (fb) esp_camera_fb_return(fb); 
    fb = esp_camera_fb_get();
    return fb;
}

#endif