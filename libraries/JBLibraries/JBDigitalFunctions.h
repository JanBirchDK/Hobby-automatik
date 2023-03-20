/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Digitale funktioner
 * Version: 1.0
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 21-02-2023
 * GNU General Public License version 3
 * This file is part of "Digitale funktioner".
 * 
 * "Digitale funktioner" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Digitale funktioner" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Digitale funktioner".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#ifndef JBDigitalFunctions_h
#define JBDigitalFunctions_h

#include <Arduino.h>
#include <JBKernel.h>

// Erklæring af flanketyper
enum EdgeTypes {EDGEDOWN, EDGEUP};

//----------

// Ansvar: Digitale funktioner foretager digital behandling med 1 bit
// value: Gemt værdi til sekventielle funktioner
// digitalFunction: Peger på næste digitale funktion
// setDigitalFct(...): Sætter pointer til næste digitale funktion
// reset(...): Resetter den digitale funktion
// dataOut(...): Beregner og leverer resultat af funktionen.
class t_DigitalFunction {
protected:
  bool value;
  t_DigitalFunction *digitalFunction;
public:
  t_DigitalFunction(void) : digitalFunction(nullptr) {}
  void setDigitalFunction(t_DigitalFunction *nextDigitalFct);
  virtual void reset(void);
  virtual bool dataOut(bool value)=0;
};

//----------

// Ansvar: Negator modtager 1 bit og negerer den. Resultatet leveres videre til næste digitale funktion eller leveres
// dataOut(...): Beregner og leverer resultat af negation.
class t_Negator : public t_DigitalFunction {
public:
  t_Negator(void){}
  bool dataOut(bool value);
};

//----------

// Ansvar: En flankedetektors opgave er at levere ON, når betingelser for et signals flanke er opfyldt.
// begin(...): Initialiserer flankedetektors.
// dataOut(...): Indstiller ny flankedetektors værdi og udfører beregninger i eventuelle efterfølgende digitale funktioner.
class t_EdgeDetector : public t_DigitalFunction {
private:
  EdgeTypes edgeType;
public:
  t_EdgeDetector(void): edgeType(EDGEDOWN) {value = OFF;}
  void begin(bool startValue, EdgeTypes edgeType);
  bool dataOut(bool value);
};

//----------

// Ansvar: Register modtager og gemmer 1 bit til senere brug. Brugeren af register sender reset, når register skal gøres klar til at modtage nye data.
// begin(...): Initialiserer register.
// dataOut(...): Indstiller ny register værdi og udfører beregninger i eventuelle efterfølgende digitale funktioner.
// reset(...): Resætter register.
class t_Register : public t_DigitalFunction {
public:
  t_Register(void) {value = OFF;}
  void begin(bool startValue);
  bool dataOut(bool value);
  void reset(void);
};

//----------

// Ansvar: Toggle skifter mellem ON og OFF, hver gang den får ON indlæst.
// begin(...): Initialiserer toggle.
// dataOut(...): Indstiller ny toggle værdi og udfører beregninger i eventuelle efterfølgende digitale funktioner.
class t_Toggle : public t_DigitalFunction {
public:
  t_Toggle(void) {value = OFF;}
  void begin(bool startValue);
  bool dataOut(bool value);
};

/*
 * CPP kode herunder
 */

// Grænseflade til digitale funktioner

void t_DigitalFunction::setDigitalFunction(t_DigitalFunction *nextDigitalFct) {
  digitalFunction = nextDigitalFct;
}

void t_DigitalFunction::reset(void) {
  if (digitalFunction != nullptr) digitalFunction->reset();  
}

//----------

// Negator

bool t_Negator::dataOut(bool value) {
  value = !value;
  if (digitalFunction != nullptr) value = digitalFunction->dataOut(value);
  return value;
}

//----------

// EdgeDetector

void t_EdgeDetector::begin(bool startValue, EdgeTypes edgeType) {
  value = startValue;
  this->edgeType = edgeType;
}

bool t_EdgeDetector::dataOut(bool value) {
  bool w_value = value;
  value = (edgeType ==  EDGEDOWN)?(this->value && !value): (!(this->value) && value);
  this->value = w_value;
  if (digitalFunction != nullptr) value = digitalFunction->dataOut(value);
  return value;
}

//----------

// Register

void t_Register::begin(bool startValue) {
  value = startValue;
}

bool t_Register::dataOut(bool value) {
  this->value = (this->value || value);
  value = this->value;
  if (digitalFunction != nullptr) value = digitalFunction->dataOut(value);
  return value;
}

void t_Register::reset(void) {
  value = OFF;
  if (digitalFunction != nullptr) digitalFunction->reset();  
}

//----------
void t_Toggle::begin(bool startValue) {
  value = startValue;
}

// Toggle
bool t_Toggle::dataOut(bool value) {
  this->value = (this->value ^ value);
  value = this->value;
  if (digitalFunction != nullptr) value = digitalFunction->dataOut(value);
  return value;
}

#endif
