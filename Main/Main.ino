/*

    Creator : Thomas Cibils
    Last update : 05.02.2019
    FastLED tuto : https://github.com/FastLED/FastLED/wiki/Basic-usage - used for WS2812B 5050 RGB LED Strip 5M 150 300 Leds 144 60LED/M Individual Addressable 5V
 *  */

#include <TimerOne.h>
#include "FastLED.h"
#include <avr/pgmspace.h>

// LED MATRIX CODE
#define displayNumberOfRows 16                          // Number of rows
#define displayNumberOfColumns 16                       // Number of coumns
#define NUM_LEDS displayNumberOfRows * displayNumberOfColumns // Number of LEDs

CRGB leds[NUM_LEDS];                                          // Defining leds table for FastLed
#define DATA_PIN 6                                            // Output pin for FastLed

/* Constantes des bits de chaque bouton */
#define BTN_A 256
#define BTN_B 1
#define BTN_X 512
#define BTN_Y 2
#define BTN_SELECT 4
#define BTN_START 8
#define BTN_UP 16
#define BTN_DOWN 32
#define BTN_LEFT 64
#define BTN_RIGHT 128
#define BTN_L 1024
#define BTN_R 2048
#define NO_GAMEPAD 61440

/* Pin mapping */
#define numberOfGamepads 2
// First pin in LATCH, second is CLOCK, third is DATA
// On my Testing Gamepad, black is ground, green is +5V, White is latch, Yellow is clock, and Red is data
const byte gamepadPins[numberOfGamepads][3] = {
  {2, 3, 4},
  {7, 8, 9}
};
static const byte PIN_LATCH = 2;
static const byte PIN_CLOCK = 3;
static const byte PIN_DATA = 4;

#define numberOfPlayers 2
// In order : {R, L, X, A, Right, Left, Down, High, Start, Select, Y, B}
// This order is not random, even if it seems to be : it corresponds to the order in which bytes are returned from the gamepad
// See here : https://skyduino.wordpress.com/2014/02/23/tuto-arduino-et-gamepad-de-snes/

byte playerButtonPushed[numberOfPlayers][12] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

byte playerButtonPushedPrevious[numberOfPlayers][12] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// LED Matrix
// top column is from 0 to 7, bottom one from 56 to 63 (for a 8x8 matrix)
byte LEDMatrix[displayNumberOfRows][displayNumberOfColumns];


// Original colours for leds.
const byte Black = 0;
const byte White = 1;
const byte Blue = 2;
const byte Red = 3;
const byte Green = 4;
const byte Purple = 5;

#define directionUp 0
#define directionRight 1
#define directionDown 2
#define directionLeft 3

#define moveForward 4
#define moveBackward 5
#define moveLeft 6
#define moveRight 7

struct pointOnMatrix {
  byte lineCoordinate;
  byte columnCoordinate;
};

// Defining players
#define numberOfPlayers 2

// So far, it's a position and a level
struct Player {
  byte lineCoordinate;
  byte columnCoordinate;
  byte level;
  byte colour;
  byte playerDirection;
};

// And the players are listed here
Player playersArray[numberOfPlayers] = {
  {3, 3, 1, Purple, directionUp},
  {13, 13, 2, Red, directionUp}
};


unsigned long lastMillis = 0;
unsigned const int growthSpeed = 1500;  // In miliseconds, how long will a "tick" be


#define numberOfShips 4                                         // Defines the number of different ships existing
#define maxShipSize 5                                           // Defines the maximum size of a ship
const byte shipSizes[numberOfShips] = {maxShipSize, 4, 3, 3};   // Each ship fits in a square, from top left, defined below for fitting on map.

// Ships are defined below.0 means "off", and 1 means "on". Set them in progmem to save RAM.
// Ships "real size", meaning taking the 0 columns and lines off, must be the size defined in the table "shipSizes".
const byte PROGMEM ships[numberOfShips][maxShipSize][maxShipSize] = {
  {
    {0, 1, 1, 1, 0},
    {1, 1, 1, 0, 0},
    {1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0},
    {0, 1, 1, 1, 0}
  },
  {
    {0, 1, 1, 1, 0},
    {1, 1, 1, 0, 0},
    {1, 1, 1, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0}
  },
  {
    {1, 1, 1, 0, 0},
    {1, 1, 0, 0, 0},
    {1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
  },
  {
    {0, 1, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
  }
};


void setup() {

  Serial.begin(9600);

  // Set matrix pins to output
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  clearLEDMatrix();

  /* Initializing each SNES gamepad */
  for (byte i = 0; i < numberOfGamepads; i++) {
    pinMode(gamepadPins[i][0], OUTPUT);   // LATCH
    pinMode(gamepadPins[i][1], OUTPUT);   // CLOCK
    pinMode(gamepadPins[i][2], INPUT);    // DATA
  }
}

void loop() {

  if (lastMillis - millis() > 500) {


    lastMillis = millis();
  }
  clearLEDMatrix();
  displayPlayer(0);
  displayPlayer(1);



  for (byte playerIndex = 0; playerIndex < numberOfPlayers; playerIndex++) {
    checkButtons(playerIndex);

    // In order : {R, L, X, A, Right, Left, Down, High, Start, Select, Y, B}
    if (playerButtonPushed[playerIndex][4] == 1) {
      movePlayer(playerIndex, moveRight);
    }

    if (playerButtonPushed[playerIndex][5] == 1) {
      movePlayer(playerIndex, moveLeft);
    }

    if (playerButtonPushed[playerIndex][6] == 1) {
      movePlayer(playerIndex, moveBackward);
    }

    if (playerButtonPushed[playerIndex][7] == 1) {
      movePlayer(playerIndex, moveForward);
    }

    resetButtons(playerIndex);
  }

  outputDisplay();
  delay(1);
}

// Displaying a player's ship on the matrix, depending on its level and position
void displayPlayer(const byte playerToDisplayID) {
  
  // Depending on the player's level, we iterate on the respective ship size
  for (byte i = 0; i < shipSizes[playersArray[playerToDisplayID].level]; i++) {
    for (byte j = 0; j < shipSizes[playersArray[playerToDisplayID].level]; j++) {
      if(playersArray[playerToDisplayID].playerDirection == directionRight) {
        // Then, we recopy in our LED Matrix the player's ship, depending on the player position and level.
        LEDMatrix[playersArray[playerToDisplayID].lineCoordinate + i][playersArray[playerToDisplayID].columnCoordinate + j] = playersArray[playerToDisplayID].colour * pgm_read_byte(&(ships[playersArray[playerToDisplayID].level][i][j]));
      }

      if(playersArray[playerToDisplayID].playerDirection == directionLeft) {
        // Transforming the ship's matrix with a mirror to make it face left
        LEDMatrix[playersArray[playerToDisplayID].lineCoordinate + i][playersArray[playerToDisplayID].columnCoordinate + j] = playersArray[playerToDisplayID].colour * pgm_read_byte(&(ships[playersArray[playerToDisplayID].level][i][shipSizes[playersArray[playerToDisplayID].level] - j -1]));
      }
      
      if(playersArray[playerToDisplayID].playerDirection == directionUp) {
        // Transforming the ship's matrix to make it face up
        LEDMatrix[playersArray[playerToDisplayID].lineCoordinate + i][playersArray[playerToDisplayID].columnCoordinate + j] = playersArray[playerToDisplayID].colour * pgm_read_byte(&(ships[playersArray[playerToDisplayID].level][j][shipSizes[playersArray[playerToDisplayID].level] - i - 1]));
      }
      
      if(playersArray[playerToDisplayID].playerDirection == directionDown) {
        LEDMatrix[playersArray[playerToDisplayID].lineCoordinate + i][playersArray[playerToDisplayID].columnCoordinate + j] = playersArray[playerToDisplayID].colour * pgm_read_byte(&(ships[playersArray[playerToDisplayID].level][j][i]));
      }
    }
  }
}


// Moves the passed played in the passed direction, and make checks before doing it (existance of another spaceship or of world map so far)
void movePlayer(const byte playerToMoveIndex, const byte moveOrder) {

  bool blockedByAnotherPlayer = false;
  bool blockedByMapBorder = false;

  // Case 1 : player is facing up
  if (playersArray[playerToMoveIndex].playerDirection == directionUp) {
    // The player is facing up, and wants to move forward
    if(moveOrder == moveForward) {
      // We need to check if another player is in the way. We check for all players
      for (byte playersIterationIndex = 0; playersIterationIndex < numberOfPlayers; playersIterationIndex++) {
        // We do not check for the player we're moving, of course
        if (playersIterationIndex != playerToMoveIndex) {
          // If the player's "just above" is the bottom of another player
          if (playersArray[playerToMoveIndex].lineCoordinate == playersArray[playersIterationIndex].lineCoordinate + shipSizes[playersArray[playersIterationIndex].level]) {
            // And if our most left point is at the left of the other player's most right point
            if(playersArray[playerToMoveIndex].columnCoordinate < playersArray[playersIterationIndex].columnCoordinate + shipSizes[playersArray[playersIterationIndex].level]) {
              // And if our most right point is at the right of the other player's most left point
              if(playersArray[playerToMoveIndex].columnCoordinate + shipSizes[playersArray[playerToMoveIndex].level] > playersArray[playersIterationIndex].columnCoordinate) {
                // THEN YES WE'RE FUCKING BLOCKED
                blockedByAnotherPlayer = true;
              }
            }
          }
        }
      }
    
      // We also check if we're near the map
      if(playersArray[playerToMoveIndex].lineCoordinate == 0) {
        blockedByMapBorder = true;
      }
  
      // If we're not blocked
      if (!blockedByMapBorder && !blockedByAnotherPlayer) {
        // We move the player by one up
        playersArray[playerToMoveIndex].lineCoordinate--;
      }
    }

    if(moveOrder == moveBackwards) {
      
    }

    // If the player wants to change direction, that's easy
    if(moveOrder == moveLeft) {
      playersArray[playerToMoveIndex].playerDirection = directionLeft;
    }
    
    if(moveOrder == moveRight) {
      playersArray[playerToMoveIndex].playerDirection = directionRight;
    }
  }


  // Case 2 : going right
  if (playersArray[playerToMoveIndex].playerDirection == directionRight) {

    if (playersArray[playerToMoveIndex].columnCoordinate < displayNumberOfColumns - shipSizes[playersArray[playerToMoveIndex].level]) {
     
      playersArray[playerToMoveIndex].columnCoordinate++;
    }
  }

  if (playersArray[playerToMoveIndex].playerDirection == directionDown) {
    
    if (playersArray[playerToMoveIndex].lineCoordinate < displayNumberOfRows - shipSizes[playersArray[playerToMoveIndex].level]) {
    
      playersArray[playerToMoveIndex].lineCoordinate++;
    }
  }

  if (playersArray[playerToMoveIndex].playerDirection == directionLeft) {

    if (playersArray[playerToMoveIndex].columnCoordinate > 0) {
      playersArray[playerToMoveIndex].columnCoordinate--;
    }
  }
}

// Check buttons state for a given gamepad, and stores the result in the accoding player table
void checkButtons(const byte gamepadID) {

  static uint16_t oldBtns = 0;      // Anciennes valeurs des boutons
  uint16_t btns = getSnesButtons(gamepadID); // Valeurs actuelles des boutons

  /* Affiche l'état des boutons uniquement en cas de changement */
  if (oldBtns != btns) {
    oldBtns = btns;
  }
  else                {
    return;
  }

  if (btns & NO_GAMEPAD) {
    Serial.println(F("No gamepad connected"));
    return;
  }


  // In order : {R, L, X, A, Right, Left, Down, High, Start, Select, Y, B}

  /* Affiche l'état de chaque bouton */
  if (btns & BTN_A) {
    playerButtonPushed[gamepadID][3] = 1;
  }

  if (btns & BTN_B) {
    playerButtonPushed[gamepadID][11] = 1;
  }


  if (btns & BTN_X) {
    playerButtonPushed[gamepadID][2] = 1;
  }

  if (btns & BTN_Y) {
    playerButtonPushed[gamepadID][10] = 1;
  }

  if (btns & BTN_SELECT) {
    playerButtonPushed[gamepadID][9] = 1;
  }

  if (btns & BTN_START) {
    playerButtonPushed[gamepadID][8] = 1;
  }

  if (btns & BTN_UP) {
    playerButtonPushed[gamepadID][7] = 1;
  }

  if (btns & BTN_DOWN) {
    playerButtonPushed[gamepadID][6] = 1;
  }

  if (btns & BTN_LEFT) {
    playerButtonPushed[gamepadID][5] = 1;
  }

  if (btns & BTN_RIGHT) {
    playerButtonPushed[gamepadID][4] = 1;
  }

  if (btns & BTN_L) {
    playerButtonPushed[gamepadID][1] = 1;
  }

  if (btns & BTN_R) {
    playerButtonPushed[gamepadID][0] = 1;
  }
}

// Resets all buttons to 0 for a given gamepad.
void resetButtons(const byte gamepadID) {
  for (byte i = 0; i < 12; i++) {
    playerButtonPushed[gamepadID][i] = 0;
  }
}

/** Retourne l'état de chaque bouton sous la forme d'un entier sur 16 bits. */
uint16_t getSnesButtons(const byte gamepadID) {

  /* 1 bouton = 1 bit */
  uint16_t value = 0;

  /* Capture de l'état courant des boutons - we need LATCH here, so 0*/
  digitalWrite(gamepadPins[gamepadID][0], HIGH);
  digitalWrite(gamepadPins[gamepadID][0], LOW);

  /* Récupère l'état de chaque bouton (12 bits + 4 bits à "1") */
  for (byte i = 0; i < 16; ++i) {

    /* Lit l'état du bouton et décale le bit reçu pour former l'entier sur 16 bits final - we need data here, so 2*/
    value |= digitalRead(gamepadPins[gamepadID][2]) << i;

    /* Pulsation du signal d'horloge- we need CLOCK here, so 1 */
    digitalWrite(gamepadPins[gamepadID][1], HIGH);
    digitalWrite(gamepadPins[gamepadID][1], LOW);
  }

  /* Retourne le résultat sous une forme facile à manipuler (bouton appuyé = bit à "1") */
  return ~value;
}


// Makes the whole "LEDMatrix" equals to 0, i.e. each LED is off
void clearLEDMatrix() {
  // Just seting le LEDmatrix to Wall
  for (byte i = 0; i < displayNumberOfRows; i++)  {
    for (byte j = 0; j < displayNumberOfColumns; j++) {
      LEDMatrix[i][j] = Black;
    }
  }
}


// We update the physical display of the LED matrix, based on the LEDMatrix
void outputDisplay() {
  for (byte rowIndex = 0; rowIndex < displayNumberOfRows; rowIndex++) {
    for (byte columnIndex = 0; columnIndex < displayNumberOfColumns; columnIndex++) {
      // Useful because of the way my matrix is soldered.
      // So we'll invert one column every two compared to our digital matrix
      // If we're on an even column, we're fine, everything is straightfoward
      if (columnIndex % 2 == 1) {

        if (LEDMatrix[rowIndex][columnIndex] == Black) {
          leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Black;
        }
        if (LEDMatrix[rowIndex][columnIndex] == White) {
          leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::White;
        }
        if (LEDMatrix[rowIndex][columnIndex] == Green) {
          leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Green;
        }
        if (LEDMatrix[rowIndex][columnIndex] == Blue) {
          leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Blue;
        }
        if (LEDMatrix[rowIndex][columnIndex] == Red) {
          leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Red;
        }
        if (LEDMatrix[rowIndex][columnIndex] == Purple) {
          leds[(columnIndex + 1)*displayNumberOfRows - rowIndex - 1] = CRGB::Purple;
        }
      }
      // If we're on an uneven column, we do a mathematical trick to invert it
      else if (columnIndex % 2 == 0) {
        if (LEDMatrix[rowIndex][columnIndex] == Black) {
          leds[columnIndex * displayNumberOfRows + rowIndex] = CRGB::Black;
        }
        if (LEDMatrix[rowIndex][columnIndex] == White) {
          leds[columnIndex * displayNumberOfRows + rowIndex] = CRGB::White;
        }
        if (LEDMatrix[rowIndex][columnIndex] == Green) {
          leds[columnIndex * displayNumberOfRows + rowIndex] = CRGB::Green;
        }
        if (LEDMatrix[rowIndex][columnIndex] == Blue) {
          leds[columnIndex * displayNumberOfRows + rowIndex] = CRGB::Blue;
        }
        if (LEDMatrix[rowIndex][columnIndex] == Red) {
          leds[columnIndex * displayNumberOfRows + rowIndex] = CRGB::Red;
        }
        if (LEDMatrix[rowIndex][columnIndex] == Purple) {
          leds[columnIndex * displayNumberOfRows + rowIndex] = CRGB::Purple;
        }
      }
    }
  }

  // Display the matrix physically
  FastLED.show();
}

// We update the digital display of the LED matrix
void digitalOutputDisplay() {
  Serial.print("\n We print digitally the current theoritical state of the LED Matrix : \n");
  for (byte i = 0; i < displayNumberOfRows; i++) {
    for (byte j = 0; j < displayNumberOfColumns; j++) {
      Serial.print(LEDMatrix[i][j]);
      if (j < displayNumberOfColumns - 1) {
        Serial.print(", ");
      }
      else {
        Serial.print("\n");
      }
    }
  }
}
