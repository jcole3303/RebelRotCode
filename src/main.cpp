#include "main.hpp"

void setup()
{
  pinMode(Relaypin, OUTPUT);

  pinMode(Xin1, OUTPUT);
  pinMode(Xin2, OUTPUT);
  pinMode(Yin1, OUTPUT);
  pinMode(Yin2, OUTPUT);
  pinMode(XEN_A, OUTPUT);
  pinMode(YEN_B, OUTPUT);

  pinMode(StartButtonpin, INPUT);

  pinMode(X1pin, INPUT);
  pinMode(X2pin, INPUT);
  pinMode(Y1pin, INPUT);
  pinMode(Y2pin, INPUT);

  pinMode(PauseButtonpin,INPUT);
  pinMode(XEncoder_indexpin,INPUT);
  pinMode(XEncoder_chA,INPUT);
  Serial.begin(9600);
    Serial.write("inizalized\n");
    Y1 = digitalRead(Y1pin);
    Serial.println(Y1);
    Y2 = digitalRead(Y2pin);
    Serial.println(Y2);
    X1 = digitalRead(X1pin);
    Serial.println(X1);
    X2 = digitalRead(X2pin);
    Serial.println(X2);

} //void setup

void loop()
{
 Serial.println(Start);
 Start = digitalRead(StartButtonpin);
 //ON/OFF switch Section
 if  (Start == HIGH)
 {
  digitalWrite(Relaypin, HIGH);  //Turn relay on
  Y1 = digitalRead(Y1pin);
  Serial.print("Y1");
  Serial.println(Y1);
  Y2 = digitalRead(Y2pin);
  Serial.print("Y2");
  Serial.println(Y2);
  X1 = digitalRead(X1pin);
  Serial.print("X1");
  Serial.println(X1);
  X2 = digitalRead(X2pin);
  Serial.print("X2");
  Serial.println(X2);

  switch (Comp_State)
  {

    case Comp_GoHome: //In motion to go home
    Serial.println("Comp_GoHome");

        Y1 = digitalRead(Y1pin);
        Y2 = digitalRead(Y2pin);
        X1 = digitalRead(X1pin);
        X2 = digitalRead(X2pin);
        Start = digitalRead(StartButtonpin);
        if (Start == HIGH)
        {
          if ((X1 == LOW) && (Y1 == LOW))
          { Comp_State = Comp_Home;
            XMotor_State = XMotorStopped;
            YMotor_State = YMotorStopped;
            digitalWrite(Xin1,LOW);
            digitalWrite(Xin2,LOW);
            analogWrite(XEN_A, 0);

            digitalWrite(Yin1,LOW);
            digitalWrite(Yin2,LOW);
            analogWrite(YEN_B, 0);
          }
          else if (X1 == LOW)
          { XMotor_State = XMotorStopped;
            digitalWrite(Xin1,LOW);
            digitalWrite(Xin2,LOW);
            analogWrite(XEN_A, 0);

            if (Y1 == HIGH)
            { YMotor_State = YMotorMovingBack;
              digitalWrite(Yin1,LOW);
              digitalWrite(Yin2,HIGH);
              analogWrite(YEN_B,125);
            }
          }
          else if (X1 == HIGH)
          { XMotor_State = XMotorMovingLeft;
            digitalWrite(Xin1,LOW);
            digitalWrite(Xin2,HIGH);
            analogWrite(XEN_A,255);
          }
          else
          { XMotor_State = XMotorMovingLeft;
            Serial.println("Comp_Home XMLeft");
            digitalWrite(Xin1,LOW);
            digitalWrite(Xin2,HIGH);
            analogWrite(XEN_A,255);
            YMotor_State = YMotorMovingBack;
            digitalWrite(Yin1,LOW);
            digitalWrite(Yin2,HIGH);
            analogWrite(YEN_B,125);
          }
        }
      /*  else
        {
          Comp_State = Comp_Stop;
          Serial.println("Comp_GoHome
          ");
        }*/
    break;

    case  Comp_Home:
    Serial.println("Comp_Home");
    delay(3000);
        MovingRight = true;
        Start = digitalRead(StartButtonpin);
        XMotor_State = XMotorStopped;
        digitalWrite(Xin1,LOW);
        digitalWrite(Xin2,LOW);
        analogWrite(XEN_A,0);
        if (Start == HIGH)
        { YMotor_State = YMotorMovingFront;
          Serial.println("YMotorMovingFront");
          digitalWrite(Yin1,HIGH);
          digitalWrite(Yin2,LOW);
          analogWrite(YEN_B,125);

          Comp_State = Comp_Mixing;
        }
    break;
    case Comp_Mixing:
          Serial.println("Comp_Mixing");
        switch (XMotor_State)
        { case XMotorStopped:
          Serial.println("XMotorStopped");
          Y1 = digitalRead(Y1pin);
          Y2 = digitalRead(Y2pin);
            if (YMotor_State == YMotorMovingFront)
              { digitalWrite(Yin1,HIGH);
                digitalWrite(Yin2,LOW);
                analogWrite(YEN_B,125);
                if (Y2 == LOW)
                { YMotor_State = YMotorStopped;
                  digitalWrite(Yin1,LOW);
                  digitalWrite(Yin2,LOW);
                  analogWrite(YEN_B, 0);
                  if (MovingRight)
                  {
                    X2 = digitalRead(X2pin);
                    if (X2 == LOW)  //auger is at X1-Y2 (right back corner)
                    {
                     YMotor_State = YMotorMovingFront;
                     digitalWrite(Yin1,HIGH);
                     digitalWrite(Yin2,LOW);
                     analogWrite(YEN_B, 155);
                     MovingRight = false;
                    }
                    else
                    {
                      MovingRight = true;
                      XMotor_State = XMotorMovingRight;
                    }
                  }

                  else
                  {
                    MovingRight = false;
                    XMotor_State = XMotorMovingLeft;
                  }
                }
              }
            else if (YMotor_State == YMotorMovingBack)
              { digitalWrite(Yin1,LOW);
                digitalWrite(Yin2,HIGH);
                analogWrite(YEN_B,125);
                if (Y1 == LOW)
                { YMotor_State = YMotorStopped;
                  digitalWrite(Yin1,LOW);
                  digitalWrite(Yin2,LOW);
                  analogWrite(YEN_B,0);
                  if (MovingRight)
                  { X2 = digitalRead(X2pin);
                      if (X2 == LOW) //auger is at Y2-X2 (Back right corner)
                      {
                        XMotor_State = XMotorStopped;
                        digitalWrite(Xin1,LOW);
                        digitalWrite(Xin2,LOW);
                        analogWrite(XEN_A,0);
                        MovingRight = false;
                        XMotor_State = XMotorMovingLeft;
                        //handled in XMotorMovingLeft case
                      }

                      XMotor_State = XMotorMovingRight;
                      //handled in XMotorMovingRight case
                  }
                  else
                  {
                    MovingRight = false;
                    XMotor_State = XMotorMovingLeft;
                    //handled in XMotorMovingLeft case
                  }
                }
              }
            break;

          case XMotorMovingRight:
           Serial.write("XMotorMovingRight");
            X2 = digitalRead(X2pin);
            Y1 = digitalRead(Y1pin);
            Y2 = digitalRead(Y2pin);

            MovingRight = true;
            if (X2==HIGH)
            {
              //while ((X2 == HIGH) && ((encodercounter<sumnumber)|(timer<sumtime))
              {
                digitalWrite(Xin1, HIGH);
                digitalWrite(Xin2, LOW);
                analogWrite(XEN_A, 255);
                //counter++
                //increase timer as well
              }
              XMotor_State = XMotorStopped;
              digitalWrite(Xin1, LOW);
              digitalWrite(Xin2, LOW);
              analogWrite(XEN_A, 0);
              //if timer > sum timer
              //Comp_State = Comp_Stop

              //if encodercounter > sumnumber & Y1 == low
              // YMotor_State = YMotorMovingFront;
              //send YMotor front
              //else YMotor_State = YMotoringBack
              //sned YMotor back
            }
              if ((X2 == LOW) && (Y1 == LOW))
                {
                  XMotor_State = XMotorStopped;
                  YMotor_State = YMotorMovingFront;
                  MovingRight = false;
                  digitalWrite(Yin1, HIGH);
                  digitalWrite(Yin2, LOW);
                  analogWrite(XEN_A, 125);
                }
              else if ((X2 == LOW) && (Y2 == LOW))
              {
                XMotor_State = XMotorStopped;
                YMotor_State = YMotorMovingBack;
                MovingRight = false;
                digitalWrite(Xin1, LOW);
                digitalWrite(Xin2, HIGH);
                analogWrite(XEN_A,255);
              }

          break;
          case XMotorMovingLeft:
          Serial.println("XMotorMovingLeft");
           X1 = digitalRead(X1pin);
              if (X1 == HIGH)
                {
                  XMotor_State = XMotorMovingLeft;
                  digitalWrite(Xin1,LOW);
                  digitalWrite(Xin2,HIGH);
                  analogWrite(XEN_A, 255);
                  delay(10000);  // only to replace optical encoder
                  XMotor_State = XMotorStopped;
                  MovingRight = true;
                }
              else if (X1 == LOW)
              {
              XMotor_State = XMotorStopped;
              digitalWrite(Xin1,LOW);
              digitalWrite(Xin2,LOW);
              analogWrite(XEN_A,0);
              Y1 = digitalRead(Y1pin);
              Y2 = digitalRead(Y2pin);
              MovingRight = false;
                if (Y2 == HIGH)
                {
                  YMotor_State = YMotorMovingFront;
                  digitalWrite(Yin1,HIGH);
                  digitalWrite(Yin2,LOW);
                  analogWrite(YEN_B,125);
                }
                else if (Y2 == LOW)
                {
                  YMotor_State = YMotorStopped;
                  digitalWrite(Yin1,LOW);
                  digitalWrite(Yin2,LOW);
                  analogWrite(YEN_B,0);
                  XMotor_State = XMotorMovingLeft;
                  digitalWrite(Xin1,LOW);
                  digitalWrite(Xin2,HIGH);
                  analogWrite(XEN_A,255);
                }
              }
          break;

          case XMotorError:
            Serial.println("XMotorError");
            Comp_State = Comp_Stop;//chang comp state and also stop motors
            digitalWrite(Xin1,LOW);//stop x motor but dont change motor state
            digitalWrite(Xin2,LOW);
            analogWrite(XEN_A,0);

            digitalWrite(Yin1,LOW);//stop y motor but dont change motor state
            digitalWrite(Yin2,LOW);
            analogWrite(YEN_B, 0);

          break;


      //Mixing_XMotor_State Switch



        case YMotorStopped:
          Serial.println("YMotorStopped");
          digitalWrite(Yin1,LOW);
          digitalWrite(Yin2,LOW);
          analogWrite(YEN_B, 0);
           if (MovingRight)
           { X2 = digitalRead(X2pin);
             if (X2 == LOW)
             {
               XMotor_State = XMotorStopped;
               digitalWrite(Xin1,LOW);
               digitalWrite(Xin2,LOW);
               analogWrite(XEN_A,0);
               delay(10000); // only to replace optical encoder
               MovingRight = false;
             }
           }
           else
           {
             X1 = digitalRead(X1pin);
             if (X1 == LOW)
             {
               XMotor_State = XMotorStopped;
               digitalWrite(Xin1,LOW);
               digitalWrite(Xin2,LOW);
               analogWrite(XEN_A,0);
               MovingRight = true;
             }
           }
        break;

        case YMotorMovingBack:
          Serial.println("YMotorMovingBack");
        break;

        case YMotorMovingFront:
          Serial.println("YMotorMovingFront");
        break;


     }//Mixing_YMotor_State switch
// Might have to denote different Xmotor enum and YMotor enum
    case Comp_Pause:
      Serial.println("Comp_Pause");
      PauseButton = digitalRead(PauseButtonpin);
      PauseCount = PauseCount;
      if (PauseButton == HIGH)
        {
          PauseCount = -(PauseCount); //questionable statment here
        }
        if (PauseCount < 0)
        {
        XMotor_State = XMotorStopped;
        digitalWrite(Xin1, LOW);
        digitalWrite(Xin2, LOW);
        analogWrite(XEN_A, 0);
        YMotor_State = YMotorStopped;
        digitalWrite(Yin1, LOW);
        digitalWrite(Yin2, LOW);
        analogWrite(YEN_B, 0);
        }

      //will never reach case where it was paused and you can click start
      //because you can't pause a stopped procedure
      break;
    case Comp_Error:
      Serial.println("Comp_Error");
      //red led blink and Both motor states = MotorStop

    break;
    case Comp_Stop:
      Serial.println("Comp_Stop");
      if ((X1 == LOW) && (Y1 == LOW))
      { Comp_State = Comp_Home;
        XMotor_State = XMotorStopped;
        YMotor_State = YMotorStopped;
        digitalWrite(Xin1,LOW);
        digitalWrite(Xin2,LOW);
        analogWrite(XEN_A, 0);

        digitalWrite(Yin1,LOW);
        digitalWrite(Yin2,LOW);
        analogWrite(YEN_B, 0);
      }
      else if (X1 == LOW)
      { XMotor_State = XMotorStopped;
        digitalWrite(Xin1,LOW);
        digitalWrite(Xin2,LOW);
        analogWrite(XEN_A, 0);

        if (Y1 == HIGH)
        { YMotor_State = YMotorMovingBack;
          digitalWrite(Yin1,LOW);
          digitalWrite(Yin2,HIGH);
          analogWrite(YEN_B,125);
        }
      }
        else if (X1 == HIGH)
        { XMotor_State = XMotorMovingLeft;
          digitalWrite(Xin1,LOW);
          digitalWrite(Xin2,HIGH);
          analogWrite(XEN_A,255);
        }
      else
      { XMotor_State = XMotorMovingLeft;
        digitalWrite(Xin1,LOW);
        digitalWrite(Xin2,HIGH);
        analogWrite(XEN_A,255);
        YMotor_State = YMotorMovingBack;
        digitalWrite(Yin1,LOW);
        digitalWrite(Yin2,HIGH);
        analogWrite(YEN_B,125);
      }
    break;

    default:
    Comp_State = Comp_GoHome;

    } // COMP Switch
} //start if
else    //STOP BUTTON
 {
   if (Comp_State == Comp_GoHome)
   {

   }
   Serial.println("Stop not pushed");
  if (Comp_State == Comp_Home)
  {   Serial.println("Comp_Home and Stop");
    digitalWrite(Relaypin, LOW);
      Serial.println("Shut down");

    //turn off mixing LED
  }
  else
  {
    digitalWrite(Relaypin, LOW);
    Serial.println("Relay Off");
    Serial.println("Start Button not pushed");
  }
 }
 //IMPLEMENT PAUSE TRIGGER

  //if Pause ==high
      //if Start == high (NOW WE ARE IN A PAUSE CASE)
        //find what state we are in and set GoingHOme = true or false cont.
          // if Comp_State = GoHome or Mixing
        //stop all motors (ACTION, not STATE)
//if PAUSE == LOW
  //if Start==high ( VALID UNPAUSE )
    //IF MOTORS at least one motor is moving,
    //READ ALL THE MOTOR STATES, and make them move based on that state
    //bUt check GoingHome boolean to know what to changed your COMP_STATE to afterwards
Serial.println("End");
delay(8000);
} // void loop
