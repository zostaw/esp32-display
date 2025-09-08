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
// TFT Display
#include "SPI.h"
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);  // Declare Sprite object "spr" with pointer to "tft" object

#define TFT_BL_PIN 5 // backlight brightness control, needs to be a PWM pin
#define TFT_BRIGHTNESS_PERCENT 30 // avoids overheating of the device
#define TFT_TEXT_SIZE 1
#define TFT_WIDTH 128
#define SCREEN_WIDTH 160 // inverted, because we use landscape
#define SCREEN_HEIGHT 128 // inverted, because we use landscape
#define TFT_LED_TEXT_START 120
#define TFT_AUTHOR_TEXT_START 145


// -------------------------------------------------------------------------------
// Onboard LED
#define ONBOARD_LED_PIN 3 // LOW = LED on, HIGH LED off

uint16_t main_color = 0xF805;

// The number of states in the Markov chain.
const int NUM_STATES = 16;


// The transition probability matrix for the Markov chain.
// P(i, j) is the probability of moving from state `i` to state `j`.
// Corresponds to Racket's `markov-probability-matrix`.
const float markov_probability_matrix[NUM_STATES][NUM_STATES] = {
  {0.1127, 0.0204, 0.0274, 0.0421, 0.0745, 0.0539, 0.0539, 0.1007, 0.0053, 0.0713, 0.1124, 0.0948, 0.0011, 0.1132, 0.1039, 0.0124},
  {0.0345, 0.0162, 0.0314, 0.0664, 0.1089, 0.1232, 0.0535, 0.0283, 0.0847, 0.0440, 0.0746, 0.1036, 0.1098, 0.0642, 0.0143, 0.0424},
  {0.1054, 0.0151, 0.0247, 0.0842, 0.0210, 0.0636, 0.0999, 0.0737, 0.0346, 0.1287, 0.1009, 0.0284, 0.0503, 0.0795, 0.0292, 0.0608},
  {0.0973, 0.0783, 0.0800, 0.0282, 0.0488, 0.0382, 0.0601, 0.0104, 0.0685, 0.0891, 0.0864, 0.0946, 0.0385, 0.0580, 0.0653, 0.0583},
  {0.1067, 0.0259, 0.0931, 0.1049, 0.0519, 0.0651, 0.0512, 0.0927, 0.0608, 0.0264, 0.0611, 0.1057, 0.1043, 0.0196, 0.0079, 0.0227},
  {0.0194, 0.1222, 0.0670, 0.1040, 0.0335, 0.1212, 0.0685, 0.0674, 0.0565, 0.0270, 0.0067, 0.0431, 0.1030, 0.0294, 0.0605, 0.0706},
  {0.0282, 0.0303, 0.0157, 0.0626, 0.0752, 0.0874, 0.0595, 0.0672, 0.1004, 0.0707, 0.0958, 0.1012, 0.0410, 0.0870, 0.0685, 0.0093},
  {0.0211, 0.0085, 0.0849, 0.0140, 0.0548, 0.0841, 0.1054, 0.0247, 0.0670, 0.1084, 0.0257, 0.1005, 0.0801, 0.0884, 0.0999, 0.0325},
  {0.0318, 0.0945, 0.0881, 0.0415, 0.1049, 0.0280, 0.0278, 0.1100, 0.0424, 0.0849, 0.0135, 0.0680, 0.0260, 0.0968, 0.0517, 0.0901},
  {0.0565, 0.1046, 0.0727, 0.1021, 0.0078, 0.0462, 0.0357, 0.0920, 0.0868, 0.0473, 0.0675, 0.1042, 0.0206, 0.0794, 0.0696, 0.0070},
  {0.1004, 0.1107, 0.0436, 0.1154, 0.0169, 0.0433, 0.0776, 0.0528, 0.0428, 0.0790, 0.0306, 0.1363, 0.0132, 0.1133, 0.0075, 0.0166},
  {0.0291, 0.1536, 0.0678, 0.0054, 0.0067, 0.0307, 0.0894, 0.0452, 0.0117, 0.0514, 0.0531, 0.1251, 0.1043, 0.0740, 0.0226, 0.1299},
  {0.0543, 0.0330, 0.0644, 0.0279, 0.1010, 0.1110, 0.0682, 0.0175, 0.0267, 0.1112, 0.1036, 0.0515, 0.0140, 0.0965, 0.0922, 0.0270},
  {0.1474, 0.0211, 0.0082, 0.0048, 0.1115, 0.0302, 0.1105, 0.0755, 0.0598, 0.0387, 0.1002, 0.0268, 0.0955, 0.0193, 0.0217, 0.1288},
  {0.1157, 0.0047, 0.0648, 0.1104, 0.1164, 0.0291, 0.1021, 0.0909, 0.0444, 0.0101, 0.1222, 0.0665, 0.0205, 0.0235, 0.0242, 0.0545},
  {0.1040, 0.0415, 0.0123, 0.0681, 0.0834, 0.0775, 0.1278, 0.0103, 0.0729, 0.0401, 0.0045, 0.0707, 0.0763, 0.1131, 0.0171, 0.0804}
};

// The probabilities for determining the very first state.
// Corresponds to Racket's `initial-state-probabilities`.
const float initial_state_probabilities[NUM_STATES] = {0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625};

// Variable to hold the current state of the system (0 to NUM_STATES-1).
int current_state;
int previous_state;


void setup() {
  // init the display
  tft.begin();

  spr.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);

  // set the brightness to 90% to avoid heating of the device
  pinMode(TFT_BL_PIN, OUTPUT);
  analogWrite(TFT_BL_PIN, 255 * TFT_BRIGHTNESS_PERCENT / 100);
  delay(10);

  // setup the onboard LED
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  digitalWrite(ONBOARD_LED_PIN, HIGH);  // LED off

  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1); // landscape
  delay(100);

  current_state = get_initial_state();
}

void loop() {
  previous_state = current_state;
  current_state = get_next_state(current_state);

  // the directions are skewed I know
  spr.fillSprite(TFT_BLACK);

  spr.drawLine(posX(current_state), posY(current_state), posX(previous_state) , posY(previous_state), main_color);
  spr.pushSprite(-2, 0);
  drawDiode(previous_state, main_color, 5);
  drawDiode(current_state, main_color, 10);
  delay(100);

}

void drawDiode(int id, uint16_t main_color, int max_radius) {
  uint16_t color;
  for(uint32_t radius=1; radius<max_radius; radius++){
    if(main_color != 0x0000)
      color = adjustColor(main_color);
    tft.drawSmoothCircle(posX(id), posY(id), radius, color, color);
  }
}

int posX(int diode_id) {
  return ((diode_id%4) + 1) * SCREEN_WIDTH  / 5;
}

int posY(int diode_id) {
  return ((diode_id/4) + 1) * SCREEN_HEIGHT / 5;
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



