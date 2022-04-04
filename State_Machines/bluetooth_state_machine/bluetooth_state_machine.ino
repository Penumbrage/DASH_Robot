/******************************************************************************
File Name: bluetooth_state_machine.ino
Author: William Wang

Description: This is the bluetooth state machine code for the DASH robot utilizing 5
different states (STOP, FORWARD, BACKWARD, LEFT, and RIGHT). A bluetooth module and 
app are required for this code to work.

Dependencies:
TB6612 SparkFun Library
SoftwareSerial
******************************************************************************/

// Include the different libraries in the code
#include <SoftwareSerial.h>
#include <SparkFun_TB6612.h>

// Create a bluetooth object and variables to store data read from bluetooth
SoftwareSerial HM10(14, 15); // RX = 14, TX = 15
char bt_msg;               // stores message from bluetooth module
String cmd = "";           // command to send to motor driver

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
#define AIN1 2
#define BIN1 6
#define AIN2 4
#define BIN2 7
#define PWMA 3
#define PWMB 5
#define STBY 9  

// Define the default speed of the motors by using the PWM to get to 3.7 V
// TODO: Tune the motor speed to mimic our original testing case
#define DEFAULT_SPD 200

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward (i.e. if the motor is wired backwards
// then you can change the offset to flip the logic via software). Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motorLeft = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motorRight = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

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
   HM10.begin(9600); // set HM10 serial at 9600 baud rate
   HM10.println("Bluetooth connection established.");
}

void loop()
{
   get_msg();
   motor_state_machine();
}

// This function gets a message from the bluetooth module when available
void get_msg(){
    HM10.listen();  // listen the HM10 port
    if (HM10.available() > 0) {   // if HM10 sends something then read
        bt_msg = HM10.read();
        cmd = String(bt_msg);  // save the data in string format
        // print confirmation message and set current state
        HM10.print("The following command has been received: "); HM10.println(cmd); 
        HM10.print("The current state is: ");
        set_current_state();
        HM10.println("");
    }
}

// This function gets the current state information from the bluetooth module
void set_current_state(){
   // Obtains a message from the serial line and updates the state case / prints confirmation message to application
   if (cmd == "0"){
      currentState = STOP;
      HM10.println("STOP");
   }
   else if (cmd == "1"){
      currentState = FORWARD;
      HM10.println("FORWARD");
   }
   else if (cmd == "2"){
      currentState = BACKWARD;
      HM10.println("BACKWARD");
   }
   else if (cmd == "3"){
      currentState = LEFT;
      HM10.println("LEFT");
   }
   else if (cmd == "4"){
      currentState = RIGHT;
      HM10.println("RIGHT");
   }
}

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