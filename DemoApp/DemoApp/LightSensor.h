/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Lyssensor
 * Version: 1.0
 * Type: Program
 * Programmeret af: Jan Birch
 * Opdateret: 14-03-2023
 * GNU General Public License version 3
 * This file is part of "Lyssensor".
 * 
 * "Lyssensor" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Lyssensor" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Lyssensor".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#ifndef LightSensor_h
#define LightSensor_h

#include <Arduino.h>
#include <JBKernel.h>

// Ansvar: Varetager lyssensor
// doClockCycle(...): Læser input og opdaterer status.
class t_LightSensor: public t_Sensor {
private:
  const int treshold = 650;
public:
  t_LightSensor(void) {}
  void doClockCycle(void);
};

/*
 * CPP kode herunder
 */

void t_LightSensor::doClockCycle(void) {
  if (driver == nullptr) return;
  int analogValue;
  driver->read(portNo, &analogValue);
  state = (analogValue < treshold)? ON: OFF;
}
#endif
