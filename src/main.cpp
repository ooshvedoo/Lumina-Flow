#include <Arduino.h>
#include <FastLED.h>

#define RGB_LED_PIN 8
#define BTN_BRIGHTNESS 9
#define BTN_RANDOM 10

CRGB leds[1];

// Параметри для "заходу"
float currentHue = 0;       // Поточний колір на колі
float targetHue = 0;        // Куди ми рухаємось
float currentVal = 20;      // Поточна яскравість
float targetVal = 20;       // Цільова яскравість
float hueSpeed = 0.002;     // Темп зміни кольору (дуже повільний)

int brightnessLevels[] = {20, 100, 255}; 
int bMode = 0;

void logStatus(String msg) {
  Serial.print("[INFO] " + msg);
  Serial.print(" | H: "); Serial.print(currentHue);
  Serial.print(" | V: "); Serial.println(currentVal);
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, RGB_LED_PIN>(leds, 1);
  pinMode(BTN_BRIGHTNESS, INPUT_PULLUP);
  pinMode(BTN_RANDOM, INPUT_PULLUP);
}

void loop() {
  bool b1 = (digitalRead(BTN_BRIGHTNESS) == LOW);
  bool b2 = (digitalRead(BTN_RANDOM) == LOW);

  // 1. Обробка кнопок
  if (b1 && b2) {
    hueSpeed = 0.05; // "Швидкий" режим при комбо
    logStatus("COMBO: Speed Up");
    delay(300);
  } else if (b1) {
    bMode = (bMode + 1) % 3;
    targetVal = brightnessLevels[bMode];
    logStatus("Brightness Changed");
    delay(300);
  } else if (b2) {
    targetHue = random(0, 255); // Стрибок в іншу частину палітри
    logStatus("New Hue Target Set");
    delay(300);
  }

  // 2. Інерційна математика (Natural Motion)
  // Плавне наближення яскравості
  currentVal += (targetVal - currentVal) * 0.01; 
  
  // Плавне наближення кольору
  // Якщо різниця велика, рухаємось трохи швидше
  float hueDiff = targetHue - currentHue;
  currentHue += hueDiff * hueSpeed; 

  // 3. Рендер
  // Насиченість 200 (пастельна, але жива)
  leds[0] = CHSV((uint8_t)currentHue, 200, (uint8_t)currentVal);
  FastLED.show();
  
  delay(15);
}