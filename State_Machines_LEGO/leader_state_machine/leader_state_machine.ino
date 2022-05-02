/******************************************************************************
File Name: bluetooth_state_machine.ino
Author: William Wang

Description: This is the bluetooth state machine code for the LEGO robot utilizing 5
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
SoftwareSerial HM10(9, 12); // RX = 9, TX = 12
char bt_msg;               // stores message from bluetooth module
String cmd = "";           // command to send to motor driver

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

#define PUMP A5 //Pump is connected to A5

// Define the default speed of the motors by using the PWM to get to 3.7 V
// TODO: Tune the motor speed to mimic our original testing case
#define DEFAULT_SPD 225

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward (i.e. if the motor is wired backwards
// then you can change the offset to flip the logic via software). Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motorLeft_F  = Motor(AIN1_F, AIN2_F, PWMA_F, 1, STBY_F);
Motor motorRight_F = Motor(BIN1_F, BIN2_F, PWMB_F, 1, STBY_F);
Motor motorLeft_B  = Motor(AIN1_B, AIN2_B, PWMA_B, 1, STBY_B);
Motor motorRight_B = Motor(BIN1_B, BIN2_B, PWMB_B, 1, STBY_B);

// Initialize an enumeration to describe the different states for the motors
typedef enum{
   FORWARD,
   BACKWARD,
   LEFT,
   RIGHT,
   STOP,
   TRAIL,
   T_STR,
   T_CUR,
   T_S_FOR,
   T_C_FOR,
   T_LEF,
   FILL
} motorState;

// Initialize the trailStep number -> Important for Autonomous trail creation.
int trailStep = 0;

// Initialize the state of the motor (STOP)
motorState currentState = STOP;

void setup()
{
   HM10.begin(9600); // set HM10 serial at 9600 baud rate
   HM10.println("Bluetooth connection established.");

   pinMode(PUMP, OUTPUT); //sets the digital pin as output
}

void loop()
{
   get_msg();
   motor_state_machine();

   if(currentState == FORWARD) {
    
    run_pump();
    
   }
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
   else if (cmd == "5"){
      currentState = TRAIL;
      HM10.println("TRAIL");
   }
   else if (cmd == "6"){
      currentState = FILL;
      HM10.println("FILL");
   }
   else {
      HM10.println("INVALID INPUT");
   }
}

void motor_state_machine(){
   // TODO: Implement varying speeds into the state machines based on the joystick
   // Develop a simple state machine for the robot that controls the motors
   // with the following states: FORWARD, BACKWARD, LEFT, RIGHT, STOP
   // Autonomous States: TRAIL,T_STR, T_CUR, T_FOR, T_LEF
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
         // TODO: figure out why robot will stall after a few seconds of running
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

      case TRAIL:
        // Begins the Autonomous Trail Creation

        currentState = T_STR;
        break;

      case T_STR:

        if(trailStep <= 5) {
          currentState = T_S_FOR;
        } else {
          currentState = T_CUR;
        }

        break;

      case T_CUR:

      if(trailStep <= 10) {
          currentState = T_C_FOR;
        } else {
          currentState = STOP;
        }


        break;

      case T_S_FOR:

        // moves the robot forward
        forward(motorLeft_F, motorRight_F, DEFAULT_SPD);
        forward(motorLeft_B, motorRight_B, DEFAULT_SPD);
        
        delay(2000);
        brake(motorLeft_F, motorRight_F);
        brake(motorLeft_B, motorRight_B);

        delay(500);
        run_pump();
        //Enumerates trailStep
        trailStep = trailStep + 1;

        //Changes motorState back to T_STR
        currentState = T_STR;

        break;

      case T_C_FOR:

        // moves the robot forward
        forward(motorLeft_F, motorRight_F, DEFAULT_SPD);
        forward(motorLeft_B, motorRight_B, DEFAULT_SPD);

        delay(2000);
        brake(motorLeft_F, motorRight_F);
        brake(motorLeft_B, motorRight_B);

        delay(500);
        run_pump();

        //Changes motorState to T_LEF
        currentState = T_LEF;

        break;

      case T_LEF:

        //Turns robot left small amount
        left(motorLeft_F, motorRight_F, DEFAULT_SPD);
        left(motorLeft_B, motorRight_B, DEFAULT_SPD);

        delay(2000);
        brake(motorLeft_F, motorRight_F);
        brake(motorLeft_B, motorRight_B);
      
        //Enumerates trailStep
        trailStep = trailStep + 1;

        //Changes motorState to T_CUR
        currentState = T_CUR;

        break;

      case FILL:
      
        digitalWrite(PUMP,HIGH); //turns the pump on
        delay(1000);
        digitalWrite(PUMP,LOW);
        delay(200);

        break;
        
   }
}

void run_pump(){

  digitalWrite(PUMP,HIGH); //turns the pump on
  delay(500);
  digitalWrite(PUMP,LOW);
  delay(1000);
  
}
