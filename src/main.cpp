#include <Arduino.h>
#include <FastLED.h>

#define RGB_LED_PIN 8
CRGB leds[1]; // Масив з одного світлодіода

void setup() {
  // Ініціалізація вбудованого діода ESP32-C6
  FastLED.addLeds<NEOPIXEL, RGB_LED_PIN>(leds, 1);
}

void loop() {
  static uint8_t hue = 0; // Змінна зберігає стан між ітераціями

  // CHSV(тон, насиченість, яскравість)
  // Насиченість 110 (пастель), яскравість 50 (щоб не сліпило)
  leds[0] = CHSV(hue, 110, 50); 
  
  FastLED.show();
  
  hue++; // Автоматичне переповнення (після 255 стане 0)
  delay(20);
}