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
 *  @file PrimaryGeneratorAction.cpp
 */

#include "../Core/DetectorConstruction.h"
#include "../Core/DetectorConstants.h"
#include "PrimaryGeneratorAction.h"

#include <G4PrimaryVertex.hh>

PrimaryGeneratorAction::PrimaryGeneratorAction() {}

PrimaryGeneratorAction::PrimaryGeneratorAction(HistoManager* histo) :
G4VUserPrimaryGeneratorAction(), fPrimaryGenerator(0), fHistoManager(histo)
{
  fPrimaryGenerator = new PrimaryGenerator();
  fBeam = new BeamParams();
  fIsotope = new SourceParams();
  fMessenger = new PrimaryGeneratorActionMessenger(this);
  DetectorConstruction::GetInstance()->SetHistoManager(histo);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fIsotope;
  delete fBeam;
  delete fPrimaryGenerator;
  delete fMessenger;
  fNemaGenerator.Clear();
}

void PrimaryGeneratorAction::SetEffectivePositronRadius(G4double radius)
{
  if (radius <= 0) {
    G4Exception(
      "PrimaryGeneratorAction", "PG0Radius", JustWarning,
      "EffectivePositronRadius can not be negative!! Value is not changed"
    );
  } else {
    fEffectivePositronRadius = radius;
  }
}

// cppcheck-suppress unusedFunction
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  //! if setup for dedicated run is set then ignore its modifications made by user
  G4int nRun = DetectorConstruction::GetInstance()->GetRunNumber();
  if (nRun != 0) {
    if (GetSourceTypeInfo() != "run") {
      SetSourceTypeInfo("run");
    }
  }

  if (GetSourceTypeInfo() == ("run")) {
    if (nRun == 3) {
      fPrimaryGenerator->GenerateEvtLargeChamber(event);
    } else if (nRun == 5) {
      fPrimaryGenerator->GenerateEvtSmallChamber(event, fEffectivePositronRadius);
    } else if (nRun == 6 || nRun == 7 || nRun == 12) {
      fPrimaryGenerator->GenerateEvtLargeChamber(event);
    } else {
      G4Exception(
        "PrimaryGeneratorAction", "PG05", FatalException,
        "Called run with non-exisitng geometry (number)"
      );
    }
  } else if (GetSourceTypeInfo() == ("beam")) {
    fPrimaryGenerator->GenerateBeam(fBeam, event);
  } else if (GetSourceTypeInfo() == ("isotope")) {
    fPrimaryGenerator->GenerateIsotope(fIsotope, event);
  } else if (GetSourceTypeInfo() == ("nema")) {
    fPrimaryGenerator->GenerateNema(event, &fNemaGenerator);
  } else if (GetSourceTypeInfo() == ("nema-mixed")) {
    fPrimaryGenerator->GenerateNema(event, &fNemaGenerator);
  } else if (GetSourceTypeInfo() == ("cosmics")) {
    fPrimaryGenerator->GenerateCosmicVertex(fIsotope, event, fHistoManager); 
  } else {
    G4Exception(
      "PrimaryGeneratorAction", "PG05", FatalException,
      "Called non-existing source"
    );
  }
  fHistoManager->SetDecayChannel(fPrimaryGenerator->GetDecayChannel());
}

void PrimaryGeneratorAction::SetSourceTypeInfo(G4String newSourceType)
{
  if (std::find(std::begin(fAllowedSourceTypes), std::end(fAllowedSourceTypes), newSourceType)
                != std::end(fAllowedSourceTypes)) {
    //! setup found
    G4int nRun = DetectorConstruction::GetInstance()->GetRunNumber();
    if ((nRun == 0) && (newSourceType != "run")) {
      fGenerateSourceType = newSourceType;
      if (newSourceType == "nema") {
        GenerateDefaultNemaPositions();
      }
    } else if (nRun > 0) {
      fGenerateSourceType = "run";
    } else {
      G4Exception(
        "PrimaryGeneratorAction", "PG01", JustWarning,
        "Chosen detector geometry corresponds to run number and it can not be changed"
      );
    }
  } else {
    G4Exception(
      "PrimaryGeneratorAction", "PG02", JustWarning,
      "Please pick from avaliable setups: beam/isotope"
    );
  }
}

void PrimaryGeneratorAction::GenerateDefaultNemaPositions()
{
  for (G4int nemaPoint=1; nemaPoint<7; nemaPoint++) {
    fNemaGenerator.AddPoint(nemaPoint);
    fNemaGenerator.AddPointWeight(nemaPoint, 1);
    
    G4double x_creation = 0.0 * cm;
    G4double y_creation = 0.0 * cm;
    G4double z_creation = 0.0 * cm;
    
    /* Nema predefined points definition (X = 0)
     * according to standard NEMA-NU-2-2012
     * DOI: 10.12693/APhysPolA.132.1645
     * 
     *      -3/8      0
     *    ---------------> Z
     *   Y
     *   |    4'      1'
     * 1 |    o       o
     *   |
     *   |    5'      2'
     * 10|    o       o
     *   |
     *   |    6'      3'
     * 20|    o       o
     *   V 
     */
    
    if (nemaPoint > 3) {
      z_creation = z_creation - DetectorConstants::scinDim[2] * 3 / 8;
    }

    if (nemaPoint == 1 || nemaPoint == 4) {
      y_creation = y_creation + 1.0 * cm;
    } else if (nemaPoint == 2 || nemaPoint == 5) {
      y_creation = y_creation + 10.0 * cm;
    } else if (nemaPoint == 3 || nemaPoint == 6) {
      y_creation = y_creation + 20.0 * cm;
    }
    
    fNemaGenerator.SetPointPosition(nemaPoint, G4ThreeVector(x_creation, y_creation, z_creation));
    fNemaGenerator.SetPointSize(nemaPoint, G4ThreeVector(0.1 * mm, 0.1 * mm, 0.1 * mm));
  }
}

void PrimaryGeneratorAction::SetNemaPointPosition(G4int nemaPoint, const G4ThreeVector& position)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG06", JustWarning,
      "Nema point for which you want to set position is less than 1. Cannot set it now properly."
    );
  } else if (fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    fNemaGenerator.SetPointPosition(nemaPoint, position);
  } else {
    fNemaGenerator.AddPoint(nemaPoint);
    fNemaGenerator.SetPointPosition(nemaPoint, position);
  }
}

void PrimaryGeneratorAction::SetNemaPositionWeight(G4int nemaPoint, G4int weight)
{
  fNemaGenerator.AddPointWeight(nemaPoint, weight);
}

void PrimaryGeneratorAction::SetNemaPointLifetime(G4int nemaPoint, PositroniumDecayMode mode, G4double lifetime)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG07", JustWarning,
      "Nema point for which you want to set lifetime is less than 1. Cannot set it now properly."
    );
  } else if (!fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    fNemaGenerator.AddPoint(nemaPoint);
  }
  if (mode == PositroniumDecayMode::pPs) {
    fNemaGenerator.SetPointPPsLifetime(nemaPoint, lifetime);
  } else if (mode == PositroniumDecayMode::oPs) {
    fNemaGenerator.SetPointOPsLifetime(nemaPoint, lifetime);
  } else if (mode == PositroniumDecayMode::direct) {
    fNemaGenerator.SetPointDirectLifetime(nemaPoint, lifetime);
  } else {
    G4Exception(
      "PrimaryGeneratorAction", "PG07", JustWarning,
      "Not implemented mode for positronium decay"
    );
  }
}

void PrimaryGeneratorAction::SetNemaPointGenerationOption(G4int nemaPoint, NemaGenerationOption which, bool option)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG08", JustWarning,
      "Nema point for which you want to set generation option is less than 1. Cannot set it now properly."
    );
    return;
  } else if (!fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    fNemaGenerator.AddPoint(nemaPoint);
  }
  if (which == NemaGenerationOption::n3G) {
    fNemaGenerator.SetPoint3GOption(nemaPoint, option);
  } else if (which == NemaGenerationOption::nPPs) {
    fNemaGenerator.SetPointParaPSOption(nemaPoint, option);
  } else if (which == NemaGenerationOption::nDirect) {
    fNemaGenerator.SetPointDirectAnniOption(nemaPoint, option);
  } else if (which == NemaGenerationOption::nPrompt) {
    fNemaGenerator.SetPointPromptOption(nemaPoint, option);
  } else if (which == NemaGenerationOption::nDirectLFDensDep) {
    fNemaGenerator.SetNemaPointDirectLFDependence(nemaPoint, option);
  } else {
    G4Exception(
      "PrimaryGeneratorAction", "PG10", JustWarning,
      "Not implemented option for nema point generation"
    );
  }
}

void PrimaryGeneratorAction::SetNemaPointSize(G4int nemaPoint, G4double x, G4double y, G4double z)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG09", JustWarning,
      "Nema point for which you want to set size is less than 1. Cannot set it now properly."
    );
  } else if (fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    fNemaGenerator.SetPointSize(nemaPoint, G4ThreeVector(x, y, z));
  } else {
    fNemaGenerator.AddPoint(nemaPoint);
    fNemaGenerator.SetPointSize(nemaPoint, G4ThreeVector(x, y, z));
  }
}

void PrimaryGeneratorAction::SetNemaPointPromptSize(G4int nemaPoint, G4double x, G4double y, G4double z)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG10", JustWarning,
      "Nema point for which you want to set prompt generation size is less than 1. Cannot set it now properly."
    );
  } else if (fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    fNemaGenerator.SetPointPromptSize(nemaPoint, G4ThreeVector(x, y, z));
  } else {
    fNemaGenerator.AddPoint(nemaPoint);
    fNemaGenerator.SetPointPromptSize(nemaPoint, G4ThreeVector(x, y, z));
  }
}

void PrimaryGeneratorAction::SetNemaPointOrientation(G4int nemaPoint, G4double theta, G4double phi)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG11", JustWarning,
      "Nema point for which you want to set size is less than 1. Cannot set it now properly."
    );
  } else if (fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    fNemaGenerator.SetPointOrientation(nemaPoint, G4ThreeVector(theta, phi, 0));
  } else {
    fNemaGenerator.AddPoint(nemaPoint);
    fNemaGenerator.SetPointOrientation(nemaPoint, G4ThreeVector(theta, phi, 0));
  }
}

void PrimaryGeneratorAction::SetNemaPointShape(G4int nemaPoint, Dimension dim, G4double direction, G4double power, G4double length)
{
  G4double lengthN = length;
  if (fabs(length) > 1)
    lengthN = lengthN/fabs(lengthN);
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG12", JustWarning,
      "Nema point for which you want to set shape is less than 1. Cannot set it now properly."
    );
  } else if (fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    if (dim == Dimension::dimY) {
      fNemaGenerator.SetNemaPointShapeInY(nemaPoint, G4ThreeVector(direction, power, lengthN));
      fNemaGenerator.GenerateElipseYNorm(nemaPoint);
    }
  } else {
    fNemaGenerator.AddPoint(nemaPoint);
    if (dim == Dimension::dimY) {
      fNemaGenerator.SetNemaPointShapeInY(nemaPoint, G4ThreeVector(direction, power, lengthN));
      fNemaGenerator.GenerateElipseYNorm(nemaPoint);
    }
  }
}

void PrimaryGeneratorAction::SetPointPositronReachShape(G4int nemaPoint, G4String shape)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG13", JustWarning,
      "Nema point for which you want to set reach shape is less than 1. Cannot set it now properly."
    );
  } else {
    fNemaGenerator.AddPoint(nemaPoint);
  }

  if (shape == "sphere" || shape == "Sphere")
    fNemaGenerator.SetPointReachShape(nemaPoint, PositronReachOption::aFixedUniform);
  else if (shape == "exponential" || shape == "Exponential" || shape == "expo" || shape == "Expo")
    fNemaGenerator.SetPointReachShape(nemaPoint, PositronReachOption::aFixedExponential);
  else if (shape == "density" || shape == "Density" || shape == "dens" || shape == "Dens")
    fNemaGenerator.SetPointReachShape(nemaPoint, PositronReachOption::aDensityDep);
  else
    fNemaGenerator.SetPointReachShape(nemaPoint, PositronReachOption::aNo);
}

void PrimaryGeneratorAction::SetPointExperimentalReach(G4int nemaPoint, G4double reach)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG14", JustWarning,
      "Nema point for which you want to set reach is less than 1. Cannot set it now properly."
    );
  } else if (fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    fNemaGenerator.SetPointExperimentalReach(nemaPoint, reach);
  } else {
    fNemaGenerator.AddPoint(nemaPoint);
    fNemaGenerator.SetPointExperimentalReach(nemaPoint, reach);
  }
}

void PrimaryGeneratorAction::SetPointShape(G4int nemaPoint, G4String shape)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG15", JustWarning,
      "Nema point for which you want to set shape is less than 1. Cannot set it now properly."
    );
  } else if (!fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    fNemaGenerator.AddPoint(nemaPoint);
  }

  if (shape == "cylinder" || shape == "Cylinder")
    fNemaGenerator.SetPointShape(nemaPoint, PointShape::aCylinder);
  else if (shape == "ball" || shape == "Ball")
    fNemaGenerator.SetPointShape(nemaPoint, PointShape::aBall);
  else
    fNemaGenerator.SetPointShape(nemaPoint, PointShape::aPhantom);
}

void PrimaryGeneratorAction::SetNemaPointIsotopeType(G4int nemaPoint, G4String type)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG16", JustWarning,
      "Nema point for which you want to set isotope is less than 1. Cannot set it now properly."
    );
  } else if (!fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    fNemaGenerator.AddPoint(nemaPoint);
  }

  if (type == "22Na" || type == "Na" || type == "sodium" || type == "Sodium" || type == "sodium-22" || type == "Sodium-22")
    fNemaGenerator.SetIsotope(nemaPoint, IsotopeType::i22Na);
  else
    fNemaGenerator.SetIsotope(nemaPoint, IsotopeType::i44Sc);
}

void PrimaryGeneratorAction::SetPhantomElementIDForNemaPoint(G4int nemaPoint, G4int phantomElement)
{
  if (nemaPoint < 1) {
    G4Exception(
      "PrimaryGeneratorAction", "PG17", JustWarning,
      "Nema point for which you want to set phantom element is less than 1. Cannot set it now properly."
    );
  } else if (!fNemaGenerator.DoesPointExistAlready(nemaPoint)) {
    fNemaGenerator.AddPoint(nemaPoint);
    fNemaGenerator.SetPhantElementID(nemaPoint, phantomElement);
  } else {
    fNemaGenerator.SetPhantElementID(nemaPoint, phantomElement);
  }
}
