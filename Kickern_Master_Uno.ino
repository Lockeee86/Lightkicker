 /*
 * Arduino Kicker App
 * with Adafruit 16x32 RGB Matrix &
 * 4 arcade button
 * 1 Potentiometer
 * Copyright Steffen Burmeister
 * eMail: st.burmeister@icloud.com
 */

#include <Arduino.h>                            // Arduino
#include <Adafruit_NeoPixel.h>                  // Adafruit LED strip


/******************************************************************************
 * set variable
 *****************************************************************************/

//FastLED Setup
#define NUM_LEDS 70
#define PIN 13

//Button Setup
int button_settings = 4;
int button_yellow = 5;
int button_blue = 6;
int button_light = 7;
int commuter = 8;
//OneButton button_light(7, true);

//intit Buttons
int button_settings_stat = 0;
int last_button_settings_stat = 0;
int button_yellow_stat = 0;
int last_button_yellow_stat = 0;
int button_blue_stat = 0;
int last_button_blue_stat = 0;
int button_light_stat = 0;
int last_button_light_stat = 0;
int commuter_stat = 0;
int sett_menu = 0;
int light_stat = 0;

//Goal/Target Setup
int final_goal = 6;
int win_wait = 10000;
int val = 0;
int val_set = 0;
int schleife = 0;

//count button push
int count_goal_yellow = 0;
int count_goal_blue = 0;

uint8_t gHue = 0;
int hue     = 0;

//strip brightness
#define MAX_BRIGHTNESS 255      // Thats full on, watch the power!
#define MIN_BRIGHTNESS 0       // set to a minimum of 25%

const int goalInPin = A5;

//Adafruit Neopixel strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

/******************************************************************************
* Arduino Setup
******************************************************************************/

void setup(){

  Serial.begin(9600);
  pinMode(button_settings, INPUT);
  pinMode(button_yellow, INPUT);
  pinMode(button_blue, INPUT);
  pinMode(button_light, INPUT);
  pinMode(commuter, INPUT);
  delay(2000);
  //strip.setBrightness(MIN_BRIGHTNESS);
  strip.begin(); // This initializes the NeoPixel library.
  strip.show();
  delay(7000);
} // end Setup

/******************************************************************************
* Arduino loop
******************************************************************************/
void loop() {
  button_settings_stat = digitalRead(button_settings);
  button_light_stat = digitalRead(button_light);
  button_yellow_stat = digitalRead(button_yellow);
  button_blue_stat = digitalRead(button_blue);
  commuter_stat = digitalRead(commuter);
  Serial.print(val);
  Serial.println(val_set);


  if (count_goal_yellow <= 2 && count_goal_blue <= 2){
    if (button_settings_stat != last_button_settings_stat) {
      if (button_settings_stat == HIGH) {
        if (val_set == 1){
          val_set = 0;
        } else if (val_set == 0){
          val_set = 1;
        }
      }
    }
  }
  if (val_set == 1){
    byte i;
    button_settings_stat = digitalRead(button_settings);
    if (val == 1){
      val = 0;
      light_stat = 1;
    }
    light_red();
    int mappedValue = map(analogRead(goalInPin), 0, 1023, 3, 9);

    final_goal = mappedValue;
  } else if (val_set == 0){
   if (button_settings_stat == LOW && light_stat == 1) {
    if (val == 0){
      val = 1;
      light_stat = 0;
    }
   }
  }
  last_button_settings_stat = button_settings_stat;

  // turn light on/off
  if (button_light_stat != last_button_light_stat) {
    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    if (button_light_stat == HIGH) {
      if (val == 1){
        val = 0;
      } else if (val == 0){
        val = 1;
      }
    }
  }
  
  if (val == 1)
  {
   light_on();
  } 
  else if (val == 0)
  {
   light_off(); 
  }
  last_button_light_stat = button_light_stat;

  // count goals team yellow
  if (button_yellow_stat != last_button_yellow_stat) {
    if (button_yellow_stat == LOW) {
      count_goal_yellow++;
      //delay(50);
      FadeIn(0xff, 0xff, 0x00);
      Strobe(0xff, 0xff, 0x00, 3, 50, 1000);
      FadeOut(0xff, 0xff, 0x00);
    }
  }
   last_button_yellow_stat = button_yellow_stat;
  
  // count goals team blue
  if (button_blue_stat != last_button_blue_stat) {
    if (button_blue_stat == LOW) {
      // count goal blue
      count_goal_blue++;
      FadeIn(0x00, 0x00, 0xff);
      Strobe(0x00, 0x00, 0xff, 3, 50, 1000);
      FadeOut(0x00, 0x00, 0xff);
    }
  }
  last_button_blue_stat = button_blue_stat;
  
  // team yellow win
  if (count_goal_yellow == final_goal) {
    NewKITT(0xff, 0xff, 0, 8, 10, 50);
    for( int i = 0; i<NUM_LEDS; i++){
      strip.setPixelColor(i, strip.Color(255, 255, 0));
    }
    strip.show();

    delay(5000);
    count_goal_yellow = 0;
    count_goal_blue = 0;
  }
  
  //team blue win
  if (count_goal_blue == final_goal) {
    NewKITT(0, 0, 0xff, 8, 10, 50);
    for( int i = 0; i<NUM_LEDS; i++){
      strip.setPixelColor(i, strip.Color(0, 0, 255));
    }
    strip.show();
    delay(5000);
    count_goal_yellow = 0;
    count_goal_blue = 0;
  } // end win

  // Start Pause Modus

  if (commuter_stat == HIGH){
    Pause();
  }

}// end loop

/******************************************************************************
* Matrix,Settings, Pause & Matrix_Win
******************************************************************************/

void Pause(){
  // start rainbowCycle
  rainbowCycle(20);
}


/******************************************************************************
* Adafruit Neopixel Effekte und Licht an und aus
******************************************************************************/

//-----------Licht AN--------------------------
void light_red(){
  setAll(255,0,0);
  showStrip();
}
//-----------Licht AN--------------------------
void light_on(){
  setAll(255,255,255);
  showStrip();
}

//-----------Licht AUS---------------------------
void light_off(){
  setAll(0,0,0);
  showStrip();
}

//-----------Rainbow Effekt---------------------------
byte * Wheel(byte WheelPos) {
  static byte c[3];
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  //for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
  for(j=0; j<256*1; j++) {
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    //strip.setBrightness(MAX_BRIGHTNESS);
    showStrip();
    delay(SpeedDelay);
  }
}

//-----------Strobo Effekt---------------------------
void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause){
  for(int j = 0; j < StrobeCount; j++) {
    setAll(red,green,blue);
    showStrip();
    delay(FlashDelay);
    setAll(0,0,0);
    showStrip();
    delay(FlashDelay);
    setAll(red,green,blue);
    showStrip();
  }
}

void FadeIn(byte red, byte green, byte blue){
  float r, g, b;

  for(int k = 0; k < 256; k=k+1) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
}
void FadeOut(byte red, byte green, byte blue){
  float r, g, b;

  for(int k = 255; k >= 0; k=k-2) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
}
//-----------Knight rider Effekt---------------------------
void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i =((NUM_LEDS-EyeSize)/2); i>=0; i--) {
    setAll(0,0,0);

    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);

    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue);
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i<=((NUM_LEDS-EyeSize)/2); i++) {
    setAll(0,0,0);

    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);

    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue);
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}


/******************************************************************************
* Setup Adafruit Effekte (Tweak4all)
******************************************************************************/
void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  //strip.setBrightness(MAX_BRIGHTNESS);
  showStrip();
}

