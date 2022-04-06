/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-limit-switch
 */

#include <ezButton.h>

ezButton limitSwitch(7);  // create ezButton object that attach to pin 7;

void setup() {
  Serial.begin(9600);
  limitSwitch.setDebounceTime(50); // set debounce time to 50 milliseconds
}

void loop() {
  limitSwitch.loop(); // MUST call the loop() function first

  if(limitSwitch.isPressed())
    Serial.println("The limit switch: UNTOUCHED -> TOUCHED");
    delay(100);

  if(limitSwitch.isReleased())
    Serial.println("The limit switch: TOUCHED -> UNTOUCHED");
    delay(100);

  int state = limitSwitch.getState();
  if(state == HIGH)
    Serial.println("The limit switch: UNTOUCHED");
    delay(100);
  else
    Serial.println("The limit switch: TOUCHED");
    delay(100);
}
