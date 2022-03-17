/*
This code serves as a test for the communication between the bluetooth module
and the Arduino
*/

// Include the different libraries in the code
#include <SoftwareSerial.h>

// Create a bluetooth object and variables to store data read from bluetooth
SoftwareSerial HM10(0, 1); // RX = 0, TX = 1
char bt_msg;               // stores message from bluetooth module
String cmd = "";           // command to send to motor driver

// Initialize an enumeration to describe the different states for the motors
typedef enum{
   FORWARD,
   BACKWARD,
   LEFT,
   RIGHT,
   STOP
} motorState;

// Initialize the state of the motor (STOP)
motorState currentState = STOP;

void setup()
{
  Serial.begin(9600);
  Serial.println("HM10 serial started at 9600");
  HM10.begin(9600); // set HM10 serial at 9600 baud rate
}

void loop()
{
  
}

// This function gets a message from the bluetooth module when available
void get_msg(){
    HM10.listen();  // listen the HM10 port
    while (HM10.available() > 0) {   // if HM10 sends something then read
        bt_msg = HM10.read();
        cmd = String(appData);  // save the data in string format
        Serial.write(appData);

    if (Serial.available()) {           // Read user input if available.
        delay(10);
        HM10.write(Serial.read());
    }
    if ( inData == "F") {
        Serial.println("LED OFF");
        delay(500);
    }
    if ( inData == "N") {
        Serial.println("LED ON");
        delay(500);
    }
}

// This function gets the current state information from the bluetooth module
void get_current_state(){
   // TODO: FInd a method to allow for different motor speeds to be inputed from the serial line
   // Obtains a message from the serial line and updates the state case
   if (Serial.available()){
      // get message from serial line
      int msg = Serial.parseInt();

      // update the state based on the message
      if (msg == 1){
         currentState = STOP;
      }
      else if (msg == 2){
         currentState = FORWARD;
      }
      else if (msg == 3){
         currentState = BACKWARD;
      }
      else if (msg == 4){
         currentState = LEFT;
      }
      else if (msg == 5){
         currentState = RIGHT;
      }
   }
   else{
      // TODO: figure out if this code is necessary
      currentState = currentState;
   }
}

// Function that holds the state machine for the robot
void motor_state_machine(){
   // TODO: Implement varying speeds into the state machines based on the joystick
   // Develop a simple state machine for the robot that controls the motors
   // with the following states: FORWARD, BACKWARD, LEFT, RIGHT, STOP
   switch(currentState)
   {
      case STOP:
         // both motors off and the robot is not moving
         brake(motorLeft, motorRight);
         break;

      case FORWARD:
         // moves the robot forward
         forward(motorLeft, motorRight, DEFAULT_SPD);
         break;

      case BACKWARD:
         // moves the robot backward
         back(motorLeft, motorRight, DEFAULT_SPD);
         break;

      case LEFT:
         // turns the robot left
         // TODO: figure out why power is decreased for this condition
         left(motorLeft, motorRight, DEFAULT_SPD);
         break;
         
      case RIGHT:
         // turns the robot right
         // TODO: figure out why power is decreased for ths condition
         right(motorLeft, motorRight, DEFAULT_SPD);
         break;
   }
}