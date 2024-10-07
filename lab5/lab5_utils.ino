/*
 * Initialize LCD based on Lab 5 schematic
 */
const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/*
 * Initialize system: set random, initialize arrow characters, set up LCD
 */
void initializeSystem() {
  randomSeed(analogRead(6));
  for (int j = 0; j < 8; j++) {
    lcd.createChar(j, tailArrows[(int)j]);
    arrowIsTail[j] = true;
  }
  lastButtonPressed = (orientation) 7;
  numButtonsPressed = -1;
  lcd.begin(16,2);
}

/*
 * Display "CALIBRATING" as a scroll on the LCD
 * Display capacitive sensor readings from pins 7-10 in serial monitor
 */
void calibrate() {
  int i = 0;
  bool left = true;
  lcd.setCursor(0,0);
  lcd.print("CALIBRATING...");
  int scrollLen = ((String)"CALIBRATING...").length();
  delay(400);
  while(true) {
    Serial.println("Capacitive sensing:");
    Serial.print("6: " );
    Serial.println(s6.capacitiveSensorRaw(10));
    Serial.print("7:  ");
    Serial.println(s7.capacitiveSensorRaw(10));
    Serial.print("8:  ");
    Serial.println(s8.capacitiveSensorRaw(10));
    Serial.print("9:  ");
    Serial.println(s9.capacitiveSensorRaw(10));
    Serial.println();
    if (i == scrollLen) {
      i = 0;
      lcd.clear();
      if (left) {
        lcd.setCursor(16-scrollLen, 1);
        lcd.print("CALIBRATING...");
      } else {
        lcd.setCursor(0, 0);
        lcd.print("CALIBRATING...");
      }
      left = !left;
    } else {
      if (left) {
        lcd.scrollDisplayLeft();
      } else {
        lcd.scrollDisplayRight();
      }
    }
    i += 1;
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
    lcd.clear();
    // A hack, because enum "orientation" defines values from 0 to 3
    for(int i = 0; i < 4; i++) {
      int capReading;
      switch(capSensors[i]) {
        case 6:
          capReading = s6.capacitiveSensorRaw(10);
          break;
        case 7:
          capReading = s7.capacitiveSensorRaw(10);
          break;
        case 8:
          capReading = s8.capacitiveSensorRaw(10);
          break;
        case 9:
          capReading = s9.capacitiveSensorRaw(10);
          break;
        default:
          break;        
      }
      if (capReading > thresholds[i]) {
        lcd.setCursor(0,0);
        lcd.print(labels[i]);
      }
    }
    delay(50);
  }
}

/*
 * Read in gamepad inputs and update numButtonsPressed and lastButtonPressed
 * We only increase "numButtonsPressed" if the current button pressed is greater than the previous button pressed
 */
void updateInputs() {
  // A hack, because enum "orientation" defines values from 0 to 3
  for(int i = 0; i < 4; i++) {
    int capReading;
    switch(capSensors[i]) {
      case 6:
        capReading = s6.capacitiveSensorRaw(10);
        break;
      case 7:
        capReading = s7.capacitiveSensorRaw(10);
        break;
      case 8:
        capReading = s8.capacitiveSensorRaw(10);
        break;
      case 9:
        capReading = s9.capacitiveSensorRaw(10);
        break;
      default:
        break;        
    }
    if (capReading > thresholds[i]) {
      if ((int) lastButtonPressed != i) {
        numButtonsPressed += 1;
      }
      lastButtonPressed = (orientation) i;
    }
  }
}

/*
 * Reset numButtonsPressed to 0 and lastButtonPressed to an undefined value
 */
void resetButtons() {
  lastButtonPressed = (orientation) 7;
  numButtonsPressed = 0;
}

/*
 * Display level l and countdown c on the LCD
 */
void displayLevel(int l, int c) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Level: ");
  lcd.print(l);
  lcd.setCursor(0,1);
  lcd.print("...");
  lcd.print(c);
  lcd.print("...");
}

/*
 * Display cursor at (x,y) with orientation o
 * with or without tail
 * also draw black squares for any LCD squares outside of [lxb, uxb) (the valid area)
 */
void displayCursor(byte x, byte y, orientation o, bool withTail, int lxb, int uxb) {
  // A hack, because enum "orientation" defines values from 0 to 3 and we defined arrow indexing in this way
  // we are converting LCD y-coordinates (0 and 1) to game y-coordinates (0, 1, 2, and 3)
  int arrowInd = o + 4 * (y%2);
  // only 8 custom characters are allowed to be stored at once, so we have to swap them out sometimes
  if (withTail and !arrowIsTail[arrowInd]) {
    lcd.createChar(arrowInd, tailArrows[arrowInd]);
    arrowIsTail[arrowInd] = true;
  } else if (!withTail and arrowIsTail[arrowInd]) {
    lcd.createChar(arrowInd, arrows[arrowInd]);
    arrowIsTail[arrowInd] = false;
  }
  lcd.clear();
  lcd.setCursor(x, y/2);
  lcd.write(byte(arrowInd));
  // character 255 is a black square
  for (int i = 0; i <= lxb - 1; i++) {
    lcd.setCursor(i, 0);
    lcd.write(byte(255));
    lcd.setCursor(i, 1);
    lcd.write(byte(255));
  }
  for (int i = 15; i >= uxb; i--) {
    lcd.setCursor(i, 0);
    lcd.write(byte(255));
    lcd.setCursor(i, 1);
    lcd.write(byte(255));
  }
}

/*
 * Display GAME OVER and the level l achieved
 */
void displayGameOver(int l) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("GAME OVER");
  lcd.setCursor(0,1);
  lcd.print("Level: ");
  lcd.print(l);
}

#ifndef TESTING // unit testing turned off: normal behavior
/*
 * Returns a random (x,y) location and orientation o within [lxb, uxb)
 */
xyo randomLocationOrientation(int lxb, int uxb) {
  xyo retval;
  retval.x = random(lxb, uxb);
  retval.y = random(0, 3);
  // A hack, because enum "orientation" defines values from 0 to 3
  retval.o = (orientation) random(0, 3);
  return retval;
}

/*
 * Returns a new orientation based on two rules:
 *   1) new orientation must be at most 90 degrees difference from o
 *   2) Probabilities of turning to a latitudinal vs longitudinal direction are based on the
 *      aspect ratio of the current valid area
 */
orientation randomTurn(orientation o, int lxb, int uxb) {
  if (o == UP or o == DOWN) {
    int r = random(0, 4 + 2 * (uxb - lxb));
    if (r < 4) {
      return o;
    } else if (r < 4 + (uxb - lxb)) {
      return LEFT;
    } else {
      return RIGHT;
    }
  } else {
    int r = random(0, 8 + (uxb - lxb));
    if (r < 4) {
      return UP;
    } else if (r < 8) {
      return DOWN;
    } else {
      return o;
    }
  }
}

/*
 * Makes sure the wall is not spawned over the cursor or over where the cursor is about to be
 * (unless lxb and uxb will end up being equal)
 */
lu shrinkBounds(byte x, orientation o, int lxb, int uxb) {
  lu retval = {lxb, uxb};
  if (x == uxb - 1 or (o == RIGHT and x == uxb - 2)) {
    retval.l += 1;
  } else if (x == lxb or (o == LEFT and x == lxb + 1)) {
    retval.u -= 1;
  } else {
    if (random(0,2) == 0) {
      retval.l += 1;
    } else {
      retval.u -= 1;
    }
  }
  return retval;
}
#else
/*
 * MOCKED UP FUNCTIONS FOR TESTING
 */
xyo randomLocationOrientation(int lxb, int uxb) {
  return {lxb, 0, UP};
}

orientation randomTurn(orientation o, int lxb, int uxb) {
  switch(o) {
    case UP:
    return RIGHT;
    case RIGHT:
    return DOWN;
    case DOWN:
    return LEFT;
    case LEFT:
    return UP;
  }
}

lu shrinkBounds(byte x, orientation o, int lxb, int uxb) {
  return {lxb + 1, uxb};
}
#endif
