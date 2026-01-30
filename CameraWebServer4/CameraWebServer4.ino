#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>
// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// ===================
// AP CONFIG
// ===================
#define AP_NAME "ESP32CAM-SETUP"
#define AP_PASS "CamSetup123"   // >= 8 chars

// ===================
// Force portal button
// ===================
#define FORCE_PORTAL_PIN 13   // BOOT button

void startCameraServer();
void setupLedFlash(int pin);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // -------------------
  // Camera configuration
  // -------------------
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;

  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

  // -------------------
  // WiFi Manager
  // -------------------
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  pinMode(FORCE_PORTAL_PIN, INPUT_PULLUP);
  bool forcePortal = (digitalRead(FORCE_PORTAL_PIN) == LOW);

  WiFiManager wm;
  wm.setConfigPortalTimeout(180); // 3 minutes
  wm.setDebugOutput(true);

  bool res;
  if (forcePortal) {
    Serial.println("BOOT pressed → Starting config portal");
    res = wm.startConfigPortal(AP_NAME, AP_PASS);
  } else {
    res = wm.autoConnect(AP_NAME, AP_PASS);
  }

  if (!res) {
    Serial.println("WiFi failed → rebooting");
    delay(2000);
    ESP.restart();
  }

  

  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start mDNS AFTER WiFi connection
if (MDNS.begin("esp32cam")) {
  Serial.println("mDNS started");
  Serial.println("Access via: http://esp32cam.local");
} else {
  Serial.println("mDNS failed to start");
}

  // -------------------
  // Start camera server
  // -------------------
  startCameraServer();

  Serial.print("Camera Ready! Open http://");
  Serial.print(WiFi.localIP());
  Serial.println();
}

void loop() {
  delay(10000);
}
