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
 *  @file EventAction.cpp
 */

#include "../Info/PrimaryParticleInformation.h"
#include "../Objects/Geant4/DetectorHit.h"
#include "../Objects/Geant4/Trajectory.h"
#include "EventAction.h"

#include <G4TrajectoryContainer.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryParticle.hh>
#include <G4EventManager.hh>
#include <G4SDManager.hh>
#include <G4Event.hh>
#include "G4RunManager.hh"

EventAction::EventAction() : is2gRec(false), is3gRec(false), fEventID(0)
{}

EventAction::EventAction(HistoManager* histo) : G4UserEventAction(), fHistoManager(histo), fScinCollID(-1), is2gRec(false), is3gRec(false), fEventID(0)
{}

EventAction::~EventAction() {}

// cppcheck-suppress unusedFunction
void EventAction::BeginOfEventAction(const G4Event*)
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  if (fScinCollID < 0) {
    G4String colNam;
    fScinCollID = SDman->GetCollectionID(colNam = "detectorCollection");
  }
  fHistoManager->Clear();
  fHistoManager->SetEventNumber(fEventID);
  fEventID++;
}

// cppcheck-suppress unusedFunction
void EventAction::EndOfEventAction(const G4Event* anEvent)
{
  if (anEvent->GetNumberOfPrimaryVertex() == 0) return;
  if (fEvtMessenger->KillEventsEscapingWorld()) {
    if (G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->IsAborted()) {
      return;
    }
  }
  bool saveEvt = true;

  if (fEvtMessenger->Save2g()) {
    CheckIf2gIsRegistered(anEvent);
    if ( ! Is2gRegistered() ) {
      saveEvt = false;
      G4RunManager::GetRunManager()->AbortEvent();
    }
  }

  if (fEvtMessenger->Save3g()) {
    CheckIf3gIsRegistered(anEvent);
    if ( ! Is3gRegistered() ) {
      saveEvt = false;
      G4RunManager::GetRunManager()->AbortEvent();
    }
  }
  if (fEvtMessenger->GetMultiplicityToSaveEvent()) {
    CheckIfEventHasEnoughSize(anEvent);
    if ( ! IsEnoughSize() ) {
      saveEvt = false;
      G4RunManager::GetRunManager()->AbortEvent();
    }
  }

  if (saveEvt) {
    WriteToFile(anEvent);
    fHistoManager->SetEventNumber(anEvent->GetEventID() + 1);
  } else
    fHistoManager->DontSaveEvent();
}

void EventAction::WriteToFile(const G4Event* anEvent)
{
  //! save information about generated events
  G4int id = anEvent->GetEventID();
  fHistoManager->SetEventNumber(id);
  fHistoManager->FillHistoGenInfo(anEvent);

  //! save information about registered events
  G4HCofThisEvent* HCE = anEvent->GetHCofThisEvent();
  DetectorHitsCollection* DHC = 0;
  if (HCE) {
    DHC = dynamic_cast<DetectorHitsCollection*>(HCE->GetHC(fScinCollID));
    int n_hit = DHC->entries();
    for (int i = 0; i < n_hit; i++) {
      DetectorHit* dh = dynamic_cast<DetectorHit*>(DHC->GetHit(i));
     
      // Forcefully cut the remnants from the cut on photon durinng first interaction----     
      double EnergyDeposit = dh->GetEdep();
      // Removing remnants from the energy deposition cut on prim photon
      if (EnergyDeposit < .511 - fEvtMessenger->GetEnergyCut() && fEvtMessenger->GetEnergyCutFlag()) continue;
     
      fHistoManager->AddNewHit(dh);
      fHistoManager->AddEventInfo(dh, n_hit);
    }
  }

  //! save to output file
  fHistoManager->SaveEvtPack();
}

void EventAction::CheckIf2gIsRegistered(const G4Event* anEvent)
{
  bool isGenerated = false;
  bool isReconstructed = true;
  is2gRec = false;
  std::vector<bool> isGammaRec{false,false,false};

  for (int i=0; i<anEvent->GetNumberOfPrimaryVertex(); i++) {
    VtxInformation* info =  dynamic_cast<VtxInformation*>(anEvent->GetPrimaryVertex(i)->GetUserInformation());
    if (info != nullptr) {
      isGenerated = isGenerated || info->GetTwoGammaGen();
    }
  }

  if (!isGenerated) { return; }

  G4HCofThisEvent * HCE = anEvent->GetHCofThisEvent();
  DetectorHitsCollection* DHC = 0;
  if (HCE) {
    DHC = dynamic_cast<DetectorHitsCollection*>(HCE->GetHC(fScinCollID));
    int n_hit = DHC->entries();
    if (n_hit<2) return;

    for (int i=0; i<n_hit; i++) {
       DetectorHit* dh =  dynamic_cast<DetectorHit*>(DHC->GetHit(i));
       if (dh->GetGenGammaMultiplicity() == 2) {
         isGammaRec[dh->GetGenGammaIndex()] = true;
       }
    }
  }

  for (int i=1; i<=2; i++) {
    isReconstructed = isReconstructed && isGammaRec[i];
  }
  is2gRec = isReconstructed;
}

void EventAction::CheckIf3gIsRegistered(const G4Event* anEvent)
{
  bool isGenerated = false;
  bool isReconstructed = true;
  is3gRec = false;
  std::vector<bool> isGammaRec{false,false,false,false};

  for (int i=0; i<anEvent->GetNumberOfPrimaryVertex(); i++) {
    VtxInformation* info =  dynamic_cast<VtxInformation*>(anEvent->GetPrimaryVertex(i)->GetUserInformation());    
    if (info != nullptr) {
      isGenerated = isGenerated || info->GetThreeGammaGen();
    }
  }

  if (!isGenerated) { return; }

  G4HCofThisEvent * HCE = anEvent->GetHCofThisEvent();
  DetectorHitsCollection* DHC = 0;
  if (HCE) {
    DHC = dynamic_cast<DetectorHitsCollection*>(HCE->GetHC(fScinCollID));
    int n_hit = DHC->entries();
    if (n_hit<3) return;

    for (int i=0; i<n_hit; i++) {
       DetectorHit* dh =  dynamic_cast<DetectorHit*>(DHC->GetHit(i));
       if (dh->GetGenGammaMultiplicity() == 3) {
         isGammaRec[dh->GetGenGammaIndex()] = true;
       }
    }
  }

  for(int i=1; i<=3; i++) {
    isReconstructed = isReconstructed && isGammaRec[i];
  }
  is3gRec = isReconstructed;
}

void EventAction::CheckIfEventHasEnoughSize(const G4Event* anEvent)
{
  isEnoughSize = false;

  int desiredSize = fEvtMessenger->GetMultiplicityToSaveEvent();
  G4double minEnergy = fEvtMessenger->GetEnergyRangeToSave().first;
  G4double maxEnergy = fEvtMessenger->GetEnergyRangeToSave().second;
  int numberOfHitsWithEnoughEnergy = 0;

  G4HCofThisEvent * HCE = anEvent->GetHCofThisEvent();
  DetectorHitsCollection* DHC = 0;
  if (HCE) {
    DHC = dynamic_cast<DetectorHitsCollection*>(HCE->GetHC(fScinCollID));
    int n_hit = DHC->entries();
    if (n_hit<desiredSize) return;
    for (int i=0; i<n_hit; i++) {
       DetectorHit* dh =  dynamic_cast<DetectorHit*>(DHC->GetHit(i));
       if (((dh->GetEdep()/keV > minEnergy) || minEnergy < 0) && ((dh->GetEdep()/keV < maxEnergy) || maxEnergy < 0)) {
         numberOfHitsWithEnoughEnergy++;
       }
    }
    if (numberOfHitsWithEnoughEnergy == desiredSize)
      isEnoughSize = true;
    if (fEvtMessenger->GetMultiplicityToSaveEventWithEnergy() && n_hit != desiredSize)
      isEnoughSize = false;
  }
}
