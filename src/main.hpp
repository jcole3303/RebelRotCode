#include <Arduino.h>
#include <digitalWriteFast.h>

// When SWITCH_CLOSED magnet is present
// When SWITCH_OPEN magnet is absent
#define SWITCH_CLOSED LOW
#define SWITCH_OPEN HIGH
#define on HIGH
#define off LOW
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
//Buttons
const int StartButtonpin = 41;  //Start button to intiate arduino
const int Relaypin = 43;        //Relay is the digital on for Auger motor
const int PauseButtonPin = 40; //Toggle for pause mixing process
//Halleffect sensors&motor output
const int X1pin  = 36;            //Halleffect sensor for X1 position
const int X2pin  = 37;             //Halleffect sensor for X2 position
const int Xin1 = 22;  //in1 Hbridge port A
const int Xin2 = 23;  //in2 Hbridge port A

const int Y1pin  = 32;          //Halleffect sensor for Y1 position
const int Y2pin = 33;           //Halleffect sensor for Y2 position
const int Yin1 = 24;  //in1 Hbridge port B
const int Yin2 = 25;   //in2 Hbridge port B

const int XEN_A = 4;        //Port A EN
const int YEN_B = 5;        //Port B EN

//indicator lights
const int indX1 = 50; //when at X1(closed) light is on
const int indX2 = 49; //""
const int indY1 = 48; //""
const int indY2 = 47;//""
const int indMixing = 46;// when in mixing state light is on







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
void indicator();

// X sweep variables:
long startX = 0; // X position at start of X operation
const long DeltaXPerSweep = 75000;
