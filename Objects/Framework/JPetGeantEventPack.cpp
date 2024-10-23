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
 *  @file JPetGeantEventPack.cpp
 */

#include "JPetGeantEventPack.h"
#include <TObjectTable.h>

ClassImp(JPetGeantEventPack)

JPetGeantEventPack::JPetGeantEventPack() : 
  fMCHits("JPetGeantScinHits", 10000),
  // fMCDecayTrees("JPetGeantDecayTree", 1000), 
  fEvtIndex(0), 
  fHitIndex(0)
  //  fMCDecayTreesIndex(0)
{
  fGenInfo = new JPetGeantEventInformation();
  // NOTE: ROOT has a special class, called TObjectTable, 
  // which optionally keeps track of any object that inherits from TObject.
  // To get rid of ROOT garbage collector issues, we need to remove this object from the table
  //gObjectTable->RemoveQuietly(this);
}

// void JPetGeantEventPack::Streamer(TBuffer &R__b) {
//     if (R__b.IsReading()) {
//         TObject::Streamer(R__b);
//         R__b >> fMCHits;
//         R__b >> fGenInfo;
//         R__b >> fEvtIndex;
//         R__b >> fHitIndex;
//     } else {
//         TObject::Streamer(R__b);
//         R__b << fMCHits;
//         R__b << fGenInfo;
//         R__b << fEvtIndex;
//         R__b << fHitIndex;
//     }
// }

JPetGeantScinHits* JPetGeantEventPack::ConstructNextHit()
{
  return dynamic_cast<JPetGeantScinHits*>(fMCHits.ConstructedAt(fHitIndex++));
}

// JPetGeantDecayTree* JPetGeantEventPack::ConstructNextDecayTree()
// {
//   return dynamic_cast<JPetGeantDecayTree*>(fMCDecayTrees.ConstructedAt(fMCDecayTreesIndex++));
// }

JPetGeantEventPack::~JPetGeantEventPack()
{
  fMCHits.Clear("C");
  //fMCDecayTrees.Clear("C");
  fEvtIndex = 0;
  fHitIndex = 0;
  // fMCDecayTreesIndex = 0;
  fGenInfo->Clear();
  delete fGenInfo;
}

void JPetGeantEventPack::Clear(Option_t *)
{
  fMCHits.Clear("C");
  // fMCDecayTrees.Clear("C");
  fHitIndex = 0;
  // fMCDecayTreesIndex = 0;
  fGenInfo->Clear();
}
