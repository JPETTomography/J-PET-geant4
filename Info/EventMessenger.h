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
 *  @file EventMessenger.h
 */

#ifndef EVENTMESSENGER_H
#define EVENTMESSENGER_H 1

#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithABool.hh>
#include <G4SystemOfUnits.hh>
#include <G4UIdirectory.hh>
#include <G4UImessenger.hh>
#include <globals.hh>

class EventMessenger : public G4UImessenger
{
public:
  static EventMessenger* GetEventMessenger();
  void SetNewValue(G4UIcommand*, G4String);

  bool KillEventsEscapingWorld() { return fKillEventsEscapingWorld; }
  bool PrintStatistics() { return fPrintStatistics; }
  bool ShowProgress() { return fShowProgress; }
  G4int GetPowerPrintStat() { return fPrintPower; }
  bool AddDatetime() { return fOutputWithDatetime; }
  G4int GetMinRegMultiplicity() { return fMinRegisteredMultiplicity; }
  G4int GetMaxRegMultiplicity() { return fMaxRegisteredMultiplicity; }
  G4int GetExcludedMultiplicity() { return fExcludedMultiplicity; }
  G4double GetAllowedMomentumTransfer() { return fAllowedMomentumTransfer; }
  G4double GetEnergyCut() { return fEnergyCut; }
  G4double GetRangeCut() { return fRangeCut; }
  bool GetEnergyCutFlag() { return fUseEnergyCut; }
  bool GetRangeCutFlag() { return fUseRangeCut; }
  G4int GetSeed() { return fSeed; }
  bool SaveSeed() { return fSaveRandomSeed; }
  bool Save2g() { return fSave2g; }
  bool Save3g() { return fSave3g; }
  G4int GetMultiplicityToSaveEvent() { return fEventDesiredSize; }
  std::pair<G4double, G4double> GetEnergyRangeToSave() { return fEnergyRangeToSave; }
  bool GetMultiplicityToSaveEventWithEnergy() { return fMultEnergyCheck; }
  bool GetCreateDecayTreeFlag() { return fCreateDecayTreeFlag; }

private:
  static EventMessenger* fInstance;
  EventMessenger();
  ~EventMessenger();

  G4UIdirectory* fDirectory = nullptr;
  G4UIcmdWithABool* fPrintStat = nullptr;
  G4UIcmdWithABool* fPrintStatBar = nullptr;
  G4UIcmdWithABool* fAddDatetime = nullptr;
  G4UIcmdWithABool* fCMDKillEventsEscapingWorld = nullptr;
  G4UIcmdWithAnInteger* fPrintStatPower = nullptr;
  G4UIcmdWithAnInteger* fCMDMinRegMulti = nullptr;
  G4UIcmdWithAnInteger* fCMDMaxRegMulti = nullptr;
  G4UIcmdWithAnInteger* fCMDExcludedMulti = nullptr;
  G4UIcmdWithAnInteger* fSetSeed = nullptr;
  G4UIcmdWithAnInteger* fCMDSaveEnoughSizedEvent = nullptr;
  G4UIcmdWithABool* fSaveSeed = nullptr;
  G4UIcmdWithADoubleAndUnit* fCMDAllowedMomentumTransfer = nullptr;
  G4UIcmdWithADoubleAndUnit* fCMDAppliedEnergyCut = nullptr;
  G4UIcmdWithADoubleAndUnit* fCMDAppliedRangeCut  = nullptr;
  G4UIcmdWithABool* fCMDSave2g = nullptr;
  G4UIcmdWithABool* fCMDSave3g = nullptr;
  G4UIcmdWithABool* fCreateDecayTree = nullptr;
  G4UIcmdWith3VectorAndUnit* fSetEnergyRangeToSave = nullptr;
  
  bool fPrintStatistics = false;
  G4int fPrintPower = 10;
  bool fShowProgress = false;
  bool fOutputWithDatetime = false;
  bool fKillEventsEscapingWorld = false;
  G4int fMinRegisteredMultiplicity = 0;
  G4int fMaxRegisteredMultiplicity = 10;
  G4int fExcludedMultiplicity = 1;
  G4int fSeed = 0;
  bool fSaveRandomSeed = false;
  G4double fAllowedMomentumTransfer = 1 * keV;
  bool fUseEnergyCut = false;
  bool fUseRangeCut = false;
  G4double fEnergyCut = 0 * keV;
  G4double fRangeCut  = 1 * mm;
  bool fSave2g = false;
  bool fSave3g = false;
  G4int fEventDesiredSize = 0;
  std::pair<G4double, G4double> fEnergyRangeToSave = {-1*keV, -1*keV};
  bool fMultEnergyCheck = false;
  bool fCreateDecayTreeFlag = false;
};

#endif /* !EVENTMESSENGER_H */
