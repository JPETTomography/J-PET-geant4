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
 *  @file JPetGeantDecayTreeBranch.cpp
 */

#include "JPetGeantDecayTreeBranch.h"
#include <TObjectTable.h>

ClassImp(JPetGeantDecayTreeBranch)

JPetGeantDecayTreeBranch::JPetGeantDecayTreeBranch()
{
  gObjectTable->RemoveQuietly(this);
}

JPetGeantDecayTreeBranch::~JPetGeantDecayTreeBranch()
{
  fNodeIDs.clear();
  fInteractionType.clear();
}

void JPetGeantDecayTreeBranch::Clear(Option_t *)
{
  fNodeIDs.clear();
  fInteractionType.clear();
}

void JPetGeantDecayTreeBranch::AddNodeID(int nodeID, InteractionType interactionType)
{
  fNodeIDs.push_back(nodeID);
  fInteractionType.push_back(interactionType);
}

int JPetGeantDecayTreeBranch::GetPreviousNodeID(int nodeID) const
{
  if (fNodeIDs.size() > 1) {
    for (unsigned i=fNodeIDs.size(); i>1; i--) {
      if (fNodeIDs[i-1] == nodeID)
        return fNodeIDs[i-2];
    }
  }
  return fNodeIDs[0];
}

InteractionType JPetGeantDecayTreeBranch::GetInteractionType(int nodeID) const
{
  for (unsigned i=fNodeIDs.size(); i>0; i--)
  {
    if (fNodeIDs[i-1] == nodeID)
      return fInteractionType[i-1];
  }
  return kUnknownInteractionType;
}
