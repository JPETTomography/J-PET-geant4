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
 *  @file RunManager.h
 */

#ifndef RUNMANAGER_H
#define RUNMANAGER_H 1

#include "../Info/EventMessenger.h"
#include <G4RunManager.hh>

#ifdef JPETMULTITHREADED
  #include "G4Threading.hh"
  #include "G4MTRunManager.hh"
  class RunManager : public G4MTRunManager
#else
  class RunManager : public G4RunManager
#endif
{
public:
  RunManager();
  virtual ~RunManager(){}
  void DoEventLoop(G4int n_event, const char* macroFile = 0, G4int n_select = -1) override;

private:
  EventMessenger* fEvtMessenger = EventMessenger::GetEventMessenger();
};

#endif /* !RUNMANAGER_H */
