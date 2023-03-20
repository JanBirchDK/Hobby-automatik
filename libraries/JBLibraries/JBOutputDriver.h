/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Output drivere
 * Version: 1.0
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 14-02-2023
 * GNU General Public License version 3
 * This file is part of Output drivere.
 * 
 * Output drivere is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Output drivere is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Output drivere.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Noter:
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#ifndef JBOutputDriver_h
#define JBOutputDriver_h

#include <Arduino.h>
#include <JBKernel.h>

// Ansvar: Er grænseflade til output porte. Al software der skal bruge output-porte skal koble til grænsefladen.
// write(...): Udlæser værdi. Sørger for kun at opdatere arduino port ved behov. Er klar til både digital og analog udlæsning.
class t_OutputDriver {
public:
  t_OutputDriver(void){}
  virtual void write(unsigned int portNo, bool value){}
  virtual void write(unsigned int portNo, int value){}
};

// Ansvar: Indeholder data til en digital parallel output port.
// pin: Arduino pin.
// value: Portens binære værdi. Værdien bruges til at tjekke om port skal opdateres.
// setPort(...): Opkobler Arduino og konfigurerer udgangen.
// write(...): Skriver værdi til port.
class t_DigitalParrOutPort {
private:
  byte pin;
  bool value;
public:
  t_DigitalParrOutPort(void): value(LOW) {}
  void setPort(byte pin, bool value=LOW);
  void write(bool value);
};

// Ansvar: Holder styr på samling af digitale parallelle output porte.
// ports: Vektor med output port konfiguration
// isSetup: Holder styr på om port er initialiseret
// setPort(...): Mapper portNo, opkobler Arduino og konfigurerer udgangen.
// write(...): Skriver værdi til en konkret port.
class t_DigitalParrOutDrv: public t_OutputDriver {
private:
  t_DigitalParrOutPort ports[MaxNoOutParrPorts];
  bool isSetup[MaxNoOutParrPorts];
public:
  t_DigitalParrOutDrv(void){for (int cnt=0; cnt < MaxNoOutParrPorts; cnt++) isSetup[cnt] = false;}
  void setPort(unsigned int portNo, byte pin, bool value=LOW);
  void write(unsigned int portNo, bool value);
};

/*
 * CPP kode herunder
 */

//----------

// Outputport

void t_DigitalParrOutPort::setPort(byte pin, bool value=LOW) {
  this->pin = pin;
  this->value = value;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, value);
}

void t_DigitalParrOutPort::write(bool value) {
  if (this->value == value) return;
  this->value = value;
  digitalWrite(pin, value);
}

//----------

// Samlingen af outputporte

void t_DigitalParrOutDrv::setPort(unsigned int portNo, byte pin, bool value=LOW) {
  if (isValidIndex(portNo, MaxNoOutParrPorts)==true) {
    ports[portNo].setPort(pin, value=LOW);
    isSetup[portNo] = true;
  }
}

void t_DigitalParrOutDrv::write(unsigned int portNo, bool value) {
  if (hasConfig(isSetup, portNo, MaxNoOutParrPorts)==true) ports[portNo].write(value);
}
#endif
