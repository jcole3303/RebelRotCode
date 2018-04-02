#include <Arduino.h>
#include <digitalWriteFast.h>

// Halleffect sensor is low when magnet is pressent
//  High when magnet is absent

enum Composter
{
  Comp_GoHome,  //Going home
  Comp_Home,    // X1 Y1 bool True
  Comp_Mixing,  // Mixin process
  Comp_Pause,   // Pause button pushed
  Comp_Error,  //halleffect sensor timeout or door open
  Comp_Stop,
};

enum motors
{
  XMotorStopped,
  XMotorMovingLeft, //random decleration
                    //Xin1 = low (red)
                    //Xin2 = high (black)
  XMotorMovingRight,//random decleration
                    //Xin1 = high
                    //Xin2 = low
  XMotorError,      // optical encoder timeout
  YMotorStopped,
  YMotorMovingBack, //random decleration
                    //Yin1 = low (red)
                    //Yin2 = high (black)
  YMotorMovingFront,//Yin1 = high
                    //Yin2 = low
  };
Composter Comp_State = Comp_Stop;
motors XMotor_State = XMotorStopped;
motors YMotor_State = YMotorStopped;

const int StartButtonpin = 40;  //Start button to intiate arduino
const int Relaypin = 38;        //Relay is the digital on for Auger motor
const int PauseButtonpin = 41; //Toggle for pause mixing process

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

const int XEncoder_indexpin = 1;//index
const int XEncoder_chA =2;       //channel A

int Start = LOW;
int PauseButton = LOW;
int PauseCount = 1;
int Relay = LOW;
int Y1 = digitalRead(Y1pin);    //Y1 Halleffect sensor no contact
int Y2 = digitalRead(Y2pin);    //Y2 Halleffect sensor no contact
int X1 = digitalRead(X1pin);    //X1 Halleffect sensor no contact
int X2 = digitalRead(X2pin);    //X2 Halleffect sensor no contact

bool MovingRight = true;
bool FirstRun = true;
