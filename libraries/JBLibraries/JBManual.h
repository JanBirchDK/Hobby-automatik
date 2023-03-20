/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Manuelle betjeninger
 * Version: 1.0
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 06-03-2023
 * GNU General Public License version 3
 * This file is part of "Manuelle betjeninger".
 * 
 * "Manuelle betjeninger" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Manuelle betjeninger" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Manuelle betjeninger".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */


#ifndef JBManual_h
#define JBManual_h

#include <Arduino.h>
#include <JBKernel.h>
#include <JBInputDriver.h>
#include <JBDigitalFunctions.h>

// Ansvar: Er grænseflade for manuelle betjeninger.
// driver: Pointer til input driver
// portNo: Bruger denne port
// state: Betjeningens tilstand
// begin(...): Initialiserer betjeningen
// doClockCycle(...): Input driver bliver aflæst i hver klokkescyklus
// status(...): Leverer betjeningens tilstand
// reset(...): Resetter digitale funktioner
class t_Manual {
protected:
  t_InputDriver *driver;
  unsigned int portNo;
  byte state;
public:
  t_Manual(void): driver(nullptr), state(OFF) {}
  void begin(t_InputDriver *driver, unsigned int portNo);
  virtual void doClockCycle()=0;
  byte status(void) {return state;}
  virtual void reset(void) {}
};

//----------

// Ansvar: I en række applikationer er der behov for en knap, der blot følger om knappen er aktiv eller passiv.
// doClockCycle(...): Input driver bliver aflæst i hver klokkescyklus.
class t_SimpleButton: public t_Manual {
public:
  t_SimpleButton(void) {}
  void doClockCycle();
};

//----------

// Ansvar: Knap simulerer de oftest brugte funktioner ved brug af trykknap og omskifter.
// digitalFunction: Pointer til en digital funktion.
// setDigitalFunktion(...): Kobler digitalfunktion til betjening.
// doClockCycle(...): Input driver bliver aflæst i hver klokkescyklus.
// reset(...): Resetter digitale funktioner. Status bliver opdateret i næste klokkecyklus.
class t_Button: public t_Manual {
private:
  t_DigitalFunction *digitalFunction;
public:
  t_Button(void) {}
  void setDigitalFunktion(t_DigitalFunction *digitalFunction) {this->digitalFunction = digitalFunction;}
  void doClockCycle();
  void reset(void);
};

/*
 * CPP kode herunder
 */

// Grænseflade til manuelle betjeninger

void t_Manual::begin(t_InputDriver *driver, unsigned int portNo) {
  this->driver = driver;
  this->portNo = portNo;
}

//----------

// Simpel knap

void t_SimpleButton::doClockCycle() {
  if (driver == nullptr) return;
  state = (driver->read(portNo) == HIGH)? ON: OFF;
}

//----------

// Knap

void t_Button::doClockCycle() {
  if (driver == nullptr) return;
  bool value = driver->read(portNo);
  if (digitalFunction != nullptr) value = digitalFunction->dataOut(value); 
  state = (value == HIGH)? ON: OFF;
}

void t_Button::reset(void) {
  if (digitalFunction == nullptr) return;
  digitalFunction->reset();
}

#endif
