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
 *  @file JPetGeantDecayTree.cpp
 */

#include "JPetGeantDecayTree.h"
#include <TObjectTable.h>

ClassImp(JPetGeantDecayTree)

JPetGeantDecayTree::JPetGeantDecayTree()  : fDecayTreeBranches("JPetGeantDecayTreeBranch", 1000)
{
  gObjectTable->RemoveQuietly(this);
}

JPetGeantDecayTree::~JPetGeantDecayTree()
{
  fEventID = 0;
  fDecayTreeBranchesIndex = 0;
  fDecayChannel = DecayChannel::kUnknown;
  fDecayTreeBranches.Clear("C");
  fTrackBranchConnection.clear();
}

void JPetGeantDecayTree::Clear(Option_t *)
{
  fEventID = 0;
  fDecayTreeBranchesIndex = 0;
  fDecayChannel = DecayChannel::kUnknown;
  fDecayTreeBranches.Clear("C");
  fTrackBranchConnection.clear();
}

void JPetGeantDecayTree::CopyDecayTree(JPetGeantDecayTree* decayTree)
{
  this->SetEventNumber(decayTree->GetEventNumber());
  this->SetDecayChannel(decayTree->GetDecayChannel());
  this->SetTrackBranchConnections(decayTree->GetTrackBranchConnections());
  for (auto i=0; i<decayTree->GetNumberOfBranches(); i++) {
    this->CopyDecayTreeBranch(decayTree->GetDecayTreeBranch(i));
  }
}

void JPetGeantDecayTree::CopyDecayTreeBranch(JPetGeantDecayTreeBranch* branchToCopy)
{
  JPetGeantDecayTreeBranch* newBranch = this->ConstructNextDecayTreeBranch();
  newBranch->SetTrackID(branchToCopy->GetTrackID());
  newBranch->SetPrimaryBranchID(branchToCopy->GetPrimaryBranchID());
  int primaryNodeID = branchToCopy->GetPrimaryNodeID();
  InteractionType primaryInterType = branchToCopy->GetInteractionType(primaryNodeID);
  newBranch->AddNodeID(primaryNodeID, primaryInterType);
  int lastNodeID = branchToCopy->GetLastNodeID();
  while (newBranch->GetNumberOfNodes() < branchToCopy->GetNumberOfNodes()) {
    int currentNodeID = lastNodeID;
    while (branchToCopy->GetPreviousNodeID(currentNodeID) > primaryNodeID) {
      currentNodeID = branchToCopy->GetPreviousNodeID(currentNodeID);
    }
    newBranch->AddNodeID(currentNodeID, branchToCopy->GetInteractionType(currentNodeID));
    primaryNodeID = currentNodeID;
  }
}

JPetGeantDecayTreeBranch* JPetGeantDecayTree::ConstructNextDecayTreeBranch()
{
  return dynamic_cast<JPetGeantDecayTreeBranch*>(fDecayTreeBranches.ConstructedAt(fDecayTreeBranchesIndex++));
}

void JPetGeantDecayTree::AddNodeToBranch(int nodeID, int trackID, InteractionType interactionType)
{
  auto search = fTrackBranchConnection.find(trackID);
  if (search == fTrackBranchConnection.end()) {
    int branchSize = GetNumberOfBranches();
    int primaryBranchID = -1;
    if (interactionType == kSecondaryPart)
      primaryBranchID = FindPrimaryPhoton(nodeID/10);
    JPetGeantDecayTreeBranch* newBranch = this->ConstructNextDecayTreeBranch();
    newBranch->SetTrackID(trackID);
    newBranch->SetPrimaryBranchID(primaryBranchID);
    newBranch->AddNodeID(nodeID, interactionType);
    fTrackBranchConnection.insert(std::make_pair(trackID, branchSize));
  } else {
    int branchID = fTrackBranchConnection.at(trackID);
    GetDecayTreeBranch(branchID)->AddNodeID(nodeID, interactionType);
  }
}

int JPetGeantDecayTree::FindPrimaryPhoton(int nodeID)
{
  for (unsigned i=GetNumberOfBranches(); i>0; i--) {
    if (GetDecayTreeBranch(i-1)->GetLastNodeID() == nodeID)
      return i-1;
  }
  return -1;
}

// cppcheck-suppress unusedFunction
JPetGeantDecayTreeBranch* JPetGeantDecayTree::GetDecayTreeBranchByTrack(unsigned trackID)
{
  auto search = fTrackBranchConnection.find(trackID);
  if (search != fTrackBranchConnection.end()) {
    int branchID = fTrackBranchConnection.at(trackID);
    return GetDecayTreeBranch(branchID);
  } else
    return GetDecayTreeBranch(fDecayTreeBranchesIndex);
}
