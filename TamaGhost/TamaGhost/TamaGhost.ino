//
// TamaGhost
//
// Description of the project
// Developed with [embedXcode](http://embedXcode.weebly.com)
//
// Author 		Alexandra Olivier
// 				Alexandra Olivier
//
// Date			1/16/16 3:13 PM
// Version		<#version#>
//
// Copyright	© Alexandra Olivier, 2016
// Licence		<#licence#>
//
// See         ReadMe.txt for references
//


// Core library for code-sense - IDE-based
#if defined(WIRING) // Wiring specific
#   include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#   include "WProgram.h"
#elif defined(MPIDE) // chipKIT specific
#   include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#   include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad specific
#   include "Energia.h"
#elif defined(LITTLEROBOTFRIENDS) // LittleRobotFriends specific
#   include "LRF.h"
#elif defined(MICRODUINO) // Microduino specific
#   include "Arduino.h"
#elif defined(SPARK) || defined(PARTICLE) // Particle / Spark specific
#   include "Arduino.h"
#elif defined(TEENSYDUINO) // Teensy specific
#   include "Arduino.h"
#elif defined(REDBEARLAB) // RedBearLab specific
#   include "Arduino.h"
#elif defined(ESP8266) // ESP8266 specific
#   include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#   include "Arduino.h"
#else // error
#   error Platform not defined
#endif // end IDE

// Include application, user and local libraries
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library


#define OE   9
#define LAT 10
#define CLK 11
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

// Input a value 0 to 24 to get a color value.
// The colours are a transition r - g - b - back to r.
uint16_t Wheel(byte WheelPos) {
if(WheelPos < 8) {
return matrix.Color333(7 - WheelPos, WheelPos, 0);
} else if(WheelPos < 16) {
WheelPos -= 8;
return matrix.Color333(0, 7-WheelPos, WheelPos);
} else {
WheelPos -= 16;
return matrix.Color333(0, WheelPos, 7 - WheelPos);
}
}

void setup() {

matrix.begin();

// draw a pixel in solid white
matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
delay(500);

// fix the screen with green
matrix.fillRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(0, 7, 0));
delay(500);

// draw a box in yellow
matrix.drawRect(0, 0, matrix.width(), matrix.height(), matrix.Color333(7, 7, 0));
delay(500);

// draw an 'X' in red
matrix.drawLine(0, 0, matrix.width()-1, matrix.height()-1, matrix.Color333(7, 0, 0));
matrix.drawLine(matrix.width()-1, 0, 0, matrix.height()-1, matrix.Color333(7, 0, 0));
delay(500);

// draw a blue circle
matrix.drawCircle(10, 10, 10, matrix.Color333(0, 0, 7));
delay(500);

// fill a violet circle
matrix.fillCircle(40, 21, 10, matrix.Color333(7, 0, 7));
delay(500);

// fill the screen with 'black'
matrix.fillScreen(matrix.Color333(0, 0, 0));

// draw some text!
matrix.setTextSize(1);     // size 1 == 8 pixels high
matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves

matrix.setCursor(8, 0);    // start at top left, with 8 pixel of spacing
uint8_t w = 0;
char *str = "Tamagotchi";
for (w=0; w<8; w++) {
matrix.setTextColor(Wheel(w));
matrix.print(str[w]);
}
matrix.setCursor(2, 8);    // next line
for (w=8; w<18; w++) {
matrix.setTextColor(Wheel(w));
matrix.print(str[w]);
}
matrix.println();
//matrix.setTextColor(matrix.Color333(4,4,4));
//matrix.println("Industries");
matrix.setTextColor(matrix.Color333(7,7,7));
matrix.println("LED MATRIX!");

// print each letter with a rainbow color
matrix.setTextColor(matrix.Color333(7,0,0));
matrix.print('3');
matrix.setTextColor(matrix.Color333(7,4,0));
matrix.print('2');
matrix.setTextColor(matrix.Color333(7,7,0));
matrix.print('x');
matrix.setTextColor(matrix.Color333(4,7,0));
matrix.print('6');
matrix.setTextColor(matrix.Color333(0,7,0));
matrix.print('4');
matrix.setCursor(34, 24);
matrix.setTextColor(matrix.Color333(0,7,7));
matrix.print("*");
matrix.setTextColor(matrix.Color333(0,4,7));
matrix.print('R');
matrix.setTextColor(matrix.Color333(0,0,7));
matrix.print('G');
matrix.setTextColor(matrix.Color333(4,0,7));
matrix.print("B");
matrix.setTextColor(matrix.Color333(7,0,4));
matrix.println("*");


// whew!
}

void loop() {
// do nothing
}


