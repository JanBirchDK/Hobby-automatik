/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Kerne med tidsstyring, ure og timere
 * Version: 1.1
 * Type: Bibliotek
 * Programmeret af: Jan Birch
 * Opdateret: 05-04-2023
 * GNU General Public License version 3
 * This file is part of Kerne med tidsstyring, ure og timere.
 * 
 * Kerne med tidsstyring, ure og timere is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kerne med tidsstyring, ure og timere is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRODUKT.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Noter:
 * Se koncept og specifikation for en detaljeret beskrivelse af biblioteket, formål og anvendelse.
 */

#ifndef JBKernel_h
#define JBKernel_h

#include <Arduino.h>

// Tidsenhed til konvertering
enum {MSEC, SECONDS};
// Generelt bruges for binære værdier
enum {OFF, ON};
// Masterblinker låses på eget nummer
enum {MASTERBLINKERNO=127};
// Enheder med blink bruger status
enum {BLINK=2};

// Ansvar: Klokken varetager taktslaget til polling.
// Det er en ventefunktion som sørger for synkronisering med arduino klokken
// og kompenserer for den tid det tager at gennemløbe programmet.
// ClockCycle: Sat til msek
// pendulum(...): Leverer takslaget
namespace Clock {
  static byte ClockCycle=5;
  void pendulum(void);
  unsigned long convertToClockCycles(unsigned long a_time);
}

#define SecondsToMilliSecs(A) A*1000

//----------

// Ansvar: Simpel timer trigger efter en tidsperiode.
// Det er et tælleværk styret af polling
// Varighed duration i msek omregnes til antal cyklus
// triggered(...): Leverer sand når tiden er udløbet
class t_SimpleTimer {
private:
  unsigned int noCycles;
  unsigned int cycle;
public:
  t_SimpleTimer(void): noCycles(1), cycle(1){};
  t_SimpleTimer(unsigned int duration);
  void setDuration(unsigned int duration, bool inSeconds = MSEC);
  bool triggered(void);    
};

//----------

// Ansvar: Blinker leverer standard blink.
// HalfPeriod: Sat til msek
// triggered(...): Leverer sand når tiden er udløbet
namespace Blinker {
  static unsigned int HalfPeriod=500;
  static bool value=false;
  static t_SimpleTimer timer(HalfPeriod);
  void doClockCycle(void);
  bool dataOut(void);
}

// Al brug af blink og multivibrator går igennem en fælles notifikation
// Returnerer: Multivibrators kurveform
#ifndef Multivibrator_h
bool blinkerNotification(unsigned int blinkerNo);
#endif

//----------

// Der er en del samlinger med arrays, hvor argumenter i metodekald skal tjekkes.
// Det er en forudsætning at samlingen har et bool array, der har data for konfigurerede elementer
// Returnerer: Om indeks er gyldigt
// Returnerer: Om element er konfigureret

bool isValidIndex(unsigned int index, unsigned int arrayLength);
bool hasConfig(bool isSetup[], unsigned int index, unsigned int arrayLength);
bool hasConfig(void *element, unsigned int index, unsigned int arrayLength);

/*
 * CPP kode herunder
 */

void Clock::pendulum(void) {
  static unsigned long cycleStart=0;
  unsigned long w_millis;     // Tiden skrider hvis millis læser flere gange
  do w_millis = millis();
  while (w_millis < (cycleStart+ClockCycle));
  cycleStart = (w_millis/ClockCycle)*ClockCycle;  // Omregner til eksakt multiplum clockcykles
}

unsigned long Clock::convertToClockCycles(unsigned long a_time) {
  return a_time/ClockCycle;
}

//----------

t_SimpleTimer::t_SimpleTimer(unsigned int duration) {
  noCycles=cycle=Clock::convertToClockCycles(duration);
}

void t_SimpleTimer::setDuration(unsigned int duration, bool inSeconds) {
  if (inSeconds==SECONDS) duration=SecondsToMilliSecs(duration);
  noCycles=cycle=Clock::convertToClockCycles(duration);
}

bool t_SimpleTimer::triggered(void) {
  cycle--;
  if (cycle == 0) cycle = noCycles;
  return (cycle == noCycles);
}

//----------

void Blinker::doClockCycle(void) {
  if (timer.triggered()==true) value = !value;
}

bool Blinker::dataOut(void) {
  return value;
}

#ifndef Multivibrator_h
bool blinkerNotification(unsigned int blinkerNo) {
  return (blinkerNo == MASTERBLINKERNO)?Blinker::dataOut():OFF;
}
#endif

//----------

bool isValidIndex(unsigned int index, unsigned int arrayLength) {
  return (index >= 0 && index < arrayLength);
}

bool hasConfig(bool isSetup[], unsigned int index, unsigned int arrayLength) {
  bool result = false;
  if (isValidIndex(index, arrayLength) == true) result = isSetup[index];
  return result;
}

bool hasConfig(void *element, unsigned int index, int unsigned arrayLength) {
  bool result = false;
  if (isValidIndex(index, arrayLength) == true) result = (element == nullptr)? false: true;
  return result;
}
#endif
