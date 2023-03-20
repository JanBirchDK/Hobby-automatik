/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Input driver
 * Version: 1.0
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 19-02-2023
 * GNU General Public License version 3
 * This file is part of Input driver.
 * 
 * "Input driver" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Input driver" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Input drivere".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af biblioteket, formål og anvendelse.
 */

#ifndef JBInputDriver_h
#define JBInputDriver_h

#include <Arduino.h>
#include <JBKernel.h>

// Kontakttyper
enum {NOPEN, NCLOSED};
// Brug af pullup
enum {INTERN_PULLUP, EXTERN_PULLUP};
// Brug af filter til kontaktprel
enum {BOUNCE_FILTER, NO_BOUNCE_FILTER};

// Ansvar: Er grænseflade til input porte. Al software der skal bruge input-porte skal koble til grænsefladen.
// doClockCycle(...): Læser input i hvert klokkecyklus.
// read(...): Indlæser værdi. Pointer til value giver grænseflade til analoge porte.
class t_InputDriver {
public:
  t_InputDriver(void){} 
  virtual void doClockCycle()=0;
  virtual bool read(unsigned int portNo, int *value=nullptr)=0;
};

//----------

// Ansvar: Varetager 1 digital parallel input port
// pin: Input pin.
// value: Gemmer den indlæste værdi til senere brug.
// defaultValue: Porten værdi, når den er passiv. Bruges til filter for kontaktprel.
// Seqs: Porten sekvens når den føres igennem filter for kontaktprel.
// seq: Porten sekvens når den føres igennem filter for kontaktprel.
// bounceWait: Timer til filter for kontaktprel.
// bounceTimeClose: Ventetid når en kontakt lukkes og slutter strøm.
// bounceTimeOpen: Ventetid når en kontakt åbnes og bryder strømmen.
// setPort(...): Opkobler Arduino og konfigurerer indgangen. En variant kan også sætte tider for kontaktprel.
// doClockCycle(...): Læser input. Udfører filter for kontaktprel hvis det skal bruges.
// read(...): Leverer driverens nuværende værdi.
class t_DigitalParrInPort {
private:
  byte pin;
  bool value;
  bool defaultValue;
  enum Seqs {STABLE, BOUNCE, NO_BOUNCE};
  Seqs seq;
  t_SimpleTimer bounceWait;
  unsigned int bounceTimeOpen;  
  unsigned int bounceTimeClose;
public:
  t_DigitalParrInPort(void): bounceTimeOpen(100), bounceTimeClose(30) {}
  void setPort(byte pin, byte ContacType, byte PullupType, byte BounceType);
  void setPort(byte pin, byte ContacType, byte PullupType, byte BounceType, unsigned int bounceTimeOpen, unsigned int bounceTimeClose);
  void doClockCycle();
  bool read(int *value=nullptr) const {return this->value;}
};

//----------

// Ansvar: Holder styr på en samling af parallelle digitale input porte.
// ports: Vektor med parallelle digitale input porte.
// isSetup: Vektor der holder styr på om port er konfigureret.
// setPort(...): Mapper portNo, opkobler Arduino og konfigurerer indgangen. En variant kan også sætte tider for kontaktprel.
// doClockCycle(...): Læser input. Udfører filter for kontaktprel hvis det skal bruges.
// read(...): Leverer driverens nuværende værdi.
class t_DigitalParrInDrv: public t_InputDriver {
private:
  t_DigitalParrInPort ports[MaxNoInParrPorts];
  bool isSetup[MaxNoInParrPorts];
public:
  t_DigitalParrInDrv(void){for (int cnt=0; cnt < MaxNoInParrPorts; cnt++) isSetup[cnt] = false;}
  void setPort(unsigned int portNo, byte pin, byte ContacType, byte PullupType, byte BounceType);
  void setPort(unsigned int portNo, byte pin, byte ContacType, byte PullupType, byte BounceType, unsigned int bounceTimeOpen, unsigned int bounceTimeClose);
  void doClockCycle();
  bool read(unsigned int portNo, int *value=nullptr);
};

/*
 * CPP kode herunder
 */

// 1 digital input port

void t_DigitalParrInPort::setPort(byte pin, byte ContacType, byte PullupType, byte BounceType) {
  this->pin = pin;
  seq = (BounceType == BOUNCE_FILTER)? STABLE: NO_BOUNCE;
  defaultValue = (ContacType == NCLOSED);
  if ((ContacType == NCLOSED) && (PullupType == INTERN_PULLUP)) pinMode(pin, INPUT_PULLUP);
  else pinMode(pin, INPUT);
  this->value = digitalRead(pin);
}

void t_DigitalParrInPort::setPort(byte pin, byte ContacType, byte PullupType, byte BounceType, unsigned int bounceTimeOpen, unsigned int bounceTimeClose) {
  setPort(pin, ContacType, PullupType, BounceType);
  this->bounceTimeOpen=bounceTimeOpen;  
  this->bounceTimeClose=bounceTimeClose;
}

void t_DigitalParrInPort::doClockCycle() {
  bool nextValue;
  unsigned int bounceTime;
  nextValue = digitalRead(pin);
  switch (seq) {
    case STABLE:
      if (value != nextValue) {
        bounceTime = (defaultValue == value)? bounceTimeClose: bounceTimeOpen;
        bounceWait.setDuration(bounceTime);
        seq = BOUNCE;
      }  
    break;
    case BOUNCE:
      if (bounceWait.triggered() == true) {
        value = nextValue;
        seq = STABLE;
      }
    break;
    case NO_BOUNCE:
      value = nextValue;         
    break;
  }
}

//----------

// Samling af parallelle digitale input porte
 
void t_DigitalParrInDrv::setPort(unsigned int portNo, byte pin, byte ContacType, byte PullupType, byte BounceType) {
  if (isValidIndex(portNo, MaxNoInParrPorts)==true) {
    ports[portNo].setPort(pin, ContacType, PullupType, BounceType);
    isSetup[portNo] = true;  
  } 
}
  
void t_DigitalParrInDrv::setPort(unsigned int portNo, byte pin, byte ContacType, byte PullupType, byte BounceType, unsigned int bounceTimeOpen, unsigned int bounceTimeClose) {
  if (isValidIndex(portNo, MaxNoInParrPorts)==true) {
    ports[portNo].setPort(pin, ContacType, PullupType, BounceType, bounceTimeOpen, bounceTimeClose);  
    isSetup[portNo] = true;  
  }
}

void t_DigitalParrInDrv::doClockCycle() {
  unsigned int portNo;
  for (portNo=0; portNo < MaxNoInParrPorts; portNo++) {
    if (isSetup[portNo] == true) ports[portNo].doClockCycle();
  }
}
  
bool t_DigitalParrInDrv::read(unsigned int portNo, int *value=nullptr) {
  bool result = LOW;
  if (hasConfig(isSetup, portNo, MaxNoInParrPorts)==true) result = ports[portNo].read();
  return result;
}

#endif
