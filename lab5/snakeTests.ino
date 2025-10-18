/*
 * THIS FILE WILL BE USED FOR LAB 6
 */

/*
 * A struct to keep all three state inputs in one place
 */
typedef struct {
  orientation control;
  bool button;
  unsigned long clock;
} state_inputs;

bool testTransition(full_state start,
                    full_state end,
                    state_inputs inputs,
                    bool verbos);
/*        
 * Helper function for printing states
 */
char* s2str(fsm_state s) {
  switch(s) {
    case s_INIT:
    return "(1) INIT";
    case s_REG_CALC:
    return "(2) REG_CALC";
    case s_REG_WAIT:
    return "(3) REG_WAIT";
    case s_AVOID_WAIT:
    return "(4) AVOID_WAIT";
    case s_AVOID_CALC:
    return "(5) AVOID_CALC";
    case s_GAME_OVER:
    return "(6) GAME_OVER";
    case s_WIN:
    return "(7) WIN";
    default:
    return "???";
  }
}

/*
 * Helper function for printing orientations
 */
char* o2str(orientation o) {
  switch(o) {
    case UP:
    return "UP";
    case RIGHT:
    return "RIGHT";
    case DOWN:
    return "DOWN";
    case LEFT:
    return "LEFT";
    case NONE:
    return "NONE";
    default:
    return "???";
  }
}

/*
 * Given a start state (including state variables), inputs, tests that
 * updateFSM returns the correct end state and updates the state variables correctly
 * returns true if this is the case (test passed) and false otherwise (test failed)
 * 
 * Need to use "verbos" instead of "verbose" because verbose is apparently a keyword
 */
bool testTransition(full_state start,
                    full_state end,
                    state_inputs inputs,
                    bool verbos) {
  full_state res = updateFSM(start, inputs.control, inputs.button, inputs.clock);

  bool passedTest =  (res.head.x == end.head.x &&
                      res.head.y == end.head.y &&
                      res.head.o == end.head.o &&
                      res.fruit.x == end.fruit.x &&
                      res.fruit.y == end.fruit.y &&
                      res.step == end.step &&
                      res.snakeLength == end.snakeLength &&
                      res.savedClock == end.savedClock &&
                      res.avoidSteps == end.avoidSteps &&
                      res.state == end.state);
  if (! verbos) {
    return passedTest;
  } else if (passedTest) {
    char sToPrint[200];
    sprintf(sToPrint, "Test from %s to %s PASSED", s2str(start.state), s2str(end.state));
    Serial.println(sToPrint);
    return true;
  } else {
    char sToPrint[200];
    sprintf(sToPrint, "Test from %s to %s FAILED", s2str(start.state), s2str(end.state));
    Serial.println(sToPrint);
    sprintf(sToPrint, "End state expected: %s | actual: %s", s2str(end.state), s2str(res.state));
    Serial.println(sToPrint);
    sprintf(sToPrint, "Inputs: control %s | button %s | clock %ld", o2str(inputs.control), inputs.button ? "true" : "false", inputs.clock);
    Serial.println(sToPrint);
    sprintf(sToPrint, "          %6s | %6s | %6s | %7s | %7s | %4s | %11s | %10s | %10s", "head.x", "head.y", "head.o", "fruit.x", "fruit.y", "step", "snakeLength", "savedClock", "avoidSteps");
    Serial.println(sToPrint);
    sprintf(sToPrint, "starting: %6d | %6d | %6s | %7d | %7d | %4d | %11d | %10ld | %10d", start.head.x, start.head.y, o2str(start.head.o), start.fruit.x, start.fruit.y, start.step, start.snakeLength, start.savedClock, start.avoidSteps);
    Serial.println(sToPrint);
    sprintf(sToPrint, "expected: %6d | %6d | %6s | %7d | %7d | %4d | %11d | %10ld | %10d", end.head.x, end.head.y, o2str(end.head.o), end.fruit.x, end.fruit.y, end.step, end.snakeLength, end.savedClock, end.avoidSteps);
    Serial.println(sToPrint);
    sprintf(sToPrint, "actual:   %6d | %6d | %6s | %7d | %7d | %4d | %11d | %10ld | %10d", res.head.x, res.head.y, o2str(res.head.o), res.fruit.x, res.fruit.y, res.step, res.snakeLength, res.savedClock, res.avoidSteps);
    Serial.println(sToPrint);
    Serial.println("");
    return false;
  }
}

/*
 * REPLACE THE FOLLOWING 4 LINES WITH THE GENERATED TEST CASES
 */
const full_state testStatesIn[0] = {}; 
const full_state testStatesOut[0] = {};
const state_inputs testInputs[0] = {};
const int numTests = 0;


/*
 * Runs through all the test cases defined above
 */
bool testAll() {
  #ifndef TESTING
  Serial.println("Testing not compiled. Need to #define TESTING!");
  return false;
  #else // TESTING defined!
  for (int i = 0; i < numTests; i++) {
    Serial.print("Running test ");
    Serial.print(i + 1);
    Serial.print(" of ");
    Serial.println(numTests);
    reset();
    if (!testTransition(testStatesIn[i], testStatesOut[i], testInputs[i], true)) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All tests passed!");
  return true;
  #endif // #ifndef TESTING
}