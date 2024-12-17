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
 *  @file RunAction.h
 */

#ifndef RUNACTION_H
#define RUNACTION_H 1

#include "../Core/HistoManager.h"

#include <G4UserRunAction.hh>
#include <globals.hh>
#include "G4Timer.hh"
#include "G4AnalysisManager.hh"

class G4Run;

/**
 * @class RunAction
 * @brief class manages the runs = set of events
 */
class RunAction : public G4UserRunAction
{
public:
  RunAction();
  // explicit RunAction(HistoManager* histo);
  virtual ~RunAction();
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  HistoManager* fHistoManager = nullptr;
  EventMessenger* fEvtMessenger{EventMessenger::GetEventMessenger()};
  G4AnalysisManager* m_anaG4Mngr{G4AnalysisManager::Instance()};
  G4Timer fTimer;
};

#endif /* !RUNACTION_H */
