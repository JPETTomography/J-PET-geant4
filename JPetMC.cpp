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
#include "Actions/EventAction.h"
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
#include <cxxopts.hpp>

int main (int argc, char** argv)
{
  auto ERROR_AND_EXIT_FAILURE = [](const std::string& message) {
    G4cout << "[ERROR]:: " << message << G4endl;
    std::exit(EXIT_FAILURE);
  };

  if (argc < 2)
    ERROR_AND_EXIT_FAILURE("Command line options missing (use '"+std::string(argv[0])+" --help' if needed)");
  
  G4Random::setTheEngine(new CLHEP::MTwistEngine());
  cxxopts::Options options("JPetMC", "J-PET Monte Carlo Simulation");
  options.add_options()
  ("h,help", "Print usage")
  ("d,debug", "Enable debugging") // a bool parameter
  ("v,vis", "Vis macro to execute", cxxopts::value<std::string>())
  ("j,job", "Job macro to execute", cxxopts::value<std::string>())
  ("n,name", "Job name, the output file name (default 'mcGeant')", cxxopts::value<std::string>())
  ("o,output", "Output path (default value is bin/)", cxxopts::value<std::string>())
  ("t,nThreads", "Number of threads to execute on", cxxopts::value<int>()->default_value("1"))
  ("m,evtMult", "Event multiplicity (default -1, no cut)", cxxopts::value<int>()->default_value("-1"))
  ;
  options.allow_unrecognised_options();
  auto cmdLineArgs = options.parse(argc, argv);

  if (cmdLineArgs.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }

  G4UIExecutive* ui = nullptr;
  if (cmdLineArgs.count("v")) {
    // interactive mode
    ERROR_AND_EXIT_FAILURE("Command line options not supported in interactive mode, yet");
    // TODO: figure it out what to pass here!
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
    if (cmdLineArgs.count("j")) {
      G4String fileName  = cmdLineArgs["j"].as<std::string>();
      #ifdef JPETMULTITHREADED
      if(argc > 2){
        G4int nCPU = cmdLineArgs["t"].as<int>();
        G4cout << "Running on " << nCPU << " CPUs" << G4endl;
        runManager->SetNumberOfThreads(nCPU);
      }
      #endif
      if (cmdLineArgs.count("m"))
        EventAction::EvtMultCut = cmdLineArgs["m"].as<int>();
      if (cmdLineArgs.count("n"))
        HistoManager::OuputFileName = cmdLineArgs["n"].as<std::string>();
      if (cmdLineArgs.count("o"))
        HistoManager::OuputDir = cmdLineArgs["o"].as<std::string>();

      UImanager->ApplyCommand(command + fileName);
    } else {
      ERROR_AND_EXIT_FAILURE("Job macro not provided, use '--help' to see usage");
    }
  } else {
    // interactive mode
    if (cmdLineArgs.count("v")){
      auto fileName = cmdLineArgs["v"].as<std::string>();
      UImanager->ApplyCommand(G4String("/control/execute ") + fileName);
      ui->SessionStart();
      delete ui;
    } else {
      ERROR_AND_EXIT_FAILURE("Job macro not provided, use '--help' to see usage");
    }
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
