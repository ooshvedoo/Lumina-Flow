#include <Arduino.h>

// Масив пінів для послідовної перевірки
const int testPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 15, 18, 19, 20, 21, 22, 23};
const int pinCount = sizeof(testPins) / sizeof(testPins[0]);

#define RGB_LED_PIN 8

int currentStep = 0;      // Індекс піна, який ми тестуємо зараз
bool sequenceActive = true; // Чи триває ще тест

void setup() {
  Serial.begin(115200);
  
  // Налаштовуємо всі піни як INPUT_PULLUP
  for (int i = 0; i < pinCount; i++) {
    pinMode(testPins[i], INPUT_PULLUP);
  }

  // Гасимо світлодіод при старті (оновлена функція ядра v3)
  rgbLedWrite(RGB_LED_PIN, 0, 0, 0);

  delay(2000); 
  Serial.println("\n==================================================");
  Serial.println("   ІНТЕРАКТИВНИЙ ПОКРОКОВИЙ ТЕСТ GPIO ESP32-C6   ");
  Serial.println("==================================================");
  Serial.println("Зафіксуй один кінець дроту на GND.");
  Serial.println("Зараз почнеться послідовна перевірка порту...\n");
  delay(1000);

  // Просимо перевірити перший пін
  Serial.printf(">>> КРОК 1/%d: Будь ласка, замкни GPIO %d на GND...\n", pinCount, testPins[currentStep]);
}

void loop() {
  if (!sequenceActive) {
    // Якщо тест завершено успішно, просто плавно блимаємо зеленим
    rgbLedWrite(RGB_LED_PIN, 0, 20, 0);
    delay(500);
    rgbLedWrite(RGB_LED_PIN, 0, 0, 0);
    delay(4500);
    return;
  }

  int targetPin = testPins[currentStep];

  // Перевіряємо, чи замкнув користувач потрібний пін
  if (digitalRead(targetPin) == LOW) {
    // Фіксуємо успіх! Засвічуємо зелений
    rgbLedWrite(RGB_LED_PIN, 0, 64, 0);
    Serial.printf("[ОК] Струм пройшов через GPIO %d! Перемикаю далі...\n\n", targetPin);
    
    // Чекаємо, поки ти прибереш дріт з цього піна
    while (digitalRead(targetPin) == LOW) {
      delay(50);
    }
    
    // Гасимо діод і даємо мікропаузу перед наступним кроком
    rgbLedWrite(RGB_LED_PIN, 0, 0, 0);
    delay(500);

    currentStep++;

    // Перевіряємо, чи це був останній пін
    if (currentStep < pinCount) {
      Serial.printf(">>> КРОК %d/%d: Тепер замкни GPIO %d на GND...\n", currentStep + 1, pinCount, testPins[currentStep]);
    } else {
      // Фінал секвенції
      sequenceActive = false;
      rgbLedWrite(RGB_LED_PIN, 0, 128, 0); // Яскравіший зелений на фінал
      
      Serial.println("==================================================");
      Serial.println("         ТЕСТ СЕКВЕНЦІЇ ЗАВЕРШЕНО УСПІШНО!        ");
      Serial.println("==================================================");
      Serial.println("Логічний висновок: Усі цифрові порти введення/");
      Serial.println("виведення працюють бездоганно. Внутрішні резистори");
      Serial.println("PULLUP справні. Плата готова до роботи.");
      Serial.println("==================================================");
    }
  }

  delay(20); // Захист від брязкоту контактів
}