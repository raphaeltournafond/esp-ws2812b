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

  // Serve the web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body>";
    html += "<h1>Swee.brz le gros bg</h1>";
    html += "<input type='color' id='colorPicker' value='#FF0000'>";
    html += "<button onclick='sendColor()'>Send Color</button>";
    html += "<script>";
    html += "function sendColor() {\n";
    html += "  var color = document.getElementById('colorPicker').value;\n";
    html += "  fetch('/setColor?color=' + encodeURIComponent(color))\n";
    html += "    .then(response => {\n";
    html += "      if (response.ok) {\n";
    html += "        return response.text();\n";
    html += "      } else {\n";
    html += "        throw new Error('Error: ' + response.status);\n";
    html += "      }\n";
    html += "    })\n";
    html += "}\n";
    html += "</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Set up a handler for the HTTP GET request with URL parameters
  server.on("/setColor", HTTP_GET, [](AsyncWebServerRequest *request){
    String hexColor = request->getParam("color")->value();
    Serial.print("Received color: ");
    Serial.println(hexColor);

    FastLED.clear();

    CRGB color;
    sscanf(hexColor.c_str(), "#%02X%02X%02X", &color.r, &color.g, &color.b);
    Serial.print("RGB color: ");
    Serial.println(color);
    fill_solid(leds, NUM_LEDS, color);

    FastLED.show();

    // Send a response to the client
    request->send(200, "text/plain", "Color received: " + color);
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