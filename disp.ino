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

//#define ESP32_C3_SUPERMINI_PLUS // uncomment this on Supermini Plus boards only

// -------------------------------------------------------------------------------
// Sketch and Board information
#ifdef ESP32_C3_SUPERMINI_PLUS
const char *PROGRAM_VERSION = "ESP32-C3 Supermini Plus ST7735 Starter V03";
const char *DEVICE_NAME = "ESP32-C3 Supermini + 1.8 inch 128 x 160 px";
#else
const char *PROGRAM_VERSION = "ESP32-C3 Supermini ST7735 Starter V03";
const char *DEVICE_NAME = "ESP32-C3 Supermini   1.8 inch 128 x 160 px";
#endif
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

#ifdef ESP32_C3_SUPERMINI_PLUS
// -------------------------------------------------------------------------------
// RGB LED
#include <FastLED.h>  // https://github.com/FastLED/FastLED
#define RGB_LED_PIN 0
#define RGB_NUM_LEDS 1
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define RGB_LED_BRIGHTNESS_PERCENT 70
CRGB leds[RGB_NUM_LEDS];
#define FRAMES_PER_SECOND 120  // rainbow effect
uint8_t gHue = 0;              // rotating "base color" used by many of the patterns
#else
// -------------------------------------------------------------------------------
// Onboard LED
#define ONBOARD_LED_PIN 3 // LOW = LED on, HIGH LED off
#endif

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

#ifdef ESP32_C3_SUPERMINI_PLUS
  // setup the RGB LED
  CFastLED::addLeds<LED_TYPE, RGB_LED_PIN, COLOR_ORDER>(leds, RGB_NUM_LEDS);
  FastLED.setBrightness(RGB_LED_BRIGHTNESS_PERCENT);
#else
  // setup the onboard LED
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  digitalWrite(ONBOARD_LED_PIN, LOW);  // LED on
#endif

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(TFT_TEXT_SIZE);
  tft.setRotation(0); // portrait

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
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawCentreString(F(" RED   LED "), TFT_WIDTH / 2, TFT_LED_TEXT_START, TFT_TEXT_SIZE);
#ifdef ESP32_C3_SUPERMINI_PLUS
  leds[0] = CRGB::Red;
  FastLED.delay(500);
#endif  
  delay(1000);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawCentreString(F(" GREEN LED "), TFT_WIDTH / 2, TFT_LED_TEXT_START, TFT_TEXT_SIZE);
#ifdef ESP32_C3_SUPERMINI_PLUS  
  leds[0] = CRGB::Green;
  FastLED.delay(500);
#endif 
  delay(1000);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.drawCentreString(F(" BLUE  LED "), TFT_WIDTH / 2, TFT_LED_TEXT_START, TFT_TEXT_SIZE);
#ifdef ESP32_C3_SUPERMINI_PLUS  
  leds[0] = CRGB::Blue;
  FastLED.delay(500);
#endif
  delay(1000);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString(F(" WHITE LED "), TFT_WIDTH / 2, TFT_LED_TEXT_START, TFT_TEXT_SIZE);
#ifdef ESP32_C3_SUPERMINI_PLUS  
  leds[0] = CRGB::White;
  FastLED.delay(500);
#endif
  delay(1000);
  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.drawCentreString(F("RAINBOW LD"), TFT_WIDTH / 2, TFT_LED_TEXT_START, TFT_TEXT_SIZE);
#ifdef ESP32_C3_SUPERMINI_PLUS  
  leds[0] = CRGB::White;
  fill_rainbow(&(leds[0]), RGB_NUM_LEDS, gHue);
#endif 

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(TFT_TEXT_SIZE);
  tft.drawCentreString(F("by AndroidCrypto"), TFT_WIDTH / 2, TFT_AUTHOR_TEXT_START, TFT_TEXT_SIZE);
}

void loop() {
#ifdef ESP32_C3_SUPERMINI_PLUS
  //Serial.println("In first branch");
  // Rainbow effect
  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) {
    gHue++;
  }  // slowly cycle the "base color" through the rainbow
  fill_rainbow(&(leds[0]), RGB_NUM_LEDS, gHue);
#else
  //Serial.println("In second branch");
  for(int i=0; i<10; i++) {
    analogWrite(TFT_BL_PIN, 255 * i / 10);
    delay(50);
  }
    tft.fillScreen(TFT_BLACK);
     tft.setCursor(60, 80);
  tft.println(DEVICE_NAME);

  for(int i=0; i<10; i++) {
    analogWrite(TFT_BL_PIN, 255 * (10-i) / 10);
    delay(50);
  }
  digitalWrite(ONBOARD_LED_PIN, HIGH); // LED off
  delay(1000);
#endif  
}

