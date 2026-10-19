#define CTIME 8
CapacitiveSensor* const caps[4] = {&s6, &s7, &s8, &s9};

/*
 * Displays capacitive sensor readings from pins 6-9 in serial monitor
 */
void calibrate() {
  while(true) {
    Serial.println("Capacitive sensing:");
    for (int i = 0; i < 4; i++) {
      Serial.print(i + 6);
      Serial.print(": ");
      Serial.println(caps[i]->capacitiveSensorRaw(CTIME));
    }
    Serial.println();
    delay(300);
  }
}

/*
 * Displays which sensor has been pressed on the gamepad, assuming capSensors[] and thresholds[]
 * have been set up correctly
 */
void testCalibration() {
  // A hack, because enum "orientation" defines values from 0 to 3
  const String labels[4] = {"UP pressed", "RIGHT pressed", "DOWN pressed", "LEFT pressed"};

  while(true) {
    // A hack, because enum "orientation" defines values from 0 to 3
    for(int i = 0; i < 4; i++) {
      if (caps[capSensors[i]-6]->capacitiveSensorRaw(CTIME) > thresholds[i]) {
        Serial.println(labels[i]);
      }
    }
    delay(50);
  }
}

/*
 * Detecst edges on each of the controller directions
 * Ignores multiple pushes (returns NONE)
 */
orientation getControl() {
  static bool prev[] = {false, false, false, false};
  bool pressed[] = {false, false, false, false};
  // A hack, because enum "orientation" defines values from 0 to 3
  for(int i = 0; i < 4; i++) {
    if (caps[capSensors[i]-6]->capacitiveSensorRaw(CTIME) > thresholds[i]) {
      if (!prev[i]) {
        pressed[i] = true;
      }
      prev[i] = true;
    } else {
      prev[i] = false;
    }
  }
  // check to see which button was pressed
  orientation ret = NONE;
  for (int i = 0; i < 4; i++) {
    if (pressed[i]) {
      if (ret != NONE) { // handle simultaneous pushes
        return NONE;
      } else {
        ret = (orientation) i;
      }
    }
  }
  return ret;
}
