/******************************************************************************
File Name: tracker_state_machine.ino
Author: William Wang, Elena Stirk

Description: This is the tracker state machine code for the LEGO robot utilizing 5
different basic states (STOP, FORWARD, BACKWARD, LEFT, and RIGHT) and some custom
functions that allow the robot to follow a pheromone trail. A bluetooth module and 
app are required for this code to work.

Dependencies:
TB6612 SparkFun Library
SoftwareSerial
Servo Library
Adafruit CCS811 Library
******************************************************************************/

// Include the different libraries in the code
#include <SoftwareSerial.h>
#include <SparkFun_TB6612.h>
#include <Servo.h>
#include "Adafruit_CCS811.h"

// Create a bluetooth object and variables to store data read from bluetooth
SoftwareSerial HM10(9, 12); // RX = 9, TX = 12
char bt_msg;               // stores message from bluetooth module
String cmd = "";           // command to send to motor driver

// Create smell sensor object
Adafruit_CCS811 ccs;

// create servo object to control a servo (twelve servo objects can be createed on most boards)
Servo myservo;   

// Define the servo sweep wait time between readings
#define SERVO_WAIT_TIME 2000

// Develop a lookup table with the turn angles for the robot
float turn_angle[10] = {-60, -30, 0, 30, 60, 60, 30, 0, -30, -60};

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// NOTE: F stands for the front motor driver; B stands for the back motor driver
// NOTE: let's define the frame of reference to be looking from the back of the car
//       We will define the all the A pins to be associated with the left side motors,
//       and the right side motors will be defined with the B pins
// NOTE: Let's also have all the positive wires from the motors go into the 1 pins, and
//       the negative wires to go into the 2 pins
#define AIN1_F A0
#define AIN2_F A1
#define BIN1_F A2
#define BIN2_F A3
#define PWMA_F 10
#define PWMB_F 11
#define STBY_F A4

#define AIN1_B 2
#define AIN2_B 4
#define BIN1_B 6
#define BIN2_B 7
#define PWMA_B 3
#define PWMB_B 5
#define STBY_B 8

// Define the default speed of the motors by using the PWM to get to 3.7 V
// TODO: Tune the motor speed to mimic our original testing case
#define DEFAULT_SPD 255

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward (i.e. if the motor is wired backwards
// then you can change the offset to flip the logic via software). Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motorLeft_F = Motor(AIN1_F, AIN2_F, PWMA_F, -1, STBY_F);
Motor motorRight_F = Motor(BIN1_F, BIN2_F, PWMB_F, 1, STBY_F);
Motor motorLeft_B = Motor(AIN1_B, AIN2_B, PWMA_B, -1, STBY_B);
Motor motorRight_B = Motor(BIN1_B, BIN2_B, PWMB_B, 1, STBY_B);

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
   
   myservo.attach(6);  // attaches the servo on pin 6 to the servo object

   if(!ccs.begin()){
      HM10.println("Failed to start sensor! Please check your wiring.");
      while(1);
   }

   // Wait for the sensor to be ready
   while(!ccs.available());
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

        // check to make sure we are not reading a new line character
        if (bt_msg != '\n'){
            cmd = String(bt_msg);  // save the data in string format
            // print confirmation message and set current state
            HM10.print("The following command has been received: "); HM10.println(cmd); 
            HM10.print("The current state is: ");
            set_current_state();
            HM10.println("");
        }

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
   else {
      HM10.println("INVALID INPUT");
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
         left(motorLeft_F, motorRight_F, 0.8*DEFAULT_SPD);
         left(motorRight_B, motorLeft_B, 0.8*DEFAULT_SPD);
         break;
         
      case RIGHT:
         // turns the robot right
         // TODO: figure out why power is decreased for ths condition
         right(motorLeft_F, motorRight_F, 0.8*DEFAULT_SPD);
         right(motorRight_B, motorLeft_B, 0.8*DEFAULT_SPD);
         break;
   }
}

// This function is used to perform the sensor sweep using the servo/VOC sensor unit
float get_turn_angle(){
   // develop an array of sensor readings
   // Create variables to store the values obtained from the smell sensor sweep
   float SensorArray[10];        // array to store values from VOC sweep
   int arrayIndex = 0;           // index for the sensor array
   float maxVOCReading;          // initialize variable to store max VOC reading
   float servo_pos;              // initialize variable to store the servo position
   int maxVOCIndex;              // initialize variable to store the index of the max VOC reading
   float req_turn_angle;         // initialize variable to return required turn angle

   // readings in the following form 0:30:120
   for(servo_pos = 0; servo_pos <= 120; servo_pos += 30){
      myservo.write(servo_pos); 
      delay(SERVO_WAIT_TIME);

      // read the VOC sensor after SERVO_WAIT_TIME and store reading in the array
      if (ccs.available()) {
         if(!ccs.readData()){
            SensorArray[arrayIndex] = ccs.getTVOC();
            arrayIndex++;
         }
      }
   }

   // readings in the following form 120:-30:0
   for(servo_pos = 120; servo_pos >= 0; servo_pos -= 30){
      myservo.write(servo_pos); 
      delay(SERVO_WAIT_TIME);

      // read the VOC sensor after SERVO_WAIT_TIME and store the reading in the array
      if (ccs.available()) {
         if(!ccs.readData()){
            SensorArray[arrayIndex] = ccs.getTVOC();
            arrayIndex++;
         }
      }
   }

   // determine the array index where the maximum VOC value is stored
   maxVOCReading = SensorArray[0];        // initialize the maxVOC reading variable to be the first element
   for (int i = 0; i < (sizeof(SensorArray) / sizeof(SensorArray[0])); i++) {
      if (maxVOCReading >= SensorArray[i]) {
         maxVOCIndex = i;
      }
   }

   // obtain the required turn angle and return its value
   req_turn_angle = turn_angle[maxVOCIndex];

   return req_turn_angle;
}

// This function takes in the servo angle measured by the servo (used as the desired turn angle) and changes it into the a turn time
float turn_timer_function(float servo_angle){
   float time_turn;

   // (102.857 degrees per second)
   time_turn = servo_angle/102.857142857;

   return time_turn;
}


// This function is used to turn the robot a specified amount given the servo angle
void robot_turn(float servo_angle){


   float time_turn = turn_timer_function(servo_angle);
}