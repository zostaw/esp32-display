#include "raylib.h"
#include <algorithm>
#include <random>
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 128;
const int NUM_STATES = 16;

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

const float initial_state_probabilities[NUM_STATES] = {
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625,
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625
};

int current_state;
int previous_state;
Color main_color = {248, 8, 40, 255}; // RGB565 0xF805 converted to RGB
Color secondary_color = {148, 248, 140, 255}; // RGB565 0xF805 converted to RGB

void calculate_cumulative_probs(const float* probs_in, float* cum_probs_out, int size) {
    if (size <= 0) return;
    
    cum_probs_out[0] = probs_in[0];
    for (int i = 1; i < size; i++) {
        cum_probs_out[i] = cum_probs_out[i - 1] + probs_in[i];
    }
}

int weighted_random_choice(const float* cum_probs, int size) {
    float r = (float)GetRandomValue(0, 1000) / 1000.0f;

    for (int i = 0; i < size; i++) {
        if (r < cum_probs[i]) {
            return i;
        }
    }
    
    return size - 1;
}

int get_next_state(int state) {
    float cumulative_probabilities[NUM_STATES];
    calculate_cumulative_probs(markov_probability_matrix[state], cumulative_probabilities, NUM_STATES);
    return weighted_random_choice(cumulative_probabilities, NUM_STATES);
}

int get_initial_state() {
    float cumulative_probabilities[NUM_STATES];
    calculate_cumulative_probs(initial_state_probabilities, cumulative_probabilities, NUM_STATES);
    return weighted_random_choice(cumulative_probabilities, NUM_STATES);
}

int posX(int diode_id) {
    return ((diode_id % 4) + 1) * SCREEN_WIDTH / 5;
}

int posY(int diode_id) {
    return ((diode_id / 4) + 1) * SCREEN_HEIGHT / 5;
}

Color adjustColor(Color color) {
    int r = std::max(0, std::min(255, (int)color.r + GetRandomValue(-3, 2)));
    int g = std::max(0, std::min(255, (int)color.g + GetRandomValue(-3, 2)));
    int b = std::max(0, std::min(255, (int)color.b + GetRandomValue(-3, 2)));
    
    return {(unsigned char)r, (unsigned char)g, (unsigned char)b, color.a};
}

void drawDiode(int id, Color main_color, int max_radius) {
    for (int radius = 1; radius < max_radius; radius++) {
        Color color = adjustColor(main_color);
        DrawCircle(posX(id), posY(id), radius, color);
    }
}

int main() {
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Markov Display");
    SetTargetFPS(10);

    SetRandomSeed(time(nullptr));
    current_state = get_initial_state();

    Vector2 dragOffset = {0, 0};
    bool dragging = false;

    while (!WindowShouldClose()) {
        // Handle window dragging
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            dragging = true;
            dragOffset = GetMousePosition();
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            dragging = false;
        }

        if (dragging) {
            Vector2 mousePos = GetMousePosition();
            Vector2 windowPos = GetWindowPosition();
            SetWindowPosition(windowPos.x + mousePos.x - dragOffset.x,
                            windowPos.y + mousePos.y - dragOffset.y);
        }

        previous_state = current_state;
        current_state = get_next_state(current_state);

        // Check if mouse is over the window
        Vector2 mousePos = GetMousePosition();
        bool mouseOver = (mousePos.x >= 0 && mousePos.x < SCREEN_WIDTH &&
                         mousePos.y >= 0 && mousePos.y < SCREEN_HEIGHT);
        Color active_color = mouseOver ? secondary_color : main_color;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawLine(posX(current_state), posY(current_state),
                 posX(previous_state), posY(previous_state), active_color);

        drawDiode(previous_state, active_color, 5);
        drawDiode(current_state, active_color, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
