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
#include <noise.h>
#include <TamaBitmaps.h>
#include <QueueArray.h>


#define OE   9
#define LAT 10
#define CLK 11
#define A   A0
#define B   A1
#define C   A2
#define D   A3

#define LOW_X 0
#define HIGH_X 64
#define LOW_Y 0
#define HIGH_Y 32

#define NUM_GHOSTS 10

int curX = 32;
int curY = 0;
int y_sign = 1;
int x_sign = 1;

unsigned long prevMillis;

int fadeIncrement = 2;
int fadeCount = 255;
bool fadeMonster = false;

int currentGhostType = 0;
float ghostTypeTimer = 0;
int ghostTypeInterval = 30 * 1000;

int currentRainType = 0;
int currentRainMode = 0;
uint16_t currentRainColor = 0;
float rainDuration = 20 * 1000;
float rainTimer = 0;
bool inRain = false;

float noRainTimer = 0;
float noRainDuration = 60;


int transitionCycles = 31;
int transitionCounter = 0;
int transitionDirection = 1;
bool inTransition = false;

struct position {
    
    int x, y;
    
};

QueueArray <position> ghostPositions;
QueueArray<position> rainPositions;

int ghostCounter = 0;

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true, 64);

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

float x1,y1,x2,y2,persistence;
int octaves;

uint16_t c = matrix.Color333(7,7,7);
uint16_t b = matrix.Color333(0,0,0);
uint16_t p = matrix.Color333(6,3,3);
uint16_t g = matrix.Color333(5,7,5);

void simpleBuildBody(int x, int y, uint16_t gc = c){
    matrix.fillCircle(x, y, 10, gc);
}


void createRain( int numDrops ){
    
    for (int i = 0; i < numDrops; i++){
        position p = position {
            .x = random(LOW_X, HIGH_X),
            .y = LOW_Y - random(10,100)
        };
        
        rainPositions.push( p );
    }
}


void drawCloud(int x, int y, int r, uint16_t red, uint16_t green, uint16_t blue){
    
    // draw a cloud.
    unsigned long curTime = millis();
    
    uint8_t seconds = int(curTime / 1000.0f);
    uint8_t remainder = 255*((curTime - seconds*1000) / 1000.0f);
    
    uint16_t noiseGen = seconds;
    noiseGen = noiseGen << 8;
    noiseGen += remainder;
    
    // Update position using noise
    float xn = inoise8( curX, noiseGen );
    float yn = inoise8( curY, noiseGen );

        for (int i = 0; i < r; i++){
    
            for (int k = 0; k < r; k++){
    
                int adjI = x + i;
                int adjK = y + k;
                
                
                // compute distance from x boundary
                int dx = min( abs( adjI - x), abs( adjI - (x+r)));
                
                // compute distance from y boundary
                int dy = min( abs( adjK - y), abs( adjK - (y+r)));
                
                float distX = (float) (map((float) dx, 0.0f, (float) r/2.0f, 0.0f, 255.0f) )/ 255.0f;
                float distY = (float) (map((float) dy, 0.0f, (float) r/2.0f, 0.0f, 255.0f) )/ 255.0f;
            
                uint16_t noiseGen = seconds + i;
    
                uint16_t noiseGen_b = k + int(curTime / 1000.0f);
                noiseGen_b = noiseGen_b << 8;
    
                noiseGen = noiseGen << 8;
                noiseGen += remainder;
    
                float in = inoise8( noiseGen, noiseGen_b) / 255.0f;
                
                float brightness = 1.0;
                
                uint16_t newRed = brightness * red * distX*distY*in;
                uint16_t newGreen =  brightness *green * distX*distY*in;
                uint16_t newBlue =  brightness *blue * distX*distY*in;
    
                if (((newRed < 0) && (red < 1)) || ((newGreen < 0) && (green > 1)) || ((newBlue < 0) && (blue > 1))){
                    
                    newRed = 0;
                    newGreen = 0;
                    newBlue = 0;
                    
                }
                
                
                uint16_t rc = matrix.Color888(newRed, newGreen, newBlue);
    
    
                matrix.drawPixel( x + i, y + k, rc);
                
            }
        }
    
    
    
}

void drawCherry(int x, int y){
    
    uint16_t r = matrix.Color333(7,0,0);
    uint16_t g = matrix.Color333(0,7,0);
    
    matrix.drawLine(x-2, y-2, x, y-6, g);
    matrix.drawLine(x+2, y-2, x, y-6, g);

    // leaf
    matrix.fillTriangle(x,y-6, x+5,y-6,x+3,y-8,g);

    matrix.fillCircle(x - 2, y, 1, r);
    matrix.fillCircle(x + 2, y, 1, r);
    
}

void drawPizza(int x, int y){
    
    uint16_t r = matrix.Color333(7,0,0);
    uint16_t yell = matrix.Color333(7,5,0);
    uint16_t o = matrix.Color333(7,3,0);
    
    matrix.fillTriangle( x, y, x + 10, y-5, x + 5, y-10, yell);
    matrix.drawLine(x + 10, y-5, x + 5, y-10,o);
    matrix.drawLine(x + 10, y-4, x + 4, y-10,o);
    
    matrix.drawPixel(x+1,y-1,r);
    matrix.drawPixel(x+1,y-2,r);
    matrix.drawPixel(x+2,y-2,r);
    matrix.drawPixel(x+2,y-3,r);
    
    matrix.drawPixel(x+6,y-3,r);
    matrix.drawPixel(x+5,y-3,r);
    matrix.drawPixel(x+4,y-2,r);
    
    matrix.fillCircle(x + 5, y-6, 1, r);
}

void drawHeart(int x, int y, uint16_t color = matrix.Color333(7,0,0)){

    matrix.drawBitmap( x, y, heart, 16, 16, color);
}

void drawCross(int x, int y, uint16_t color = matrix.Color333(6,3,3)){

    matrix.drawBitmap( x, y, heart, 16, 16, c);
}

void drawPoop(int x, int y, uint16_t color = matrix.Color333(3,7,3)){
    
    matrix.drawBitmap( x, y, heart, 16, 16, color);
}

void drawRain(int x, int y, int type, uint16_t color){
    
    if (currentRainMode == 1){
        currentRainColor = color;
    }
    
    if (type == 0){
        drawCherry(x,y);
    }else if (type == 1){
        drawPizza(x,y);
    }else if (type == 2){
        drawHeart(x, y, currentRainColor);
    }else if (type == 3){
        drawCross(x, y, currentRainColor);
    }else if (type == 4){
        drawPoop(x, y, currentRainColor);
    }else{
        drawPizza(x, y);
    }
}

uint16_t getMultiColor(int index){
    
    // num colors =
    // pink, blue, green, white
    if (index % 4 == 0 ){
        
        return matrix.Color333(7,7,7);
        
    }else if (index % 4 == 0 ){
        
        return matrix.Color333(7,4,4);
        
    }else if (index % 4 == 0 ){

        return matrix.Color333(4,7,4);
        
    }else{
        
        return matrix.Color333(4,4,7);
        
    }
}



void updateRain(int type){
    
    int numRain = rainPositions.count();
    
    int i = 0;
    
    while ( i < numRain ){
        
        int ind = (NUM_GHOSTS+NUM_GHOSTS*0.8) - i;
        
        position thisPos = rainPositions.pop();
        
        drawRain(thisPos.x, thisPos.y, type, getMultiColor(ind));

        position newPos = {
            
            .x = thisPos.x,
            .y = thisPos.y + 2
        };
        
        if (newPos.y > (HIGH_Y + 10)){
            
            if (inRain){
                newPos.y = LOW_Y - random(10,20);
            }
        }
        
        // add back on
        rainPositions.push( newPos );
        
        i++;
        
    }
}



void drawGhosts(void (*f)(int,int,uint16_t) ){
    
    int i = 0;
    int numGhosts = ghostPositions.count();
    

    while ( i < NUM_GHOSTS ){
        
        int ind = (NUM_GHOSTS+NUM_GHOSTS*0.8) - i;
        
        position thisPos = ghostPositions.pop();
        
        float dev = 255.0f * ((float)(i+1) / (float)( NUM_GHOSTS+NUM_GHOSTS*0.8));
        
        uint16_t thisC = matrix.Color888( dev, dev, dev);
        
        // Draw the thing.
        f( thisPos.x, thisPos.y, thisC);
        
        // add back on
        ghostPositions.push( thisPos );
 
        i++;
        
    }
}


void selectRainColor(){
    
    currentRainMode = 0; // use color
    
    int type = currentRainType;
    
    // Cherry, don't change
    if (type == 2){

        // pick random number
        int _rand = random(0,11);
        
        // red
        if (_rand <= 3){
            
            currentRainColor = matrix.Color333(7, 0, 0);
         
        // pink
        }else if (_rand <= 6){
            
            currentRainColor = matrix.Color333(7, 4, 4);
            
        // multi
        }else{
            
            currentRainMode = 1;
        }
        
    // CROSS
    }else if (type == 3){
        
        // pick random number
        int _rand = random(0,11);
        
        // pink
        if (_rand <= 2){
            
            currentRainColor = matrix.Color333(7, 4, 4);
            
        // blue
        }else if (_rand <= 5){
            
            currentRainColor = matrix.Color333(4, 4, 7);
            
        // white
        }else if (_rand <= 7){
            
            currentRainColor = matrix.Color333(7,7,7);
        }else{
            currentRainMode = 1;
        }
        
        
    // poopy
    }else if (type == 4){
        
        // green red
        currentRainColor = matrix.Color333(3,7,3);

    }
}



void startRain(){
    
    
    noRainTimer = 0;
    
    rainDuration = (float) random(5,20) * 1000;
    rainTimer = 0;
    inRain = true;
    currentRainType = (int) floor(random(0,5));
    selectRainColor();
    
    
    Serial.println("rain: " + currentRainType);
    
}

void setup() {
    
    currentRainColor = matrix.Color333(7,7,7);
    
    createRain(10);

    Serial.begin(9600);
    Serial.println("Hello world");
    
    matrix.begin();

    // fill the screen with 'black'
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    delay(500);

    Serial.begin(9600);
    Serial.println("hello color");

    // Choose random duration
    
    startRain();
    

}



void stopRain(){
    
    inRain = false;
    rainTimer = 0;
    noRainTimer = 0;
    noRainDuration = 1000 * random(30,60);
    
    
}

void changeGhost(){
    
    ghostTypeTimer = 0;
    ghostTypeInterval = random(60,90) * 1000;
    
    // Change current monster
    currentGhostType = (int) floor(random(0,4));
    
}


void startTransition(){
    
    inTransition = true;
    transitionDirection = 1;

}

void stopTransition(){
    
    
    inTransition = false;
    transitionCounter = 0;
    transitionDirection = 1;
    
}

void updateTransition(){
    
    if ( transitionCounter > transitionCycles){
        
        transitionCounter = transitionCycles;
        transitionDirection = -1;
        
        changeGhost();
        
        // Draw sunset with counter number of lines
        drawGradient( transitionCounter + 1);

        
    }else if (transitionCounter < 0){
        
        // DONE, change monster
        stopTransition();
        
    }else{
    
        // Draw sunset with counter number of lines
        drawGradient( transitionCounter + 1);
        
    }

}

void updateTimers(){
    
    // Calculate delta time for this frame
    unsigned long curMillis = millis();
    float dt = curMillis - prevMillis;
    
    // Update each timer
    ghostTypeTimer += dt;
    
    if (ghostTypeTimer >= ghostTypeInterval){
        startTransition();
    }
    
    if (inTransition){
        transitionCounter++;
        updateTransition();
    }
    
    if (inRain){
        rainTimer += dt;
        
        if (rainTimer >= rainDuration){
            stopRain();
        }
    }else{
        
        noRainTimer += dt;
        
        if (noRainTimer >= noRainDuration){
            
            startRain();
        }
    }
    
    
    
    // At end
    prevMillis = curMillis;
}



void drawMonster(int type, int red, int green, int blue){
    
    uint16_t val = matrix.Color888(red, green, blue);
    
    if (type == 0){
        
        if (x_sign < 0){
            
            matrix.drawBitmap(curX, curY, m_a, 32,32, val) ;
            matrix.drawFastHLine(curX + 5, 30, 15, val);
            
        }else{
            
            matrix.drawBitmap(curX, curY, m_a_r, 32,32, val) ;
            matrix.drawFastHLine(curX + 7, 30, 15, val);
        }
        
    }else if (type == 1){
        
        matrix.drawBitmap(curX, curY, m_b, 32,32, val) ;
        matrix.drawFastHLine(curX + 5, 30, 15, val);

        
    }else if (type == 2){
        
        if (x_sign < 0){
            
            matrix.drawBitmap(curX, curY, m_c, 32,32, val) ;
            matrix.drawFastHLine(curX + 12, 30, 20, val);
            
        }else{
            
            matrix.drawBitmap(curX, curY, m_c_r, 32,32, val) ;
            matrix.drawFastHLine(curX, 30, 20, val);
        }
        
    }else{
        drawCloud(20,0,30, 200.0, 150.0f,255.0f);
    }
}

void loop() {
    
    matrix.fillScreen(matrix.Color333(0,0,0));
    
    // Perlin test.
    
    unsigned long curTime = millis();
    
    uint8_t seconds = int(curTime / 1000.0f);
    uint8_t remainder = 255*((curTime - seconds*1000) / 1000.0f);
    
    uint16_t noiseGen = seconds;
    noiseGen = noiseGen << 8;
    noiseGen += remainder;
    
    // Update position using noise
    float xn = inoise8( curX, noiseGen );
    float yn = inoise8( curY, noiseGen );
    
    long remapX = map(xn, 0.0, 255.0, 0.0, 2.0);
    long remapY = map(yn, 0.0, 255.0, 2.0, 4.0);
    
    // Increment position
    position p = {
        .x = curX,
        .y = curY
    };
    
    ghostPositions.push( p );
    
    if (ghostPositions.count() > NUM_GHOSTS){
        ghostPositions.pop();
    }

    updateTimers();

    updateRain( currentRainType );

    drawMonster(currentGhostType, 255, 255, 255);
    
    float bobVar = sin( radians( curTime ) * 0.3f ) * 2.0;
    
    curY = bobVar;
    curX += x_sign * remapX;

    
    // Check if out of bounds, if so, flip sign
    if (curX <= LOW_X){
        
        x_sign = 1;
        
    }else if (curX >= HIGH_X){
        
        x_sign = -1;
        
    }
    
    if (curX <= LOW_Y){
        
        y_sign = 1;
        
    }else if (curY >= HIGH_Y){
        
        y_sign = -1;
    }

    matrix.swapBuffers(true);

}


