/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Mediator
 * Version: 1.0
 * Type: Program
 * Programmeret af: Jan Birch
 * Opdateret: 14-03-2023
 * GNU General Public License version 3
 * This file is part of "Mediator".
 * 
 * "Mediator" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Mediator" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Mediator".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#ifndef Mediator_h
#define Mediator_h

#include <Arduino.h>
#include <JBKernel.h>
#include <JBManual.h>
#include <JBSensor.h>
#include <JBCtrlUnits.h>
#include <JBStateMachine.h>

// Ansvar: Holder samling af betjeninger, sensorer, styreenheder og tilstande.
// manuals[]: Vektor med pointere til alle betjeninger
// sensor[]: Vektor med pointere til alle sensorer
// ctrlUnits[]: Vektor med pointere til alle styreenheder
// states[]: Vektor med pointere til alle tilstande
struct t_Collection {
  t_Manual *manuals[MaxNoManuals];
  t_Sensor *sensors[MaxNoSensors];
  t_CtrlUnit *ctrlUnits[MaxNoCtrlUnits];
  t_StateMachine *states[MaxNoStates];  
} collection;

// Ansvar: Varetager kommunikation mellem betjeninger, sensorer, styrede enheder og tilstandsmaskine.
// Designet gør det muligt at koble forskellige typer af komponenter sammen, uden at hele softwaren skal opdateres.
// stateNo: Nuværende tilstand
// entryState: Hver gang der skiftes en tilstand skal indgangsmetoden kaldes. Det holder variablen styr på.
// begin(...): Initialiserer den første tilstand, som applikationen skal starte med.
// Desuden varetager metoden styring af overkørslens tilstand.
// status(...): Er en service til et tilstandsobjekt, som leverer en betjeningsenhed eller sensorenheds status.
// reset(...): Er en service til et tilstandsobjekt, som kan resette en betjeningsenhed eller sensorenhed.
// to(...): Er en service til et tilstandsobjekt, som kan sende en besked til en ydre enhed.
// doClockCycle(...): Sørger for at alle tilkoblede komponenter udfører polling.
class t_Mediator {
private:
  byte stateNo;
  bool entryState;
public:
  t_Mediator(void) {}
  void begin(byte stateName);
  byte statusManual(byte manualName) {return collection.manuals[manualName]->status();}
  byte statusSensor(byte sensorName) {return collection.sensors[sensorName]->status();}
  void to(byte ctrlUnitName, byte ctrlUnitState) {collection.ctrlUnits[ctrlUnitName]->to(ctrlUnitState);}
  void doClockCycle(void);
};

/*
 * CPP kode herunder
 */

void t_Mediator::begin(byte stateName) {
  stateNo = stateName;
  entryState = true;
}

void t_Mediator::doClockCycle(void) {
  byte cnt;  // Loop tæller
  byte nextStateNo;
  for (cnt=0; cnt < MaxNoManuals; cnt++) collection.manuals[cnt]->doClockCycle();
  for (cnt=0; cnt < MaxNoSensors; cnt++) collection.sensors[cnt]->doClockCycle();
  if (entryState == true) {
    collection.states[stateNo]->onEntry();
    entryState = false;
  }
  if (collection.states[stateNo]->changeState(&nextStateNo) == true) {
    collection.states[stateNo]->onExit();
    stateNo = nextStateNo;
    entryState = true;    
  }
}

#endif
