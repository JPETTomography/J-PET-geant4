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
 *  @file JPetMC.cpp
 */

#include "Actions/ActionInitialization.h"
#include "Core/DetectorConstruction.h"
#include "Info/EventMessenger.h"
#include "Core/PhysicsList.h"
#include "Core/RunManager.h"

#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>
#include <G4INCLRandom.hh>
#include <G4UImanager.hh>
#include <fstream>
#include <random>

int main (int argc, char** argv)
{
  G4Random::setTheEngine(new CLHEP::MTwistEngine());

  G4UIExecutive* ui = 0;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }

  RunManager* runManager = new RunManager;
  runManager->SetUserInitialization(DetectorConstruction::GetInstance());
  runManager->SetUserInitialization(new PhysicsList);
  runManager->SetUserInitialization(new ActionInitialization);

  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  if (!ui) {
    //! batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    #ifdef JPETMULTITHREADED
    if(argc > 2){
      G4int nCPU = atoi(argv[2]);
      G4cout << "Running on " << nCPU << " CPUs" << G4endl;
      runManager->SetNumberOfThreads(nCPU);
    }
    #endif

    UImanager->ApplyCommand(command + fileName);
  } else {
    //! interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;

  if (EventMessenger::GetEventMessenger()->SaveSeed()) {
    long seed = G4Random::getTheSeed();
    std::ofstream file;
    file.open ("seed", std::ofstream::out | std::ofstream::app);
    file << seed << "\n";
    file.close();
  }
  #ifdef JPETMULTITHREADED
    HistoManager::MergeNTuples(true); // merge ntuples and clean up
  #endif
  return 0;
}
