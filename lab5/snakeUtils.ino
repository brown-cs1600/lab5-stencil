#include "Arduino_LED_Matrix.h"
static ArduinoLEDMatrix matrix;

static unsigned long snakePixels[] = {0, 0, 0}; // snake body pixels stored in LED matrix format
static unsigned long frame[] = {0, 0, 0}; // current LED matrix display
static xy fruit; // location of the fruit
static bool inverted = false; // whether the display is currently inverted

// circular array tracking snake body locations as they're added
static xy orderedSnake[96]; 
static byte head = 95;
static byte tail = 0;
static byte snakeLen = 0;

/*
 * Initialize system:
 * Initialize random seed, LED matrix
 * Initialize head at random position
 */
void initializeSystem() {
  randomSeed(analogRead(6));
  matrix.begin();
  xy newHead = randomPos();
  add(newHead.x, newHead.y);
}

/*
 * Re-initializes the system
 */
void reset() {
  head = 95;
  tail = 0;
  snakeLen = 0;
  inverted = false;
  for (int i = 0; i < 3; i++) {
    snakePixels[i] = 0;
    frame[i] = 0;
  }
  xy newHead = randomPos();
  add(newHead.x, newHead.y);
}

/*
 * Return a random x,y within the playable game area ((1, 1) to (12, 8))
 */
#ifndef TESTING // unit testing turned off: normal behavior
static xy randomPos() {
  return {random(12) + 1, random(8) + 1};
}
#else // unit testing turned on: return 1, 1
static xy randomPos() {
  return {1, 1};
}
#endif

/*
 * @return Return a random x,y that does not collide with the snake
 * Modifies variable fruit
 */
#ifndef TESTING // unit testing turned off: normal behavior
xy randomFruit() {
  byte randPos = random(96 - snakeLen);
  for (int i = 0; i < 3; i++) {
    unsigned long snakeLine = snakePixels[i];
    for (int j = 0; j < 32; j++) {
      if ((snakeLine & 0x1) == 0) {
        if (randPos == 0) {
          byte ij = i * 32 + 31 - j;
          byte x = ij % 12 + 1;
          byte y = ij / 12 + 1;
          fruit = {x, y};
          return {x, y};
        } else {
          randPos--;
        }
      }
      snakeLine >>= 1;
    }
  }
}
#else // unit testing turned on: return {2, 1}
xy randomFruit() {
  return {2, 1};
}
#endif

/*
 * Flip a /snake/ (not fruit) pixel at coordinate x,y
 * Tracks both presence of snake body (in snakePixels) and regular or inverted display (frame)
 */
static void flipPixel(byte x, byte y) {
  byte xyPos = 12 * (y - 1) + x - 1;
  unsigned long mask = 1 << 31 - (xyPos % 32);
  snakePixels[xyPos / 32] ^= mask;

  // sync frame to head (necessary bc fruit might have been blinking beforehand)
  if (inverted) {
    frame[xyPos / 32] |= mask; // set bit to 1 (xor with 0 = 1, xor with 1 = 0)
    frame[xyPos / 32] ^= (snakePixels[xyPos / 32] & mask); // frame bit = flipped snake bit
  } else {
    frame[xyPos / 32] &= ~mask; // set bit to 0
    frame[xyPos / 32] |= (snakePixels[xyPos / 32] & mask); // frame bit = snake bit
  }
  matrix.loadFrame(frame);
}

/*
 * Toggles the frame pixel corresponding to the current fruit
 */
void toggleFruit() {
  byte xyPos = 12 * (fruit.y - 1) + fruit.x - 1;
  frame[xyPos / 32] ^= 1 << (31 - (xyPos % 32));
  matrix.loadFrame(frame);
}

/*
 * Inverts the display by flipping each pixel
 */
void invert() {
  inverted = !inverted;
  for (int i = 0; i < 3; i++) {
    frame[i] ^= 0xffffffff;
  }
  matrix.loadFrame(frame);
}

/*
 * Computes whether a piece of the snake body exists at the given x, y
 */
bool collision(byte x, byte y) {
  byte xyPos = 12 * (y - 1) + x - 1;
  return (snakePixels[xyPos / 32] & (0x1 << (31 - (xyPos % 32)))) != 0;
}

/*
 * Returns the current head of the orderedSnake array
 */
#ifndef TESTING // unit testing turned off: normal behavior
xy peek() {
  if (snakeLen != 0) {
    return orderedSnake[head];
  } else {
    Serial.println("ERROR: tried to peek at a snake of length 0");
    while(true);
  }
}
#else // unit testing turned on: return {1, 1}
xy peek() {
  return {1, 1};
}
#endif

/*
 * Adds a new head at coordinate x,y to orderedSnake and updates the display
 */
bool add(byte x, byte y) {
  if (!collision(x, y)) {
    head = head == 95 ? 0 : head + 1;
    orderedSnake[head].x = x;
    orderedSnake[head].y = y;
    flipPixel(x, y);
    snakeLen++;
    int idx = tail;
    for (int i = 0; i < snakeLen; i++) {
      idx = (tail + i) % 96;
    }
    return true;
  } else {
    Serial.println("ERROR: tried to add a pixel that is already used");
    return false;
  }
}

/*
 * Removes the tail from orderedSnake and updates the display
 */
bool remove() {
  if (snakeLen != 0) {
    if (!collision(orderedSnake[tail].x, orderedSnake[tail].y)) {
      Serial.println("ERROR: tried to remove for which we don't have a pixel");
      while(true);
    }
    flipPixel(orderedSnake[tail].x, orderedSnake[tail].y);
    tail = tail == 95 ? 0 : tail + 1;
    snakeLen--;
    return true;
  } else {
    Serial.println("ERROR: tried to remove from a snake of length 0");
    return false;
  }
}