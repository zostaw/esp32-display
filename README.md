# running TFT 128x160 display on ESP32 C3

## What is it

Setup for a Aliexpress-bought display and fake ESP32 C3.  
The display runs a primitive Markov Chain process with 16 states (grid 4x4).  

## Requirements

Board:
- esp32 by Espressif Systems - 2.0.13

Libraries:
- TFT_eSPI - 2.5.43
- Adafruit dependencies for TFT_eSPI - especially Adafruit ST7735 which this display is compatible with
- FastLED (optional) - 3.9.15

Additionally to installing these libs, one has to modify TFT_eSPI code.  
Modify *TFT_eSPI/User_Setup.h*.
That file is present in ~/Documents/Arduino/libraries/TFT_eSPI/User_Setup.h, but adding same to repo for backup.  

These are crucial:

```
#define ST7735_DRIVER
#define TFT_WIDTH  128
#define TFT_HEIGHT 160
#define ST7735_GREENTAB2
#define TFT_MOSI 10
#define TFT_SCLK 6
#define TFT_CS   21
#define TFT_DC   7
#define TFT_RST  9
```

These are for debugging purposes if you need to test signals on oscilloscope (you won't see much unless you have a good oscilloscope, but at least you can detect RST spike and SPI_SCLK and other signals on your 20 MHz scope).  

```
#define SPI_FREQUENCY   100000
#define SPI_READ_FREQUENCY  100000
#define SPI_TOUCH_FREQUENCY  2500000
```

