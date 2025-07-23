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
 *  @file DetectorHit.h
 */

#ifndef DETECTORHIT_H
#define DETECTORHIT_H 1

#include <G4THitsCollection.hh>
#include <G4ThreeVector.hh>
#include <G4Allocator.hh>
#include <G4Types.hh>
#include <G4VHit.hh>

/**
 * @class DetectorHit
 * @brief Class containing hits in the sensitive part of the detector
 *
 * Objects of this class are created in DetectorSD class
 * and rewritten into ROOT tree in HistoManager::FillScin()
 */
class DetectorHit : public G4VHit
{
public:
  DetectorHit();
  virtual ~DetectorHit();

  void SetEdep(G4double de) { fEdep = de; }
  void SetTime(G4double val, G4double weight);
  void SetPosition(G4ThreeVector xyz, G4double weight);
  void SetTrackID(G4int i) { fTrackID = i; }
  void SetParentTrackID(G4int i) { fTrackParentID = i; }
  void SetInteractionType(G4int i) { fInteractionType = i; }
  void SetDecayChannel(G4int i) { fDecayChannel = i; }
  void SetTrackPDG(G4int i) { fTrackPDG = i; }
  void SetScinID(G4int i) { fScinID = i; }
  void SetInteractionNumber() { fNumInteractions = 1; }

  //! Many interactions in the scintillator are merged into a single hit
  void AddInteraction() { fNumInteractions += 1; }
  void AddEdep(G4double de) { fEdep += de; }
  void AddTime(G4double val, G4double weight);
  void AddPosition(G4ThreeVector xyz, G4double weight);

  void SetPolarizationIn(G4ThreeVector xyz) { fPolarizationIn = xyz; }
  void SetPolarizationOut(G4ThreeVector xyz) { fPolarizationOut = xyz; }
  void SetMomentumIn(G4ThreeVector xyz) { fMomentumIn = xyz; }
  void SetMomentumOut(G4ThreeVector xyz) { fMomentumOut = xyz; }
  void SetProcessName(G4String str) { fName = str; }
  void SetGenGammaMultiplicity(G4int i) { fGenGammaMultiplicity = i; }
  void SetGenGammaIndex(G4int i) { fGenGammaIndex = i; }

  //! Time and position are weighted by energy
  G4double GetTime();
  G4ThreeVector GetPosition();
  G4int GetScinID() { return fScinID; }
  G4int GetTrackID() { return fTrackID; }
  G4int GetParentID() { return fTrackParentID; }
  G4int GetInteractionType() const { return fInteractionType; }
  G4int GetDecayChannel() const { return fDecayChannel; }
  G4double GetEdep() { return fEdep; }
  G4int GetTrackPDG() { return fTrackPDG; }
  G4ThreeVector GetPolarizationIn() { return fPolarizationIn; }
  G4ThreeVector GetPolarizationOut() { return fPolarizationOut; }
  G4ThreeVector GetMomentumIn() { return fMomentumIn; }
  G4ThreeVector GetMomentumOut() { return fMomentumOut; }
  G4int GetNumInteractions() { return fNumInteractions; }
  G4String GetProcessName() { return fName; }
  G4int GetGenGammaMultiplicity() { return fGenGammaMultiplicity; }
  G4int GetGenGammaIndex() { return fGenGammaIndex; }

private:
  //! Scintillator number (arbitrary!; not consistent with convention used in laboratory)
  G4int fScinID;
  //! Track identificator
  G4int fTrackID;
  //! Parent Track identificator
  G4int fTrackParentID;
  //! Interaction type
  G4int fInteractionType;
  //! Decay channel,
  G4int fDecayChannel;
  //! Particle Data Group numbering for particles
  G4int fTrackPDG;
  //! Total energy deposited in the strip
  G4double fEdep;
  //! Interaction time
  G4double fTime;
  //! 3D position of interaction
  G4ThreeVector fPos;
  //! Counts interactions classified as single hit
  G4int fNumInteractions;
  //! Polarization vector before interaction
  G4ThreeVector fPolarizationIn;
  //! Polarization vector after interaction
  G4ThreeVector fPolarizationOut;
  //! Momentum vector before interaction
  G4ThreeVector fMomentumIn;
  //! Momentum vector after interaction
  G4ThreeVector fMomentumOut;
  // Assigned from PrimaryParticleInformation
  G4int fGenGammaMultiplicity;
  G4int fGenGammaIndex;
  //! process name
  G4String fName;
  //! Weight used for position
  G4double fSumWeightPosition;
  //! Weight used for time
  G4double fSumWeightTime;
};

typedef G4THitsCollection<DetectorHit> DetectorHitsCollection;

#endif /* !DETECTORHIT_H */
