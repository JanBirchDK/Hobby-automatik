/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Styrenheder
 * Version: 1.3
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 20-04-2023
 * GNU General Public License version 3
 * This file is part of "Styrenheder".
 * 
 * "Styrenheder" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Styrenheder" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Styrenheder".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#ifndef JBCtrlUnits_h
#define JBCtrlUnits_h

#include <Arduino.h>
#include <JBKernel.h>
#include <JBOutputDriver.h>

// Ansvar: Er grænseflade for styrenheder.
// driver: Pointer til output driver
// portNo: Bruger denne port
// state: Styrenhedens tilstand
// setPort(...): Forbinder til output driver
// doClockCycle(...): Styrenheder med tidsstyring opdateres i hver klokkescyklus
// to(...): Modtager styreenheds næste tilstand
class t_CtrlUnit {
protected:
  t_OutputDriver *driver;
  unsigned int portNo;
  byte state;
  void setPort(t_OutputDriver *driver, unsigned int portNo);
public:
  t_CtrlUnit(void): driver(nullptr) {}
  virtual void doClockCycle(void) {}
  virtual void to(byte state)=0;
};

//----------

// Ansvar: Styrenhed med tænd og sluk.
// begin(...): Initialiserer styrenheden
// to(...): Modtager styreenheds næste tilstand
class t_OnOffOut: public t_CtrlUnit {
public:
  t_OnOffOut(void) {}
  void begin(t_OutputDriver *driver, unsigned int portNo, byte state=OFF);
  void to(byte state);
};

//----------

// Ansvar: Styrenhed med blink.
// begin(...): Initialiserer styrenheden
// doClockCycle(...): Styrenheder med tidsstyring opdateres i hver klokkescyklus
// to(...): Modtager styreenheds næste tilstand
class t_WithBlinkOut: public t_CtrlUnit {
public:
  t_WithBlinkOut(void) {}
  void begin(t_OutputDriver *driver, unsigned int portNo, byte state=OFF);
  void doClockCycle(void);
  void to(byte state) {this->state = state;}
};

/*
 * CPP kode herunder
 */

// Grænseflade til styrenheder

void t_CtrlUnit::setPort(t_OutputDriver *driver, unsigned int portNo) {
  this->driver = driver;
  this->portNo = portNo;
}

//----------

// Styrenhed med tænd og sluk

void t_OnOffOut::begin(t_OutputDriver *driver, unsigned int portNo, byte state=OFF) {
  setPort(driver, portNo);
  to(state);
}

void t_OnOffOut::to(byte state) {
  if (driver == nullptr) return;
  this->state = state;
  bool driverState = (state == ON)? HIGH: LOW;
  driver->write(portNo, driverState);
}

//----------

// Styrenhed med blink

void t_WithBlinkOut::begin(t_OutputDriver *driver, unsigned int portNo, byte state=OFF) {
  setPort(driver, portNo);
  this->state = state;
}

void t_WithBlinkOut::doClockCycle(void) {
  if (driver == nullptr) return;
  bool driverState = (state == BLINK)? blinkerNotification(MASTERBLINKERNO): LOW;
  driver->write(portNo, driverState);
}

#endif
