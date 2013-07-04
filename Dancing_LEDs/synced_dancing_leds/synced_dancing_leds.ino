/*
 * Dancing LEDs sync to musical timing provided by a Python script
 * Uses the FastSPI2 library <https://code.google.com/p/fastspi/> 
 * 
 * Timing extracted into text file using pygame and timings sent to Arduino using pySerial
 * 
 * Edited by 
 * Ankit Daftery
 * 27 June 2013
 * 
 */

#include "FastSPI_LED2.h"


/* Not used, but useful snippet */
template <typename T, size_t N>
inline
size_t SizeOfArray( const T(&)[ N ] )
{
  return N;
}

#define NUM_LEDS 180  

struct CRGB leds[NUM_LEDS];
int current_brightness = 0;
boolean mad_rush=false;
boolean toggle=true;

void setup() {
  Serial.begin(9600);
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);

  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  LEDS.setBrightness(120);

  // Store brightness level, for use later
  current_brightness = 120;

  LEDS.addLeds<LPD8806>(leds, NUM_LEDS)->clearLeds(300);

  // Test LEDs and wait for further instructions
  leds_setup();
}

void leds_setup()
{
  memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));

  // Repeating pattern of R-G-B-Off-Off-Off
  for (int i=0;i<NUM_LEDS/6;i++) {
    leds[6*i+0].r=128;
    leds[6*i+1].g=128;
    leds[6*i+2].b=128;
    leds[6*i+3]=0;
    leds[6*i+4]=0;
    leds[6*i+5]=0;
  }
  LEDS.show();
  /*
  // Do some random effects and wait. Just a visual cue for testing
  fadeUp();
  fadeDown();
  LEDS.setBrightness(current_brightness);
  LEDS.show();
  delay(100);
  LEDS.setBrightness(0);
  LEDS.show();
  delay(100);
  LEDS.setBrightness(current_brightness);
  LEDS.show();
  delay(250);
  delay(250);
  delay(250);
   */
  // Handshake to sync the music with the LEDs

  Serial.write('1');  // Tell the Python script we are ready to dance

}

void loop() {

  if (Serial.available()) {
    char i=Serial.read();
    if(i=='f') shiftright(leds,1);    // Move forward
    else if (i=='b') shiftleft(leds,1);  // Move backward
    if(i=='t') {                // Tile pressed, give visual feedback
      fadeUp();                 // TODO : Pause tkinter and the timers, while that happens
      fadeDown();
      delay(200);
      LEDS.setBrightness(current_brightness);
      delay(100);
      LEDS.setBrightness(0);
      delay(100);
      LEDS.setBrightness(current_brightness);
    }
    if(i=='s') {      // Shuffle leds, another visual cue
      toggle = !toggle;
      if(toggle) shuffle_left(leds);  // Need to toggle, because shuffle left twice reverts back to original
      else shuffle_right(leds);       //  if done immediately
    }
    if(i=='u') mad_rush=true; 
    if(i=='v') mad_rush=false;
  }
  if(mad_rush) rush();      // Fast piece of music, go into a mad rush
}

// Fade up the lights

void fadeUp() {
  fadeUp(0,127,15);
}

// Fade down the lights, with minimum and maximum values, and time delay as parameters


void fadeDown(int from,int to,int time) {
  if(from>255) from=255;
  if(to<0) to=0;
  if(to>from) from=to;
  for(int i=from;i>to-1;i--)
  {
    LEDS.setBrightness(i);
    LEDS.show();
    delay(time);
  }

}
// Fade up the lights, with minimum and maximum values, and time delay as parameters

void fadeUp(int from,int to,int time) {
  if(from<0) from=0;
  if(to>255) to=255;
  if(from>to) to=from;
  for(int i=from;i<to+1;i++)
  {
    LEDS.setBrightness(i);
    LEDS.show();
    delay(time);
  }

}

// Fade down the lights

void fadeDown() {

  fadeDown(127,0,15);
}

// Circular shift the entire strip forward

void shiftright(CRGB led[], int n) {
  struct CRGB temp[n];
  memcpy(temp,led+NUM_LEDS-n,n*sizeof(*led));
  memmove(led+n,led,(NUM_LEDS-n)*sizeof(*led));
  memcpy(led,temp,n*sizeof(*led));
  LEDS.show();
}

// Circular shift the entire strip backward

void shiftleft(CRGB led[], int n) {
  struct CRGB temp[n];
  memcpy(temp,led,n*sizeof(*led));
  memmove(led,led+n,(NUM_LEDS-n)*sizeof(*led));
  memcpy(led+NUM_LEDS-n,temp,n*sizeof(*led));
  LEDS.show();
}

// A blinding rush of lights

void rush() {
  shiftright(leds,1);
  delay(50);
}

// LEDs appear to "flip over"

void shuffle_left(CRGB leds[]) {
  struct CRGB temp[NUM_LEDS];
  memcpy(temp,leds,NUM_LEDS*sizeof(*leds));
  for (int i=3;i<NUM_LEDS;i=i+3) {
    temp[i-3]=    leds[i];
  }
  temp[177]= leds[0];
  memcpy(leds,temp,NUM_LEDS*sizeof(*leds));
  LEDS.show();
}

// LEDs appear to "flip over", but in the other direction

void shuffle_right(CRGB leds[]) {
  struct CRGB temp[NUM_LEDS];
  memcpy(temp,leds,NUM_LEDS*sizeof(*leds));
  for (int i=5;i<NUM_LEDS;i=i+3) {
    temp[i-3]=    leds[i];
  }
  temp[179]= leds[2];
  memcpy(leds,temp,NUM_LEDS*sizeof(*leds));
  LEDS.show();
}
