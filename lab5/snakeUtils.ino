#include "Arduino_LED_Matrix.h"
static ArduinoLEDMatrix matrix;

static unsigned long snakePixels[] = {0, 0, 0}; // snake body pixels stored in LED matrix format
static unsigned long frame[] = {0, 0, 0}; // current LED matrix display (includes blinking fruit and inversion)
static xy fruit; // location of the fruit
static bool inverted = false; // whether the display is currently inverted

// circular array tracking snake body locations as they're added
static xy orderedSnake[AREA]; 
static byte head = AREA - 1;
static byte tail = 0;
static byte snakeLen = 0;

/*
 * Initializes system:
 * Initializes LED matrix, random seed
 * Initializes head at random position
 */
void initializeSystem() {
  randomSeed(analogRead(A5));
  matrix.begin();
  xy newHead = randomPos();
  add(newHead.x, newHead.y);
}

/*
 * Re-initializes the system
 */
void reset() {
  head = AREA - 1;
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
 * Returns a random x,y within the playable game area ((1, 1) to (12, 8))
 */
#ifndef TESTING // unit testing turned off: normal behavior
static xy randomPos() {
  return {random(W) + 1, random(H) + 1};
}
#else // unit testing turned on: return 1, 1
static xy randomPos() {
  return {1, 1};
}
#endif

/*
 * Returns a random x,y that does not collide with the snake
 * Modifies variable fruit
 */
#ifndef TESTING // unit testing turned off: normal behavior
xy randomFruit() {
  byte randPos = random(AREA - snakeLen);
  // compute the randPos-th blank pixel
  for (int i = 0; i < 3; i++) {
    unsigned long snakeLine = snakePixels[i];
    for (int j = 0; j < 32; j++) {
      if ((snakeLine & 0x1) == 0) { // pixel does not include snake
        if (randPos == 0) {
          // compute x,y from matrix coordinate i,j
          byte ij = i * 32 + 31 - j;
          byte x = ij % W + 1;
          byte y = ij / W + 1;
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
 * Flips a /snake/ (not fruit) pixel at coordinate x,y
 * Tracks both presence of snake body (in snakePixels) and regular or inverted display (frame)
 */
static void flipPixel(byte x, byte y) {
  byte xyPos = W * (y - 1) + x - 1;
  // mask off the specific pixel by converting between x,y coordinates and matrix coordinates
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
  byte xyPos = W * (fruit.y - 1) + fruit.x - 1;
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
  byte xyPos = W * (y - 1) + x - 1;
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
    head = head == (AREA - 1) ? 0 : head + 1;
    orderedSnake[head].x = x;
    orderedSnake[head].y = y;
    flipPixel(x, y);
    snakeLen++;
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
    tail = tail == (AREA - 1) ? 0 : tail + 1;
    snakeLen--;
    return true;
  } else {
    Serial.println("ERROR: tried to remove from a snake of length 0");
    return false;
  }
}