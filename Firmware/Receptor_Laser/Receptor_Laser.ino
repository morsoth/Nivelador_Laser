#include <math.h>

#include <Adafruit_MCP3008.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// BUZZER
#define BUZZ 3

// ADCs
#define CS1 10
#define CS2 9
#define CS3 8
#define CS4 7
#define CS5 6
#define CS6 5

Adafruit_MCP3008 adc1;
Adafruit_MCP3008 adc2;
Adafruit_MCP3008 adc3;
Adafruit_MCP3008 adc4;
Adafruit_MCP3008 adc5;
Adafruit_MCP3008 adc6;

// DISPLAY
#define SCREEN_HEIGHT 128
#define SCREEN_WIDTH 32

#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire, -1);

#define TOTAL_PHOTODIODES 48
int photodiode_values[TOTAL_PHOTODIODES] = {0};

void setup() {
  Serial.begin(115200);

  pinMode(BUZZ, OUTPUT);

  if (!adc1.begin(CS1) || !adc2.begin(CS2) || !adc3.begin(CS3) || !adc4.begin(CS4) || !adc5.begin(CS5) || !adc6.begin(CS6)) {
    Serial.println(F("Error: No se pudo inicializar el MCP3008."));
    while (1);
  }
  Serial.println(F("MCP3008 inicializado correctamente."));

  if (!display.begin(SCREEN_ADDRESS, 0x3C)) {
    Serial.println(F("Error: No se pudo inicializar la pantalla OLED."));
    while (1);
  }
  Serial.println(F("Pantalla OLED inicializada correctamente."));

  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
 
  display.clearDisplay();
  display.display();

  digitalWrite(BUZZ, HIGH);
  delay(200);
  digitalWrite(BUZZ, LOW);
}

void loop() {
  readADCs();

  calculateLaserPosition();

  //printPhotodiodeValues();

  delay(100);
}

void readADCs() {
  int ph_index = 0;

  Adafruit_MCP3008* adcs[6] = {&adc1, &adc2, &adc3, &adc4, &adc5, &adc6};

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 8; j++) {
      photodiode_values[ph_index++] = adcs[i]->readADC(j);
    }
  }
}

void calculateLaserPosition() {
  const int THRESHOLD = 600;

  int max_value = 0;
  int max_photodiode = -1;

  for (int ph = 0; ph < TOTAL_PHOTODIODES; ph++) {
    if (photodiode_values[ph] > THRESHOLD && photodiode_values[ph] > max_value) {
      max_value = photodiode_values[ph];
      max_photodiode = ph;
    }
  }
    
  display.clearDisplay();

  display.drawLine(0, 64, 16, 64, SSD1306_WHITE);

  if (max_photodiode != -1) {
    int top_photodiode, bot_photodiode;

    // Left column
    if (max_photodiode < TOTAL_PHOTODIODES / 2) {
      top_photodiode = TOTAL_PHOTODIODES - 1 - max_photodiode;

      if (max_photodiode != 0) bot_photodiode = top_photodiode + 1;
      else bot_photodiode = -1;
    }
    // Right column
    else {
      bot_photodiode = TOTAL_PHOTODIODES - 1 - max_photodiode;
      
      if (max_photodiode != 24) top_photodiode = bot_photodiode + 1;
      else top_photodiode = -1;
    }

    if (max_photodiode > 16 && max_photodiode < 32) {
      display.fillTriangle(16, 0, 0, 10, 31, 10, SSD1306_WHITE);
    }
    else if (max_photodiode < 5 || max_photodiode > 43) {
      display.fillTriangle(16, 127, 0, 117, 31, 117, SSD1306_WHITE);
    }
    else {
      display.setCursor(2,2);
      display.println(max_photodiode);

      int height;
      
      if (max_photodiode < TOTAL_PHOTODIODES / 2) height = -11*max_photodiode + 176;
      else height = 11*max_photodiode - 346;

      display.drawLine(17, height, 32, height, SSD1306_WHITE);
      
      if (height == 64) {
        digitalWrite(BUZZ, HIGH);
        delay(200);
        digitalWrite(BUZZ, LOW);
      }
    }
    // Adjust position

    // Map the photodiode with the display height
  }

  display.display();
}

void printPhotodiodeValues() {
  for (int ph = 0; ph < TOTAL_PHOTODIODES; ph++) {
    if (ph % 8 == 7) Serial.println((String)"PH" + ph + ": \t" + photodiode_values[ph]);
    else Serial.print((String)"PH" + ph + ": \t" + photodiode_values[ph] + ", \t");
  }
  Serial.println("------------------------------------------------------------------");
}

// 16 - 5
// 32 - 43
