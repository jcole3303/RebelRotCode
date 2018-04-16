#include <Arduino.h>
#include <digitalWriteFast.h>

// When SWITCH_CLOSED magnet is present
// When SWITCH_OPEN magnet is absent
#define SWITCH_CLOSED LOW
#define SWITCH_OPEN HIGH

enum COMPOSTER
{
  GoingHome,  //Going home
  StartingMixing,    // X1 Y1 bool True
  Mixing,  // Mixin process
  Paused,   // Pause button on voltage High (Green LED on)
  Error,   //halleffect sensor timeout or door open
  Stopping,// Stop button pressed and sent home to shut down
  Stopped, // all motors off
};

enum XMOTOR {
  XMotorStopped,
  XMotorMovingLeft, //random decleration
                    //Xin1 = low (red)
                    //Xin2 = high (black)
  XMotorMovingRight,//random decleration
                    //Xin1 = high
                    //Xin2 = low
  XMotorError,      // optical encoder timeout
};

enum YMOTOR {
  YMotorStopped,
  YMotorMovingBack, //random decleration
                    //Yin1 = low (red)
                    //Yin2 = high (black)
  YMotorMovingFront,//Yin1 = high
                    //Yin2 = low
};

COMPOSTER Comp_State = Stopped;
XMOTOR XMotor_State = XMotorStopped;
YMOTOR YMotor_State = YMotorStopped;

// Saved State variables
COMPOSTER Comp_State_Saved = Stopped;
XMOTOR XMotor_State_Saved = XMotorStopped;
YMOTOR YMotor_State_Saved = YMotorStopped;
bool Auger_State_Saved = false;

const int StartButtonpin = 40;  //Start button to intiate arduino
const int Relaypin = 38;        //Relay is the digital on for Auger motor
const int PauseButtonPin = 41; //Toggle for pause mixing process

const int X1pin  = 34;
const int X2pin  = 36;             //Halleffect sensor for X1 position
const int Xin1 = 22;  //in1 Hbridge port A
const int Xin2 = 24;  //in2 Hbridge port A

const int Y1pin  = 35;          //Halleffect sensor for Y1 position
const int Y2pin = 37;
const int Yin1 = 26;  //in1 Hbridge port B
const int Yin2 = 28;   //in2 Hbridge port B

const int XEN_A = 4;        //Port A EN
const int YEN_B = 5;        //Port B EN


int Y1 = digitalRead(Y1pin);    //Y1 Halleffect sensor no contact
int Y2 = digitalRead(Y2pin);    //Y2 Halleffect sensor no contact
int X1 = digitalRead(X1pin);    //X1 Halleffect sensor no contact
int X2 = digitalRead(X2pin);    //X2 Halleffect sensor no contact

bool MovingRight = true;
bool FirstRun = true;
bool atFront;
void SetXMotor(XMOTOR state);
void SetYMotor(YMOTOR state);
bool atHome();
void showPosition();

// State verbs
void goHome();
void pause();
void unpause();
void mix();

// X sweep variables:
long startX = 0; // X position at start of X operation
const int DeltaXPerSweep = 1000;
