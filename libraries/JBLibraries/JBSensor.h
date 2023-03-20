/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Sensorer
 * Version: 1.0
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 07-03-2023
 * GNU General Public License version 3
 * This file is part of "Sensorer".
 * 
 * "Sensorer" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Sensorer" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Sensorer".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */


#ifndef JBSensor_h
#define JBSensor_h

#include <Arduino.h>
#include <JBKernel.h>
#include <JBInputDriver.h>
#include <JBDigitalFunctions.h>

// Ansvar: Er grænseflade for sensorer.
// driver: Pointer til input driver
// portNo: Bruger denne port
// state: Sensorens tilstand
// begin(...): Initialiserer sensoren
// doClockCycle(...): Input driver bliver aflæst i hver klokkescyklus
// status(...): Leverer sensorens tilstand
// reset(...): Resetter digitale funktioner
class t_Sensor {
protected:
  t_InputDriver *driver;
  unsigned int portNo;
  byte state;
public:
  t_Sensor(void): driver(nullptr), state(OFF) {}
  void begin(t_InputDriver *driver, unsigned int portNo);
  virtual void doClockCycle()=0;
  byte status(void) {return state;}
  virtual void reset(void) {}
};

//----------

// Ansvar: Den simple sensor simulerer de oftest brugte funktioner til overvågning.
// digitalFunction: Pointer til en digital funktion.
// setDigitalFunktion(...): Kobler digitalfunktion til betjening.
// doClockCycle(...): Input driver bliver aflæst i hver klokkescyklus.
// reset(...): Resetter digitale funktioner. Status bliver opdateret i næste klokkecyklus.
class t_SimpleSensor: public t_Sensor {
private:
  t_DigitalFunction *digitalFunction;
public:
  t_SimpleSensor(void) {}
  void setDigitalFunktion(t_DigitalFunction *digitalFunction) {this->digitalFunction = digitalFunction;}
  void doClockCycle();
  void reset(void);
};

/*
 * CPP kode herunder
 */

// Grænseflade til sensorer

void t_Sensor::begin(t_InputDriver *driver, unsigned int portNo) {
  this->driver = driver;
  this->portNo = portNo;
}

//----------

// Simpel sensor

void t_SimpleSensor::doClockCycle() {
  if (driver == nullptr) return;
  bool value = driver->read(portNo);
  if (digitalFunction != nullptr) value = digitalFunction->dataOut(value); 
  state = (value == HIGH)? ON: OFF;
}

void t_SimpleSensor::reset(void) {
  if (digitalFunction == nullptr) return;
  digitalFunction->reset();
}

#endif
