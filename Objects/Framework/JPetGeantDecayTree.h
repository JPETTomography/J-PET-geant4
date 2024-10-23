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
 *  @file JPetGeantDecayTree.h
 */

#ifndef JPETGEANTDECAYTREE_H
#define JPETGEANTDECAYTREE_H 1

#include "JPetGeantDecayTreeBranch.h"
#include <TClonesArray.h>
#include <TObject.h>
#include <vector>
#include <map>

/**
 * @class JPetGeantDecayTree
 * @brief Class stores decay tree structures (in form of nodes and tracks)
 */

enum DecayChannel { 
  kPara2G, kDirect2G, kOrtho2G, kPara3G, kDirect3G, kOrtho3G, jpgUnknown
};

class JPetGeantDecayTree : public TObject
{

public:
  JPetGeantDecayTree();
  ~JPetGeantDecayTree();
  
  void Clear(Option_t *);
  void CopyDecayTree(JPetGeantDecayTree* decayTree);
  void CopyDecayTreeBranch(JPetGeantDecayTreeBranch* branchToCopy);
  
  void SetEventNumber(int eventID) { fEventID = eventID; };
  void SetDecayChannel(DecayChannel decayChannel) { fDecayChannel = decayChannel; };
  void SetNumberOfBranches(int branchesIndex) {fDecayTreeBranchesIndex = branchesIndex; };
  void SetTrackBranchConnections(std::map<int, int> trackBranchConnections) {fTrackBranchConnection = trackBranchConnections; };
  JPetGeantDecayTreeBranch* ConstructNextDecayTreeBranch();
  void AddNodeToBranch(int nodeID, int trackID, InteractionType interactionType);
  
  JPetGeantDecayTreeBranch* GetDecayTreeBranch(int i) {
    return dynamic_cast<JPetGeantDecayTreeBranch*>(fDecayTreeBranches[i]);
  };
  JPetGeantDecayTreeBranch* GetDecayTreeBranchByTrack(unsigned trackID);
  int FindPrimaryPhoton(int nodeID);
  int GetEventNumber() { return fEventID; };
  int GetNumberOfBranches() { return fDecayTreeBranchesIndex; };
  DecayChannel GetDecayChannel() { return fDecayChannel; };
  std::map<int, int> GetTrackBranchConnections() { return fTrackBranchConnection; };

private:
  int fEventID = 0;
  DecayChannel fDecayChannel = DecayChannel::jpgUnknown;
  std::map<int, int> fTrackBranchConnection = {};
  TClonesArray fDecayTreeBranches;
  unsigned int fDecayTreeBranchesIndex = 0;
     
  ClassDef(JPetGeantDecayTree,3)

};

#endif /* !JPETGEANTDECAYTREE_H */
