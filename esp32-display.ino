/*
TFT 128 x 160 pixels 1.8 inch ST7735 display wiring to an ESP32-C3 Supermini
Terminals on display's pcb from left to right

TFT   ESP32-C3
BLK   5 *1)
SCL   8 (= "SCK")
SDA   10 (= "MOSI")
DC    7
RST   9
CS    21
GND   GND
VDD   3.3V 

Note *1) If you don't need a dimming you can connect BLK with 3.3V
Note *2) The display does not have a MISO ("output") terminal, so it is not wired
*/

// -------------------------------------------------------------------------------
// Sketch and Board information
const char *PROGRAM_VERSION = "ESP32-C3 Supermini ST7735 Starter V03";
const char *DEVICE_NAME = "ESP32-C3 Supermini   1.8 inch 128 x 160 px";

const char *PROGRAM_VERSION_SHORT = "ST7735 Starter V03";
const char *DIVIDER = "---------------------";

// -------------------------------------------------------------------------------
// TFT Display
#include "SPI.h"
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();

#define TFT_BL_PIN 5 // backlight brightness control, needs to be a PWM pin
#define TFT_BRIGHTNESS_PERCENT 20 // avoids overheating of the device
#define TFT_TEXT_SIZE 1
#define TFT_WIDTH 128
#define TFT_LED_TEXT_START 120
#define TFT_AUTHOR_TEXT_START 145

// -------------------------------------------------------------------------------
// Onboard LED
#define ONBOARD_LED_PIN 3 // LOW = LED on, HIGH LED off


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(PROGRAM_VERSION);

  // init the display
  tft.begin();

  // set the brightness to 90% to avoid heating of the device
  pinMode(TFT_BL_PIN, OUTPUT);
  analogWrite(TFT_BL_PIN, 255 * TFT_BRIGHTNESS_PERCENT / 100);
  delay(10);

  // setup the onboard LED
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  digitalWrite(ONBOARD_LED_PIN, LOW);  // LED on


  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(TFT_TEXT_SIZE);
  tft.setRotation(1); // landscape

  tft.setTextSize(TFT_TEXT_SIZE);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(0, 0);
  tft.println(DEVICE_NAME);
  tft.setTextColor(TFT_RED);
  tft.println(PROGRAM_VERSION_SHORT);

  tft.setTextColor(TFT_WHITE);
  tft.println(DIVIDER);

  tft.setTextColor(TFT_BLUE);
  tft.print(F("Processor: "));
  tft.setTextColor(TFT_YELLOW);
  tft.println(ESP.getChipModel());

  tft.setTextColor(TFT_BLUE);
  tft.print(F("Revision:  "));
  tft.setTextColor(TFT_YELLOW);
  tft.println(ESP.getChipRevision());

  tft.setTextColor(TFT_BLUE);
  tft.print(F("Cores:     "));
  tft.setTextColor(TFT_YELLOW);
  tft.println(ESP.getChipCores());

  tft.setTextColor(TFT_BLUE);
  tft.print(F("CPU Freq:  "));
  tft.setTextColor(TFT_YELLOW);
  tft.print(ESP.getCpuFreqMHz());
  tft.println(F(" MHz"));

  tft.setTextColor(TFT_BLUE);
  tft.print(F("Flash:     "));
  tft.setTextColor(TFT_YELLOW);
  tft.print(ESP.getFlashChipSize() / (1024.0 * 1024));
  tft.println(F(" mb"));

  tft.setTextColor(TFT_BLUE);
  tft.print(F("PSRAM:     "));
  tft.setTextColor(TFT_YELLOW);
  tft.print(ESP.getPsramSize() / (1024.0 * 1024));
  tft.println(F(" mb"));

  tft.setTextColor(TFT_BLUE);
  tft.print(F("HEAP:      "));
  tft.setTextColor(TFT_YELLOW);
  tft.print(ESP.getHeapSize() / 1024.0);
  tft.println(F(" kb"));

  tft.setTextColor(TFT_BLUE);
  tft.print(F("SDK: "));
  tft.setTextColor(TFT_YELLOW);
  tft.println(ESP.getSdkVersion());

  tft.setTextColor(TFT_WHITE);
  tft.println(DIVIDER);

  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawCentreString(F(" GREEN LED "), TFT_WIDTH / 2, TFT_LED_TEXT_START, TFT_TEXT_SIZE);
  
  delay(200);
  tft.fillScreen(TFT_BLACK);
  delay(200);
}

void loop() {
  digitalWrite(ONBOARD_LED_PIN, LOW); // LED off


  //tft.fillScreen(TFT_BLACK);
  //tft.setCursor(16, 13);
  //tft.println("Mateusz Kowalkowski");
  //tft.setCursor(0, 0);

  
  static uint32_t radius = 2;
  static uint32_t index = 0;

  uint16_t fg_color = rainbow(index);
  uint16_t bg_color = TFT_BLACK;       // This is the background colour used for smoothing (anti-aliasing)

  uint16_t x = tft.width() / 2; // Position of centre of arc
  uint16_t y = tft.height() / 2;

  tft.drawSmoothCircle(x, y, radius, fg_color, bg_color);

  radius += 11;
  index += 5;
  index = index%192;


  if (radius > tft.height()/2) {
    delay(400);
    radius = 2;
  }


  digitalWrite(ONBOARD_LED_PIN, HIGH); // LED off
  delay(100);
}



unsigned int rainbow(byte value)
{
  // If 'value' is in the range 0-159 it is converted to a spectrum colour
  // from 0 = red through to 127 = blue to 159 = violet
  // Extending the range to 0-191 adds a further violet to red band
 
  value = value%192;
  
  byte red   = 0; // Red is the top 5 bits of a 16-bit colour value
  byte green = 0; // Green is the middle 6 bits, but only top 5 bits used here
  byte blue  = 0; // Blue is the bottom 5 bits

  byte sector = value >> 5;
  byte amplit = value & 0x1F;

  switch (sector)
  {
    case 0:
      red   = 0x1F;
      green = amplit; // Green ramps up
      blue  = 0;
      break;
    case 1:
      red   = 0x1F - amplit; // Red ramps down
      green = 0x1F;
      blue  = 0;
      break;
    case 2:
      red   = 0;
      green = 0x1F;
      blue  = amplit; // Blue ramps up
      break;
    case 3:
      red   = 0;
      green = 0x1F - amplit; // Green ramps down
      blue  = 0x1F;
      break;
    case 4:
      red   = amplit; // Red ramps up
      green = 0;
      blue  = 0x1F;
      break;
    case 5:
      red   = 0x1F;
      green = 0;
      blue  = 0x1F - amplit; // Blue ramps down
      break;
  }
  return red << 11 | green << 6 | blue;
}

