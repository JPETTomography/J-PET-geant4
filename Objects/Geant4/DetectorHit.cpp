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
 *  @file DetectorHit.cpp
 */

#include "DetectorHit.h"

DetectorHit::DetectorHit() : G4VHit(), fScinID(0), fTrackID(-1), 
fTrackParentID(-1), fInteractionType(0), fDecayChannel(0), fTrackPDG(0),
fEdep(0.0), fTime(0), fPos(0), fNumInteractions(0), fPolarizationIn(0, 0, 0),
fPolarizationOut(0, 0, 0), fMomentumIn(0, 0, 0), fMomentumOut(0, 0, 0),
fGenGammaMultiplicity(0), fGenGammaIndex(0) {}

DetectorHit::~DetectorHit() {}

void DetectorHit::SetTime(G4double val, G4double weight)
{
  fTime = weight * val;
  fSumWeightTime = weight;
}

void DetectorHit::AddTime(G4double val, G4double weight)
{
  fTime += weight * val;
  fSumWeightTime += weight;
}

void DetectorHit::SetPosition(G4ThreeVector xyz, G4double weight)
{
  fPos = weight * xyz;
  fSumWeightPosition = weight;
}

void DetectorHit::AddPosition(G4ThreeVector xyz, G4double weight)
{
  fPos += weight * xyz;
  fSumWeightPosition += weight;
}

G4double DetectorHit::GetTime() { return fTime / fSumWeightTime; }

G4ThreeVector DetectorHit::GetPosition() { return fPos / fSumWeightPosition; }
