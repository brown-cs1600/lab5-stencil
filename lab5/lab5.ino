#include "lab5.h"

// FSM variables
/*
 File-static: "global" to the file only
(to avoid spaghetti code by minimizing chance of other files changing these variables)
Note that even safer would be to make these static in the updateFSM function,
but then we wouldn't be able to expose them for unit testing (lab 6)
*/
static byte x, y;
static orientation o;
static int lxb, uxb, level, timeStep, savedClock, countdown;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  initializeSystem();

  /*
   * LAB STEP 4b
   */
  calibrate();
  // Change to 7, 8, 9, 10 based on calibration
  capSensors[UP] = 0;
  capSensors[RIGHT] = 0;
  capSensors[DOWN] = 0;
  capSensors[LEFT] = 0;
  // Change based on calibration
  thresholds[UP] = 0;
  thresholds[RIGHT] = 0;
  thresholds[DOWN] = 0;
  thresholds[LEFT] = 0;

  /*
   * LAB STEP 4c
   */
  testCalibration();

  /*
   * LAB STEP 6
   * Initialize all FSM variables
   */
  lxb = 0;
  uxb = 0;
  xyo randomXYO = randomLocationOrientation(lxb, uxb);
  x = randomXYO.x;
  y = randomXYO.y;
  o = randomXYO.o;
  level = 0;
  timeStep = 0;
  savedClock = 0;
  countdown = 0;
  // display something on the LCD
}

void loop() {
  static state CURRENT_STATE = sDISP_COUNTDOWN;
  updateInputs();
  CURRENT_STATE = updateFSM(CURRENT_STATE, millis(), numButtonsPressed, lastButtonPressed);
  delay(10);
}

/*
 * LAB STEP 5
 */
bool facingWall(byte x, byte y, byte o, int lxb, int uxb) {
  return false;
}

/*
 * LAB STEP 5
 */
xy move(byte x, byte y, byte o) {
  xy retval = {x, y};
  return retval;
}

/*
 * LAB STEP 7
 */
state updateFSM(state curState, long mils, int numButtons, int lastButton) {
  state nextState;
  switch(curState) {
  case sDISP_COUNTDOWN:
    if ((mils - savedClock) >= 500 and countdown >= 0) { // transition 1-1
      displayLevel(level, countdown);
      countdown -= 1;
      savedClock = mils;
      nextState = sDISP_COUNTDOWN;
    }
    // add else if/else
    break;
  case sWAIT_AFTER_ROT:
    if ((mils - savedClock) >= timeStep and numButtons > 0) { // transition 2-3 (a)
      displayCursor(x, y, o, false, lxb, uxb);
      lu shrunken = shrinkBounds(x, o, lxb, uxb);
      lxb = shrunken.l;
      uxb = shrunken.u;
      xy newXY = move(x, y, o);
      x = newXY.x;
      y = newXY.y;
      nextState = sMOV;
    } else if ((mils - savedClock) >= timeStep and numButtons == 0) { // transition 2-3 (b)
      displayCursor(x, y, o, false, lxb, uxb);
      xy newXY = move(x, y, o);
      x = newXY.x;
      y = newXY.y;
      nextState = sMOV;
    } else {
      nextState = sWAIT_AFTER_ROT;
    }
    break;
  case sMOV:
    break;
  case sWAIT_AFTER_MOV:
    break;
  case sROT:
    break;
  case sWAIT_FOR_BUT:
    break;
  case sGAME_OVER: // no transitions from state 7
    nextState = sGAME_OVER;
    break;
  }
  return nextState;
}
