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
 *  @file JPetGeantDecayTreeBranch.h
 */

#ifndef JPETGEANTDECAYTREEBRANCH_H
#define JPETGEANTDECAYTREEBRANCH_H 1

#include <TObject.h>
#include <vector>

/**
 * @class JPetGeantDecayTreeBranch
 * @brief Class stores decay tree structures (in form of nodes and tracks)
 */

enum InteractionType
{
  kPrimaryGamma,
  kScattActivePart,
  kScattNonActivePart,
  kSecondaryPart,
  kUnknownInteractionType
};

class JPetGeantDecayTreeBranch : public TObject 
{
    
public:
  JPetGeantDecayTreeBranch();
  ~JPetGeantDecayTreeBranch();
  
  void Clear(Option_t *);
  
  void AddNodeID(int nodeID, InteractionType interactionType);
  void SetTrackID(int trackID) { fTrackID = trackID; };
  void SetPrimaryBranchID(int primarybranchID) { fPrimaryBranchID = primarybranchID; };
  int GetTrackID() const { return fTrackID; };
  int GetPrimaryBranchID() const { return fPrimaryBranchID; };
  int GetNumberOfNodes() const { return fNodeIDs.size(); };
  int GetPrimaryNodeID() const { if (fNodeIDs.size()) return fNodeIDs[0]; else return -1; };
  int GetLastNodeID() const { if (fNodeIDs.size()) return fNodeIDs[fNodeIDs.size()-1]; else return -1; };
  int GetPreviousNodeID(int nodeID) const;
  InteractionType GetInteractionType(int nodeID) const;
  
private:
  int fTrackID = -1;             //ID of the track corresponding to this branch
  int fPrimaryBranchID = -1;       //-1 for branch coming from primary photon, primary branchId otherwise
  std::vector<int> fNodeIDs = {};    //container for all of the nodes
  std::vector<InteractionType> fInteractionType = {};
  
  ClassDef(JPetGeantDecayTreeBranch,1)
    
};

#endif /* !JPETGEANTDECAYTREEBRANCH_H */
