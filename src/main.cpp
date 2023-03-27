#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 6
#define DATA_PIN 13
#define BRIGTHNESS 255

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGTHNESS);
}

void loop() {
    // Rainbow cycle
  for (int j = 0; j < 256; j++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV((i * 256 / NUM_LEDS) + j, 255, 255);
    }
    FastLED.show();
    delay(20);
  }
}