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
 *  @file JPetGeantEventInformation.cpp
 */

#include "JPetGeantEventInformation.h"
#include <TObjectTable.h>

ClassImp(JPetGeantEventInformation)

JPetGeantEventInformation::JPetGeantEventInformation() :
fVtxPosition(0, 0, 0), fVtxPromptPosition(0, 0, 0), fGenGammaNum(fMaxGammaNumberIndex),
fnRun(0), fLifetime(0), fPromptLifetime(0), fMomentumGamma(4), fCosmicEventTag(false) {
  // NOTE: ROOT has a special class, called TObjectTable, 
  // which optionally keeps track of any object that inherits from TObject.
  // To get rid of ROOT garbage collector issues, we need to remove this object from the table
  //gObjectTable->RemoveQuietly(this);
}

JPetGeantEventInformation::~JPetGeantEventInformation() {}

void JPetGeantEventInformation::Clear(Option_t*)
{
  fGenGammaNum.ResetAllBits();
  fnRun = 0;
  fLifetime = 0.0;
  fPromptLifetime = 0.0;
  fVtxPosition.SetXYZ(0, 0, 0);
  fVtxPromptPosition.SetXYZ(0.0, 0.0, 0.0);
  fMomentumGamma.clear();
  fMomentumGamma.resize(4);
  fCosmicEventTag = false;
}
