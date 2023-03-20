/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Tilstandsmaskine
 * Version: 1.0
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 08-03-2023
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

#ifndef JBStateMachine_h
#define JBStateMachine_h

#include <Arduino.h>
#include <JBKernel.h>

// Ansvar: Er grænseflade til tilstandsmaskine.
// transitTimer: Bruges af tilstand, med tidsstyret overgang til næste tilstand.
// onEntry(...): Udfører funktioner for ankomst til en "state"..
// doCondition(...): Svarer på om betingelser for overgang til næste tilstand er opfyldt.
// onExit(...): Udfører funktioner for afgang fra en "state".
class t_StateMachine {
protected:
  static t_SimpleTimer transitTimer;
public:
  t_StateMachine(void) {}
  virtual void onEntry(void) {}
  virtual bool changeState(byte *nextStateNo) = 0;
  virtual void onExit(void) {}  
};

t_SimpleTimer t_StateMachine::transitTimer;

#endif
