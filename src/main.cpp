#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "InstaLED";
const char* password = "InstaLED";

// create server instance
AsyncWebServer server(80);

#define NUM_LEDS 6
#define DATA_PIN 13
#define BRIGTHNESS 255

CRGB leds[NUM_LEDS];

// Array of effect names
const String effectNames[] = {
  "Rainbow",
  "K2000",
  "Wipe",
  "Fade",
  "Twinkle",
  "Comet"
};

int currentEffectIndex = -1;
CRGB currentColor = CRGB::Red;

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
    html += "<button onclick='sendColor()'>Choisir une couleur fixe</button>";
    html += "<br />";
    html += "<label for='effectSelect'>Choisir un effet:</label>";
    html += "<select id='effectSelect' onchange='sendEffect()'>"; // Combo box for selecting effects
    html += "<option value='0'>Rainbow</option>";
    html += "<option value='1'>K2000</option>";
    html += "<option value='2'>Wipe</option>";
    html += "<option value='3'>Fade</option>";
    html += "<option value='4'>Twinkle</option>";
    html += "<option value='5'>Comet</option>";
    html += "</select>";
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
    html += "function sendEffect() {\n";
    html += "  var effectSelect = document.getElementById('effectSelect');\n";
    html += "  fetch('/setEffect?effect=' + encodeURIComponent(effectSelect.selectedIndex))\n";
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

    currentColor = color;
    currentEffectIndex = -1;

    // Send a response to the client
    request->send(200, "text/plain", "Color received: " + color);
  });

  // Set up a handler for the HTTP GET request with URL parameters
  server.on("/setEffect", HTTP_GET, [](AsyncWebServerRequest *request){
    int effectIndex = request->getParam("effect")->value().toInt();
    Serial.print("Received effect: ");
    Serial.println(effectNames[effectIndex]);

    currentEffectIndex = effectIndex;

    // Send a response to the client
    request->send(200, "text/plain", "Effect received: " + effectNames[currentEffectIndex]);
  });

  // Start the server
  server.begin();

  // FastLED setup
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGTHNESS);
}

void rainbowEffect(int speed) {
  static uint8_t hue = 0; // Static variable to store current hue value

  // Increment the hue value and update LED colors
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue + i * 256 / NUM_LEDS, 255, 255);
  }
  FastLED.show();

  // Increment hue for next iteration based on speed parameter
  hue += speed;
  delay(50);
}

// Define the variables for the K2000 effect
int k2000Pos = 0;
int k2000Dir = 1;

// Function to implement the K2000 effect
void K2000Effect(CRGB color) {
  // Update the K2000 effect
  k2000Pos += k2000Dir;
  
  // Reverse direction if we hit the end of the strip
  if (k2000Pos == 0 || k2000Pos == NUM_LEDS - 1) {
    k2000Dir *= -1;
  }
  
  // Set all LEDs to black
  FastLED.clear();
  
  // Set the color of the moving LED
  leds[k2000Pos] = color;
  
  // Show the updated LED colors
  FastLED.show();
  
  // Delay to control the speed of the effect
  delay(100);
}

// Function to implement the "Color Wipe" effect
void ColorWipe(CRGB color) {
  // Loop through each LED in the strip
  for (int i = 0; i < NUM_LEDS; i++) {
    // Set the current LED to the desired color
    leds[i] = color;
    FastLED.show();  // Update the LED strip
    delay(100);  // Delay to control the speed of the effect
  }
  
  // Turn off all LEDs
  FastLED.clear();
  FastLED.show();
}

void fadeEffect(CRGB color1, CRGB color2, int duration) {
  // Fade between two colors with a specified duration
  for (int i = 0; i <= 255; i++) {
    CRGB color = blend(color1, color2, i);
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
    delay(duration / 255);
  }
}

void twinkleEffect(CRGB color) {
  // Randomly turn on and off LEDs with a specified color
  for (int i = 0; i < NUM_LEDS; i++) {
    if (random8() < 64) {
      leds[i] = color;
    } else {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
}

void cometEffect(CRGB color, int tailSize, int speed) {
  // Move a comet with a tail across the LED strip
  for (int i = 0; i < NUM_LEDS + tailSize; i++) {
    for (int j = 0; j < NUM_LEDS; j++) {
      if (j == i) {
        leds[j] = color;
      } else if (j > i && j <= i + tailSize) {
        leds[j] = color.fadeToBlackBy(128);
      } else {
        leds[j] = CRGB::Black;
      }
    }
    FastLED.show();
    delay(speed);
  }
}

void loop() {
  switch (currentEffectIndex)
  {
  case 0:
    rainbowEffect(12);
    break;
  case 1:
    K2000Effect(currentColor);
    break;
  case 2:
    ColorWipe(currentColor);
    break;
  case 3:
    fadeEffect(currentColor, CRGB::Black, 200);
    break;
  case 4:
    twinkleEffect(currentColor);
    delay(100);
    break;
  case 5:
    cometEffect(currentColor, 2, 100);
    break;
  default:
    fill_solid(leds, NUM_LEDS, currentColor);
    FastLED.show();
    break;
  }
}