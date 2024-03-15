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
 *  @file RunAction.cpp
 */

#include "RunAction.h"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <Randomize.hh>
#include <TRandom3.h>
#include <G4Run.hh>
#include <chrono>

RunAction::RunAction() {}

RunAction::RunAction(HistoManager* histo) : G4UserRunAction(), fHistoManager(histo) {}

RunAction::~RunAction() {}

// cppcheck-suppress unusedFunction
void RunAction::BeginOfRunAction(const G4Run*)
{
  // TODO MT fHistoManager->Book();

  int mask = 01001010;

  if (fEvtMessenger->GetSeed() == 0) {
    /**
     * If seed 0 is used we are seeding random generator with a number which is a
     * combination of current time and process PID. Bu doing so seed is unique in
     * time and space.
     * Number 667 (123th prime number) is chosen to secure that different processes
     * run at the same time don't have seeds that are close to each other - otherwise
     * it may cause random nummber sequences to be also close to each other.
     *
     */
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    long seed = (UInt_t)(system_clock::to_time_t(now)) * 677 * ::getpid();
    G4Random::setTheSeed(seed);
    gRandom->SetSeed(seed ^ mask);
  } else {
    gRandom->SetSeed(fEvtMessenger->GetSeed());
    G4Random::setTheSeed(fEvtMessenger->GetSeed() ^ mask);
  }
}

// cppcheck-suppress unusedFunction
void RunAction::EndOfRunAction(const G4Run*) { 
  // TODO MT fHistoManager->Save();
  }
