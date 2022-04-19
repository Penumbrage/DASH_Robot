/******************************************************************************
File Name: default_state_machine.ino
Author: William Wang

Description: This is the state machine code for the DASH robot utilizing 5
different states (STOP, FORWARD, BACKWARD, LEFT, and RIGHT).

Dependencies:
TB6612 SparkFun Library
******************************************************************************/

// This is the library for the TB6612 that contains the class Motor and all the
// functions
#include <SparkFun_TB6612.h>

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// NOTE: F stands for the front motor driver; B stands for the back motor driver
#define AIN1_F A1
#define BIN1_F A4
#define AIN2_F A0
#define BIN2_F A5
#define PWMA_F 11
#define PWMB_F 10
#define STBY_F A3

#define AIN1_B 7
#define BIN1_B 4
#define AIN2_B 8
#define BIN2_B 2
#define PWMA_B 5
#define PWMB_B 3
#define STBY_B 6

// Define the default speed of the motors by using the PWM to get to 3.7 V
// TODO: Tune the motor speed to mimic our original testing case
#define DEFAULT_SPD 180

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward (i.e. if the motor is wired backwards
// then you can change the offset to flip the logic via software).  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motorLeft_F = Motor(AIN1_F, AIN2_F, PWMA_F, offsetA, STBY_F);
Motor motorRight_F = Motor(BIN1_F, BIN2_F, PWMB_F, offsetB, STBY_F);
Motor motorLeft_B = Motor(AIN1_B, AIN2_B, PWMA_B, offsetA, STBY_B);
Motor motorRight_B = Motor(BIN1_B, BIN2_B, PWMB_B, offsetB, STBY_B);

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

// create variables to store the messages
char msg;
String cmd = "";

void setup()
{
   // start the serial line
   Serial.begin(9600);
   // indicate we have connected to the serial line
   Serial.println("Connected to the serial line.");
}


void loop()
{
   get_current_state();
   motor_state_machine();
}

void get_current_state(){
   // TODO: FInd a method to allow for different motor speeds to be inputed from the serial line
   // Obtains a message from the serial line and updates the state case
   if (Serial.available() > 0){
      // get message from serial line
      msg = Serial.read();

      // execute command if we aren't receiving a newline character
      if (msg != '\n'){
         cmd = String(msg);
         Serial.print("The following message has been received: "); Serial.println(cmd);

         // update the state based on the message
         Serial.print("The current state is: ");
         set_current_state();
         Serial.println("");
      }
   }
}

void set_current_state() {
   // Set the current state for the state machine based on the message received
   if (cmd == "0"){
      currentState = STOP;
      Serial.println("STOP");
   }
   else if (cmd == "1"){
      currentState = FORWARD;
      Serial.println("FORWARD");
   }
   else if (cmd == "2"){
      currentState = BACKWARD;
      Serial.println("BACKWARD");
   }
   else if (cmd == "3"){
      currentState = LEFT;
      Serial.println("LEFT");
   }
   else if (cmd == "4"){
      currentState = RIGHT;
      Serial.println("RIGHT");
   }
   else {
      Serial.println("INVALID INPUT");
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
         brake(motorLeft_F, motorRight_F);
         brake(motorLeft_B, motorRight_B);
         break;

      case FORWARD:
         // moves the robot forward
         forward(motorLeft_F, motorRight_F, DEFAULT_SPD);
         forward(motorLeft_B, motorRight_B, DEFAULT_SPD);
         break;

      case BACKWARD:
         // moves the robot backward
         back(motorLeft_F, motorRight_F, DEFAULT_SPD);
         back(motorLeft_B, motorRight_B, DEFAULT_SPD);
         break;

      case LEFT:
         // turns the robot left
         // TODO: figure out why power is decreased for this condition
         left(motorLeft_F, motorRight_F, DEFAULT_SPD);
         left(motorLeft_B, motorRight_B, DEFAULT_SPD);
         break;
         
      case RIGHT:
         // turns the robot right
         // TODO: figure out why power is decreased for ths condition
         right(motorLeft_F, motorRight_F, DEFAULT_SPD);
         right(motorLeft_B, motorRight_B, DEFAULT_SPD);
         break;
   }
}