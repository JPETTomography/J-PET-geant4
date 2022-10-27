/**
 *  @copyright Copyright 2020 The J-PET Monte Carlo Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  @file EventMessenger.cpp
 */

#include "../Core/RunManager.h"
#include "EventMessenger.h"

EventMessenger* EventMessenger::fInstance = nullptr;

EventMessenger* EventMessenger::GetEventMessenger()
{
  if (fInstance == nullptr) {
    fInstance = new EventMessenger();
  }
  return fInstance;
}

EventMessenger::EventMessenger()
{
  fDirectory = new G4UIdirectory("/jpetmc/event/");
  fDirectory->SetGuidance("Define events to save");

  fCMDKillEventsEscapingWorld = new G4UIcmdWithABool("/jpetmc/event/saveEvtsDetAcc", this);
  fCMDKillEventsEscapingWorld->SetGuidance("Killing events when generated particle escapes detector");

  fPrintStat = new G4UIcmdWithABool("/jpetmc/event/printEvtStat", this);
  fPrintStat->SetGuidance("Print how many events was generated");

  fPrintStatPower = new G4UIcmdWithAnInteger("/jpetmc/event/printEvtFactor", this);
  fPrintStatPower->SetGuidance("Define X in divisor (10^X) for number of printed event ");

  fCMDMinRegMulti = new G4UIcmdWithAnInteger("/jpetmc/event/minRegMulti", this);
  fCMDMinRegMulti->SetGuidance("Set the lower value of registered multiplicity (works only with saveEvtsDetAcc); def: 0");

  fCMDMaxRegMulti = new G4UIcmdWithAnInteger("/jpetmc/event/maxRegMulti", this);
  fCMDMaxRegMulti->SetGuidance("Set the upper value of registered multiplicity (works only with saveEvtsDetAcc); def: 10");

  fCMDExcludedMulti = new G4UIcmdWithAnInteger("/jpetmc/event/excludedMulti", this);
  fCMDExcludedMulti->SetGuidance("Set excluded  multiplicity (works only with saveEvtsDetAcc); def: 1");

  fCMDSave2g = new G4UIcmdWithABool("/jpetmc/event/save2g", this);
  fCMDSave2g->SetGuidance("Events with registered 2g will be saved (default false)");

  fCMDSave3g = new G4UIcmdWithABool("/jpetmc/event/save3g",this);
  fCMDSave3g->SetGuidance("Events with registered 3g will be saved (default false)");

  fPrintStatBar = new G4UIcmdWithABool("/jpetmc/event/ShowProgress", this);
  fPrintStatBar->SetGuidance("Print how many events was generated (in %)");

  fAddDatetime = new G4UIcmdWithABool("/jpetmc/output/AddDatetime", this);
  fAddDatetime->SetGuidance("Adds to the output file name date and time of simulation start.");

  fSetSeed = new G4UIcmdWithAnInteger("/jpetmc/SetSeed", this);
  fSetSeed->SetGuidance("Use specific seed. If 0 provided seed will be random.");
  fSetSeed->SetDefaultValue(0);

  fSaveSeed = new G4UIcmdWithABool("/jpetmc/SaveSeed", this);
  fSaveSeed->SetGuidance("Save random seed (default false).");

  fCMDAllowedMomentumTransfer = new G4UIcmdWithADoubleAndUnit("/jpetmc/setAllowedMomentumTransfer", this);
  fCMDAllowedMomentumTransfer->SetGuidance("Limit on momentum transfer that will classify interaction as background (10keV)");
  fCMDAllowedMomentumTransfer->SetDefaultValue(1 * keV);
  fCMDAllowedMomentumTransfer->SetUnitCandidates("keV");
	
  fCMDAppliedEnergyCut = new G4UIcmdWithADoubleAndUnit("/jpetmc/event/SetEnergyCut",this);
  fCMDAppliedEnergyCut->SetGuidance("Cut on kinetic energy of primary photon after first interaction in scintillator");
  fCMDAppliedEnergyCut->SetDefaultValue(0.0001 * keV);
  fCMDAppliedEnergyCut->SetUnitCandidates("keV");
	
  fCMDAppliedRangeCut = new G4UIcmdWithADoubleAndUnit("/jpetmc/event/SetRangeCut",this);
  fCMDAppliedRangeCut->SetGuidance(" Compton e- should have sufficient K.E enabling it to travel the given range in material");
  fCMDAppliedRangeCut->SetDefaultValue(1 * mm);
  fCMDAppliedRangeCut->SetUnitCandidates("mm");

  fCMDSaveEnoughSizedEvent = new G4UIcmdWithAnInteger("/jpetmc/event/saveEventWithSize", this);
  fCMDSaveEnoughSizedEvent->SetGuidance("Set the size of the event you want to save");

  fSetEnergyRangeToSave = new G4UIcmdWith3VectorAndUnit("/jpetmc/event/setEnergyRange", this);
  fSetEnergyRangeToSave->SetGuidance("Set energy range for checking the event size (first minEnergy then maxEnergy) - works with saveEventWithSize!!");
  fSetEnergyRangeToSave->SetDefaultValue(G4ThreeVector(-1*keV, -1*keV, 0));
  fSetEnergyRangeToSave->SetDefaultUnit("keV");
  fSetEnergyRangeToSave->SetUnitCandidates("keV");
  fSetEnergyRangeToSave->SetParameterName("MinEnergy", "MaxEnergy", "none", false);

  fCreateDecayTree = new G4UIcmdWithABool("/jpetmc/output/CreateDecayTree", this);
  fCreateDecayTree->SetGuidance("Creates decay trees for each event.");
}

EventMessenger::~EventMessenger()
{
  delete fPrintStat;
  delete fPrintStatPower;
  delete fPrintStatBar;
  delete fAddDatetime;
  delete fSetSeed;
  delete fSaveSeed;
  delete fCMDKillEventsEscapingWorld;
  delete fCMDSave2g;
  delete fCMDSave3g;
  delete fCMDMinRegMulti;
  delete fCMDMaxRegMulti;
  delete fCMDExcludedMulti;
  delete fCMDAppliedEnergyCut;
  delete fCMDAppliedRangeCut;
  delete fCMDSaveEnoughSizedEvent;
  delete fSetEnergyRangeToSave;
  delete fCreateDecayTree;
}

void EventMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fPrintStat) {
    fPrintStatistics = fPrintStat->GetNewBoolValue(newValue);
  } else if (command == fPrintStatPower) {
    fPrintPower = fPrintStatPower->GetNewIntValue(newValue);
  } else if (command == fCMDMinRegMulti) {
    fMinRegisteredMultiplicity = fCMDMinRegMulti->GetNewIntValue(newValue);
  } else if (command == fCMDMaxRegMulti) {
    fMaxRegisteredMultiplicity = fCMDMaxRegMulti->GetNewIntValue(newValue);
  } else if (command == fPrintStatBar) {
    fShowProgress = fPrintStatBar->GetNewBoolValue(newValue);
  } else if (command == fAddDatetime) {
    fOutputWithDatetime = fAddDatetime->GetNewBoolValue(newValue);
  } else if (command == fCMDKillEventsEscapingWorld) {
    fKillEventsEscapingWorld = fCMDKillEventsEscapingWorld->GetNewBoolValue(newValue);
  } else if (command == fCMDExcludedMulti) {
    fExcludedMultiplicity = fCMDExcludedMulti->GetNewIntValue(newValue);
  } else if (command == fSetSeed) {
    fSeed = fSetSeed->GetNewIntValue(newValue);
  } else if (command == fSaveSeed) {
    fSaveRandomSeed = fSaveSeed->GetNewBoolValue(newValue);
  } else if (command == fCMDAllowedMomentumTransfer) {
    fAllowedMomentumTransfer = fCMDAllowedMomentumTransfer->GetNewDoubleValue(newValue);
  } else if (command == fCMDAppliedEnergyCut) {
    fEnergyCut = fCMDAppliedEnergyCut->GetNewDoubleValue(newValue);
    fUseEnergyCut = true;
  } else if (command == fCMDAppliedRangeCut) {
    fRangeCut = fCMDAppliedRangeCut->GetNewDoubleValue(newValue);
    fUseRangeCut = true;
  } else if (command == fCMDSave2g) {
    fSave2g = fCMDSave2g->GetNewBoolValue(newValue);
  } else if (command == fCMDSave3g) {
    fSave3g = fCMDSave3g->GetNewBoolValue(newValue);
  } else if (command == fCMDSaveEnoughSizedEvent) {
    fEventDesiredSize = fCMDSaveEnoughSizedEvent->GetNewIntValue(newValue);
  } else if (command == fSetEnergyRangeToSave) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4double minEnergy;
    G4double maxEnergy;
    bool strictMultCheck;
    is >> minEnergy >> maxEnergy >> strictMultCheck;
    fEnergyRangeToSave = std::make_pair(minEnergy, maxEnergy);
    fMultEnergyCheck = strictMultCheck;
  } else if (command == fCreateDecayTree) {
    fCreateDecayTreeFlag = fCreateDecayTree->GetNewBoolValue(newValue);
  }
}
