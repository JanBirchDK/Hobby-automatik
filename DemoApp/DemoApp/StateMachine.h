/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Tilstandsmaskine
 * Version: 1.0
 * Type: Program
 * Programmeret af: Jan Birch
 * Opdateret: 14-03-2023
 * GNU General Public License version 3
 * This file is part of "Tilstandsmaskine".
 * 
 * "Tilstandsmaskine" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Tilstandsmaskine" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Tilstandsmaskine".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#ifndef StateMachine_h
#define StateMachine_h

#include <Arduino.h>
#include <JBKernel.h>

// Ansvar: Enhver tilstand ser på kombinationen af betjeninger til rumlys, derfor en fælles funktion.
// +roomButtonCombiStatus(...): Returnerer kombineret betjening af rumlys 
namespace ButtonCombi {
  bool roomButtonStatus();
}

/*
Alle tilstande er erklæret således:
Ansvar: Hver klasse varetager den konrekte tilstand
onEntry(...): Udfører funktioner for ankomst til en "state".
doCondition(...): Udfører tjek for næste tilstand og returnerer nuværende eller næste tilstand.
onExit(...): Udfører funktioner for afgang fra en "state".
*/

class t_HvileState: public t_StateMachine {
public:
  t_HvileState(void) {}
  void onEntry(void);
  bool changeState(byte *nextStateNo);
};

class t_RumlysOnState: public t_StateMachine {
public:
  t_RumlysOnState(void) {}
  void onEntry(void);
  bool changeState(byte *nextStateNo);
};

class t_LedelysManState: public t_StateMachine {
public:
  t_LedelysManState(void) {}
  void onEntry(void);
  bool changeState(byte *nextStateNo);
};

class t_LedelysAutState: public t_StateMachine {
public:
  t_LedelysAutState(void) {}
  void onEntry(void);
  bool changeState(byte *nextStateNo);
};

class t_LedelysOffState: public t_StateMachine {
private:
  const unsigned int deferSeconds = 4;
public:
  t_LedelysOffState(void) {}
  void onEntry(void);
  bool changeState(byte *nextStateNo);
  void onExit(void);
};

/*
 * CPP kode herunder
 */

bool ButtonCombi::roomButtonStatus() {
  return ((demoApp.statusManual(RumKnapV) == ON) || (demoApp.statusManual(RumKnapH) == ON));
}

//----------

// Hvile tilstand

void t_HvileState::onEntry(void) {
  demoApp.to(LedelysLamper, OFF);
  demoApp.to(RumLamper, OFF);
}

bool t_HvileState::changeState(byte *nextStateNo) {
  if (ButtonCombi::roomButtonStatus() == true) {
    *nextStateNo = RumlysOn;
    return true;
  }
  if (demoApp.statusSensor(LysSensor) == ON) {
    *nextStateNo = LedelysAut;
    return true;
  }
  if (demoApp.statusManual(LedelysKnap) == ON) {
    *nextStateNo = LedelysManuel;
    return true;
  }
  return false;
}

//----------

// RumlysOn tilstand

void t_RumlysOnState::onEntry(void) {
  demoApp.to(LedelysLamper, OFF);
  demoApp.to(RumLamper, ON);
}

bool t_RumlysOnState::changeState(byte *nextStateNo) {
  if (ButtonCombi::roomButtonStatus() == true) {
    *nextStateNo = Hvile;
    return true;
  }
  return false;
}

//----------

// LedelysMan tilstand

void t_LedelysManState::onEntry(void) {
  demoApp.to(LedelysLamper, ON);
}

bool t_LedelysManState::changeState(byte *nextStateNo) {
  if (ButtonCombi::roomButtonStatus() == true) {
    *nextStateNo = RumlysOn;
    return true;
  }
  if (demoApp.statusManual(LedelysKnap) == ON) {
    *nextStateNo = Hvile;
    return true;
  }
  return false;
}

//----------

// LedelysAut tilstand

void t_LedelysAutState::onEntry(void) {
  demoApp.to(LedelysLamper, ON);
}

bool t_LedelysAutState::changeState(byte *nextStateNo) {
  if (ButtonCombi::roomButtonStatus() == true) {
    *nextStateNo = RumlysOn;
    return true;
  }
  if (demoApp.statusSensor(LysSensor) == OFF) {
    *nextStateNo = LedelysOff;
    return true;
  }
  return false;
}

//----------

// Ledelys off tilstand

void t_LedelysOffState::onEntry(void) {
  transitTimer.setDuration(deferSeconds, SECONDS);
}

bool t_LedelysOffState::changeState(byte *nextStateNo) {
  if (ButtonCombi::roomButtonStatus() == true) {
    *nextStateNo = RumlysOn;
    return true;
  }
  if (transitTimer.triggered() == ON) {
    *nextStateNo = Hvile;
    return true;
  }
  return false;
}

void t_LedelysOffState::onExit(void) {
  demoApp.to(LedelysLamper, OFF);
}
#endif
