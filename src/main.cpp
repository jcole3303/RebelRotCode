#include "main.hpp"
#include <Encoder.h>

Encoder XEncoder(2, 3);

void setup() {
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

  pinMode(PauseButtonPin,INPUT);


  Serial.begin(115200);

  Serial.write("Initialized\n");
  Y1 = digitalRead(Y1pin);
  Serial.println(Y1);
  Y2 = digitalRead(Y2pin);
  Serial.println(Y2);
  X1 = digitalRead(X1pin);
  Serial.println(X1);
  X2 = digitalRead(X2pin);
  Serial.println(X2);
} //void setup

void loop() {
  // Debug:
  /*
  Y1 = digitalRead(Y1pin);
  Serial.print("Y1");
  Serial.println(Y1);
  Serial.print("X1");
  Y2 = digitalRead(Y2pin);
  Serial.print("Y2");
  Serial.println(Y2);
  X1 = digitalRead(X1pin);
  Serial.println(X1);
  X2 = digitalRead(X2pin);
  Serial.print("X2");
  Serial.println(X2);
  */

  // We are stopped, they hit the start button.
  if (digitalRead(StartButtonpin) == HIGH && Comp_State == Stopped) {
    // We were in the Stopped state and they pushed the
    // Start button; ensure we are home and begin the cycle.
    Comp_State = GoingHome;

    // Also, activate the auger.
    digitalWrite(Relaypin, HIGH);

    Serial.println("Start button pressed and machine stopped; now starting.");
  }

  // We are not stopped, they hit the stop button.
  if (digitalRead(StartButtonpin) == LOW && Comp_State != Stopped) {
    Comp_State = Stopping;
    Serial.println("Stop button pressed and going home to shut down");
  }

  // The pause button is pressed and we aren't paused.
  bool pauseButtonState = digitalRead(PauseButtonPin);
  if (pauseButtonState && Comp_State != Paused) {
    // Saves current state, stops motors, and
    // sets Comp_State to Paused:
    pause();
    Serial.println("Pause button pressed entering pause state");
  }
  else if (!pauseButtonState && Comp_State == Paused) {
    // Restores current state (including motor state)
    // and restores Comp_State to previous value.
    unpause();
    Serial.println("Paused button turned off - running unpause");
  }


  switch (Comp_State) {
    case GoingHome: //In motion to go home
      Serial.println("Comp_State = GoingHome");
      if (!atHome()) {
        goHome();
      }
      else {
        SetXMotor(XMotorStopped);
        SetYMotor(YMotorStopped);

        Serial.println("Reached home, beginning mixing.");
        Comp_State = StartingMixing;

        Serial.println("Zeroing X position.");
        showPosition();
        XEncoder.write(0);

      }
      break;

    case StartingMixing:
      Serial.println("Comp_State = StartingMixing");
      delay(3000);
      MovingRight = true;
      atFront = false;
      SetXMotor(XMotorStopped);
      Serial.println("YMotorMovingFront");
      SetYMotor(YMotorMovingFront);
      Comp_State = Mixing;
      break;

    case Mixing:
      Serial.println("Comp_State = Mixing");
      mix();
      break;

    case Paused:
      Serial.println("Comp_State = Paused");
      // Do nothing while we wait.
      break;

    case Error:
      // Halt everything.
      Serial.println("Comp_State = Error");
      digitalWrite(Relaypin, LOW);

      // TODO: Shut down X and Y motors as well.

      Serial.println("Comp_Error");
      //red led blink and Both motor states = MotorStop

      Comp_State = Stopped;
    break;

    case Stopping:
      Serial.println("Comp_State = Stopping");
      if (!atHome()) {
        goHome();
      }
      else {
        // We are home. Shut down the auger.
        SetXMotor(XMotorStopped);
        SetYMotor(YMotorStopped);
        digitalWrite(Relaypin, LOW);
        Serial.println("Machine has returned home.  Entering stop state.");
        Comp_State = Stopped;
      }
    break;

    case Stopped:
      Serial.println("Comp_State = Stopped");
      // Do nothing.

    break;

    default:
      // Somehow in an invalid state; rehome.
      Serial.print("Ended up in an invalid state:");
      Serial.println(Comp_State);
      Comp_State = Error;
      break;

  } // COMP Switch

  Serial.println("End");
  delay(1000);
} // void loop


void SetXMotor(XMOTOR state) {
  if (state == XMotorStopped) {
    digitalWrite(Xin1,LOW);
    digitalWrite(Xin2,LOW);
    analogWrite(XEN_A, 0);
    Serial.println("X motor now stopped.");
  }
  else if (state == XMotorMovingLeft) {
    digitalWrite(Xin1,LOW);
    digitalWrite(Xin2,HIGH);
    analogWrite(XEN_A,255);
    Serial.println("X motor now moving left.");
    showPosition();
  }
  else if (state == XMotorMovingRight) {
    digitalWrite(Xin1,HIGH);
    digitalWrite(Xin2,LOW);
    analogWrite(XEN_A,255);
    Serial.println("X motor now moving right.");
    showPosition();
  }
  XMotor_State = state;
}

void SetYMotor(YMOTOR state) {
  if (state == YMotorStopped) {
    digitalWrite(Yin1,LOW);
    digitalWrite(Yin2,LOW);
    analogWrite(YEN_B,0);
    Serial.println("Y motor now stopped.");
  }
  else if (state == YMotorMovingFront) {
    digitalWrite(Yin1,HIGH);
    digitalWrite(Yin2,LOW);
    analogWrite(YEN_B, 155);
    Serial.println("Y motor now moving to front.");
  }
  else if (state == YMotorMovingBack) {
    digitalWrite(Yin1,LOW);
    digitalWrite(Yin2,HIGH);
    analogWrite(YEN_B,125);
    Serial.println("Y motor now moving to back.");
  }
  YMotor_State = state;
}

bool atHome() {
  Y1 = digitalRead(Y1pin);
  X1 = digitalRead(X1pin);
  if ((X1 == SWITCH_CLOSED) && (Y1 == SWITCH_CLOSED)) {
    return true;
  }
  return false;
}

void goHome() {
  Serial.println("Running goHome()");
  Y1 = digitalRead(Y1pin);
  Y2 = digitalRead(Y2pin);
  X1 = digitalRead(X1pin);
  X2 = digitalRead(X2pin);

  if ((X1 == SWITCH_CLOSED) && (Y1 == SWITCH_CLOSED)) {
    // Probably never gets called since once atHome()
    // is true, we stop calling goHome();
    SetXMotor(XMotorStopped);
    SetYMotor(YMotorStopped);
  }
  else if (X1 == SWITCH_CLOSED) {
    SetXMotor(XMotorStopped);
    if (Y1 == SWITCH_OPEN) {
      SetYMotor(YMotorMovingBack);
    }
  }
  else if (X1 == SWITCH_OPEN) {
    SetXMotor(XMotorMovingLeft);
  }
  else {
    SetXMotor(XMotorMovingLeft);
    Serial.println("Comp_Homed XMotorMovingLeft");
    SetYMotor(YMotorMovingBack);
  }
}

void pause() {
  // Save current state.
  Comp_State_Saved = Comp_State;
  Serial.print("Comp_State_Saved = ");
  Serial.println(Comp_State_Saved);
  XMotor_State_Saved = XMotor_State;
  YMotor_State_Saved = YMotor_State;
  Auger_State_Saved = digitalRead(Relaypin);

  // Stop all the motors
  SetXMotor(XMotorStopped);
  SetYMotor(YMotorStopped);
  digitalWrite(Relaypin, false); // Stop auger
  Comp_State = Paused;

}

void unpause() {
  Serial.println("Unpause system");
  SetXMotor(XMotor_State_Saved);
  SetYMotor(YMotor_State_Saved);
  digitalWrite(Relaypin, Auger_State_Saved);
  Comp_State_Saved = Comp_State;

}

void mix() {
  Serial.println("Mixing");
  switch (XMotor_State) {
    case XMotorStopped:
      Serial.println("XMotorStopped");
      Y1 = digitalRead(Y1pin);
      Y2 = digitalRead(Y2pin);
      if (YMotor_State == YMotorMovingFront) {
        if (MovingRight) {
          X2 = digitalRead(X2pin);
          if (X2 == SWITCH_CLOSED && Y2 == SWITCH_CLOSED) {
            SetYMotor(YMotorMovingBack); //Auger is at X2/Y2 and will start mixing left
            MovingRight = false;
            startX = XEncoder.read();
            Serial.println("Reached front-right corner.  Now moving back.");
          }
          else if (Y2 == SWITCH_CLOSED) {
            // X is moving right, Y-to-front reached front.
            SetYMotor(YMotorStopped);
            SetXMotor(XMotorMovingRight);
            startX = XEncoder.read();
            Serial.print("Reached front, now moving right.  startX = ");
            Serial.println(startX);
          }
        }
        else  // Moving left
          X1 = digitalRead(X1pin);
          //Auger is at X1/Y2 (Front Left corner)
          if (X1 == SWITCH_CLOSED && Y2 == SWITCH_CLOSED) {
            SetYMotor(YMotorStopped);
            SetXMotor(XMotorMovingRight);
            MovingRight = true;
            startX = XEncoder.read();
            Serial.println("Reached front-left corner.  Now moving right.");
          }
          else if (Y2 == SWITCH_CLOSED) {
            SetYMotor(YMotorStopped);
            SetXMotor(XMotorMovingLeft);
            startX = XEncoder.read();
            Serial.print("Reached front, now moving left.  startX = ");
            Serial.println(startX);
          }
      }
      else if (YMotor_State == YMotorMovingBack) {
        if (MovingRight) {
          X2 = digitalRead(X2pin);
          //Auger is at X2/Y1 (Back Right Corner)
          if (X2 == SWITCH_CLOSED && Y1 == SWITCH_CLOSED) {
            SetYMotor(YMotorMovingFront);
            MovingRight = false;
            startX = XEncoder.read();
            Serial.println("Reached back-right corner.  Now moving forward.");
          }
          else if (Y1 == SWITCH_CLOSED) {
            SetYMotor(YMotorStopped);
            SetXMotor(XMotorMovingRight);
            startX = XEncoder.read();
            Serial.print("Reached back, now moving right.  startX = ");
            Serial.println(startX);
          }
        }
        else  //Moving Right is False
          X1 = digitalRead(X1pin);
          //Auger is at X1/Y1 (Back Left corner)
          if (X1 == SWITCH_CLOSED && Y1 == SWITCH_CLOSED) {
            SetYMotor(YMotorStopped);
            SetXMotor(XMotorMovingRight);
            MovingRight = true;
            startX = XEncoder.read();
            Serial.println("Reached back-left corner.  Now moving right.");
          }
          else if (Y1 == SWITCH_CLOSED) {
            SetYMotor(YMotorStopped);
            SetXMotor(XMotorMovingLeft);
            startX = XEncoder.read();
            Serial.print("Reached front, now moving left.  startX = ");
            Serial.println(startX);
          }
      }
      break;

      case XMotorMovingRight:
        Serial.write("XMotorMovingRight");
        X2 = digitalRead(X2pin);
        Y1 = digitalRead(Y1pin);
        Y2 = digitalRead(Y2pin);

        if ((X2 == SWITCH_CLOSED) && (Y1 == SWITCH_CLOSED)) {
          SetXMotor(XMotorStopped);
          SetYMotor(YMotorMovingFront);
          MovingRight = false;
        }
        else if ((X2 == SWITCH_CLOSED) && (Y2 == SWITCH_CLOSED)) {
          SetXMotor(XMotorStopped);
          SetYMotor(YMotorMovingBack);
          MovingRight = false;
        }
        else if (XEncoder.read() >= startX + DeltaXPerSweep) {
          Serial.println("Reached limit moving right.");
          showPosition();
          SetXMotor(XMotorStopped);
          if (!atFront) {
            SetYMotor(YMotorMovingFront);
            Serial.println("At back, so now moving forward.");
          }
          else if (atFront) {
            SetYMotor(YMotorMovingBack);
            Serial.println("At front, so now moving back.");
          }
          else {
            // TODO: What if we moved away from the sensor,
            // and neither Y1 or Y2 is closed?
            Serial.println("EXCEPTION: Can't change direction because not at front of back.");
          }
        }
        break;

      case XMotorMovingLeft:
        Serial.println("XMotorMovingLeft");
        X1 = digitalRead(X1pin);
        Y1 = digitalRead(Y1pin);
        Y2 = digitalRead(Y2pin);

        //Composter is at back left corner
        if ((X1 == SWITCH_CLOSED) && (Y1 == SWITCH_CLOSED)) {
          SetXMotor(XMotorStopped);
          SetYMotor(YMotorMovingFront);
          MovingRight = true;
        }
        //Composter is at front right corner
        else if ((X1 == SWITCH_CLOSED) && (Y2 == SWITCH_CLOSED)) {
          SetXMotor(XMotorStopped);
          SetYMotor(YMotorMovingBack);
          MovingRight = false;
        }
        else if (XEncoder.read() <= startX - DeltaXPerSweep) {
          Serial.println("Reached limit moving left.");
          showPosition();
          SetXMotor(XMotorStopped);
          if (!atFront) {
            SetYMotor(YMotorMovingFront);
            Serial.println("At back, so now moving forward.");
          }
          else if (atFront) {
            SetYMotor(YMotorMovingBack);
            Serial.println("At front, so now moving back.");
          }
          else {
            // TODO: What if we moved away from the sensor,
            // and neither Y1 or Y2 is closed?
            Serial.println("EXCEPTION: Can't change direction because not at front of back.");
          }
        }

        break;

      case XMotorError:
        Serial.println("XMotorError");
        Comp_State = Error;//chang comp state and also stop motors
        SetXMotor(XMotorStopped);
        SetYMotor(YMotorStopped);
        break;
        //To Start the Composter after this state you must restart the Compoter
        //push Stop then Start.

    } // end switch(YMotor_State)

    switch(YMotor_State) {
      case YMotorStopped:

        Serial.println("YMotorStopped");
        if (MovingRight) {
          X2 = digitalRead(X2pin);
          Y2 = digitalRead(Y2pin);
          if (X1 == SWITCH_CLOSED && !atFront) {
            SetXMotor(XMotorMovingRight);
          }
          else if (X2 == SWITCH_CLOSED && atFront) {
            SetXMotor(XMotorStopped);
            SetYMotor(YMotorMovingBack);
            MovingRight = false;
          }
          else if (X2 == SWITCH_CLOSED && !atFront) {
            SetYMotor(YMotorMovingFront);
            SetXMotor(XMotorStopped);
          }
          else {
            SetXMotor(XMotorMovingRight);
          }
        }
        else { //Meaning MovingRight == false
          X1 = digitalRead(X1pin);
          Y1 = digitalRead(Y1pin);

          if (X2 == SWITCH_CLOSED && !atFront) {
            SetXMotor(XMotorMovingLeft);
          }
          else if (X1 == SWITCH_CLOSED && atFront) {
            SetXMotor(XMotorStopped);
            SetYMotor(YMotorMovingBack);
            MovingRight =true;
          }
          else if (X1 == SWITCH_CLOSED && !atFront) {
            SetXMotor(XMotorStopped);
            SetYMotor(YMotorMovingFront);

          }
          else {
            SetXMotor(XMotorMovingLeft);
          }
        }
        break;

      case YMotorMovingBack:
        Y1 = digitalRead(Y1pin);
        if (MovingRight) {
          if (Y1 == SWITCH_CLOSED) {
            SetYMotor(YMotorStopped);
            SetXMotor(XMotorMovingRight);
          }
        }
        else if (Y1 == SWITCH_CLOSED) {
            SetYMotor(YMotorStopped);
            SetXMotor(XMotorMovingLeft);
        }
        atFront = false;
        Serial.println("YMotorMovingBack");
        Serial.print("atFront state =");
        Serial.println(atFront);
        Serial.print("Moving Right State =");
        Serial.println(MovingRight);
        break;

      case YMotorMovingFront:
        Y2 = digitalRead(Y2pin);
        if (MovingRight) {
          if (Y2 == SWITCH_CLOSED) {
            SetYMotor(YMotorStopped);
            SetXMotor(XMotorMovingRight);

          }
        }
        else if (Y2 == SWITCH_CLOSED) {
          SetYMotor(YMotorStopped);
          SetXMotor(XMotorMovingLeft);
        }
        atFront = true;
        Serial.println("YMotorMovingFront");
        Serial.print("atFront state =");
        Serial.println(atFront);
        Serial.print("Moving Right State =");
        Serial.println(MovingRight);
        break;
  }  // end switch(YMotor_State)
}

void showPosition() {
  Serial.print("XEncoder position = ");
  Serial.print(XEncoder.read());
  Serial.print(", startX = ");
  Serial.print(startX);
  Serial.print(", DeltaXPerSweep = ");
  Serial.println(DeltaXPerSweep);
}
