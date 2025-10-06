#define CTIME 8
/*
 * Display capacitive sensor readings from pins 6-9 in serial monitor
 */
void calibrate() {
  while(true) {
    Serial.println("Capacitive sensing:");
    Serial.print("6: " );
    Serial.println(s6.capacitiveSensorRaw(CTIME));
    Serial.print("7:  ");
    Serial.println(s7.capacitiveSensorRaw(CTIME));
    Serial.print("8:  ");
    Serial.println(s8.capacitiveSensorRaw(CTIME));
    Serial.print("9:  ");
    Serial.println(s9.capacitiveSensorRaw(CTIME));
    Serial.println();
    delay(300);
  }
}

/*
 * Display which sensor has been pressed on the gamepad, assuming capSensors[] and thresholds[]
 * have been set up correctly
 */
void testCalibration() {
  String labels[4];
  // A hack, because enum "orientation" defines values from 0 to 3
  labels[UP] = "UP pressed";
  labels[LEFT] = "LEFT pressed";
  labels[DOWN] = "DOWN pressed";
  labels[RIGHT] = "RIGHT pressed";

  while(true) {
    // A hack, because enum "orientation" defines values from 0 to 3
    for(int i = 0; i < 4; i++) {
      CapacitiveSensor* s;
      switch(capSensors[i]) {
        case 6:
        s = &s6;
        break;
        case 7:
        s = &s7;
        break;
        case 8:
        s = &s8;
        break;
        case 9:
        s = &s9;
        break;
        default:
        break;      
      }
      if (s->capacitiveSensorRaw(CTIME) > thresholds[i]) {
        Serial.println(labels[i]);
      }
    }
    delay(50);
  }
}

/*
 * Detects edges on each of the controller directions
 * Ignores multiple pushes (returns NONE);
 */
orientation getControl() {
  static bool prev[] = {false, false, false, false};
  bool pressed[] = {false, false, false, false};
  // A hack, because enum "orientation" defines values from 0 to 3
  for(int i = 0; i < 4; i++) {
    CapacitiveSensor *s;
    switch(capSensors[i]) {
      case 6:
      s = &s6;
      break;
      case 7:
      s = &s7;
      break;
      case 8:
      s = &s8;
      break;
      case 9:
      s = &s9;
      break;
      default:
      break;    
    }
    if (s->capacitiveSensorRaw(CTIME) > thresholds[i]) {
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
      if (ret != NONE) {
        return NONE;
      } else {
        ret = (orientation) i;
      }
    }
  }
  return ret;
}