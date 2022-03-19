// /*
// The following code is used to test the bluetooth module (obtained from an online resource)
// */
// // NOTE: SoftwareSerial is a library that allows us to define other pins as TX/RX pins
// #include <SoftwareSerial.h>       
// SoftwareSerial HM10(2, 3); // RX = 2, TX = 3
// char appData;  
// String inData = "";

// void setup()
// {
//  Serial.begin(9600);
//  Serial.println("HM10 serial started at 9600");
//  HM10.begin(9600); // set HM10 serial at 9600 baud rate
// }

// void loop()
// {
// //  HM10.listen();  // listen the HM10 port
//  while (HM10.available() > 0) {   // if HM10 sends something then read
//    appData = HM10.read();
//    inData = String(appData);  // save the data in string format
//    HM10.write(appData);
//    Serial.write(appData);
//  }

//  if (Serial.available()) {           // Read user input if available.
//    delay(10);
//    Serial.write(Serial.read());
//  }
//  if ( inData == "F") {
//    HM10.println("LED OFF");
//    Serial.println("LED OFF COMPUTER");
//    delay(500);
//  }
//  if ( inData == "N") {
//    HM10.println("LED ON");
//    Serial.println("LED ON COMPUTER");
//    delay(500);
//  }
// }




/*
This code serves as a test for the communication between the bluetooth module
and the Arduino
*/

// Include the different libraries in the code
#include <SoftwareSerial.h>

// Create a bluetooth object and variables to store data read from bluetooth
SoftwareSerial HM10(14, 15); // RX = 14, TX = 15 (NOTE: Analog in pins cannot handle TX RX commands)
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
  // Serial.begin(9600);
  // Serial.println("HM10 serial started at 9600");
  HM10.begin(9600); // set HM10 serial at 9600 baud rate
  HM10.println("Bluetooth connection established.");
}

void loop()
{
   get_msg();
   set_current_state();
   motor_state_machine();
}

// This function gets a message from the bluetooth module when available
void get_msg(){
    HM10.listen();  // listen the HM10 port
    while (HM10.available() > 0) {   // if HM10 sends something then read
        bt_msg = HM10.read();
        cmd = String(bt_msg);  // save the data in string format
        HM10.print("The following command has been received: "); HM10.println(cmd); HM10.println("");
        delay(1000);
    }
}

// This function gets the current state information from the bluetooth module
void set_current_state(){
   // Obtains a message from the serial line and updates the state case
   if (cmd == "0"){
      currentState = STOP;
   }
   else if (cmd == "1"){
      currentState = FORWARD;
   }
   else if (cmd == "2"){
      currentState = BACKWARD;
   }
   else if (cmd == "3"){
      currentState = LEFT;
   }
   else if (cmd == "4"){
      currentState = RIGHT;
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
         HM10.print("The current state is: "); HM10.println("STOP");
         delay(1000);
         break;

      case FORWARD:
         // moves the robot forward
         HM10.print("The current state is: "); HM10.println("FORWARD");
         delay(1000);
         break;

      case BACKWARD:
         // moves the robot backward
         HM10.print("The current state is: "); HM10.println("BACKWARD");
         delay(1000);
         break;

      case LEFT:
         // turns the robot left
         // TODO: figure out why power is decreased for this condition
         HM10.print("The current state is: "); HM10.println("LEFT");
         delay(1000);
         break;
         
      case RIGHT:
         // turns the robot right
         // TODO: figure out why power is decreased for ths condition
         HM10.print("The current state is: "); HM10.println("RIGHT");
         delay(1000);
         break;
   }
}
