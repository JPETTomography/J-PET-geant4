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
 *  @file PrimaryGeneratorActionMessenger.cpp
 */

#include "../Actions/PrimaryGeneratorAction.h"
#include "PrimaryGeneratorActionMessenger.h"
#include "../Core/DetectorConstruction.h"
#include "../Core/DetectorConstants.h"

PrimaryGeneratorActionMessenger::PrimaryGeneratorActionMessenger() {}

PrimaryGeneratorActionMessenger::PrimaryGeneratorActionMessenger(PrimaryGeneratorAction* primGeneratorAction) : fPrimGen(primGeneratorAction)
{
  fDirectory = new G4UIdirectory("/jpetmc/source/");
  fDirectory->SetGuidance("Commands for controling the gamma quanta source (beam/target/nema) and its parameters");

  fDirectoryRun = new G4UIdirectory("/jpetmc/run/");
  fDirectoryRun->SetGuidance("Commands for controling  parameters");

  fSourceType = new G4UIcmdWithAString("/jpetmc/source/setType", this);
  fSourceType->SetCandidates("beam isotope nema");
  fSourceType->SetDefaultValue("beam");

  fGammaBeamSetEnergy = new G4UIcmdWithADoubleAndUnit("/jpetmc/source/gammaBeam/setEnergy", this);
  fGammaBeamSetEnergy->SetGuidance("Set energy (value and unit) for beam of gamma quanta");
  fGammaBeamSetEnergy->SetDefaultValue(511.0);
  fGammaBeamSetEnergy->SetDefaultUnit("keV");
  fGammaBeamSetEnergy->SetUnitCandidates("keV");

  fGammaBeamSetPosition = new G4UIcmdWith3VectorAndUnit("/jpetmc/source/gammaBeam/setPosition", this);
  fGammaBeamSetPosition->SetGuidance("Set vertex position of the gamma quanta beam");
  fGammaBeamSetPosition->SetDefaultValue(G4ThreeVector(0, 0, 0));
  fGammaBeamSetPosition->SetDefaultUnit("cm");
  fGammaBeamSetPosition->SetUnitCandidates("cm");
  fGammaBeamSetPosition->SetParameterName("Xvalue", "Yvalue", "Zvalue", false);

  fGammaBeamSetMomentum = new G4UIcmdWith3VectorAndUnit("/jpetmc/source/gammaBeam/setMomentum", this);
  fGammaBeamSetMomentum->SetGuidance("Set momentum of the gamma quanta beam");
  fGammaBeamSetMomentum->SetDefaultValue(G4ThreeVector(1, 0, 0));
  fGammaBeamSetMomentum->SetDefaultUnit("keV");
  fGammaBeamSetMomentum->SetUnitCandidates("keV");
  fGammaBeamSetMomentum->SetParameterName("Xvalue", "Yvalue", "Zvalue", false);

  fIsotopeSetShape = new G4UIcmdWithAString("/jpetmc/source/isotope/setShape", this);
  fIsotopeSetShape->SetCandidates("cylinder");

  fIsotopeSetGenGammas = new G4UIcmdWithAnInteger("/jpetmc/source/isotope/setNGamma", this);
  fIsotopeSetGenGammas->SetGuidance("Give number of gamma quanta to generate 1 / 2 / 3");

  fIsotopeSetShapeDimCylinderRadius = new G4UIcmdWithADoubleAndUnit("/jpetmc/source/isotope/setShape/cylinderRadius", this);
  fIsotopeSetShapeDimCylinderRadius->SetGuidance("For cylindrical shape - set radius");
  fIsotopeSetShapeDimCylinderRadius->SetDefaultValue(10);
  fIsotopeSetShapeDimCylinderRadius->SetDefaultUnit("cm");
  fIsotopeSetShapeDimCylinderRadius->SetUnitCandidates("cm");

  fIsotopeSetShapeDimCylinderZ = new G4UIcmdWithADoubleAndUnit("/jpetmc/source/isotope/setShape/cylinderZ", this);
  fIsotopeSetShapeDimCylinderZ->SetGuidance("For cylindrical shape - set z (half length)");
  fIsotopeSetShapeDimCylinderZ->SetDefaultValue(10);
  fIsotopeSetShapeDimCylinderZ->SetDefaultUnit("cm");
  fIsotopeSetShapeDimCylinderZ->SetUnitCandidates("cm");

  fIsotopeSetCenter = new G4UIcmdWith3VectorAndUnit("/jpetmc/source/isotope/setPosition", this);
  fIsotopeSetCenter->SetGuidance("Set position of the source");
  fIsotopeSetCenter->SetDefaultValue(G4ThreeVector(0, 0, 0));
  fIsotopeSetCenter->SetParameterName("Xvalue", "Yvalue", "Zvalue", false);
  fIsotopeSetCenter->SetDefaultUnit("cm");
  fIsotopeSetCenter->SetUnitCandidates("cm");

  fNemaPosition = new G4UIcmdWithAnInteger("/jpetmc/source/nema", this);
  fNemaPosition->SetGuidance("Give nema point number to simulate (1-6)");
  fNemaPosition->SetDefaultValue(1);
  
  fNemaMixed = new G4UIcmdWithoutParameter("/jpetmc/source/nema/mixed", this);
  fNemaMixed->SetGuidance("Generate nema with all sources at once");
  
  fNemaSetPosition = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setPosition", this);
  fNemaSetPosition->SetGuidance("Setting the position of a given point in nema generation (int - point, 3Dvector - x, y, z [cm3]). Use it when do you not want default positions of the nema points");
  
  fNemaSetShape = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setShape", this);
  fNemaSetShape->SetGuidance("Setting the shape of a given point in nema generation (int - point, string - shape [cylinder, ball, phantom])");

  fNemaSetPositionWeight = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setWeight", this);
  fNemaSetPositionWeight->SetGuidance("Setting the weight of a given point in nema generation (int - point, int - weight), if 0 it removes a point from generation");
  
  fNemaSetPositionOPSLifetime = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setOPsLifetime", this);
  fNemaSetPositionOPSLifetime->SetGuidance("Setting the mean ortho-Ps lifetime of a given point in nema generation (int - point, double - lifetime), lifetime should be positive, instead generated with default value 0.3 ns");
  
  fNemaSetPositionPPSLifetime = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setPPsLifetime", this);
  fNemaSetPositionPPSLifetime->SetGuidance("Setting the mean para-Ps lifetime of a given point in nema generation (int - point, double - lifetime), lifetime should be positive, instead generated with default value 0.125 ns");

  fNemaSetPositionDirectLifetime = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setDirectLifetime", this);
  fNemaSetPositionDirectLifetime->SetGuidance("Setting the mean direct annihilation lifetime of a given point in nema generation (int - point, double - lifetime), lifetime should be positive, instead generated with default value 0.4 ns");

  fNemaSetPosition3GOption = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/allow3G", this);
  fNemaSetPosition3GOption->SetGuidance("Setting if the position of a given point in nema generation should have possibility to decay into three photons");

  fNemaSetPositionPPSOption = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/allowPPs", this);
  fNemaSetPositionPPSOption->SetGuidance("Setting if the position of a given point in nema generation should have possibility to decay by means of para-Ps");

  fNemaSetPositionDirectOption = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/allowDirect", this);
  fNemaSetPositionDirectOption->SetGuidance("Setting if the position of a given point in nema generation should have possibility to decay directly");
  
  fNemaSetPositionSize = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setPointSize", this);
  fNemaSetPositionSize->SetGuidance("Setting the size of the point in which annihilation position is simulated (int - point, double - x/radius [cm], double - y/length [cm], double - z [cm])");
  
  fNemaSetPositionPromptOption = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/allowPrompt", this);
  fNemaSetPositionPromptOption->SetGuidance("Setting if the position of a given point in nema generation should have possibility to emit prompt photon");
  
  fNemaSetPositionPromptSize = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setPromptSourceSize", this);
  fNemaSetPositionPromptSize->SetGuidance("Setting the size of the point in which prompt photon emission position is simulated (int - point, double - x/radius [cm], double - y/length [cm], double - z [cm])");
  
  fNemaSetPositionCylinderRotation = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setCylinderRotation", this);
  fNemaSetPositionCylinderRotation->SetGuidance("Setting the orientation angles of the cylinder in which annihilation position is simulated (int - point, double - theta [deg], double - phi [deg])");
  
  fNemaSetPositionCylinderShapeY = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setCylinderShapeParametersY", this);
  fNemaSetPositionCylinderShapeY->SetGuidance("Setting the shape of the cylinder in Y directory in which annihilation position is simulated (int - point, double - direction of the change, double - power of the shape change, double (0,1) - cut-off of the shape)");
  
  fNemaSetPositionPositronReachShape = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setEffectivePositronRangeShape", this);
  fNemaSetPositionPositronReachShape->SetGuidance("Set effective positron radius shape for given nema point (int, string - sphere, expo, dens or no)");
  
  fNemaSetPositionPositronReach = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setEffectivePositronRange", this);
  fNemaSetPositionPositronReach->SetGuidance("Set effective positron radius for given nema point (int, double)");

  fNemaSetPositionPositronReachDensDep = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setEffectivePositronRangeDensityDependence", this);
  fNemaSetPositionPositronReachDensDep->SetGuidance("Set effective positron radius dependent on the density for given nema point flag");

  fNemaSetPositionIsotopeType = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setIsotopeType", this);
  fNemaSetPositionIsotopeType->SetGuidance("Setting the type of isotope simulated for a given point (sodium or scandium)");
  
  fNemaSetPhantomElementID = new G4UIcmdWithAString("/jpetmc/source/nema/mixed/setPhantomElementID", this);
  fNemaSetPhantomElementID->SetGuidance("Set phantom element ID to simulate source inside it");
  
  fSetChamberCenter = new G4UIcmdWith3VectorAndUnit("/jpetmc/run/setChamberCenter", this);
  fSetChamberCenter->SetGuidance("Set position of the annihilation chamber");
  fSetChamberCenter->SetDefaultValue(G4ThreeVector(0, 0, 0));
  fSetChamberCenter->SetDefaultUnit("cm");
  fSetChamberCenter->SetUnitCandidates("cm");
  fSetChamberCenter->SetParameterName("Xvalue", "Yvalue", "Zvalue", false);

  fSetChamberEffectivePositronRadius = new G4UIcmdWithADoubleAndUnit("/jpetmc/run/setEffectivePositronRange", this);
  fSetChamberEffectivePositronRadius->SetGuidance("Set effective positron radius (RUN5)");
  fSetChamberEffectivePositronRadius->SetDefaultValue(0.5);
  fSetChamberEffectivePositronRadius->SetDefaultUnit("cm");
  fSetChamberEffectivePositronRadius->SetUnitCandidates("cm");
  
  fCosmicOnly = new G4UIcmdWithoutParameter("/jpetmc/source/cosmicOnly",this);
  fCosmicOnly->SetGuidance("Option to generate cosmic rays only");
  fCosmicOnly->SetGuidance("Generate only cosmics");
  
  fCosmicGenShape = new G4UIcmdWithAString("/jpetmc/source/cosmicGenShape", this);
  fCosmicGenShape->SetGuidance("Cosmics can be generated on a cylinder or in the cuboid");
  fCosmicGenShape->SetCandidates("cylinder cuboid"); 
}

PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger()
{
  delete fIsotopeSetShape;
  delete fIsotopeSetShapeDimCylinderRadius;
  delete fIsotopeSetShapeDimCylinderZ;
  delete fSourceType;
  delete fGammaBeamSetEnergy;
  delete fGammaBeamSetPosition;
  delete fGammaBeamSetMomentum;
  delete fIsotopeSetCenter;
  delete fNemaPosition;
  delete fNemaSetShape;
  delete fNemaMixed;
  delete fNemaSetPosition;
  delete fNemaSetPositionWeight;
  delete fNemaSetPositionOPSLifetime;
  delete fNemaSetPositionPPSLifetime;
  delete fNemaSetPositionDirectLifetime;
  delete fNemaSetPosition3GOption;
  delete fNemaSetPositionPPSOption;
  delete fNemaSetPositionDirectOption;
  delete fNemaSetPositionSize;
  delete fNemaSetPositionPromptOption;
  delete fNemaSetPositionPromptSize;
  delete fNemaSetPositionCylinderRotation;
  delete fNemaSetPositionCylinderShapeY;
  delete fNemaSetPositionPositronReachShape;
  delete fNemaSetPositionPositronReach;
  delete fNemaSetPositionPositronReachDensDep;
  delete fNemaSetPositionIsotopeType;
  delete fNemaSetPhantomElementID;
  delete fSetChamberCenter;
  delete fSetChamberEffectivePositronRadius;
  delete fCosmicOnly;
  delete fCosmicGenShape;
}

void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fSourceType) {
    fPrimGen->SetSourceTypeInfo(newValue);
  } else if (command == fGammaBeamSetEnergy) {
    ChangeToBeam();
    fPrimGen->GetBeamParams()->SetEnergy(fGammaBeamSetEnergy->GetNewDoubleValue(newValue));
  } else if (command == fGammaBeamSetPosition) {
    ChangeToBeam();
    fPrimGen->GetBeamParams()->SetVtxPosition(fGammaBeamSetPosition->GetNew3VectorValue(newValue));
  } else if (command == fGammaBeamSetMomentum) {
    ChangeToBeam();
    fPrimGen->GetBeamParams()->SetMomentum(fGammaBeamSetMomentum->GetNew3VectorValue(newValue));
  } else if (command == fIsotopeSetShape) {
    fPrimGen->GetIsotopeParams()->SetShape(newValue);
  } else if (command == fIsotopeSetGenGammas) {
    fPrimGen->GetIsotopeParams()->SetGammasNumber(fIsotopeSetGenGammas->GetNewIntValue(newValue));
  } else if (command == fIsotopeSetShapeDimCylinderRadius) {
    ChangeToIsotope();
    fPrimGen->GetIsotopeParams()->SetShapeDim(0, fIsotopeSetShapeDimCylinderRadius->GetNewDoubleValue(newValue));
  } else if (command == fIsotopeSetShapeDimCylinderZ) {
    ChangeToIsotope();
    fPrimGen->GetIsotopeParams()->SetShapeDim(1, fIsotopeSetShapeDimCylinderRadius->GetNewDoubleValue(newValue));
  } else if (command == fIsotopeSetCenter) {
    ChangeToIsotope();
    G4ThreeVector loc = fIsotopeSetCenter->GetNew3VectorValue(newValue);
    fPrimGen->GetIsotopeParams()->SetShapeCenterPosition(loc.x(), loc.y(), loc.z());
  } else if (command == fNemaPosition) {
    fPrimGen->SetSourceTypeInfo("nema");
    fPrimGen->SetNemaPoint(fNemaPosition->GetNewIntValue(newValue));
  } else if (command == fNemaMixed) {
    fPrimGen->SetSourceTypeInfo("nema-mixed");
  } else if (command == fNemaSetPosition) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4double x;
    G4double y;
    G4double z;
    is >> nemaPoint >> x >> y >> z;
    fPrimGen->SetNemaPointPosition(nemaPoint, G4ThreeVector(x*cm, y*cm, z*cm));
  } else if (command == fNemaSetShape) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4String shape;
    is >> nemaPoint >> shape;
    fPrimGen->SetPointShape(nemaPoint, shape);
  } else if (command == fNemaSetPositionWeight) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4double weight;
    is >> nemaPoint >> weight;
    fPrimGen->SetNemaPositionWeight(nemaPoint, weight);
  } else if (command == fNemaSetPositionOPSLifetime) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4double lifetime;
    is >> nemaPoint >> lifetime;
    fPrimGen->SetNemaPointLifetime(nemaPoint, PositroniumDecayMode::oPs, lifetime);
  } else if (command == fNemaSetPositionPPSLifetime) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4double lifetime;
    is >> nemaPoint >> lifetime;
    fPrimGen->SetNemaPointLifetime(nemaPoint, PositroniumDecayMode::pPs, lifetime);
  } else if (command == fNemaSetPositionDirectLifetime) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4double lifetime;
    is >> nemaPoint >> lifetime;
    fPrimGen->SetNemaPointLifetime(nemaPoint, PositroniumDecayMode::direct, lifetime);
  } else if (command == fNemaSetPosition3GOption) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    bool option;
    is >> nemaPoint >> option;
    fPrimGen->SetNemaPointGenerationOption(nemaPoint, NemaGenerationOption::n3G, option);
  } else if (command == fNemaSetPositionPPSOption) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    bool option;
    is >> nemaPoint >> option;
    fPrimGen->SetNemaPointGenerationOption(nemaPoint, NemaGenerationOption::nPPs, option);
  } else if (command == fNemaSetPositionDirectOption) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    bool option;
    is >> nemaPoint >> option;
    fPrimGen->SetNemaPointGenerationOption(nemaPoint, NemaGenerationOption::nDirect, option);
  } else if (command == fNemaSetPositionSize) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4double x; //radius
    G4double y; //length
    G4double z;
    is >> nemaPoint >> x >> y >> z;
    fPrimGen->SetNemaPointSize(nemaPoint, x*cm, y*cm, z*cm);
  } else if (command == fNemaSetPositionPromptOption) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    bool option;
    is >> nemaPoint >> option;
    fPrimGen->SetNemaPointGenerationOption(nemaPoint, NemaGenerationOption::nPrompt, option);
  } else if (command == fNemaSetPositionPromptSize) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4double x; //radius
    G4double y; //length
    G4double z;
    is >> nemaPoint >> x >> y >> z;
    fPrimGen->SetNemaPointPromptSize(nemaPoint, x*cm, y*cm, z*cm);
  } else if (command == fNemaSetPositionCylinderRotation) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4double theta, phi;
    is >> nemaPoint >> theta >> phi;
    fPrimGen->SetNemaPointOrientation(nemaPoint, theta, phi);
  } else if (command == fNemaSetPositionCylinderShapeY) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4double direction, power, length;
    is >> nemaPoint >> direction >> power >> length;
    fPrimGen->SetNemaPointShape(nemaPoint, Dimension::dimY, direction*cm, power, length);
  } else if (command == fNemaSetPositionPositronReachShape) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4String shape;
    is >> nemaPoint >> shape;
    fPrimGen->SetPointPositronReachShape(nemaPoint, shape);
  } else if (command == fNemaSetPositionPositronReach) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4double reach;
    is >> nemaPoint >> reach;
    fPrimGen->SetPointExperimentalReach(nemaPoint, reach);
  } else if (command == fNemaSetPositionPositronReachDensDep) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    bool option;
    is >> nemaPoint >> option;
    fPrimGen->SetNemaPointGenerationOption(nemaPoint, NemaGenerationOption::nDirectLFDensDep, option);
  } else if (command == fNemaSetPositionIsotopeType) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4String isotopeType;
    is >> nemaPoint >> isotopeType;
    fPrimGen->SetNemaPointIsotopeType(nemaPoint, isotopeType);
  } else if (command == fNemaSetPhantomElementID) {
    G4String paramString = newValue;
    std::istringstream is(paramString);
    G4int nemaPoint;
    G4int phantomElement;
    is >> nemaPoint >> phantomElement;
    fPrimGen->SetPhantomElementIDForNemaPoint(nemaPoint, phantomElement);
  } else if (command == fSetChamberCenter) {
    if (!CheckIfRun()) { ChangeToRun(); }
    DetectorConstants::SetChamberCenter(fSetChamberCenter->GetNew3VectorValue(newValue));
  } else if (command == fSetChamberEffectivePositronRadius) {
    if (!CheckIfRun()) { ChangeToRun(); }
    fPrimGen->SetEffectivePositronRadius(
      fSetChamberEffectivePositronRadius->GetNewDoubleValue(newValue)
    );
  } else if (command == fCosmicOnly) {
    fPrimGen->SetSourceTypeInfo("cosmics");
  } else if (command == fCosmicGenShape) {
    fPrimGen->GetIsotopeParams()->SetShape(newValue);
  }
}

void PrimaryGeneratorActionMessenger::ChangeToIsotope()
{
  if (fPrimGen->GetSourceTypeInfo() != "isotope") {
    G4Exception(
      "PrimaryGeneratorActionMessenger", "PGM02",
      JustWarning, "Changed sourceType to isotope"
    );
    fPrimGen->SetSourceTypeInfo("isotope");
  }
}

bool PrimaryGeneratorActionMessenger::CheckIfRun()
{
  G4int nRun = DetectorConstruction::GetInstance()->GetRunNumber();
  if (nRun != 0) {
    fPrimGen->SetSourceTypeInfo("run");
    return true;
  }
  return false;
}

void PrimaryGeneratorActionMessenger::ChangeToRun()
{
  if (fPrimGen->GetSourceTypeInfo() != "run") {
    G4Exception(
      "PrimaryGeneratorActionMessenger", "PGM01",
      JustWarning, "Changed run number to RUN5"
    );
    DetectorConstruction::GetInstance()->LoadGeometryForRun(5);
    fPrimGen->SetSourceTypeInfo("run");
    DetectorConstruction::GetInstance()->UpdateGeometry();
  }
}

void PrimaryGeneratorActionMessenger::ChangeToBeam()
{
  //! check if we really changing parameters corresponding to the beam
  if (fPrimGen->GetSourceTypeInfo() != "beam") {
    G4Exception(
      "PrimaryGeneratorActionMessenger", "PGM01",
      JustWarning, "Changed sourceType to beam"
    );
    fPrimGen->SetSourceTypeInfo("beam");
  }
}
