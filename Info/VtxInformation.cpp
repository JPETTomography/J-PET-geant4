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
 *  @file VtxInformation.cpp
 */

#include "VtxInformation.h"

VtxInformation::VtxInformation() :
fVtxPosition(0), fTwoGammaGen(false), fThreeGammaGen(false),
fPromptGammaGen(false), fCosmicGen(false), fnRun(0), fLifetime(0), fDensity(0) {}

VtxInformation::~VtxInformation() {}

void VtxInformation::SetVtxPosition(G4ThreeVector position)
{
  fVtxPosition = position;
}

void VtxInformation::SetVtxPosition(G4double x, G4double y, G4double z)
{
  fVtxPosition.setX(x);
  fVtxPosition.setY(y);
  fVtxPosition.setZ(z);
}

void VtxInformation::Clear()
{
  fTwoGammaGen = false;
  fThreeGammaGen = false;
  fPromptGammaGen = false;
  fCosmicGen = false;
  fnRun = 0;
  fLifetime = 0.;
  fDensity = 0.;
  fVtxPosition.setX(0);
  fVtxPosition.setY(0);
  fVtxPosition.setZ(0);
}

void VtxInformation::Print() const {}
