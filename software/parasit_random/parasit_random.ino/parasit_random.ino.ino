
#include "LedControl.h"  // LedControl Library created by Eberhard Fahle at http://playground.arduino.cc/Main/LedControl

LedControl lc = LedControl(3,4,5,2);

//PIN_READING THE SWITCH
int SWITCH_PIN = 11;
//pin relay
int RELAY_PIN = 12;

//total seconds of the countdown i.e 300s=5m 600s=10m  305s=5m5s
int total_seconds = 9;
//current seconds in the counter
int current_seconds = 9;

void setup() {
  lc.shutdown(0, false); // Wake up the display
  lc.shutdown(1, false); // Wake up the display
  lc.setIntensity(0, 15); // Set Brightness 0-15
  lc.setIntensity(1, 15); // Set Brightness 0-15
  lc.clearDisplay(0);  // Clear display
  lc.clearDisplay(1);  // Clear display
}

void loop() {

  delay(10);

  current_seconds = current_seconds - 1;

  if (current_seconds <= 0) {
    current_seconds = total_seconds;
  }

  //lc.setIntensity(random(0,2), random(0,15)); // Set Brightness 0-15
  lc.setDigit(random(0,2), random(0,8), current_seconds , false);

}

