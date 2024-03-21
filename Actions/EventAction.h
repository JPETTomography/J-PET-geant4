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
 *  @file EventAction.h
 */

#ifndef EVENTACTION_H
#define EVENTACTION_H 1

#include "JPetGeantDecayTree.h"
#include "JPetGeantScinHits.h"
#include "EventMessenger.h"
#include "HistoManager.h"

#include <G4UserEventAction.hh>
#include <globals.hh>
#include <memory>

/**
 * @class EventAction
 * @brief class managing events
 *
 * Here also one can reach for information
 * after propagation of all tracks in detector.
 */
class EventAction : public G4UserEventAction
{
public:
  EventAction();
  explicit EventAction(HistoManager* histo);
  virtual ~EventAction();
  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event* anEvent);
  bool Is2gRegistered();
  bool Is3gRegistered();

private:
  HistoManager* fHistoManager = nullptr;
  G4int fScinCollID;
  EventMessenger* fEvtMessenger = EventMessenger::GetEventMessenger();
  void WriteToFile(const G4Event* anEvent);

  bool is2gRec;
  bool is3gRec;
  int fEventID;
  void CheckIf3gIsRegistered(const G4Event* anEvent);
  void CheckIf2gIsRegistered(const G4Event* anEvent);

};

#endif /* !EVENTACTION_H */
