#include <CapacitiveSensor.h>
#include <LiquidCrystal.h>

int capSensors[4];
int thresholds[4];

/*
 * Initialize capsensors on pins 7, 8, 9, 10 with 11 being the driving pin for all
 */
CapacitiveSensor s7 = CapacitiveSensor(11, 7);
CapacitiveSensor s8 = CapacitiveSensor(11, 8);
CapacitiveSensor s9 = CapacitiveSensor(11, 9);
CapacitiveSensor s10 = CapacitiveSensor(11, 10);

/*
 * Type (enum and struct) definitions for state, orientation,
 * (xy) coordinate, (xyo) coordinate, (upper, lower) bounds
 */
typedef enum {
  sDISP_COUNTDOWN = 1,
  sWAIT_AFTER_ROT = 2,
  sMOV = 3,
  sWAIT_AFTER_MOV = 4,
  sROT = 5,
  sWAIT_FOR_BUT = 6,
  sGAME_OVER = 7,
} state;

typedef enum {
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3,
} orientation;

typedef struct {
  byte x;
  byte y;
  orientation o;
} xyo;

typedef struct {
  int l;
  int u;
} lu;

typedef struct {
  byte x;
  byte y;
} xy;

/*
 * Variables to keep track of inputs
 */
orientation lastButtonPressed;
int numButtonsPressed;

/*
 * Pixel-by-pixel definitions of cursors with and without tails
 */
byte arrows[8][8] = {{ B00100, B01110, B11111, B00000, B00000, B00000, B00000, B00000 },
                     { B00000, B11111, B01110, B00100, B00000, B00000, B00000, B00000 },
                     { B00100, B01100, B11100, B01100, B00100, B00000, B00000, B00000 },
                     { B00100, B00110, B00111, B00110, B00100, B00000, B00000, B00000 },
                     { B00000, B00000, B00000, B00000, B00100, B01110, B11111, B00000 },
                     { B00000, B00000, B00000, B00000, B00000, B11111, B01110, B00100 },
                     { B00000, B00000, B00000, B00100, B01100, B11100, B01100, B00100 },
                     { B00000, B00000, B00000, B00100, B00110, B00111, B00110, B00100 }};

byte tailArrows[8][8] = {{ B00100, B01110, B11111, B00100, B00000, B00000, B00000, B00000 },
                          { B00100, B11111, B01110, B00100, B00000, B00000, B00000, B00000 },
                          { B00100, B01100, B11110, B01100, B00100, B00000, B00000, B00000 },
                          { B00100, B00110, B01111, B00110, B00100, B00000, B00000, B00000 },
                          { B00000, B00000, B00000, B00000, B00100, B01110, B11111, B00100 },
                          { B00000, B00000, B00000, B00000, B00100, B11111, B01110, B00100 },
                          { B00000, B00000, B00000, B00100, B01100, B11110, B01100, B00100 },
                          { B00000, B00000, B00000, B00100, B00110, B01111, B00110, B00100 }};
                          
// Keep track of which custom cursor is "cached" for which index
bool arrowIsTail[8];

/*
 * Setup functions
 */
void initializeSystem();
void calibrate();
void testCalibration();

/*
 * Read in gamepad inputs and update corresponding variables
 */
void updateInputs();

/*
 * Helper function definitions
 */
void resetButtons();
void displayLevel(int l, int c);
void displayCursor(byte x, byte y, orientation o, bool withTail, int lxb, int uxb);
xyo randomLocationOrientation(int lxb, int uxb);
orientation randomTurn(orientation o, int lxb, int uxb);
lu shrinkBounds(byte x, orientation o, int lxb, int uxb);
void displayGameOver(int l);

state updateFSM(state curState, long mils, int numButtons, int lastButton);
