#include <Arduino.h>

#define RGB_LED_PIN 8

float hue = 0.0;

// Власна функція конвертації HSV в RGB
void hsvToRgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b) {
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;
    float r1, g1, b1;

    if (h < 60) { r1 = c; g1 = x; b1 = 0; }
    else if (h < 120) { r1 = x; g1 = c; b1 = 0; }
    else if (h < 180) { r1 = 0; g1 = c; b1 = x; }
    else if (h < 240) { r1 = 0; g1 = x; b1 = c; }
    else if (h < 300) { r1 = x; g1 = 0; b1 = c; }
    else { r1 = c; g1 = 0; b1 = x; }

    *r = (uint8_t)((r1 + m) * 255);
    *g = (uint8_t)((g1 + m) * 255);
    *b = (uint8_t)((b1 + m) * 255);
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  uint8_t r, g, b;
  
  // Пастельні налаштування: низька насиченість (0.4)
  hsvToRgb(hue, 0.43, 0.20, &r, &g, &b);

  rgbLedWrite(RGB_LED_PIN, r, g, b);

  hue += 0.5;
  if (hue >= 360.0) hue = 0.0;

  delay(20);
}