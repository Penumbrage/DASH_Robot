/******************************************************************************
Flex_Sensor_Example.ino
Example sketch for SparkFun's flex sensors
  (https://www.sparkfun.com/products/10264)
Jim Lindblom @ SparkFun Electronics
April 28, 2016

Create a voltage divider circuit combining a flex sensor with a 10k resistor.
- The resistor should connect from A0 to GND.
- The flex sensor should connect from A0 to 5V
As the resistance of the flex sensor increases (meaning it's being bent), the
voltage at A0 should decrease.

Development environment specifics:
Arduino 1.6.7
******************************************************************************/
#include <SoftwareSerial.h>
SoftwareSerial HM10(14, 15); // RX = 14, TX = 15
#define FLEX_PIN A0 // Pin connected to voltage divider output

// Measure the voltage at 5V and the actual resistance of your
// 10k resistor, and enter them below:
const float VCC = 5.15; // Measured voltage of Ardunio 5V line
const float R_DIV = 10000; // Measured resistance of 10k resistor

// Upload the code, then try to adjust these values to more
// accurately calculate bend degree.
const float STRAIGHT_RESISTANCE = 80000; // resistance when straight
const float BEND_RESISTANCE = 300000; // resistance at 90 deg

void setup() 
{
//  Serial.begin(9600);
  HM10.begin(9600); // set HM10 serial at 9600 baud rate
  HM10.println("Bluetooth connection established.");
  pinMode(FLEX_PIN, INPUT);
}

void loop() 
{
  // Read the ADC, and calculate voltage and resistance from it
  int flexADC = analogRead(FLEX_PIN);
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);
  HM10.println("Resistance: " + String(flexR) + " ohms");

  // Use the calculated resistance to estimate the sensor's
  // bend angle:
  float angle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE,
                   0, 90.0);
  HM10.println("Bend: " + String(angle) + " degrees");
  HM10.println();

  delay(1000);
}