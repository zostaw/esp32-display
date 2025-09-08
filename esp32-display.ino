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
#define TFT_BRIGHTNESS_PERCENT 30 // avoids overheating of the device
#define TFT_TEXT_SIZE 1
#define TFT_WIDTH 128
#define TFT_LED_TEXT_START 120
#define TFT_AUTHOR_TEXT_START 145

// -------------------------------------------------------------------------------
// Onboard LED
#define ONBOARD_LED_PIN 3 // LOW = LED on, HIGH LED off

uint16_t main_color = 0xF805;

// The number of states in the Markov chain.
const int NUM_STATES = 5;


// The transition probability matrix for the Markov chain.
// P(i, j) is the probability of moving from state `i` to state `j`.
// Corresponds to Racket's `markov-probability-matrix`.
const float markov_probability_matrix[NUM_STATES][NUM_STATES] = {
  {0.3231, 0.1078, 0.2390, 0.1901, 0.1400},
  {0.0023, 0.7456, 0.1201, 0.0820, 0.0500},
  {0.2000, 0.2500, 0.3500, 0.0999, 0.1001},
  {0.1111, 0.2222, 0.3333, 0.2223, 0.1111},
  {0.0501, 0.0502, 0.0503, 0.0504, 0.7990}
};

// The probabilities for determining the very first state.
// Corresponds to Racket's `initial-state-probabilities`.
const float initial_state_probabilities[NUM_STATES] = {0.2, 0.2, 0.2, 0.2, 0.2};

// Variable to hold the current state of the system (0 to NUM_STATES-1).
int current_state;


//=============================================================================
// == PROBABILITY HELPER FUNCTIONS
//=============================================================================

/**
 * @brief Corresponds to the Scheme function `cumulative-probs`.
 *
 * Takes an array of probabilities and calculates its cumulative distribution,
 * storing the result in the `cum_probs_out` array.
 * For example, {0.1, 0.2, 0.7} becomes {0.1, 0.3, 1.0}.
 *
 * @param probs_in Pointer to the input array of probabilities.
 * @param cum_probs_out Pointer to the output array for the cumulative probabilities.
 * @param size The number of elements in the arrays.
 */
void calculate_cumulative_probs(const float* probs_in, float* cum_probs_out, int size) {
  if (size <= 0) return;
  
  cum_probs_out[0] = probs_in[0];
  for (int i = 1; i < size; i++) {
    cum_probs_out[i] = cum_probs_out[i - 1] + probs_in[i];
  }
}

/**
 * @brief Corresponds to the Scheme function `random-value`.
 *
 * Performs a weighted random selection using inverse transform sampling.
 * It selects an index based on a pre-calculated cumulative probability distribution.
 *
 * @param cum_probs Pointer to an array of cumulative probabilities.
 * @param size The number of elements in the array.
 * @return The chosen index (state).
 */
int weighted_random_choice(const float* cum_probs, int size) {
  // Generate a random float between 0.0 and 1.0
  float r = (float)random(1001) / 1000.0;

  for (int i = 0; i < size; i++) {
    if (r < cum_probs[i]) {
      return i; // Return the index of the first bin the random number falls into.
    }
  }
  
  // Fallback in case of floating point rounding errors.
  return size - 1;
}

/**
 * @brief Corresponds to the Scheme function `next-state`.
 *
 * Determines the next state based on the current state and the transition matrix.
 *
 * @param state The current state of the system.
 * @return The calculated next state.
 */
int get_next_state(int state) {
  // Create a temporary array to hold the cumulative probabilities for the next transition.
  float cumulative_probabilities[NUM_STATES];
  
  // Calculate the CDF from the row of the matrix corresponding to the current state.
  calculate_cumulative_probs(markov_probability_matrix[state], cumulative_probabilities, NUM_STATES);
  
  // Make a weighted random choice to determine the next state.
  return weighted_random_choice(cumulative_probabilities, NUM_STATES);
}

/**
 * @brief Determines the initial state using its dedicated probability distribution.
 */
int get_initial_state() {
  float cumulative_probabilities[NUM_STATES];
  calculate_cumulative_probs(initial_state_probabilities, cumulative_probabilities, NUM_STATES);
  return weighted_random_choice(cumulative_probabilities, NUM_STATES);
}



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
  digitalWrite(ONBOARD_LED_PIN, HIGH);  // LED off


  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1); // landscape

  delay(200);

  current_state = get_initial_state();


}

void loop() {

  current_state = get_next_state(current_state);
  //delay(10);

  for(int diode_id=0; diode_id < 5; diode_id++){
    if(diode_id == current_state) {
      drawDiode(diode_id, main_color);
    } else {
      drawDiode(diode_id, 0x0000);
    }
  }
 
  delay(1);
}

void drawDiode(int id, uint16_t main_color) {
  uint16_t color;
  for(uint32_t radius=1; radius<17; radius++){
    if(main_color != 0x0000)
      color = adjustColor(main_color);

    switch(id){
      case 0:
        tft.drawSmoothCircle(TFT_HEIGHT/4, TFT_WIDTH/4, radius, color, color);
        break;
      case 1:
        tft.drawSmoothCircle(3*TFT_HEIGHT/4, TFT_WIDTH/4, radius, color, color);
        break;
      case 2:
        tft.drawSmoothCircle(TFT_HEIGHT/4, 3*TFT_WIDTH/4, radius, color, color);
        break;
      case 3:
        tft.drawSmoothCircle(3*TFT_HEIGHT/4, 3*TFT_WIDTH/4, radius, color, color);
        break;
      case 4:
        tft.drawSmoothCircle(TFT_HEIGHT/2, TFT_WIDTH/2, radius, color, color);
        break;
      default:
        break;
    }
  }
}

uint16_t adjustColor(uint16_t color) {
    // Extract channels
    int r = (color >> 11) & 0x1F;
    int g = (color >> 5)  & 0x3F;
    int b =  color        & 0x1F;

    // Random small adjustment (-2..+2 here)
    r = std::max(0, std::min(31, r + (rand() % 5 - 3)));
    g = std::max(0, std::min(63, g + (rand() % 5 - 3)));
    b = std::max(0, std::min(31, b + (rand() % 5 - 3)));

    // Pack back into RGB565
    return (r << 11) | (g << 5) | b;
}



