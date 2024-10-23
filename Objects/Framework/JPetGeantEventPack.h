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
 *  @file JPetGeantEventPack.h
 */

#ifndef JPETGEANTEVENTPACK_H
#define JPETGEANTEVENTPACK_H 1

#include "JPetGeantEventInformation.h"
#include "JPetGeantDecayTree.h"
#include "JPetGeantScinHits.h"
#include <TClonesArray.h>
#include <TVector3.h>
#include <TObject.h>
#include <TBits.h>
#include "TBuffer.h"

/**
 * @class JPetGeantEventPack
 * @brief container that keeps information about single event:
 *  initial parameters (in JPetGeantEventInformation)
 *  detector hits (in JPetGeantScinHits - true information, without detector resolution)
 *  and decay tree (in JPetGeantDecayTree)
 *  Class is directly processed in JPetGeantParser
 */
class JPetGeantEventPack //: public TObject
{
public:
  JPetGeantEventPack();
  ~JPetGeantEventPack();
  void Clear(Option_t* = "");

  JPetGeantScinHits* ConstructNextHit();
  // JPetGeantDecayTree* ConstructNextDecayTree();
  JPetGeantScinHits* GetHit(int i) {
    return dynamic_cast<JPetGeantScinHits*>(fMCHits[i]);
  };
  // JPetGeantDecayTree* GetDecayTree(int i) {
  //   return dynamic_cast<JPetGeantDecayTree*>(fMCDecayTrees[i]);
  // };
  JPetGeantEventInformation* GetEventInformation() { return fGenInfo; };
  unsigned int GetNumberOfHits() { return fHitIndex; };
  // unsigned int GetNumberOfDecayTrees() { return fMCDecayTreesIndex; };
  unsigned int GetEventNumber() { return fEvtIndex; };
  void SetEventNumber(int x) { fEvtIndex = x; };

  // Override Streamer method
  // void Streamer(TBuffer &R__b);

private:
  TClonesArray fMCHits;
  //TClonesArray fMCDecayTrees;
  JPetGeantEventInformation* fGenInfo;
  unsigned int fEvtIndex;
  unsigned int fHitIndex;
  // unsigned int fMCDecayTreesIndex;

  ClassDef(JPetGeantEventPack, 4)
};

#endif /* !JPETGEANTEVENTPACK_H */
