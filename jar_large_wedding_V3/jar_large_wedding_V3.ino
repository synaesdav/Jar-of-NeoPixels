//major update 2/21 added individual sine wave and plasma effect
//major update 9/2020 added sine wave color
//written by David Crittenden December 2015
//for feather MO
//Uses a 10 meters of 30/meter mini-skinny
//momentary switch on pin 11 for mode changes

//Modified from various code sources including:
//https://learn.adafruit.com/neopixel-pixie-dust-bag/code-it
//NeoPixie Dust Bag by John Edgar Park jpixl.net
//Adafruit GEMMA earring code and Adafruit NeoPixel buttoncycler code
//Adafruit NeoPixel strand test
//https://github.com/danasf/attiny_pixel_switch/blob/master/attiny_pixel_touch.ino
//https://github.com/adafruit/CircuitPlayground_Power_Reactor
//https://codebender.cc/sketch:47850#John%20Ericksen's%20Neopixel%20Shield%20Plasma%20Effect%20that%20I'm%20screwing%20with.ino


#include "lerp.h"//deals with linear interpolation
#include <Adafruit_NeoPixel.h>  //Include the NeoPixel library

#define NEO_PIN 6// DIGITAL IO pin for NeoPixel OUTPUT
#define TOUCH_PIN 11// DIGITAL IO pin for momentary touch sensor INPUT 
#define N_Pixels 300// Number of NeoPixels 

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_Pixels, NEO_PIN, NEO_GRB + NEO_KHZ800);

//Used in multiple functions
uint8_t r;
uint8_t g;
uint8_t b;
int pixPoint;

//used with sine wave
uint8_t R;
uint8_t G;
uint8_t B;
uint8_t red;
uint8_t green;
uint8_t blue;
uint8_t RA;
uint8_t GA;
uint8_t BA;
float frequency;

//used with individual sine wave
float frequencies[N_Pixels];//array to hold individual frequencies
float f0 = 0.001;//slowest speed a frequency could be
float f1 = 0.2;//fastest speed a frequency could be

//beginning color
uint8_t red0;
uint8_t green0;
uint8_t blue0;

//ending color
uint8_t red1;
uint8_t green1;
uint8_t blue1;

//used with plasma effect
//try playing with these numbers for different effect
//it is creating a matrix from width and height definitions,
//but different values can give different effects with different layouts
uint8_t wide = 38;
uint8_t tall = 8;

// Convenient 2D point structure
struct Point
{
  float x;
  float y;
};

float phase = 0.0;
//try playing with these values
float phaseIncrement = 0.0015;  // Controls the speed of the moving points. Higher == faster. I like 0.08 ..03 change to .02
float colorStretch = 0.44;    // Higher numbers will produce tighter color bands. I like 0.11 . ok try .11 instead of .03


//used in rainbow and rainbowcycle function
uint8_t j = 0;//color starts at 0
int i;

//used in buttonCheck function
bool newState;
bool oldState = HIGH; //sets the initial variable for counting touch sensor button pushes
uint8_t modeMax = 9;//number of modes
uint8_t mode = 1;    //mode for cycling

void setup()
{
  randomSeed(analogRead(0));//initialize the randomizer
  pinMode(TOUCH_PIN, INPUT_PULLUP);//Initialize touch sensor pin as input using internal pull-up resistor
  strip.begin();
  strip.show();//Set all pixels to "off"

  //fill an array with individual randomized frequencies
  for (int i = 0; i < strip.numPixels(); i++)
  {
    frequencies[i] = lerp(random(4096), 0, 4095, f0, f1);//4096 represents 12 bit resolution
  }
}

void loop()
{
  buttonCheck();
  //mode = 8; //test code
  switch (mode)
  {
    case 1:
      //rainbow bright
      strip.setBrightness(225);
      rainbowCycle(8); //larger number for slower action
      buttonCheck();
      break;

    case 2:
      //rainbow dim
      strip.setBrightness(50);
      rainbowCycle(35); //larger number for slower action
      buttonCheck();
      break;

    case 3://sineGlow and sparkly bits
      strip.setBrightness(150);
      R = 50;
      G = 0;
      B = 50;
      RA = 100;
      GA = 150;
      BA = 0;
      frequency = .099;//larger number for faster speed

      sineGlow();

      //the sparkly bits
      for (i = 0; i < 3; i++)
      {
        r = random(100, 255);
        g = random(100, 255);
        b = random(100, 255);
        pixPoint = random(N_Pixels);
        strip.setPixelColor(pixPoint, r, g, b);
        strip.show();
        delay(15);
      }

      buttonCheck();
      break;

    case 4:
      //individual sine wave

      //beginning color
      red0 = 0;
      green0 = 0;
      blue0 = 255;
      //ending color
      red1 = 255;
      green1 = 255;
      blue1 = 0;

      strip.setBrightness(50);

      pixelSineWave();
      buttonCheck();
      break;

    case 5:
      //individual sine wave

      //beginning color
      red0 = 0;
      green0 = 0;
      blue0 = 20;
      //ending color
      red1 = 150;
      green1 = 0;
      blue1 = 225;

      strip.setBrightness(50);

      pixelSineWave();
      buttonCheck();
      break;

    case 6:
      //plasma effect

      strip.setBrightness(75);
      plasmaEffect();
      buttonCheck();
      break;

    case 7:
      //random generator red
      strip.setBrightness(150);
      r = 255;
      g = 0;
      b = random(200);
      randomGenerator(100); //smaller number for faster action
      buttonCheck();
      break;

    case 8:
      //sineLarson
      strip.setBrightness(255);
      R = 45;
      G = 0;
      B = 0;
      RA = 55;
      GA = 0;
      BA = 80;
      frequency = -0.25;//larger number for faster speed, negitive numbers move the band down

      sineLarson();

      buttonCheck();
      break;

    case 9:
      //spiralChase
      strip.setBrightness(150);
      getRandom();
      //spiralChase(strip.Color(r, g, b), 110);
      spiralChase((Wheel((j) & 255)), 140);
      j = j + 7;
      if (j > 256)
        j = 0;
      buttonCheck();
      break;
  }
}

void buttonCheck()
{
  //button check to cycle through modes
  newState = digitalRead(TOUCH_PIN); //Get the current button state
  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH)
  {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(TOUCH_PIN);
    if (newState == LOW)
    {
      blackOut(strip.Color(0, 0, 0), 0);
      strip.show();
      delay(300); //delay here allows for visual confirmation of button press
      mode++;
      if (mode > modeMax)
        mode = 1;
    }
  }
  // Set the last button state to the old state.
  oldState = newState;
}

void randomGenerator(uint8_t wait)
{
  uint8_t rf = r / 10; //larger number for dimmer fade
  uint8_t gf = g / 10;
  uint8_t bf = b / 10;

  int pos = random(N_Pixels);
  strip.setPixelColor(pos, r, g, b);
  strip.show();
  delay(wait);
  strip.setPixelColor(pos + 1 , rf, gf, bf);
  strip.setPixelColor(pos - 1 , rf, gf, bf);
  strip.show();
  delay(wait);
  strip.setPixelColor(pos + 2 , rf / 2, gf / 2, bf / 2);
  strip.setPixelColor(pos - 2 , rf / 2, gf / 2, bf / 2);
  strip.show();
  delay(wait);
  strip.setPixelColor(pos + 3, 0, 0, 0);
  strip.setPixelColor(pos - 3, 0, 0, 0);
  strip.setPixelColor(pos + 4, 0, 0, 0);
  strip.setPixelColor(pos - 4, 0, 0, 0);
  strip.setPixelColor(pos + 5, 0, 0, 0);
  strip.setPixelColor(pos - 5, 0, 0, 0);
  strip.show();
  delay(wait);
}

void getRandom() // choose a random color
{
  pixPoint = random(N_Pixels);
  r = random(255);
  g = random(255);
  b = random(255);
}

void rainbow(uint8_t wait)
{
  for (i = 0; i < N_Pixels; i++)
  {
    strip.setPixelColor(i, strip.gamma32(Wheel((i + j) & 255)));
  }
  strip.show();
  delay(wait);

  j++;
  if (j > 256)
    j = 0;
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait)
{
  for (i = 0; i < N_Pixels; i++)
  {

    strip.setPixelColor(i, strip.gamma32(Wheel(((i * 256 / N_Pixels + j) & 255))));
  }
  strip.show();
  delay(wait);

  // buttonCheck();

  j++;
  if (j > 256)
    j = 0;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85)
  {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void colorWipe(uint32_t c, uint8_t wait)
{
  for (uint16_t i = 0; i < N_Pixels; i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void blackOut(uint32_t c, uint8_t wait)
{
  for (uint16_t i = 0; i < N_Pixels; i++)
  {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void pixelSprinkle(uint8_t wait)
{
  getRandom();
  strip.setPixelColor(pixPoint, r, g, b);
  getRandom();
  strip.setPixelColor(pixPoint, 0, 0, 0);
  getRandom();
  strip.setPixelColor(pixPoint, 0, 0, 0);
  getRandom();
  strip.setPixelColor(pixPoint, 0, 0, 0);
  getRandom();
  strip.setPixelColor(pixPoint, 0, 0, 0);
  strip.show();
  delay(wait);
}

void lineChase(uint32_t color, int wait)
{
  for (int b = 0; b < 4; b++) { //  'b' counts from 0 to 3...
    strip.clear();         //   Set all pixels in RAM to 0 (off)
    // 'c' counts up from 'b' to end of strip in steps of 3...
    for (int c = b; c < strip.numPixels(); c += 4) {
      strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void spiralChase(uint32_t color, int wait)
{
  for (int b = 0; b < 7; b++)
  {
    //strip.clear();         //   Set all pixels in RAM to 0 (off)
    for (i = 0; i < N_Pixels; i++)
    {
      strip.setPixelColor(i, r % 20, g % 20, b % 20);//color the background
      //strip.setPixelColor(i,(Wheel((j) & 255))/2);
    }

    for (int c = b; c < strip.numPixels(); c += 7)
    {
      strip.setPixelColor(c, color);
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void sineLarson()//strip is gradiated from one end to the other based on a sine wave
{
  float currentS = millis() / 1000.0;
  for (int i = 0; i < N_Pixels; i++)
  {
    float phase = (float)i / (float)(N_Pixels - 1) * 2.0 * PI;
    float t = sin(2.0 * PI * frequency * currentS + phase);

    red = lerp(t, -1.0, 1.0, R, RA);
    green = lerp(t, -1.0, 1.0, G, GA);
    blue = lerp(t, -1.0, 1.0, B, BA);

    strip.setPixelColor(i, strip.gamma32(strip.Color(red, green, blue)));
  }
  strip.show();
}

void sineGlow()//entire strip is colored one color based on a sine wave gradient
{
  //sine wave sets the color
  float currentS = millis() / 1000.0;
  float t = sin(2.0 * PI * frequency * currentS);
  red = lerp(t, -1.0, 1.0, R, RA);
  green = lerp(t, -1.0, 1.0, G, GA);
  blue = lerp(t, -1.0, 1.0, B, BA);

  //show
  for (uint16_t i = 0; i < N_Pixels; i++)
  {
    strip.setPixelColor(i, strip.gamma32(strip.Color(red, green, blue)));
  }
  strip.show();
}

void pixelSineWave()
{
  float currentS = millis() / 1000.0;//get the current time

  for (int i = 0; i < strip.numPixels(); i++)
  {
    float t = sin(2.0 * PI * frequencies[i] * currentS);

    uint8_t red = lerp(t, -1.0, 1.0, red0, red1);
    uint8_t green = lerp(t, -1.0, 1.0, green0, green1);
    uint8_t blue = lerp(t, -1.0, 1.0, blue0, blue1);

    strip.setPixelColor(i, strip.gamma32(strip.Color(red, green, blue)));
  }
  strip.show();
}

void plasmaEffect()
{
  phase += phaseIncrement;

  // The two points move along Lissajious curves, see: http://en.wikipedia.org/wiki/Lissajous_curve
  // We want values that fit the LED grid: x values between 0..13, y values between 0..8 .
  // The sin() function returns values in the range of -1.0..1.0, so scale these to our desired ranges.
  // The phase value is multiplied by various constants; I chose these semi-randomly, to produce a nice motion.
  Point p1 = { (sin(phase * 1.000) + 1.0) * 4.5, (sin(phase * 1.310) + 1.0) * 4.0 };
  Point p2 = { (sin(phase * 1.770) + 1.0) * 4.5, (sin(phase * 2.865) + 1.0) * 4.0 };
  Point p3 = { (sin(phase * 0.250) + 1.0) * 4.5, (sin(phase * 0.750) + 1.0) * 4.0 };

  byte row, col;

  // For each row...
  for ( row = 0; row < wide; row++ )
  {
    float row_f = float(row);  // Optimization: Keep a floating point value of the row number, instead of recasting it repeatedly.

    // For each column...
    for ( col = 0; col < tall; col++ )
    {
      float col_f = float(col);  // Optimization.

      // Calculate the distance between this LED, and p1.
      Point dist1 = { col_f - p1.x, row_f - p1.y };  // The vector from p1 to this LED.
      float distance1 = sqrt( dist1.x * dist1.x + dist1.y * dist1.y );

      // Calculate the distance between this LED, and p2.
      Point dist2 = { col_f - p2.x, row_f - p2.y };  // The vector from p2 to this LED.
      float distance2 = sqrt( dist2.x * dist2.x + dist2.y * dist2.y );

      // Calculate the distance between this LED, and p3.
      Point dist3 = { col_f - p3.x, row_f - p3.y };  // The vector from p3 to this LED.
      float distance3 = sqrt( dist3.x * dist3.x + dist3.y * dist3.y );

      // Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
      // You can use a cos() for slightly different shading, or experiment with other functions. Go crazy!
      float color_1 = distance1;  // range: 0.0...1.0
      float color_2 = distance2;
      float color_3 = distance3;
      float color_4 = (sin( distance1 * distance2 * colorStretch )) + 2.0 * 0.5;

      // Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
      color_1 *= color_1 * color_4;
      color_2 *= color_2 * color_4;
      color_3 *= color_3 * color_4;
      color_4 *= color_4;

      // Scale the color up to 0..7 . Max brightness is 7.
      //strip.setPixelColor(col + (8 * row), strip.Color(color_4, 0, 0) );
      strip.setPixelColor(col + (8 * row), strip.gamma32(strip.Color(color_1, color_2, color_3)));
    }
  }
  strip.show();
}
