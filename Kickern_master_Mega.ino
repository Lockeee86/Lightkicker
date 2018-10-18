 /*
 * Arduino Kicker App
 * with Adafruit 16x32 RGB Matrix &
 * 4 arcade button
 * 1 Potentiometer
 * Copyright Steffen Burmeister
 * eMail: st.burmeister@icloud.com
 */

#include <Arduino.h>                            // Arduino
#include <Adafruit_GFX.h>                       // Core graphics library
#include <RGBmatrixPanel.h>                     //Adafruit RGB MAtrix library
#include <Wire.h>                               // communicate with I2C / TWI
#include "RTClib.h"                             // Real Time Clock library

/******************************************************************************
 * set variable
 *****************************************************************************/
 // Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

// Clock type
RTC_PCF8523 rtc;
//MATRIX Setup
#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);

//Button Setup
int button_settings = 4;
int button_yellow = 5;
int button_blue = 6;
int button_light = 7;
int commuter = 53;
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

//Goal/Target Setup
int final_goal = 6;
int win_wait = 10000;
int val = 0;
int val_set = 0;
int schleife = 0;

//count button push
int count_goal_yellow = 0;
int count_goal_blue = 0;

int wait_delay = 250;

//Zeit
unsigned long previousMillis = 0;  // speichert wie viele Sekunden seit
                                   // derletzten Änderung vergangen sind
unsigned long interval =   300000;    // Interval zwischen zwei
                                   // Änderungen (900000 15min, 600000 10 min,)
//refresh cycle Matrix in Pause
unsigned long interval_Pause =  5000;
unsigned long previousMillis_Pause = 0;

// wait 10 seconds to exit settings
unsigned long interval_settings =  20000;
unsigned long previousMillis_settings = 0;

unsigned long interval_reset =  3000;
unsigned long previousMillis_reset = 0;


uint8_t gHue = 0;
//int hue     = 0;
const char str[] PROGMEM = "Pause | push 'LIGHT button' to exit";
int    textX   = matrix.width(),
       textMin = sizeof(str) * -12,
       hue     = 0;

//strip brightness
#define MAX_BRIGHTNESS 255      // Thats full on, watch the power!
#define MIN_BRIGHTNESS 0       // set to a minimum of 25%
const int goalInPin = A5;

int8_t ball[3][4] = {
  {  3,  0,  1,  1 }, // Initial X,Y pos & velocity for 3 bouncy balls
  { 20, 15,  1, -1 }
  //{ 27,  4, -1,  1 }
};
static const uint16_t PROGMEM ballcolor[2] = {
  0x1080, // Green=1
  0x0002 // Blue=1
  //0x1000  // Red=1
};

/******************************************************************************
* Arduino Setup
******************************************************************************/

void setup(){
  if (! rtc.begin()) {
    //Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
     //rtc.adjust(DateTime(2017, 4, 15, 12, 10, 20));
  }
  
  Serial.begin(9600);
  pinMode(button_settings, INPUT);
  pinMode(button_yellow, INPUT);
  pinMode(button_blue, INPUT);
  pinMode(button_light, INPUT);
  pinMode(commuter, OUTPUT);
  delay(2000);
  matrix.begin();
  welcome();
  delay(5000);
  Hello();
  delay(2000);
  Matrix();
} // end Setup

/******************************************************************************
* Arduino loop
******************************************************************************/
void loop() {
  button_settings_stat = digitalRead(button_settings);
  button_light_stat = digitalRead(button_light);
  button_yellow_stat = digitalRead(button_yellow);
  button_blue_stat = digitalRead(button_blue);
  Serial.print(button_light_stat);
  Serial.println(button_settings_stat);

  if (count_goal_yellow <= 2 && count_goal_blue <= 2){
    if (button_settings_stat != last_button_settings_stat) {
      if (button_settings_stat == HIGH) {
        if (val_set == 1){
          val_set = 0;
        } else if (val_set == 0){
          val_set = 1;
        }
        delay(wait_delay);
      }
    }
  }
  if (val_set == 1){
    byte i;
    button_settings_stat = digitalRead(button_settings);
    //light_red();
    int mappedValue = map(analogRead(goalInPin), 0, 1023, 3, 9);

    final_goal = mappedValue;

    matrix.fillScreen(0);

    // Bounce three balls around
    for(i=0; i<3; i++) {
      // Draw 'ball'
      matrix.fillCircle(ball[i][0], ball[i][1], 5, pgm_read_word(&ballcolor[i]));
      // Update X, Y position
      ball[i][0] += ball[i][2];
      ball[i][1] += ball[i][3];
      // Bounce off edges
      if((ball[i][0] == 0) || (ball[i][0] == (matrix.width() - 1)))
        ball[i][2] *= -1;
      if((ball[i][1] == 0) || (ball[i][1] == (matrix.height() - 1)))
        ball[i][3] *= -1;
    }

    //reset Matrix screen
    //--------------------------------------------------------
    matrix.setTextWrap(false);
    matrix.setTextSize(1);
    matrix.setCursor(1, 0);
    //matrix.fillRect(0, 0, 32, 16, matrix.Color333(1, 1, 1));
    matrix.setTextColor(matrix.Color333(2,0,0));
    matrix.print("SETUP");
    matrix.setCursor(1, 8);
    matrix.print(" ");
    matrix.setTextColor(matrix.Color333(2,2,2));
    matrix.drawRect(6, 7, 19, 9, matrix.Color333(1, 0, 0));
    matrix.fillRect(7, 8, 17, 7, matrix.Color333(0, 0, 0));
    matrix.print("G");
    matrix.print("=");
    matrix.print(final_goal);
    matrix.print(" ");
    matrix.swapBuffers(true);

 } else if (val_set == 0){
   if (button_settings_stat == LOW) {
     Matrix();
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
      delay(wait_delay);
    }
  }
  if (val == 1){
    
  } else if (val == 0){
     
  }
  last_button_light_stat = button_light_stat;

  // count goals team yellow
  if (button_yellow_stat != last_button_yellow_stat) {
    if (button_yellow_stat == LOW) {
      delay(wait_delay);
      count_goal_yellow++;
      previousMillis = millis(); // Zähler auf null setzen
      Matrix();
      //matrix.swapBuffers(false);
      //delay(50);
    }
  }
  last_button_yellow_stat = button_yellow_stat;
  
  // count goals team blue
  if (button_blue_stat != last_button_blue_stat) {
    if (button_blue_stat == LOW) {
      delay(wait_delay);
      // count goal blue
      count_goal_blue++;
      previousMillis = millis(); //Zähler auf null setzen
      Matrix();
      //matrix.swapBuffers(true);
      //delay(50);
    }
  }
  last_button_blue_stat = button_blue_stat;
  
  // team yellow win
  if (count_goal_yellow == final_goal) {
    Matrix_yellow_win(); 
    delay(13000);
    count_goal_yellow = 0;
    count_goal_blue = 0;
    Matrix();
  }
  //team blue win
  if (count_goal_blue == final_goal) {
    Matrix_blue_win();
    delay(13000);
    count_goal_yellow = 0;
    count_goal_blue = 0;
    Matrix();
  } // end win

  // Start Pause Modus
  if (millis() - previousMillis > interval) {

    digitalWrite(commuter, HIGH);
    Pause();
  }

}// end loop

/******************************************************************************
* Matrix,Settings, Pause & Matrix_Win
******************************************************************************/

void Pause(){
  static int m = LOW;
  while (m == LOW) {
    //if (millis() - previousMillis_Pause > interval_Pause) {
      button_light_stat = digitalRead(button_light);
      digitalWrite(commuter, HIGH);
      //set time
      DateTime now = rtc.now();
      //reset Matrix screen
      //--------------------------------------------------------
      matrix.fillScreen(0);

      matrix.setTextWrap(false);
      matrix.setTextSize(1);
      //matrix.drawRect(0, 0, 32, 16, matrix.Color333(1, 0, 0));
      //matrix.drawPixel(15, 0, matrix.Color333(7, 7, 7));
      //matrix.drawPixel(15, 15, matrix.Color333(7, 7, 7));
      matrix.drawLine(0, 0, 15, 0, matrix.ColorHSV(hue, 150, 150, true));
      matrix.drawLine(0, 0, 0, 15, matrix.ColorHSV(hue, 150, 150, true));
      matrix.drawLine(0, 15, 15, 15, matrix.ColorHSV(hue, 150, 150, true));
      matrix.drawLine(16, 0, 31, 0, matrix.ColorHSV(hue, 150, 150, true));
      matrix.drawLine(31, 0, 31, 15, matrix.ColorHSV(hue, 150, 150, true));
      matrix.drawLine(16, 15, 31, 15, matrix.ColorHSV(hue, 150, 150, true));
      matrix.setTextColor(matrix.Color333(2,0,0));
      matrix.setCursor(1, 1);
      if (now.hour() < 10) {
        matrix.print("0");
      }
      matrix.print(now.hour(), DEC);
      matrix.print(":");
      if (now.minute() < 10) {
        matrix.print("0");
      }
      matrix.print(now.minute(), DEC);
      hue += 3;
      if(hue >= 1536) hue -= 1536;
      matrix.setCursor(textX, 8);
      matrix.print(F2(str));
      //refresh Matrix
      matrix.swapBuffers(true);
      //--------------------------------------------------------
      // reset timer
      //previousMillis_Pause = millis();
      if((--textX) < textMin) textX = matrix.width();
      // exit while loop by push down light button
      if (button_light_stat == HIGH) {
        previousMillis = millis();   // aktuelle Zeit abspeichern
        digitalWrite(commuter, LOW);
        Matrix();
        return;
      }
    //}   
  }
}

void welcome(){
  DateTime now = rtc.now();
  //reset Matrix screen
  //--------------------------------------------------------
  matrix.fillScreen(0);

  matrix.setTextWrap(false);
  matrix.setTextSize(1);
  //matrix.drawRect(0, 0, 32, 16, matrix.Color333(1, 0, 0));
  matrix.drawPixel(15, 0, matrix.Color333(7, 7, 7));
  matrix.drawPixel(15, 15, matrix.Color333(7, 7, 7));
  matrix.drawLine(0, 0, 14, 0, matrix.Color333(3, 3, 0));
  matrix.drawLine(0, 0, 0, 15, matrix.Color333(3, 3, 0));
  matrix.drawLine(0, 15, 14, 15, matrix.Color333(3, 3, 0));
  matrix.drawLine(16, 0, 31, 0, matrix.Color333(0, 0, 3));
  matrix.drawLine(31, 0, 31, 15, matrix.Color333(0, 0, 3));
  matrix.drawLine(16, 15, 31, 15, matrix.Color333(0, 0, 3));
  matrix.setTextColor(matrix.Color333(2,0,0));
  matrix.setCursor(1, 4);
  if (now.hour() < 10) {
    matrix.print("0");
  }
  matrix.print(now.hour(), DEC);
  matrix.print(":");
  if (now.minute() < 10) {
    matrix.print("0");
  }
  matrix.print(now.minute(), DEC);
  //refresh Matrix
  matrix.swapBuffers(true);

}

void Hello(){
  //reset Matrix screen
  matrix.fillScreen(0);
  //Korpus
  matrix.fillCircle(16, 4, 4, matrix.Color333(4, 0, 0));
  //matrix.fillCircle(14, 3, 1, matrix.Color333(0, 0, 4));
  //griff
  matrix.drawLine(15, 0, 17, 0, matrix.Color333(3, 3, 3));
  //Griff Korpus
  matrix.drawLine(15, 5, 17, 5, matrix.Color333(3, 3, 3));
  //Füße
  matrix.drawLine(14, 8, 11, 15, matrix.Color333(3, 3, 3));
  matrix.drawLine(16, 9, 16, 15, matrix.Color333(3, 3, 3));
  matrix.drawLine(18, 8, 21, 15, matrix.Color333(3, 3, 3));
  //Räder
  matrix.drawLine(4, 15, 28, 15, matrix.Color333(0, 3, 0));
  //matrix.drawPixel(16, 15, matrix.Color333(7, 7, 7));
  //refresh Matrix
  matrix.swapBuffers(true);

}

void Matrix(){
  //Clear background
  matrix.fillScreen(0);
  matrix.setTextWrap(false);
  matrix.setTextSize(2);
  matrix.setCursor(2, 1);
  matrix.setTextColor(matrix.Color333(3,3,0));
  matrix.print(count_goal_yellow);
  matrix.drawRect(15, 5, 2, 2, matrix.Color333(2, 2, 2));
  matrix.drawRect(15, 10, 2, 2, matrix.Color333(2, 2, 2));
  matrix.setCursor(20, 1);
  matrix.setTextColor(matrix.Color333(0,0,4));
  matrix.print(count_goal_blue);
  //refresh Display
  matrix.swapBuffers(true);
}

void Matrix_yellow_win(){
  //Clear background
  matrix.fillScreen(0);
  matrix.setTextWrap(false);
  matrix.drawLine(0, 0, 15, 0, matrix.Color333(3, 3, 0));
  matrix.drawLine(0, 0, 0, 15, matrix.Color333(3, 3, 0));
  matrix.drawLine(0, 15, 15, 15, matrix.Color333(3, 3, 0));
  matrix.drawLine(16, 0, 31, 0, matrix.Color333(3, 3, 0));
  matrix.drawLine(31, 0, 31, 15, matrix.Color333(3, 3, 0));
  matrix.drawLine(16, 15, 31, 15, matrix.Color333(3, 3, 0));
  matrix.setTextSize(1);
  matrix.setCursor(2, 1);
  matrix.setTextColor(matrix.Color333(0,0,0));
  matrix.print("*");
  matrix.setTextColor(matrix.Color333(4,4,0));
  matrix.print(count_goal_yellow);
  matrix.setTextColor(matrix.Color333(4,4,4));
  matrix.print(":");
  matrix.setTextColor(matrix.Color333(0,0,7));
  matrix.print(count_goal_blue);
  matrix.setTextColor(matrix.Color333(0,0,0));
  matrix.print("*");
  matrix.setCursor(2, 8);
  matrix.setTextColor(matrix.Color333(4,4,4));
  matrix.print(" ");
  matrix.print("W");
  matrix.print("I");
  matrix.print("N");
  matrix.print(" ");

  //refresh Display
  matrix.swapBuffers(false);
}

void Matrix_blue_win(){
  //Clear background
  matrix.fillScreen(0);
  matrix.setTextWrap(false);
  matrix.drawLine(0, 0, 15, 0, matrix.Color333(0, 0, 3));
  matrix.drawLine(0, 0, 0, 15, matrix.Color333(0, 0, 3));
  matrix.drawLine(0, 15, 15, 15, matrix.Color333(0, 0, 3));
  matrix.drawLine(16, 0, 31, 0, matrix.Color333(0, 0, 3));
  matrix.drawLine(31, 0, 31, 15, matrix.Color333(0, 0, 3));
  matrix.drawLine(16, 15, 31, 15, matrix.Color333(0, 0, 3));
  matrix.setTextSize(1);
  matrix.setCursor(2, 1);
  matrix.setTextColor(matrix.Color333(0,0,0));
  matrix.print("*");
  matrix.setTextColor(matrix.Color333(4,4,0));
  matrix.print(count_goal_yellow);
  matrix.setTextColor(matrix.Color333(4,4,4));
  matrix.print(":");
  matrix.setTextColor(matrix.Color333(0,0,7));
  matrix.print(count_goal_blue);
  matrix.setTextColor(matrix.Color333(0,0,0));
  matrix.print("*");
  matrix.setCursor(2, 8);
  matrix.setTextColor(matrix.Color333(4,4,4));
  matrix.print(" ");
  matrix.print("W");
  matrix.print("I");
  matrix.print("N");
  matrix.print(" ");
  //refresh Display
  matrix.swapBuffers(false);
}
