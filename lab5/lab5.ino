#include "snake.h"

#define W 12
#define H 8

void setup() {
  Serial.begin(9600);
  while(!Serial);

  calibrate();
  // LAB STEP 4:
  // Change to 6, 7, 8, 9 based on calibration
  capSensors[UP] = 0;
  capSensors[RIGHT] = 0;
  capSensors[DOWN] = 0;
  capSensors[LEFT] = 0;
  // Change based on calibration
  thresholds[UP] = 0;
  thresholds[DOWN] = 0;
  thresholds[LEFT] = 0;
  thresholds[RIGHT] = 0;

  testCalibration();

  initializeSystem();
}

/*
 * Computes whether the given x,y is out of bounds
 * Playable game area is from (1, 1) (top-left) to (12, 8) (bottom-right)
 */
bool outOfBounds(byte x, byte y) {
  return false; // LAB STEP 5
}

/*
 * Computes new x,y one step in direction o from given x,y
 */
xy newPos(byte x, byte y, orientation o) {
  // LAB STEP 5
  switch(o) {
    case NONE:
    case UP:
    case RIGHT:
    case DOWN:
    case LEFT:
    default:
    return {x, y};
  }
}

/*
 * Computes whether newO is a 90 degree turn from currO
 * If either newO or currO is NONE, returns false
 */
bool turn(orientation currO, orientation newO) {
  // LAB STEP 5
  switch(currO) {
    case UP:
    case DOWN:
    case LEFT:
    case RIGHT:
    default:
    return false;
  }
}

void loop() {
  // LAB STEP 6
  /*
  static full_state fs = {
    {peek().x, peek().y, ___}, // snakeHead
    randomFruit(), // fruit
    ___, // step
    ___, // length
    millis(), // savedClock
    0, // avoidSteps
    s_INIT // state
  };*/

  // detect edge on button push
  static bool prevButton = false;
  bool curButton = false;
  if (digitalRead(2) == HIGH) {
    if (!prevButton) {
      curButton = true;
    }
    prevButton = true;
  } else {
    prevButton = false;
  }

  // UNCOMMENT AFTER COMPLETING LAB STEP 6
  //fs = updateFSM(fs, getControl(), curButton, millis());

  static int loopCount = 5;
  loopCount++;
  if (loopCount == 6) { // toggle every 60ms
    toggleFruit();
    loopCount = 0;
  }
  delay(10);
}

/*
 * Takes in a struct representing the current FSM state and state variables, as well as the three inputs
 * and returns the updated FSM state and state variables
 */
full_state updateFSM(full_state currState, orientation control, bool button, unsigned long clock) {
  xyo head = currState.head;
  xy fruit = currState.fruit;
  unsigned long step = currState.step;
  byte snakeLength = currState.snakeLength;
  unsigned long savedClock = currState.savedClock;
  byte avoidSteps = currState.avoidSteps;
  fsm_state state = currState.state;

  full_state ret = currState; // FSM state and state variables to return. Default is to keep unchanged.
  // LAB STEP 7
  switch(state) {
    case s_INIT:
    if (control != NONE) { // t 1-2
      ret.savedClock = clock;
      xy newHead = newPos(head.x, head.y, control);
      ret.head = {newHead.x, newHead.y, control};
      ret.state = s_REG_CALC;
    } 
    break; // s_INIT

    case s_REG_CALC:
    if ((head.x != fruit.x || head.y != fruit.y) && !collision(head.x, head.y) &&
        !outOfBounds(head.x, head.y)) { // t 2-3a
      add(head.x, head.y);
      remove();
      ret.savedClock = clock;
      ret.state = s_REG_WAIT;
    }
    // add else ifs for other transitions out of state 2
    break; // s_REG_CALC

    case s_REG_WAIT:
    break; // s_REG_WAIT

    case s_AVOID_WAIT:
    break; // s_AVOID_WAIT

    case s_AVOID_CALC:
    break; // s_AVOID_CALC

    case s_WIN:
    // terminal state: no outgoing transitions
    break; // s_WIN

    case s_GAME_OVER:
    // terminal state: no outgoing transitions
    break; // s_GAME_OVER

    default:
    Serial.println("Invalid state");
  }
  return ret;
}
