#include <CapacitiveSensor.h>

/*
 * Initialize capsensors on pins 6, 7, 8, 9 with 10 being the driving pin for all
 */
CapacitiveSensor s6 = CapacitiveSensor(10, 6);
CapacitiveSensor s7 = CapacitiveSensor(10, 7);
CapacitiveSensor s8 = CapacitiveSensor(10, 8);
CapacitiveSensor s9 = CapacitiveSensor(10, 9);

int capSensors[4];
int thresholds[4];

typedef enum {
  UP = 0,
  RIGHT = 1,
  DOWN = 2,
  LEFT = 3,
  NONE = 4,
} orientation;

typedef struct {
  byte x : 4;
  byte y : 4;
} xy;

typedef struct {
  byte x : 4;
  byte y : 4;
  orientation o;
} xyo;

typedef enum {
  s_INIT = 0,
  s_REG_CALC = 1,
  s_REG_WAIT = 2,
  s_AVOID_WAIT = 3,
  s_AVOID_CALC = 4,
  s_GAME_OVER = 5,
  s_WIN = 6,
} fsm_state;

typedef struct {
  xyo head;
  xy fruit;
  unsigned long step;
  byte snakeLength;
  unsigned long savedClock;
  byte avoidSteps;
  fsm_state state;
} full_state;

/*
 * Utilities for game controller
 */
void calibrate();
void testCalibration();
orientation getControl();

/*
 * Utilities to store and display snake
 */
void initializeSystem();
xy randomFruit();
void toggleFruit();
bool collision(byte x, byte y);
xy peek();
bool add(byte x, byte y);
bool remove();
void invert();
