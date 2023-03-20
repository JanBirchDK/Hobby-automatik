/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Driver til servomotor
 * Version: 1.0
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 25-02-2023
 * GNU General Public License version 3
 * This file is part of "Driver til servomotor".
 * 
 * "Driver til servomotor" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Driver til servomotor" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Driver til servomotor".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#ifndef JBServoDrv_h
#define JBServoDrv_h

#include <Arduino.h>
#include <Servo.h>
#include <JBKernel.h>

// Specifikationer for  servomotor 
struct t_ServoMotorSpecs {
  int PulseWidthMin;  // Motorens mindste pulsbredde som giver mindste vinkel
  int PulseWidthMax;  // Motorens største pulsbredde som giver største vinkel
  int AngleMin;       // Motorens mindste vinkel
  int AngleMax;       // Motorens største vinkel
};

// Specifikke servomotorer
const struct t_ServoMotorSpecs ESU51804Specs = {600, 2300, 0, 180};
const struct t_ServoMotorSpecs SG90Specs = {544, 2400, 0, 180};

// Specifikation af standard periode for sample
unsigned int servoPeriod = REFRESH_INTERVAL/1000;  // Se kildekode for servo.h

// Ansvar: Grænseflade for tilpasning af ethvert bevægelsesmønster.
// sampleTime: Samplingstid i beregning
// calCoefficient(...): Initialiserer beregninger
// calcNextPW(...): Udfører beregning og returner næste pulsbredde
class t_ServoMoveCalculator {
protected:
  unsigned int sampleTime;
public:
  t_ServoMoveCalculator(void): sampleTime(1) {}
  virtual int calCoefficient(int fromPW, int toPW, unsigned int deltaTime, unsigned int sampleTime)=0;
  virtual int calcNextPW(void)=0;  
};

//----------

// Ansvar: En simpel algoritme for bevægelse er indbygget og bruges default.
// Float beregninger er dyre i program memory, derfor bruges integer med decimal præcision i stedet for.
// PWSpeed: Pulsbredde ændring per sample
// currentPW: Nuværende pulsbredde
// precision: Decimal præcision. Integer * og divider f.eks. 10 giver 1 decimal nøjagtighed
// sampleTimer: Holder styr på tiden for næste pulsbredde
// calCoefficient(...): Initialiserer beregninger
// calcNextPW(...): Udfører beregning og returner næste pulsbredde
class t_ServoLinearMove: public t_ServoMoveCalculator {
private:
  unsigned int sampleTime;
  long PWSpeed;
  long currentPW;
  const int precision = 10;
  t_SimpleTimer sampleTimer;
public:
  t_ServoLinearMove(void) {}
  int calCoefficient(int fromPW, int toPW, unsigned int deltaTime, unsigned int sampleTime);
  int calcNextPW(void);
};

//----------

// Ansvar: Konfigurer Arduino med en PWM port til servomotor. Modtager data og styrer PWM port.
// servoPort: Kobling til pwm-port
// motorSpecs: Servomotor specifikation
// PWCalculator: Pointer til algoritme for bevægelse
// isSetup: Holder styr på om driver er initialiseret
// fromPW: Bevægelse fra pulsbredde
// toPW: Bevægelse til pulsbredde
// currentPW: Nuværende pulsbredde opbevares her og leveres både til beregning og til port.
// Seqs: Sekvenser driver løber igennem
// seq: Nuværende sekvens
// begin(...): Initialiserer driver til servomotor.
// write(...): Får opdateret driver med en specifik pulsbredde
// write(...): Modtager et vinkelinterval og gør klar til bevægelse af motorens arm.
// doClockCycle(...): Ved behov får beregnet en ny pulsbredde og får opdateret pwm-port.
// sendOut(...): Opdaterer Arduino port.
class t_ServoMotor {
private:
  Servo servoPort;
  t_ServoMotorSpecs *motorSpecs;
  t_ServoMoveCalculator *PWCalculator;
  bool isSetup;
  int fromPW;
  int toPW;
  int currentPW;
  enum Seqs {STABLE, GOUP, GODOWN};
  Seqs seq;
  void sendOut(int nextPW);
public:
  t_ServoMotor(void): isSetup(false) {}
  void begin(byte pin, t_ServoMotorSpecs *motorSpecs, t_ServoMoveCalculator *PWCalculator);
  void write(int nextPW);
  void write(int fromAngle, int toAngle, unsigned int deltaTime, byte timeUnit, unsigned int sampleTime=servoPeriod);
  void doClockCycle(void);
};

/*
 * CPP kode herunder
 */

// ServoLinearMove

int t_ServoLinearMove::calCoefficient(int fromPW, int toPW, unsigned int deltaTime, unsigned int sampleTime) {
  this->sampleTime = sampleTime;
  sampleTimer.setDuration(sampleTime);
  long noSamples = long(deltaTime/sampleTime);
  PWSpeed = (toPW-fromPW)*precision/noSamples;
  currentPW = fromPW*precision;
  return fromPW;
}

int t_ServoLinearMove::calcNextPW(void) {
  if (sampleTimer.triggered() == true) currentPW = currentPW + PWSpeed;
  return int(currentPW/precision);
}

//----------

// ServoMotor

void t_ServoMotor::sendOut(int nextPW){
  if (nextPW == currentPW) return;
  currentPW = constrain(nextPW, motorSpecs->PulseWidthMin, motorSpecs->PulseWidthMax);
  servoPort.writeMicroseconds(currentPW);
}

void t_ServoMotor::begin(byte pin, t_ServoMotorSpecs *motorSpecs, t_ServoMoveCalculator *PWCalculator) {
  servoPort.attach(pin);
  this->motorSpecs = motorSpecs;
  this->PWCalculator = PWCalculator;
  fromPW = currentPW = motorSpecs->PulseWidthMin;
  toPW = motorSpecs->PulseWidthMax;
  seq = STABLE;
  isSetup = true;
}

void t_ServoMotor::write(int nextPW) {
  if (isSetup) sendOut(nextPW);
}

void t_ServoMotor::write(int fromAngle, int toAngle, unsigned int deltaTime, byte timeUnit, unsigned int sampleTime=servoPeriod) {
  if (!isSetup) return;
  fromPW = map(fromAngle, motorSpecs->AngleMin, motorSpecs->AngleMax, motorSpecs->PulseWidthMin, motorSpecs->PulseWidthMax);
  toPW = map(toAngle, motorSpecs->AngleMin, motorSpecs->AngleMax, motorSpecs->PulseWidthMin, motorSpecs->PulseWidthMax);
  deltaTime = (timeUnit == SECONDS)? SecondsToMilliSecs(deltaTime): deltaTime;
  int nextPW = PWCalculator->calCoefficient(fromPW, toPW, deltaTime, sampleTime);
  seq = (toPW > fromPW)? GOUP: GODOWN;
  sendOut(nextPW);
}

void t_ServoMotor::doClockCycle(void) {
  if (!isSetup || (seq == STABLE)) return;
  int nextPW; // Der er decimal beregninger, som kan medføre at slutpunkt ikke rammer slutpunkt med heltal. Heltal sendes når bevægelse skal slutte
  nextPW = PWCalculator->calcNextPW();
  if (((seq == GOUP) && (nextPW >= toPW)) || ((seq == GODOWN) && (nextPW <= toPW))) {
    nextPW = toPW;
    seq = STABLE;
  }
  sendOut(nextPW);
}

#endif
