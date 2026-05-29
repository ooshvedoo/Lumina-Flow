#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "secrets.h" 

#define RGB_LED_PIN 8
#define BTN_BRIGHTNESS 9
#define BTN_RANDOM 10

CRGB leds[1];
AsyncWebServer server(80);

// Параметри (пастельні: насиченість 100)
float currentHue = 120, targetHue = 120, currentVal = 60, targetVal = 60;
bool btnB_held = false, btnR_held = false;
String logBuffer = "System Ready";

void addLog(String msg) { 
  logBuffer = msg + " | " + logBuffer; 
  if(logBuffer.length() > 250) logBuffer = logBuffer.substring(0, 250); 
  Serial.println(msg); 
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
  <style>
    html, body { background: #121212; color: #eee; text-align: center; font-family: sans-serif; 
                 user-select: none; touch-action: none; overscroll-behavior: none; }
    #xy-pad { width: 90vw; height: 90vw; max-width: 300px; max-height: 300px; 
              background: linear-gradient(to right, red, yellow, lime, cyan, blue, magenta, red); 
              border-radius: 10px; margin: 20px auto; border: 2px solid #555; }
    .btn { width: 30%; padding: 20px; margin: 5px; background: #333; border: 1px solid #555; 
           color: white; border-radius: 8px; user-select: none; -webkit-tap-highlight-color: transparent; }
    #logs { width: 90vw; height: 60px; background: #000; margin: 10px auto; font-family: monospace; 
            font-size: 10px; text-align: left; overflow: hidden; padding: 5px; color: #0f0; border-radius: 5px; }
  </style>
</head>
<body>
  <h1>Lumina Flow</h1>
  <div id="xy-pad"></div>
  <button class="btn" onpointerdown="send('/action?btn=B&s=1')" onpointerup="send('/action?btn=B&s=0')">Bright</button>
  <button class="btn" onpointerdown="send('/action?btn=R&s=1')" onpointerup="send('/action?btn=R&s=0')">Color</button>
  <button class="btn" onpointerdown="send('/action?btn=C&s=1')" onpointerup="send('/action?btn=C&s=0')">Combo</button>
  <div id="logs"></div>
  <script>
    function send(url) { fetch(url); }
    const pad = document.getElementById('xy-pad');
    pad.addEventListener('pointermove', (e) => {
      if(e.buttons > 0) {
        let rect = pad.getBoundingClientRect();
        let x = Math.floor(((e.clientX - rect.left) / rect.width) * 255);
        let y = Math.floor(255 - ((e.clientY - rect.top) / rect.height) * 255);
        send('/action?h=' + x + '&v=' + y);
      }
    });
    setInterval(() => {
      fetch('/status').then(r=>r.json()).then(d => {
        document.getElementById('logs').innerText = d.logs;
      });
    }, 200);
  </script>
</body>
</html>)rawliteral";

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  targetHue = random(0, 255);
  FastLED.addLeds<NEOPIXEL, RGB_LED_PIN>(leds, 1);
  pinMode(BTN_BRIGHTNESS, INPUT_PULLUP); pinMode(BTN_RANDOM, INPUT_PULLUP);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) delay(500);
  if (WiFi.status() != WL_CONNECTED) WiFi.softAP("Lumina-Flow", "Lumina-Flow");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){ req->send_P(200, "text/html", index_html); });
  server.on("/action", HTTP_GET, [](AsyncWebServerRequest *req){
    if(req->hasParam("h")) { 
        targetHue = req->getParam("h")->value().toInt(); 
        targetVal = req->getParam("v")->value().toInt(); 
    } else {
      String btn = req->getParam("btn")->value(); 
      bool state = req->getParam("s")->value() == "1";
      if(btn == "B") btnB_held = state; if(btn == "R") btnR_held = state;
      if(btn == "C") { btnB_held = state; btnR_held = state; }
      if(!state) { targetVal += targetVal * random(-20, 20) / 100.0; } // Дрейф при відпусканні
      addLog("Action: " + btn + (state ? " Hold" : " Release"));
    }
    req->send(200, "text/plain", "OK");
  });
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", "{\"logs\":\"" + logBuffer + "\"}");
  });
  server.begin();
  addLog("System Started");
}

void loop() {
  if (btnB_held || digitalRead(BTN_BRIGHTNESS) == LOW) targetVal = constrain(targetVal + 2, 40, 255);
  if (btnR_held || digitalRead(BTN_RANDOM) == LOW) targetHue = (int)(targetHue + 2) % 255;

  currentVal += (targetVal - currentVal) * 0.1;
  currentHue += (targetHue - currentHue) * 0.1;
  leds[0] = CHSV((uint8_t)currentHue, 100, (uint8_t)currentVal);
  FastLED.show();
  delay(15);
}