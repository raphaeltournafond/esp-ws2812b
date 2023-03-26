#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 6
#define DATA_PIN 13

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
}