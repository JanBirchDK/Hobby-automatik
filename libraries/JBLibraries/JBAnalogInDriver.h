/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Analoge input driver
 * Version: 1.0
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 21-02-2023
 * GNU General Public License version 3
 * This file is part of Input drivere.
 * 
 * "Analoge input driver" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Analoge input driver" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Analoge input driver".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af biblioteket, formål og anvendelse.
 */

#ifndef JBAnalogInDriver_h
#define JBAnalogInDriver_h

#include <Arduino.h>
#include <JBKernel.h>

// Ansvar Varetager 1 analog parallel input port
// pin: Input pin.
// value: Gemmer den indlæste værdi til senere brug.
// setPort(...): Opkobler Arduino og konfigurerer indgangen.
// doClockCycle(...): Læser input.
// read(...): Leverer driverens nuværende værdi.
class t_AnalogParrInPort {
private:
  byte pin;
  int value;
public:
  t_AnalogParrInPort(void) {}
  void setPort(byte pin);
  void doClockCycle(void) {value = analogRead(pin);}
  bool read(int *value);
};

//----------

// Ansvar: Holder styr på en samling af analoge input porte.
// ports: Vektor med analoge input porte.
// isSetup: Vektor der holder styr på om port er konfigureret.
// setPort(...): Mapper portNo, opkobler Arduino og konfigurerer indgangen.
// doClockCycle(...): Læser input.
// read(...): Leverer driverens nuværende værdi.
class t_AnalogParrInDrv: public t_InputDriver {
private:
  t_AnalogParrInPort ports[MaxNoInAnlPorts];
  bool isSetup[MaxNoInAnlPorts];
public:
  t_AnalogParrInDrv(void) {for (int cnt=0; cnt < MaxNoInAnlPorts; cnt++) isSetup[cnt] = false;}
  void setPort(unsigned int portNo, byte pin);
  void doClockCycle();
  bool read(unsigned int portNo, int *value);
};

/*
 * CPP kode herunder
 */

// 1 analog input port

void t_AnalogParrInPort::setPort(byte pin) {
  this->pin = pin;
  value = analogRead(pin);
}

bool t_AnalogParrInPort::read(int *value) {
  bool validRead = false;
  if (value != nullptr) {
    *value=this->value;
    validRead = true;
  }
  return validRead;
}

// Samling af analoge input porte

void t_AnalogParrInDrv::setPort(unsigned int portNo, byte pin) {
  if (isValidIndex(portNo, MaxNoInAnlPorts)==true) {
    ports[portNo].setPort(pin);
    isSetup[portNo] = true;   
  }
}

void t_AnalogParrInDrv::doClockCycle() {
  unsigned int portNo;
  for (portNo=0; portNo < MaxNoInAnlPorts; portNo++) {
    if (isSetup[portNo] == true) ports[portNo].doClockCycle();
  }
}

bool t_AnalogParrInDrv::read(unsigned int portNo, int *value) {
  bool validRead = false;
  if (hasConfig(isSetup, portNo, MaxNoInAnlPorts)==true) validRead = ports[portNo].read(value);
  return validRead;
}

#endif
