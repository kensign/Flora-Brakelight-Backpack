#include <Wire.h>
#include <Adafruit_LSM303.h>
#include "SPI.h"
#include "Adafruit_WS2801.h"

Adafruit_LSM303 lsm;

#define BRAKETHRESHOLD        350
#define BRAKETIMETHRESHOLD    200

int dataPin  = 10;    // Yellow wire on Adafruit Pixels
int clockPin = 9;    // Green wire on Adafruit Pixels
const int cPin = 12;
const int dPin = 6;

// Set the first variable to the NUMBER of pixels. 32 = 32 pixels in a row
// The LED strips are 32 LEDs per meter but you can extend/cut the strip
Adafruit_WS2801 strip = Adafruit_WS2801(25,dataPin,clockPin);

int start = 0;
int prevX = 0;
int currentX = 0;
int cState = 0;
int dState = 0;

long brakeTime = 0;

void setup() 
{
  Serial.begin(9600);
  
  //while (!Serial) {
    //Serial.println("serial has not started");
  //}
  
  
  // Start up the LED strip
  strip.begin();
  // Update the strip, to start they are all 'off'
  strip.show();
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    //Serial.println("Oops ... unable to initialize the LSM303. Check your wiring!");
    while (1);
  } else{
  
  //Serial.println("lsm started");
  
  }
  pinMode(cPin, INPUT); 
  pinMode(dPin, INPUT);
}

void loop() 
{
  check_switches();      // when we check the switches we'll get the current state
  //Serial.println("still looping");
  //Serial.println("before the lsm read");
  lsm.read();
  
  //Serial.println("after the lsm read");
  currentX = abs(lsm.accelData.x);

  if (start == 0){
    prevX = currentX;
    start = 1;
  }

  int i = currentX - prevX;

  if (abs(i) > BRAKETHRESHOLD) {
    brakeTime = millis();
    int strikes = 0;
    while ((abs(i) > BRAKETHRESHOLD) && (strikes < 3)) {
      if (abs(i) < BRAKETHRESHOLD) {
        strikes = strikes + 1; 
      }
      lsm.read();
      currentX = abs(lsm.accelData.x);
      i = currentX - prevX;

      if ((millis() - brakeTime) > BRAKETIMETHRESHOLD) {
        brakeLights(Color(255,0,0),250);
        while (abs(i) > BRAKETHRESHOLD) {
          lsm.read();
          currentX = abs(lsm.accelData.x);
          i = currentX - prevX;
          Serial.println(i);
          delay(100);
        }
        hideAll();
        brakeTime = millis();
        i = 0;
        lsm.read();
        currentX = abs(lsm.accelData.x);
      }
    }
  }

  prevX = currentX;
  delay(200);
}

void check_switches()
{
  cState = digitalRead(cPin);
  dState = digitalRead(dPin);

  //Serial.println(cState);
  //Serial.println(dState);
  
  if (cState == HIGH) {     
    // left blinker
    //Serial.println("left blink on"); 
    hideAll();
    leftTurn(Color(255,63,0),250);
    delay(300);
    //Serial.println("left blink off");
    hideAll();
    delay(300);  
  }

  if (dState == HIGH) {     
    // right blinker
    //Serial.println("right blink on"); 
    hideAll();
    rightTurn(Color(255,63,0),250);
    delay(300);
    //Serial.println("right blink off");
    hideAll();
    delay(300);  
  }
}

void leftTurn(uint32_t c,uint8_t wait){
  allLeft(c);
  strip.show(); 
  delay(wait);
  hideAll();
  allLeft(c);
  strip.show(); 
  delay(wait);
  hideAll();
}

void rightTurn(uint32_t c,uint8_t wait){
  allRight(c);
  strip.show(); 
  delay(wait);
  hideAll();
  allRight(c);
  strip.show(); 
  delay(wait);
  hideAll();
}

void brakeLights(uint32_t c, uint8_t wait){
  allRight(c);
  allLeft(c);
  strip.show(); 
  delay(wait);
  hideAll();
  allLeft(c);
  allRight(c);
  strip.show(); 
  delay(wait);
  hideAll();
}

void allRight(uint32_t c){
  
  for(int i=6; i<= 12; i++){
    strip.setPixelColor(i, c);
  }  
  
  for(int i=18; i<= 24; i++){
    strip.setPixelColor(i, c);
  }
}

void allLeft(uint32_t c){
  int firstIn = 24;
  int firstOut = 12;
  
  strip.setPixelColor(firstIn, c);
  strip.setPixelColor(firstOut, c);  
  
  for(int i=1; i<= 6; i++){
    strip.setPixelColor(i, c);
  }
  
  for(int i=13; i<= 18; i++){
    strip.setPixelColor(i, c);
  }  
}


void hideAll(){
  for(int i = 0; i < strip.numPixels();i++){
    strip.setPixelColor(i,Color(0,0,0));
  }
  strip.show();
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}


