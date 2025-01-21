#include <cmath>

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

Adafruit_SSD1306 display(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire, -1);

#define TOTAL_PHOTODIODES 48
int photodiode_values[TOTAL_PHOTODIODES] = {0};

void setup() {
  Serial.begin(115200);

  if (!adc1.begin(CS1) || !adc2.begin(CS2) || !adc3.begin(CS3) || !adc4.begin(CS4) || !adc5.begin(CS5) || !adc6.begin(CS6)) {
    Serial.println(F("Error: No se pudo inicializar el MCP3008."));
    while (1);
  }
  Serial.println(F("MCP3008 inicializado correctamente."));

  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("Error: No se pudo inicializar la pantalla OLED."));
    while (1);
  }
  Serial.println(F("Pantalla OLED inicializada correctamente."));

  display.clearDisplay();
  display.setRotation(3);
}

void loop() {
  readADCs();

  calculateLaserPosition();

  printPhotodiodeValues();

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

  int maxValue = 0;
  int maxPhotodiode = -1;

  for (int ph = 0; ph < TOTAL_PHOTODIODES; ph++) {
    if (photodiode_values[i] > THRESHOLD && photodiode_values[i] > max_value) {
      max_value = photodiode_values[i];
      max_index = i;
    }
  }

  if (maxPhotodiode != -1) {
    int topPhotodiode, botPhotodiode;

    // Left column
    if (maxPhotodiode < TOTAL_PHOTODIODES / 2) {
      topPhotodiode = TOTAL_PHOTODIODES - 1 - maxPhotodiode;
      botPhotodiode = topPhotodiode + 1;
    }
    // Right column
    else {
      topPhotodiode = TOTAL_PHOTODIODES + 1 - maxPhotodiode;
      botPhotodiode = topPhotodiode - 1;
    }

    // Adjust psotiion

    // Map the photodiode with the display height
  }
}

void printPhotodiodeValues() {
  for (int ph = 0; ph < TOTAL_PHOTODIODES; ph++) {
    if (ph % 8 == 7) Serial.println((String)"PH" + ph + ": \t" + photodiode_values[ph]);
    else Serial.print((String)"PH" + ph + ": \t" + photodiode_values[ph] + ", \t");
  }
}