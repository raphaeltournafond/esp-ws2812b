#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "InstaLED";
const char* password = "InstaLED";

// create server instance
AsyncWebServer server(80);

// Variable to store the HTTP request
String header;

#define NUM_LEDS 6
#define DATA_PIN 13
#define BRIGTHNESS 255

CRGB leds[NUM_LEDS];

void setup() {
  // Webserver setup
  // Start serial communication
  Serial.begin(115200);

  // Set ESP32 as AP with the given SSID and password
  WiFi.softAP(ssid, password);

  // Print the AP IP address
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Set up a handler for the HTTP GET request with URL parameters
  server.on("/color", HTTP_GET, [](AsyncWebServerRequest *request){
    // Access the URL parameters using request->arg()
    String r = request->arg("r");
    String g = request->arg("g");
    String b = request->arg("b");
    String l = request->arg("l");

    // Process the data as needed
    Serial.println("r: " + r);
    Serial.println("g: " + g);
    Serial.println("b: " + b);
    Serial.println("l: " + l);

    FastLED.setBrightness(l.toInt());

    FastLED.clear();

    int red = r.toInt();
    int green = g.toInt();
    int blue = b.toInt();

    CRGB clientColor = CRGB(red, green, blue);
    fill_solid(leds, NUM_LEDS, clientColor);

    FastLED.show();

    // Send a response to the client
    String response = "r: " + r + "\ng: " + g + "\nb: " + b + "\nl: " + l;
    request->send(200, "text/plain", response);
  });

  // Start the server
  server.begin();

  // FastLED setup
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGTHNESS);
}

void loop() {
  // Rainbow cycle
  // for (int j = 0; j < 256; j++) {
  //   for (int i = 0; i < NUM_LEDS; i++) {
  //     leds[i] = CHSV((i * 256 / NUM_LEDS) + j, 255, 255);
  //   }
  //   FastLED.show();
  //   delay(5);
  // }
}