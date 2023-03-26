#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 6
#define DATA_PIN 13

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
  // put your main code here, to run repeatedly:
}