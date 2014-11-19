#include <OctoWS2811.h>
#include <Wire.h>

//The Arduino Wire library uses the 7-bit version of the address, so the code example uses 0x70 instead of the 8-bit 0xE0
#define Sensor1Address byte(0x70)
#define Sensor2Address byte(0x71)
//The sensors ranging command has a value of 0x51
#define RangeCommand byte(0x51)
//These are the two commands that need to be sent in sequence to change the sensor address
#define ChangeAddressCommand1 byte(0xAA)
#define ChangeAddressCommand2 byte(0xA5)

// RGB colours
#define GREEN	0x00FF00
#define BLACK	0x000000
#define BROWN	0x8B4513
#define WHITE	0x808080
#define LGRAY	0x404040
#define DGRAY	0x202020
#define BLUE	0x0000FF
#define RED	0xFF0000
#define YELLOW  0xFFFF00
#define PURPLE  0xFF00FF
#define ORANGE  0xFF2500
#define ORANGE2 0xFF5500

// ASCII alphabet
uint8_t ascii_alph[26][6] = {
  {0x18, 0x24, 0x7E, 0x42, 0x42, 0x42},  // A
  {0x7C, 0x42, 0x7C, 0x42, 0x42, 0x7C},  // B
  {0x3E, 0x40, 0x40, 0x40, 0x40, 0x3E},  // C
  {0x7C, 0x42, 0x42, 0x42, 0x42, 0x7C},  // D
  {0x7E, 0x40, 0x7C, 0x40, 0x40, 0x7E},  // E
  {0x7E, 0x40, 0x7E, 0x40, 0x40, 0x40},  // F
  {0x3E, 0x40, 0x40, 0x46, 0x42, 0x3E},  // G
  {0x42, 0x42, 0x7E, 0x42, 0x42, 0x42},  // H
  {0x7E, 0x18, 0x18, 0x18, 0x18, 0x7E},  // I
  {0x7E, 0x08, 0x08, 0x08, 0x48, 0x70},  // J
  {0x44, 0x48, 0x70, 0x48, 0x44, 0x42},  // K
  {0x40, 0x40, 0x40, 0x40, 0x40, 0x7E},  // L
  {0x24, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A},  // M
  {0x42, 0x62, 0x52, 0x4A, 0x46, 0x42},  // N
  {0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C},  // O
  {0x7C, 0x42, 0x42, 0x7C, 0x40, 0x40},  // P
  {0x3C, 0x42, 0x42, 0x4A, 0x46, 0x3E},  // Q
  {0x7C, 0x42, 0x42, 0x7C, 0x44, 0x42},  // R
  {0x3E, 0x40, 0x3C, 0x02, 0x04, 0x78},  // S
  {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18},  // T
  {0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},  // U
  {0x42, 0x42, 0x42, 0x42, 0x24, 0x18},  // V
  {0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x24},  // W
  {0x42, 0x24, 0x18, 0x18, 0x24, 0x42},  // X
  {0x42, 0x24, 0x18, 0x18, 0x18, 0x18},  // Y
  {0x7E, 0x04, 0x08, 0x10, 0x20, 0x7E}  // Z
};

// ASCII numbers
uint8_t ascii_num[10][6] = {
  {0x3C, 0x46, 0x4A, 0x52, 0x62, 0x3C},  // 0
  {0x08, 0x18, 0x28, 0x08, 0x08, 0x08},  // 1
  {0x3C, 0x42, 0x04, 0x08, 0x10, 0x3E},  // 2
  {0x7C, 0x02, 0x7C, 0x02, 0x02, 0x7C},  // 3
  {0x04, 0x0C, 0x14, 0x3E, 0x04, 0x04},  // 4
  {0x7E, 0x40, 0x7E, 0x02, 0x02, 0x7C},  // 5
  {0x3C, 0x42, 0x40, 0x7E, 0x42, 0x3C},  // 6
  {0x7E, 0x02, 0x04, 0x08, 0x10, 0x20},  // 7
  {0x3C, 0x42, 0x3C, 0x42, 0x42, 0x3C},  // 8
  {0x3C, 0x42, 0x3E, 0x02, 0x42, 0x3C}  // 9
};

const int ledsPerStrip = 168;
const int ledsPerLine = 56;
const int linesPerStrip = 3;

DMAMEM int displayMemory[ledsPerStrip*8];
int drawingMemory[ledsPerStrip*8];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

const int buttonPin = 23;

volatile int animation;  // Select which animation
const int numAnimations = 6;  // How many different animations exist
const int numPositions = 28;  // How many positions for bitmaps exist

//double ratio0;
//int side0;	// 1 = left, 2 = right
//int viewRange = 90;	// Maximum distance (cm) for object in view

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  pinMode(buttonPin, INPUT_PULLUP);
  
  leds.begin();
  
  animation = 0;

//  // Get initial sensor readings
//  takeRangeReading(Sensor1Address); //Tell the sensor to perform a ranging cycle
//  delay(60); //Wait for sensor to finish
//  word rangeLeft = requestRange(Sensor1Address); //Get the range from the sensor
//  takeRangeReading(Sensor2Address); //Tell the sensor to perform a ranging cycle
//  delay(60); //Wait for sensor to finish
//  word rangeRight = requestRange(Sensor2Address); //Get the range from the sensor
//
//  if (rangeLeft >= rangeRight) {	// Object on right side
//    ratio0 = (double)rangeLeft / (double)rangeRight;
//    side0 = 2;
//  } else {	// Object on left side
//    ratio0 = (double)rangeRight / (double)rangeLeft;
//    side0 = 1;
//  }

  // Change sensor address
  /* Wire.beginTransmission(0x38); //Start addressing */
  /* Wire.write(ChangeAddressCommand1); //send range command */
  /* delay(60); */
  /* Wire.write(ChangeAddressCommand2); //send range command */
  /* delay(60); */
  /* Wire.write(0xE2); //send range command */
  /* Wire.endTransmission(); //Stop and do something else now */
}

void loop() {
  switch (animation) {
    case 0:  // Write "ICEWIRE MAKERSPACE" or anything
      clearAll();
      drawstr("ICEWIRE", 0, BLUE);
      drawstr("MAKER", 15, ORANGE);
      drawstr("SPACE", 22, ORANGE2);
//      clearAll();
//      drawstr("NAME", 7, RED);
      leds.show();
      checkAndChange();
      break;
    case 1:  // Write the TMNT names in sequence
      clearAll();
      drawstr("LEO", 0, BLUE);
      leds.show();
      delay(500);
      clearAll();
      drawstr("DONNIE", 7, PURPLE);
      leds.show();
      delay(500);
      clearAll();
      drawstr("RAPH", 14, RED);
      leds.show();
      delay(500);
      clearAll();
      drawstr("MIKEY", 21, ORANGE);
      leds.show();
      delay(500);
      checkAndChange();
      break;
    case 2:  // Draw a smiley face at random positions
      randomsmiley();
      checkAndChange();
      break;
    case 3: {  // Draw a circle that gets bigger as distance decreases
      // Get sensor reading
      takeRangeReading(Sensor1Address);  // Tell the sensor to perform a ranging cycle
      delay(60);  // Wait for sensor to finish
      word rangeLeft = requestRange(Sensor1Address);  // Get the range from the sensor
  
      Serial.print("RangeLeft: ");
      Serial.println(rangeLeft);
  
      int farthest = 270;  // Farthest distance in cm
      int closest = 60;  // Closest distance in cm
      int increment = (farthest - closest) / 8;
      int circlesize;
      if (rangeLeft >= farthest) {
        circlesize = 0;
      } else if (rangeLeft >= (farthest - increment)) {
        circlesize = 1;
      } else if (rangeLeft >= (farthest - increment*2)) {
        circlesize = 2;
      } else if (rangeLeft >= (farthest - increment*3)) {
        circlesize = 3;
      } else if (rangeLeft >= (farthest - increment*4)) {
        circlesize = 4;
      } else if (rangeLeft >= (farthest - increment*5)) {
        circlesize = 5;
      } else if (rangeLeft >= (farthest - increment*6)) {
        circlesize = 6;
      } else if (rangeLeft >= (farthest - increment*7)) {
        circlesize = 7;
      } else {
        circlesize = 8;
      }
      drawcircle(circlesize);
      
      checkAndChange();
      break;
    }
    case 4: {  // Draw a splash that gets bigger with a fast decrease in distance
      drawsplash(0);
      
      // Get sensor reading
      takeRangeReading(Sensor1Address);  // Tell the sensor to perform a ranging cycle
      delay(60);  // Wait for sensor to finish
      word range0 = requestRange(Sensor1Address);  // Get the range from the sensor
      
      delay(50);  // Delay between range readings to get change in distance
      
      // Get sensor reading
      takeRangeReading(Sensor1Address);  // Tell the sensor to perform a ranging cycle
      delay(60);  // Wait for sensor to finish
      word range1 = requestRange(Sensor1Address);  // Get the range from the sensor
      
      int rangediff = range0 - range1;
      if (rangediff > 8 && rangediff < 20) {
        drawsplash(4);
        delay(2000);
      } else if (rangediff > 6 && rangediff < 20) {
        drawsplash(3);
        delay(2000);
      } else if (rangediff > 4 && rangediff < 20) {
        drawsplash(2);
        delay(2000);
      } else if (rangediff > 2 && rangediff < 20) {
        drawsplash(1);
        delay(2000);
      }
      Serial.print("rangediff: ");
      Serial.println(rangediff);
      
      checkAndChange();
      break;
    }
    case 5: {  // Display the sensor's readings
      // Get sensor reading
      takeRangeReading(Sensor1Address);  // Tell the sensor to perform a ranging cycle
      delay(60);  // Wait for sensor to finish
      word range = requestRange(Sensor1Address);  // Get the range from the sensor
      clearAll();
      drawstr("SENSOR", 0, BLUE);
      drawnum(range, 17, YELLOW);
      drawstr("CM", 19, YELLOW);
      leds.show();
      checkAndChange();
      break;
    }
    default:
      break;
  }
}

//Commands the sensor to take a range reading
void takeRangeReading(uint8_t sensorAddress) {
	Wire.beginTransmission(sensorAddress); //Start addressing
	Wire.write(RangeCommand); //send range command
	Wire.endTransmission(); //Stop and do something else now
}

//Returns the last range that the sensor determined in its last ranging cycle in centimeters. Returns 0 if there is no communication.
word requestRange(uint8_t sensorAddress) {
	Wire.requestFrom(sensorAddress, byte(2));
	if (Wire.available() >= 2) { //Sensor responded with the two bytes
		byte HighByte = Wire.read(); //Read the high byte back
		byte LowByte = Wire.read(); //Read the low byte back
		//		word range = word(HighByte, LowByte); //Make a 16-bit word out of the two bytes for the range
		word range = (HighByte << 8) | LowByte;
		return range;
	} else {
		return 0; //Else nothing was received, return 0
	}
}

// Set all LEDs to black
void clearAll(void) {
  for (int i = 0; i < ledsPerStrip*8; i++) {
    leds.setPixel(i, BLACK);
  }
}

// Draw a 8x6 bitmap at position 0-27
void drawbmp(uint8_t matrix[], int pos, int color) {
  int p;
  int posmod = pos % 7;
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 8; j++) {
      p = (i * 168) + (j + (posmod*8)) + ((pos/7)*336);
      if (matrix[i*3] & (0x80 >> j)) {  // Check bit
        leds.setPixel(p, color);
      } else {
        leds.setPixel(p, BLACK);
      }
    }
    for (int j = 0; j < 8; j++) {
      p = (111 + (i*3*56)) - (j + (posmod*8)) + ((pos/7)*336);
      if (matrix[(i*3)+1] & (0x80 >> j)) {  // Check bit
        leds.setPixel(p, color);
      } else {
        leds.setPixel(p, BLACK);
      }
    }
    for (int j = 0; j < 8; j++) {
      p = (112 + i*3*56) + (j + (posmod*8)) + ((pos/7)*336);
      if (matrix[(i*3)+2] & (0x80 >> j)) {  // Check bit
        leds.setPixel(p, color);
      } else {
        leds.setPixel(p, BLACK);
      }
    }
  }
}

// Draw a smiley at random positions every 1 second
void randomsmiley(void) {
  uint8_t bmp[] = {0x24, 0x24, 0x00, 0x81, 0x42, 0x3C};
  clearAll();
  drawbmp(bmp, (int)random(0, 28), GREEN);
  leds.show();
  delay(1000);
}

// Draw a circle of size s (0-8)
void drawcircle(int s) {
  clearAll();
  int color;
  switch (s) {
    case 0: {
      color = GREEN;
      uint8_t c1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x18};
      uint8_t c2[] = {0x18, 0x00, 0x00, 0x00, 0x00, 0x00};
      drawbmp(c1, 10, color);
      drawbmp(c2, 17, color);
      break;
    }
    case 1: {
      color = BLUE;
      uint8_t c1[] = {0x00, 0x00, 0x00, 0x00, 0x18, 0x24};
      uint8_t c2[] = {0x24, 0x18, 0x00, 0x00, 0x00, 0x00};
      drawbmp(c1, 10, color);
      drawbmp(c2, 17, color);
      break;
    }
    case 2: {
      color = BLUE;
      uint8_t c1[] = {0x00, 0x00, 0x00, 0x18, 0x24, 0x42};
      uint8_t c2[] = {0x42, 0x24, 0x18, 0x00, 0x00, 0x00};
      drawbmp(c1, 10, color);
      drawbmp(c2, 17, color);
      break;
    }
    case 3: {
      color = BLUE;
      uint8_t c1[] = {0x00, 0x00, 0x3C, 0x42, 0x81, 0x81};
      uint8_t c2[] = {0x81, 0x81, 0x42, 0x3C, 0x00, 0x00};
      drawbmp(c1, 10, color);
      drawbmp(c2, 17, color);
      break;
    }
    case 4: {
      color = BLUE;
      uint8_t c1[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01};
      uint8_t c2[] = {0x00, 0x3C, 0x42, 0x81, 0x00, 0x00};
      uint8_t c3[] = {0x00, 0x00, 0x00, 0x00, 0x80, 0x80};
      uint8_t c4[] = {0x01, 0x01, 0x00, 0x00, 0x00, 0x00};
      uint8_t c5[] = {0x00, 0x00, 0x81, 0x42, 0x3C, 0x00};
      uint8_t c6[] = {0x80, 0x80, 0x00, 0x00, 0x00, 0x00};
      drawbmp(c1, 9, color);
      drawbmp(c2, 10, color);
      drawbmp(c3, 11, color);
      drawbmp(c4, 16, color);
      drawbmp(c5, 17, color);
      drawbmp(c6, 18, color);
      break;
    }
    case 5: {
      color = BLUE;
      uint8_t c1[] = {0x00, 0x00, 0x01, 0x02, 0x02, 0x02};
      uint8_t c2[] = {0x7E, 0x81, 0x00, 0x00, 0x00, 0x00};
      uint8_t c3[] = {0x00, 0x00, 0x80, 0x40, 0x40, 0x40};
      uint8_t c4[] = {0x02, 0x02, 0x02, 0x01, 0x00, 0x00};
      uint8_t c5[] = {0x00, 0x00, 0x00, 0x00, 0x81, 0x7E};
      uint8_t c6[] = {0x40, 0x40, 0x40, 0x80, 0x00, 0x00};
      drawbmp(c1, 9, color);
      drawbmp(c2, 10, color);
      drawbmp(c3, 11, color);
      drawbmp(c4, 16, color);
      drawbmp(c5, 17, color);
      drawbmp(c6, 18, color);
      break;
    }
    case 6: {
      color = BLUE;
      uint8_t c1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x7E};
      uint8_t c2[] = {0x00, 0x01, 0x02, 0x04, 0x04, 0x04};
      uint8_t c3[] = {0x81, 0x00, 0x00, 0x00, 0x00, 0x00};
      uint8_t c4[] = {0x00, 0x80, 0x40, 0x20, 0x20, 0x20};
      uint8_t c5[] = {0x04, 0x04, 0x04, 0x02, 0x01, 0x00};
      uint8_t c6[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x81};
      uint8_t c7[] = {0x20, 0x20, 0x20, 0x40, 0x80, 0x00};
      uint8_t c8[] = {0x7E, 0x00, 0x00, 0x00, 0x00, 0x00};
      drawbmp(c1, 3, color);
      drawbmp(c2, 9, color);
      drawbmp(c3, 10, color);
      drawbmp(c4, 11, color);
      drawbmp(c5, 16, color);
      drawbmp(c6, 17, color);
      drawbmp(c7, 18, color);
      drawbmp(c8, 24, color);
      break;
    }
    case 7: {
      color = BLUE;
      uint8_t c1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
      uint8_t c2[] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0x00};
      uint8_t c3[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x80};
      uint8_t c4[] = {0x02, 0x04, 0x08, 0x08, 0x08, 0x08};
      uint8_t c5[] = {0x40, 0x20, 0x10, 0x10, 0x10, 0x10};
      uint8_t c6[] = {0x08, 0x08, 0x08, 0x08, 0x04, 0x02};
      uint8_t c7[] = {0x10, 0x10, 0x10, 0x10, 0x20, 0x40};
      uint8_t c8[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
      uint8_t c9[] = {0x00, 0xFF, 0x00, 0x00, 0x00, 0x00};
      uint8_t c10[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00};
      drawbmp(c1, 2, color);
      drawbmp(c2, 3, color);
      drawbmp(c3, 4, color);
      drawbmp(c4, 9, color);
      drawbmp(c5, 11, color);
      drawbmp(c6, 16, color);
      drawbmp(c7, 18, color);
      drawbmp(c8, 23, color);
      drawbmp(c9, 24, color);
      drawbmp(c10, 25, color);
      break;
    }
    case 8: {
      color = RED;
      uint8_t c1[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02};
      uint8_t c2[] = {0x00, 0x00, 0x00, 0xFF, 0x00, 0x00};
      uint8_t c3[] = {0x00, 0x00, 0x00, 0x00, 0x80, 0x40};
      uint8_t c4[] = {0x07, 0x0B, 0x11, 0x10, 0x10, 0x10};
      uint8_t c5[] = {0x00, 0x80, 0xC0, 0xE0, 0x70, 0x38};
      uint8_t c6[] = {0x20, 0x10, 0x08, 0x08, 0x08, 0x08};
      uint8_t c7[] = {0x10, 0x10, 0x10, 0x10, 0x08, 0x04};
      uint8_t c8[] = {0x1C, 0x0E, 0x07, 0x03, 0x01, 0x00};
      uint8_t c9[] = {0x08, 0x08, 0x08, 0x88, 0xD0, 0xE0};
      uint8_t c10[] = {0x02, 0x01, 0x00, 0x00, 0x00, 0x00};
      uint8_t c11[] = {0x00, 0x00, 0xFF, 0x00, 0x00, 0x00};
      uint8_t c12[] = {0x40, 0x80, 0x00, 0x00, 0x00, 0x00};
      drawbmp(c1, 2, color);
      drawbmp(c2, 3, color);
      drawbmp(c3, 4, color);
      drawbmp(c4, 9, color);
      drawbmp(c5, 10, color);
      drawbmp(c6, 11, color);
      drawbmp(c7, 16, color);
      drawbmp(c8, 17, color);
      drawbmp(c9, 18, color);
      drawbmp(c10, 23, color);
      drawbmp(c11, 24, color);
      drawbmp(c12, 25, color);
      break;
    }
    default:
      break;
  }
  leds.show();
}

// Draw a splash of size s (0-4)
void drawsplash(int s) {
  clearAll();
  int color = BROWN;
  switch (s) {
    case 0: {
      uint8_t m1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x18};
      uint8_t m2[] = {0x18, 0x00, 0x00, 0x00, 0x00, 0x00};
      drawbmp(m1, 10, color);
      drawbmp(m2, 17, color);
      break;
    }
    case 1: {
      uint8_t m[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      drawbmp(m, 10, color);
      drawbmp(m, 17, color);
      break;
    }
    case 2: {
      uint8_t m[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      drawbmp(m, 9, color);
      drawbmp(m, 10, color);
      drawbmp(m, 11, color);
      drawbmp(m, 16, color);
      drawbmp(m, 17, color);
      drawbmp(m, 18, color);
      break;
    }
    case 3: {
      uint8_t m[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      for (int i = 2; i < 26; i += 4) {
        for (int j = 0; j < 3; j++, i++) {
          drawbmp(m, i, color);
        }
      }
      break;
    }
    case 4: {
      uint8_t m[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      for (int i = 0; i < numPositions; i++) {
        drawbmp(m, i, color);
      }
      break;
    }
    default:
      break;
  }
  leds.show();
}

// Change animation if button is pressed
void checkAndChange(void) {
  if (digitalRead(buttonPin) == LOW) {  // Button is pressed down
    delay(50);  // Debouncing
    changeAnimation();
    while (digitalRead(buttonPin) == LOW);  // Wait for release
    delay(50);  // Debouncing
  }
}

// Change to next animation
void changeAnimation(void) {
  animation = (animation + 1) % numAnimations;
  clearAll();
  leds.show();
}

// Write a string on the display, given starting position and color
void drawstr(const char *str, int pos, int color) {
  while (*str) {
    drawbmp(ascii_alph[(*str++)-0x41], pos, color);
    pos++;
  }
}

// Write a positive number on the display that is right-justified, given the rightmost position and color
void drawnum(int n, int pos, int color) {
  int digits;
  int tmp = n;
  // Count the digits in the number
  for (digits = 0; tmp > 0; digits++) {
    tmp /= 10;
  }
  while (digits > 0) {
    drawbmp(ascii_num[n%10], pos, color);
    digits--;
    pos--;
    n /= 10;
  }
}
