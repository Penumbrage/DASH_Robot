/******************************************************************************
TestRun.ino
TB6612FNG H-Bridge Motor Driver Example code
Michelle @ SparkFun Electronics
8/20/16
https://github.com/sparkfun/SparkFun_TB6612FNG_Arduino_Library

Uses 2 motors to show examples of the functions in the library.  This causes
a robot to do a little 'jig'.  Each movement has an equal and opposite movement
so assuming your motors are balanced the bot should end up at the same place it
started.

Resources:
TB6612 SparkFun Library

Development environment specifics:
Developed on Arduino 1.6.4
Developed with ROB-9457
******************************************************************************/

// This is the library for the TB6612 that contains the class Motor and all the
// functions
#include <SparkFun_TB6612.h>
#include <Wire.h>

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// the default pins listed are the ones used on the Redbot (ROB-12097) with
// the exception of STBY which the Redbot controls with a physical switch
#define AIN1 2
#define BIN1 6
#define AIN2 4
#define BIN2 7
#define PWMA 3
#define PWMB 5
#define STBY 9

// Define the default speed of the motors by using the PWM to get to 3.3 V
// TODO: Tune the motor speed to mimic our original testing case
#define DEFAULT_SPD 180

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward.  Value can be 1 or -1
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
   // start the serial line
   Serial.begin(9600);

   // initialize the current message to be STOP state
   // TODO: Check if this is causing the state to reset
   msg = 1;
}


void loop()
{
   get_current_state();
   motor_state_machine();
}

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

void example_code(){
   //Use of the drive function which takes as arguements the speed
   //and optional duration.  A negative speed will cause it to go
   //backwards.  Speed can be from -255 to 255.  Also use of the 
   //brake function which takes no arguements.
   // motor1.drive(255,1000);
   // motor1.drive(-255,1000);
   // motor1.brake();
   // delay(1000);
  
   //Use of the drive function which takes as arguements the speed
   //and optional duration.  A negative speed will cause it to go
   //backwards.  Speed can be from -255 to 255.  Also use of the 
   //brake function which takes no arguements.
   // motor2.drive(255,1000);
   // motor2.drive(-255,1000);
   // motor2.brake();
   // delay(1000);
   
   //Use of the forward function, which takes as arguements two motors
   //and optionally a speed.  If a negative number is used for speed
   //it will go backwards
   // forward(motor1, motor2, 150);
   // delay(1000);
   
   //Use of the back function, which takes as arguments two motors 
   //and optionally a speed.  Either a positive number or a negative
   //number for speed will cause it to go backwards
   // back(motor1, motor2, -150);
   // delay(1000);
   
   //Use of the brake function which takes as arguments two motors.
   //Note that functions do not stop motors on their own.
   // brake(motor1, motor2);
   // delay(1000);
   
   //Use of the left and right functions which take as arguements two
   //motors and a speed.  This function turns both motors to move in 
   //the appropriate direction.  For turning a single motor use drive.
   // left(motor1, motor2, 100);
   // delay(1000);
   // right(motor1, motor2, 100);
   // delay(1000);
   
   //Use of brake again.
   // brake(motor1, motor2);
   // delay(1000);
}
