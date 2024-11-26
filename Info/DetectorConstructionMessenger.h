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
 *  @file DetectorConstructionMessenger.h
 */

#ifndef DETECTORCONSTRUCTIONMESSENGER_H
#define DETECTORCONSTRUCTIONMESSENGER_H 1

#include "../Core/DetectorConstruction.h"

#include <G4SystemOfUnits.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIdirectory.hh>
#include <G4UImessenger.hh>
#include <globals.hh>

class DetectorConstructionMessenger : public G4UImessenger
{
public:
  DetectorConstructionMessenger();
  explicit DetectorConstructionMessenger(DetectorConstruction* detector);
  ~DetectorConstructionMessenger();
  void SetNewValue(G4UIcommand*, G4String);
  const int kDefaultRunNumber = 5;

private:
  DetectorConstruction* fDetector = nullptr;
  G4UIdirectory* fDirectory = nullptr;
  G4UIcmdWithAnInteger* fLoadTargetForRun = nullptr;
  G4UIcmdWithAnInteger* fLoadIdealGeometry = nullptr;
  G4UIcmdWithoutParameter* fLoadJPetBasicGeometry = nullptr;
  G4UIcmdWithoutParameter* fLoadOnlyScintillators = nullptr;
  G4UIcmdWithABool* fLoadWrapping = nullptr;
  //! Null pointer assigned to the modular layer -
  G4UIcmdWithAString* fLoadModularLayer = nullptr;
  G4UIcmdWithoutParameter* fLoadModularLayerOnly = nullptr;
  G4UIcmdWithADoubleAndUnit* fScinHitMergingTime = nullptr;
  G4UIcmdWithAString* fGeometryFileName = nullptr;
  G4UIcmdWithAString* fCreateGeometryType = nullptr;
  G4UIcmdWithAString* fJSONSetupFile = nullptr;
  G4UIcmdWithAnInteger* fJSONSetupRunNum = nullptr;
  G4UIcmdWithADoubleAndUnit* fPressureInChamber = nullptr;
  G4UIcmdWithoutParameter* fConstructNemaPhantom = nullptr;
  G4UIcmdWithAString* fAddMaterialElement = nullptr;
  G4UIcmdWithAString* fAddMaterialIsotopeElement = nullptr;
  G4UIcmdWithAnInteger* fAddCustomMaterialWithID = nullptr;
  G4UIcmdWithAString* fAddElementToCustomMaterial = nullptr;
  G4UIcmdWithAString* fAddPhantomElementWithShape = nullptr;
  G4UIcmdWithAnInteger* fConstructPhantomElement = nullptr;
  G4UIcmdWithAString* fSetPhantomElementDimensions = nullptr;
  G4UIcmdWithAString* fSetPhantomElementLocation = nullptr;
  G4UIcmdWithAString* fSetPhantomElementRotation = nullptr;
  G4UIcmdWithAString* fSetPhantomElementAction = nullptr;
  G4UIcmdWithAString* fSetPhantomElementMaterialAndDensity = nullptr;
};

#endif /* !DETECTORCONSTRUCTIONMESSENGER_H */
