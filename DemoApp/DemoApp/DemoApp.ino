/*
 * Projekt: Generelle Arduino biblioteker
 * Produkt: Demo applikation
 * Version: 1.0
 * Type: Program
 * Programmeret af: Jan Birch
 * Opdateret: 14-03-2023
 * GNU General Public License version 3
 * This file is part of "Demo applikation".
 * 
 * "Demo applikation" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Demo applikation" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Demo applikation".  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se koncept og specifikation for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#include <JBKernel.h>

// Erklæring af input porte
const unsigned int MaxNoInParrPorts =3;
enum {RumlysVPort, LedelysPort, RumlysHPort};
enum {RumlysVPin=2, LedelysPin=3, RumlysHPin=4};
#include <JBInputDriver.h>
t_DigitalParrInDrv digitalParrInDrv;

const unsigned int MaxNoInAnlPorts =1;
enum {LyssensorPort};
enum {LyssensorPin=A0};
#include <JBAnalogInDriver.h>
t_AnalogParrInDrv analogParrInDrv;

// Erklæring af output porte
const unsigned int MaxNoOutParrPorts = 2;
enum {LedelampePort, RumlamperPort};
enum {LedelampePin=7, RumlamperPin=8};
#include <JBOutputDriver.h>
t_DigitalParrOutDrv digitalParrOutDrv;

// Erklæring af manuelle betjeninger
const unsigned int MaxNoManuals = 3;
enum {RumKnapV, LedelysKnap, RumKnapH};
#include <JBManual.h>
t_Button rumKnapVButton;
t_Button ledelysButton;
t_Button rumKnapHButton;

// Erklæring af flankedetektorer
#include <JBDigitalFunctions.h>
t_EdgeDetector rumKnapVFlankDet;
t_EdgeDetector ledelysFlankDet;
t_EdgeDetector rumKnapHFlankDet;

// Erklæring af sensorer
const unsigned int MaxNoSensors = 1;
enum {LysSensor};
#include <JBSensor.h>
#include "LightSensor.h"
t_LightSensor ledelysSensor;

// Erklæring af styreenheder
const unsigned int MaxNoCtrlUnits = 2;
enum {LedelysLamper, RumLamper};
#include <JBCtrlUnits.h>
t_OnOffOut ledelysLamperOut;
t_OnOffOut rumLamperOut;

// Erklæring af grænseflade til tilstandsmaskine
const unsigned int MaxNoStates = 5;
enum {Hvile, RumlysOn, LedelysManuel, LedelysAut, LedelysOff}; 
#include <JBStateMachine.h>

// Erklæring af mediator
#include "Mediator.h"
t_Mediator demoApp;

// Oprettelse af tilstande
#include "StateMachine.h"
t_HvileState hvileState;
t_RumlysOnState rumlysOnState;
t_LedelysManState ledelysManState;
t_LedelysAutState ledelysAutState;
t_LedelysOffState ledelysOffState;

//----------

void setup() {
// Opsætning af input porte
  digitalParrInDrv.setPort(RumlysVPort, RumlysVPin, NCLOSED, INTERN_PULLUP, BOUNCE_FILTER);
  digitalParrInDrv.setPort(LedelysPort, LedelysPin, NOPEN, EXTERN_PULLUP, BOUNCE_FILTER);
  digitalParrInDrv.setPort(RumlysHPort, RumlysHPin, NCLOSED, EXTERN_PULLUP, BOUNCE_FILTER);
  analogParrInDrv.setPort(LyssensorPort, LyssensorPin);
// Opsætning af output porte
  digitalParrOutDrv.setPort(LedelampePort, LedelampePin);
  digitalParrOutDrv.setPort(RumlamperPort, RumlamperPin);
// Opsætning af betjeninger
  rumKnapVButton.begin(&digitalParrInDrv, RumlysVPort);
  ledelysButton.begin(&digitalParrInDrv, LedelysPort);
  rumKnapHButton.begin(&digitalParrInDrv, RumlysHPort);
// Opsætning af flankedetektorer
  rumKnapVFlankDet.begin(ON, EDGEDOWN);
  ledelysFlankDet.begin(OFF, EDGEUP);
  rumKnapHFlankDet.begin(ON, EDGEDOWN);
  rumKnapVButton.setDigitalFunktion(&rumKnapVFlankDet);
  ledelysButton.setDigitalFunktion(&ledelysFlankDet);
  rumKnapHButton.setDigitalFunktion(&rumKnapHFlankDet);
// Opsætning af sensor
  ledelysSensor.begin(&analogParrInDrv, LyssensorPort);
// Opsætning af styrenheder
  ledelysLamperOut.begin(&digitalParrOutDrv, LedelampePort);
  rumLamperOut.begin(&digitalParrOutDrv, RumlamperPort);
// Opsætning af samling
  collection.manuals[RumKnapV] = &rumKnapVButton; collection.manuals[LedelysKnap] = &ledelysButton; collection.manuals[RumKnapH] = &rumKnapHButton;
  collection.sensors[LysSensor] = &ledelysSensor;
  collection.ctrlUnits[LedelysLamper] = &ledelysLamperOut; collection.ctrlUnits[RumLamper] = &rumLamperOut;
  collection.states[Hvile] = &hvileState; collection.states[RumlysOn] = &rumlysOnState; collection.states[LedelysManuel] = &ledelysManState;
  collection.states[LedelysAut] = &ledelysAutState; collection.states[LedelysOff] = &ledelysOffState;
// Start applikation
  demoApp.begin(Hvile);
}

void loop() {
  Clock::pendulum();
  digitalParrInDrv.doClockCycle();
  analogParrInDrv.doClockCycle();
  demoApp.doClockCycle();
}
