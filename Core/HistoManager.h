/**
 *  @copyright Copyright 2021 The J-PET Monte Carlo Authors. All rights reserved.
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
 *  @file HistoManager.h
 */

#ifndef HISTOMANAGER_H
#define HISTOMANAGER_H 1

#include "../Info/EventMessenger.h"
#include "../Info/VtxInformation.h"
#include "../Objects/Framework/JPetGeantDecayTree.h"
#include "../Objects/Framework/JPetGeantDecayTreeBranch.h"
#include "../Objects/Framework/JPetGeantEventInformation.h"
#include "../Objects/Framework/JPetGeantEventPack.h"
#include "../Objects/Framework/JPetGeantScinHits.h"
#include "../Objects/Geant4/DetectorHit.h"

#include <G4Event.hh>
#include <G4PrimaryParticle.hh>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TTree.h>
#include <globals.hh>
#include <set>
#include <unordered_map>

class TFile;
class TTree;

struct doubleCheck
{
  bool isChanged = false;
  double value = 0.;
  doubleCheck() {}
  explicit doubleCheck(double newValue)
  {
    value = newValue;
    isChanged = true;
  }
};

/**
 * @class HistoManager
 * @brief class reach for informations stored during sumulations
 * and saves them into ROOT tree structures
 */
class HistoManager
{
public:
  HistoManager();
  ~HistoManager();

  void Book(); //! call once; book (create) all trees and histograms
  void Save(); //! call once; save all trees and histograms
  void SaveEvtPack();
  void Clear() { fEventPack->Clear(); };
  void AddGenInfo(VtxInformation* info);
  void AddGenInfoParticles(G4PrimaryParticle* particle);
  void AddNewHit(DetectorHit*);
  void AddNodeToDecayTree(int nodeID, int trackID);
  void SetParentIDofPhoton(int x) { fParentIDofPhoton = x; };
  int GetParentIDofPhoton() const { return fParentIDofPhoton; };
  void SetEventNumber(int x) { fEventPack->SetEventNumber(x); };
  int GetEventNumber() { return fEventPack->GetEventNumber(); };
  void SetHistogramCreation(bool tf) { fMakeControlHisto = tf; };
  bool GetMakeControlHisto() const { return fMakeControlHisto; };
  void SetDecayChannel(DecayChannel decayChannel) { fDecayChannel = decayChannel; };
  void FillHistoGenInfo(const G4Event* anEvent);
  void FillCosmicInfo(G4double theta, G4ThreeVector init, G4ThreeVector vtx);
  const JPetGeantEventInformation* GetGeantInfo() const { return fGeantInfo; }
  void createHistogramWithAxes(TObject* object, TString xAxisName = "Default X axis title [unit]", TString yAxisName = "Default Y axis title [unit]",
                               TString zAxisName = "Default Z axis title [unit]");

  void fillHistogram(const char* name, double xValue, doubleCheck yValue = doubleCheck(), doubleCheck zValue = doubleCheck());
  void writeError(const char* nameOfHistogram, const char* messageEnd);

private:
  HistoManager(const HistoManager& histoManagerToCopy);

  int fParentIDofPhoton = 0;
  bool fEndOfEvent = true;
  bool fEmptyEvent = true;
  DecayChannel fDecayChannel;
  bool fBookStatus = false;
  bool fMakeControlHisto = true;
  TFile* fRootFile = nullptr;
  TTree* fTree = nullptr;
  TBranch* fBranchTrk = nullptr;
  TBranch* fBranchScin = nullptr;
  TBranch* fBranchEventPack = nullptr;

  JPetGeantDecayTree* fTempDecayTree = nullptr;
  JPetGeantEventPack* fEventPack = nullptr;
  JPetGeantEventInformation* fGeantInfo = nullptr;
  EventMessenger* fEvtMessenger = EventMessenger::GetEventMessenger();

  void BookHistograms();

protected:
  std::unordered_map<std::basic_string<char>, TObject*> fControlHistograms;
  std::set<std::string> fErrorCounts;
};

#endif /* !HISTOMANAGER_H */
