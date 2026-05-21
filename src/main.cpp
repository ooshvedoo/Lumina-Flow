#include <Arduino.h>
#include <FastLED.h>

#define RGB_LED_PIN 8
#define BTN_BRIGHTNESS 9
#define BTN_RANDOM 10

CRGB leds[1];
CRGBPalette16 currentPalette = PartyColors_p;

// Налаштування плавності
float currentHue = 0;
float currentVal = 20;
float targetVal = 20;

// Логіка кнопок
unsigned long btnPressTime = 0;
bool isLongPress = false;

// Генеративні змінні
uint8_t paletteType = 0; // 0-3 типи гармоній

void nextPalette() {
    paletteType = (paletteType + 1) % 4;
    switch(paletteType) {
        case 0: currentPalette = OceanColors_p; break;
        case 1: currentPalette = ForestColors_p; break;
        case 2: currentPalette = PartyColors_p; break;
        case 3: currentPalette = HeatColors_p; break;
    }
}

void setup() {
  FastLED.addLeds<NEOPIXEL, RGB_LED_PIN>(leds, 1);
  pinMode(BTN_BRIGHTNESS, INPUT_PULLUP);
  pinMode(BTN_RANDOM, INPUT_PULLUP);
  randomSeed(analogRead(0));
}

void loop() {
  // --- Обробка кнопок з логікою часу ---
  // Кнопка 1: Короткий клік - перемикач яскравості, Довгий - темп анімації
  if (digitalRead(BTN_BRIGHTNESS) == LOW) {
    if (!btnPressTime) btnPressTime = millis();
  } else if (btnPressTime > 0) {
    if (millis() - btnPressTime > 1000) { /* Long Press Action */ } 
    else { targetVal = (targetVal >= 255) ? 20 : targetVal + 50; }
    btnPressTime = 0;
  }

  // Кнопка 2: Короткий клік - зміна палітри, Довгий - "Генеративний хаос"
  if (digitalRead(BTN_RANDOM) == LOW) {
     nextPalette();
     delay(300);
  }

  // --- Генеративна математика ---
  // Плавне перетікання яскравості (LERP)
  currentVal += (targetVal - currentVal) * 0.05; 
  
  // Плавний зсув кольору за теорією палітри
  static float colorShift = 0;
  colorShift += 0.2 + (sin8(millis()/100) / 255.0); // Ритмічне прискорення
  
  // Рендер кольору
  leds[0] = ColorFromPalette(currentPalette, (uint8_t)colorShift, (uint8_t)currentVal, LINEARBLEND);
  
  FastLED.show();
  delay(15);
}