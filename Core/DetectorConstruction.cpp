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
 *  @file DetectorConstruction.cpp
 */

#include "DetectorConstruction.h"
#include "DetectorConstructionMessenger.h"
#include "DetectorConstants.h"
#include "MaterialExtension.h"
#include "MaterialParameters.h"
#include "RunManager.h"

#include <G4LogicalVolumeStore.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4Polycone.hh>
#include <G4RegionStore.hh>
#include <G4SolidStore.hh>
#include <G4Sphere.hh>
#include <G4SubtractionSolid.hh>
#include <G4Tubs.hh>
#include <G4UnionSolid.hh>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iomanip>
#include <iostream>
#include <vector>
#include <CADMesh.hh>

namespace pt = boost::property_tree;

namespace {
  G4Mutex detectorConstructionMutex = G4MUTEX_INITIALIZER;
}

DetectorConstruction* DetectorConstruction::fInstance = 0;

DetectorConstruction* DetectorConstruction::GetInstance()
{
  if (fInstance == 0)
    fInstance = new DetectorConstruction();
  return fInstance;
}

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(), fRunNumber(0), fLoadScintillators(true), fLoadCADFrame(false), fLoadWrapping(true), fLoadModularLayer(false)
{
  InitializeMaterials();
  fMessenger = new DetectorConstructionMessenger(this);
}

DetectorConstruction::~DetectorConstruction() { delete fMessenger; }

void DetectorConstruction::SetHistoManager(HistoManager* histo) { fHistoManager = histo; }

// cppcheck-suppress unusedFunction
G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  //! world
  fWorldSolid = new G4Box("world", DetectorConstants::world_size[0], DetectorConstants::world_size[1], DetectorConstants::world_size[2]);
  fWorldLogical = new G4LogicalVolume(fWorldSolid, fAir, "worldLogical");
  fWorldPhysical = new G4PVPlacement(0, G4ThreeVector(), fWorldLogical, "worldPhysical", 0, false, 0, checkOverlaps);

  //! Scintillators in the detector can be constructed from a custom setup file or with coded methods
  //! Creating thegeometry file is available only for former
  if (fReadJSONSetup)
  {
    ConstructFromSetupFile(fJSONSetupFileName);
  }
  else
  {
    if (fLoadScintillators)
    {
      //! scintillators for standard setup
      ConstructScintillators();
    }
    if (fLoadModularLayer)
    {
      ConstructScintillatorsModularLayer();
    }
    if (fCreateGeometryFile)
    {
      CreateGeometryFile();
    }
  }

  if (fLoadCADFrame)
  {
    ConstructFrameCAD();
  }
  if (fRunNumber == 3)
  {
    ConstructTargetRun3();
  }
  else if (fRunNumber == 5)
  {
    ConstructTargetRun5();
  }
  else if (fRunNumber == 6)
  {
    ConstructTargetRun6();
  }
  else if (fRunNumber == 7)
  {
    ConstructTargetRun7();
  }
  else if (fRunNumber == 12)
  {
    ConstructTargetRun12();
  }

  return fWorldPhysical;
}

// cppcheck-suppress unusedFunction
void DetectorConstruction::ConstructSDandField()
{
  G4AutoLock lock(&detectorConstructionMutex);

  if (!fDetectorSD.Get())
  {
    DetectorSD* det = new DetectorSD("/mydet/detector", getNumberOfScintillators(), DetectorConstants::GetMergingTimeValueForScin());
    det->SetHistoManager(fHistoManager);
    fDetectorSD.Put(det);

    G4SDManager::GetSDMpointer()->AddNewDetector(fDetectorSD.Get());
    if (fScinLog)
    {
      SetSensitiveDetector(fScinLog, fDetectorSD.Get());
    }
    if (fScinLogInModule)
    {
      SetSensitiveDetector(fScinLogInModule, fDetectorSD.Get());
    }
    if (fReadJSONSetup)
    {
      for (auto scinLog : fStripsFromSetup)
      {
        SetSensitiveDetector(scinLog, fDetectorSD.Get());
      }
    }
  }
}

void DetectorConstruction::LoadGeometryForRun(G4int nr)
{
  fRunNumber = nr;
  if (fRunNumber != 3 && fRunNumber != 5 && fRunNumber != 6 && fRunNumber != 7 && fRunNumber != 12 && fRunNumber != 0)
  {
    G4Exception("DetectorConstruction", "DC02", FatalException, "This run setup is not implemented ");
  }
}

G4int DetectorConstruction::getNumberOfScintillators()
{
  if (fLoadModularLayer)
  {
    return 504;
  }
  else if (fReadJSONSetup)
  {
    return fMaxCreatedScinID;
  }
  else
  {
    return 192;
  }
}

void DetectorConstruction::UpdateGeometry() { RunManager::GetRunManager()->ReinitializeGeometry(); }

void DetectorConstruction::ReloadMaterials(const G4String& material)
{
  if (material == "xad4")
  {
    fXADMaterial->ChangeMaterialConstants();
    fXADMaterial->FillIntensities();
  }
  else if (material == "kapton")
  {
    fKapton->ChangeMaterialConstants();
    fKapton->FillIntensities();
  }
  else if (material == "aluminium")
  {
    fAluminiumMaterial->ChangeMaterialConstants();
    fAluminiumMaterial->FillIntensities();
    fSmallChamberMaterial->ChangeMaterialConstants();
    fSmallChamberMaterial->FillIntensities();
  }
  else if (material == "plexiglass")
  {
    fPlexiglass->ChangeMaterialConstants();
    fPlexiglass->FillIntensities();
  }
  else if (material == "pa6")
  {
    fSmallChamberRun7Material->ChangeMaterialConstants();
    fSmallChamberRun7Material->FillIntensities();
  }
  else if (material == "stainlessSteel")
  {
    fStainlessSteel->ChangeMaterialConstants();
    fStainlessSteel->FillIntensities();
  }
  else if (material == "siliconDioxide")
  {
    fSiliconDioxide->ChangeMaterialConstants();
    fSiliconDioxide->FillIntensities();
  }
  else if (material == "polycarbonate")
  {
    fPolycarbonate->ChangeMaterialConstants();
    fPolycarbonate->FillIntensities();
  }
  else if (material == "polyoxymethylene")
  {
    fPolyoxymethylene->ChangeMaterialConstants();
    fPolyoxymethylene->FillIntensities();
  }
  else
  {
    G4Exception("DetectorConstruction", "DC01", FatalException, "Wrong material ID given to reload");
  }
}

void DetectorConstruction::InitializeMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();

  //! Air
  nistManager->FindOrBuildMaterial("G4_AIR");
  fAir = new MaterialExtension(MaterialParameters::MaterialID::mAir, "air", G4Material::GetMaterial("G4_AIR"));

  //! Kapton
  nistManager->FindOrBuildMaterial("G4_KAPTON");
  fKapton = new MaterialExtension(MaterialParameters::MaterialID::mKapton, "kapton", G4Material::GetMaterial("G4_KAPTON"));
  nistManager->FindOrBuildMaterial("G4_Galactic");
  fVacuum = new MaterialExtension(MaterialParameters::MaterialID::mAir, "vacuum", G4Material::GetMaterial("G4_Galactic"));

  //! Plexiglass
  nistManager->FindOrBuildMaterial("G4_PLEXIGLASS");
  fPlexiglass = new MaterialExtension(MaterialParameters::MaterialID::mPlexiglass, "bigChamberRun6", G4Material::GetMaterial("G4_PLEXIGLASS"));
  fPlexiglass->AllowsAnnihilations(true);

  //! XAD material, ref: https://www.sigmaaldrich.com/catalog/product/sigma/xad4
  nistManager->FindOrBuildMaterial("G4_POLYSTYRENE");
  fXADMaterial = new MaterialExtension(MaterialParameters::MaterialID::mXAD4, "XAD", G4Material::GetMaterial("G4_POLYSTYRENE"));
  fXADMaterial->AllowsAnnihilations(true);

  //! Scintillator material
  nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  fScinMaterial = new MaterialExtension(MaterialParameters::MaterialID::mScin, "scinMaterial", G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE"));

  //! Aluminium
  nistManager->FindOrBuildMaterial("G4_Al");
  fAluminiumMaterial = new MaterialExtension(MaterialParameters::MaterialID::mAl, "aluminium", G4Material::GetMaterial("G4_Al"));
  fAluminiumMaterial->AllowsAnnihilations(true);
  //! Small chamber is also made out of aluminium
  fSmallChamberMaterial = new MaterialExtension(MaterialParameters::MaterialID::mAl, "smallChamber", G4Material::GetMaterial("G4_Al"));
  fSmallChamberMaterial->AllowsAnnihilations(true);

  //! Polyamide PA6
  nistManager->FindOrBuildMaterial("G4_NYLON-6-6");
  fSmallChamberRun7Material =
      new MaterialExtension(MaterialParameters::MaterialID::mPA6, "smallChamberRun7", G4Material::GetMaterial("G4_NYLON-6-6"));
  fSmallChamberRun7Material->AllowsAnnihilations(true);

  //! Vacuum
  G4double EffAtomNumbZ = 1.;
  G4double EffMolMassA = 1.01 * g / mole;
  G4String name = "G4_Galactic";
  G4double density = (fPressure / 1.e-19 * pascal) * 1.209e-24 * g / cm3;
  //! Density linearly extrapolated from the air density and pressure
  G4double temperature = 295. * kelvin;

  nistManager->FindOrBuildMaterial("G4_Galactic");
  vacuum = new G4Material(name, EffAtomNumbZ, EffMolMassA, density, kStateGas, temperature, fPressure);
  fVacuum = new MaterialExtension(MaterialParameters::MaterialID::mAir, "vacuum", vacuum);

  //! Polycarbonate
  nistManager->FindOrBuildMaterial("G4_POLYCARBONATE");
  fPolycarbonate = new MaterialExtension(MaterialParameters::MaterialID::mPolycarbonate, "cydChamber", G4Material::GetMaterial("G4_POLYCARBONATE"));
  fPolycarbonate->AllowsAnnihilations(true);

  //! Polyoxymethylene
  nistManager->FindOrBuildMaterial("G4_POLYOXYMETHYLENE");
  fPolyoxymethylene =
      new MaterialExtension(MaterialParameters::MaterialID::mPolyoxymethylene, "boltsMaterial", G4Material::GetMaterial("G4_POLYOXYMETHYLENE"));
  fPolyoxymethylene->AllowsAnnihilations(true);

  //! Silicon-dioxide
  nistManager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  fSiliconDioxide =
      new MaterialExtension(MaterialParameters::MaterialID::mSiliconDioxide, "SilicaCoating", G4Material::GetMaterial("G4_SILICON_DIOXIDE"));
  fSiliconDioxide->AllowsAnnihilations(true);

  //! Stainless-steel
  nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  fStainlessSteel =
      new MaterialExtension(MaterialParameters::MaterialID::mStainlessSteel, "StainlessSteel", G4Material::GetMaterial("G4_STAINLESS-STEEL"));
}

/**
 * This method uses CAD library to read stl file
 * with construction of J-PET metal frame. Note that
 * in the file the scintillator slots were made bigger
 *          length    width
 *  lab     2.1 cm    0.9 cm
 *  file    2.6 cm    1.7 cm
 */
void DetectorConstruction::ConstructFrameCAD()
{
  auto cad_frame_mesh = CADMesh::TessellatedMesh::FromSTL((char*)"stl_geometry/Frame_JPET.ascii.stl");
  cad_frame_mesh->SetScale(mm);

  G4VSolid* cad_frame_solid = cad_frame_mesh->GetSolid();
  G4LogicalVolume* cad_frame_logical = new G4LogicalVolume(cad_frame_solid, fAluminiumMaterial, "cad_frame_logical");

  G4VisAttributes* detVisAtt = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));
  detVisAtt->SetForceWireframe(true);
  detVisAtt->SetForceSolid(true);
  cad_frame_logical->SetVisAttributes(detVisAtt);

  G4RotationMatrix rot = G4RotationMatrix();
  rot.rotateY(90 * deg);
  G4ThreeVector loc = G4ThreeVector(0 * cm, 306.5 * cm, -23 * cm);
  G4Transform3D transform(rot, loc);
  new G4PVPlacement(transform, cad_frame_logical, "cadGeom", fWorldLogical, true, 0, checkOverlaps);
}

void DetectorConstruction::ConstructScintillators()
{
  G4Box* scinBox =
      new G4Box("scinBox", DetectorConstants::scinDim[0] / 2.0, DetectorConstants::scinDim[1] / 2.0, DetectorConstants::scinDim[2] / 2.0);

  fScinLog = new G4LogicalVolume(scinBox, fScinMaterial, "scinLogical");
  G4VisAttributes* boxVisAtt = new G4VisAttributes(G4Colour(0.447059, 0.623529, 0.811765));
  boxVisAtt->SetForceWireframe(true);
  boxVisAtt->SetForceSolid(true);
  fScinLog->SetVisAttributes(boxVisAtt);

  G4Box* scinBoxFree = new G4Box("scinBoxFree", DetectorConstants::scinDim[0] / 2.0 + DetectorConstants::wrappingShift,
                                 DetectorConstants::scinDim[1] / 2.0 + DetectorConstants::wrappingShift, DetectorConstants::scinDim[2] / 2.0);

  G4Box* wrappingBox =
      new G4Box("wrappingBox", DetectorConstants::scinDim[0] / 2.0 + DetectorConstants::wrappingThickness,
                DetectorConstants::scinDim[1] / 2.0 + DetectorConstants::wrappingThickness, DetectorConstants::scinDim[2] / 2.0 - 1 * cm);

  G4LogicalVolume* wrappingLog;
  G4VisAttributes* boxVisAttWrapping = new G4VisAttributes(G4Colour(0.447059, 0.623529, 0.811765));
  boxVisAttWrapping->SetForceWireframe(true);
  boxVisAttWrapping->SetForceSolid(true);

  G4int oldLayerNumber = fLayerNumber;
  G4int moduleNumber = 0;
  for (int j = 0; j < DetectorConstants::layers; j++)
  {
    fLayerNumber = oldLayerNumber + j + 1;
    Layer layTemp(fLayerNumber, "Layer nr " + std::to_string(fLayerNumber), DetectorConstants::radius[j] / 10 /*to cm*/, 1);
    fLayerContainer.push_back(layTemp);
    for (int i = 0; i < DetectorConstants::nSegments[j]; i++)
    {
      moduleNumber++;
      G4double phi = i * 2 * M_PI / DetectorConstants::nSegments[j];
      G4double fi = M_PI / DetectorConstants::nSegments[j];

      if (j == 0)
        fi = 0.0;

      G4RotationMatrix rot = G4RotationMatrix();
      rot.rotateZ(phi + fi);

      G4ThreeVector loc = G4ThreeVector(DetectorConstants::radius[j] * (cos(phi + fi)), DetectorConstants::radius[j] * (sin(phi + fi)), 0.0);

      G4Transform3D transform(rot, loc);
      G4String name = "scin_" + G4UIcommand::ConvertToString(fMaxScinID);

      if (fCreateGeometryFile)
      {
        Scin scinTemp(moduleNumber, moduleNumber, DetectorConstants::scinDim[0], DetectorConstants::scinDim[1], DetectorConstants::scinDim[2],
                      DetectorConstants::radius[j] * cos((phi + fi) * 180 / M_PI) / 10,
                      DetectorConstants::radius[j] * sin((phi + fi) * 180 / M_PI) / 10, 0);
        fScinContainer.push_back(scinTemp);
        Slot slotTemp(moduleNumber, fLayerNumber, (phi + fi) * 180 / M_PI, "single");
        fSlotContainer.push_back(slotTemp);
      }

      new G4PVPlacement(transform, fScinLog, name, fWorldLogical, true, fMaxScinID, checkOverlaps);

      if (fLoadWrapping)
      {
        G4VSolid* unionSolid = new G4SubtractionSolid("wrapping", wrappingBox, scinBoxFree);
        wrappingLog = new G4LogicalVolume(unionSolid, fKapton, "wrappingLogical");
        wrappingLog->SetVisAttributes(boxVisAttWrapping);
        
        G4String nameWrapping = "wrapping_" + G4UIcommand::ConvertToString(fMaxScinID);
        new G4PVPlacement(transform, wrappingLog, nameWrapping, fWorldLogical, true, fMaxScinID, checkOverlaps);
      }
      fMaxScinID++;
    }
  }
}

/**
 * Construction of modular layer (4th) - added by S. Sharma 20.06.2018
 */
void DetectorConstruction::ConstructScintillatorsModularLayer()
{
  G4Box* scinBoxInModule = new G4Box("scinBoxInModule", DetectorConstants::scinDim_inModule[0] / 2.0, DetectorConstants::scinDim_inModule[1] / 2.0,
                                     DetectorConstants::scinDim_inModule[2] / 2.0);

  fScinLogInModule = new G4LogicalVolume(scinBoxInModule, fScinMaterial, "scinBoxInModule");

  G4VisAttributes* boxVisAttI = new G4VisAttributes(G4Colour(0.105, 0.210, 0.210, 0.9));
  boxVisAttI->SetForceWireframe(true);
  boxVisAttI->SetForceSolid(true);
  fScinLogInModule->SetVisAttributes(boxVisAttI);

  //! 13 straight scintillators in single module
  //! radius of each scintillator in tabular form
  //! taken from equation: radius0/cos(j*displacement) where
  //! j -> number of scintillator
  //! radius0 = 38.186*cm
  //! displacement -> Angular displacement
  //! (1.04 degree / 0.01815 radius - fixed; value provided by Sushil)

  const G4double radius_24[13] = {38.416, 38.346, 38.289, 38.244, 38.212, 38.192, 38.186, 38.192, 38.212, 38.244, 38.289, 38.346, 38.416};

  const G4double radius_8[13] = {13.4037, 13.2011, 13.0330, 12.9007, 12.8055, 12.7479, 12.7287, 12.7479, 12.8055, 12.9007, 13.0330, 13.2011, 13.4037};

  const G4double radius_16[13] = {25.8015, 25.69680, 25.61085, 25.54379, 25.49579, 25.4669, 25.45733,
                                  25.4669, 25.49579, 25.54379, 25.61085, 25.6968,  25.8015};

  std::vector<G4double> radius_dynamic = std::vector<G4double>(13, 0.0);

  //! starting ID for modular layer
  fMaxScinID = 201;
  G4double angDisp_8 = 0.0531204920;  // 3.0435^0
  G4double angDisp_16 = 0.0272515011; // 1.561396^0
  G4double angDisp_24 = 0.01815;      // 1.04^0

  G4double angDisp_dynamic = 0;
  G4int numberofModules = 0;

  // Assume: enum GeometryKind { Geo24ModulesLayer, Geo24ModulesLayerDistributed};
  // Single : for 24 modules layer
  // Double : for 8 and 16 layer configuration

  switch (fGeoKind)
  {
  case GeometryKind::Geo24ModulesLayer:
    for (int i = 0; i < 13; i++)
    {
      radius_dynamic[i] = radius_24[i];
    }
    numberofModules = 24;
    angDisp_dynamic = angDisp_24;
    ConstructLayers(radius_dynamic, numberofModules, angDisp_dynamic, fMaxScinID);
    break;
  case GeometryKind::Geo24ModulesLayerDistributed:
    for (int i = 0; i < 13; i++)
    {
      radius_dynamic[i] = radius_8[i];
    }
    numberofModules = 8;
    angDisp_dynamic = angDisp_8;
    ConstructLayers(radius_dynamic, numberofModules, angDisp_dynamic, fMaxScinID);
    for (int i = 0; i < 13; i++)
    {
      radius_dynamic[i] = radius_16[i];
    }
    numberofModules = 16;
    angDisp_dynamic = angDisp_16;
    fMaxScinID += 8 * 13;
    ConstructLayers(radius_dynamic, numberofModules, angDisp_dynamic, fMaxScinID);
    break;
  default:
    G4cout << " Wrong option for modular setup: choose either Single or Double" << G4endl;
    break;
  }
}

void DetectorConstruction::ConstructLayers(std::vector<G4double>& radius_dynamic, G4int& numberofModules, G4double& angDisp_dynamic, G4int& icopyI)
{
  G4double phi = 0.0;
  G4double phi1 = 0.0;
  fLayerNumber++;

  Layer layTemp(fLayerNumber, "Layer nr " + std::to_string(fLayerNumber), radius_dynamic[6], 1);
  fLayerContainer.push_back(layTemp);

  G4int moduleNumber = 0;
  for (int i = 0; i < numberofModules; i++)
  {
    phi = (i * 2 * M_PI / numberofModules);
    for (int j = -6; j < 7; j++)
    {
      phi1 = phi + j * angDisp_dynamic;
      G4double radius_new = radius_dynamic[j + 6] * cm;
      G4RotationMatrix rot = G4RotationMatrix();
      rot.rotateZ(phi);
      G4ThreeVector loc = G4ThreeVector(radius_new * cos(phi1), radius_new * sin(phi1), 0.0);
      G4Transform3D transform(rot, loc);
      moduleNumber = icopyI + i * 13 + j + 6;
      G4String nameNewI = "scin_" + G4UIcommand::ConvertToString(moduleNumber);

      if (fCreateGeometryFile)
      {
        Scin scinTemp(moduleNumber, moduleNumber, DetectorConstants::scinDim_inModule[0], DetectorConstants::scinDim_inModule[1],
                      DetectorConstants::scinDim_inModule[2], (radius_new / cm) * cos(phi1 * 180 / M_PI), (radius_new / cm) * sin(phi1 * 180 / M_PI),
                      0);
        fScinContainer.push_back(scinTemp);
        Slot slotTemp(moduleNumber, fLayerNumber, phi * 180 / M_PI, "module");
        fSlotContainer.push_back(slotTemp);
      }

      new G4PVPlacement(transform, fScinLogInModule, nameNewI, fWorldLogical, true, icopyI + i * 13 + j + 6, checkOverlaps);
    }
  }
}

void DetectorConstruction::ConstructFromSetupFile(G4String fileName)
{
  //! Reading JSON file to BOOST property tree
  pt::ptree tree;
  pt::read_json(fileName, tree);

  //! Attribute definitions (outside of the loop)
  G4VisAttributes* boxVisAtt = new G4VisAttributes(G4Colour(0.447059, 0.623529, 0.811765));
  boxVisAtt->SetForceWireframe(true);
  boxVisAtt->SetForceSolid(true);

  G4VisAttributes* boxVisAttWrapping = new G4VisAttributes(G4Colour(0.447059, 0.623529, 0.811765));
  boxVisAttWrapping->SetForceWireframe(true);
  boxVisAttWrapping->SetForceSolid(true);

  BOOST_FOREACH (pt::ptree::value_type& scin, tree.get_child(G4UIcommand::ConvertToString(fJSONSetupRunNum) + ".scin"))
  {
    //! Reading parameters of the scintillator from property tree
    int id = scin.second.get<int>("id");
    G4double height = scin.second.get<double>("height") * cm;
    G4double width = scin.second.get<double>("width") * cm;
    G4double length = scin.second.get<double>("length") * cm;
    G4double xcenter = scin.second.get<double>("xcenter") * cm;
    G4double ycenter = scin.second.get<double>("ycenter") * cm;
    G4double zcenter = scin.second.get<double>("zcenter") * cm;
    G4double rot_x = scin.second.get<double>("rot_x");
    G4double rot_y = scin.second.get<double>("rot_y");
    G4double rot_z = scin.second.get<double>("rot_z");

    //! Updating maximum scin ID
    if (fMaxCreatedScinID < id)
    {
      fMaxCreatedScinID = id;
    }

    G4String name = "scin_" + G4UIcommand::ConvertToString(id);
    G4Box* scinBox = new G4Box(name + "_box", height / 2.0, width / 2.0, length / 2.0);
    G4LogicalVolume* scinLog = new G4LogicalVolume(scinBox, fScinMaterial, name + "_logical");
    scinLog->SetVisAttributes(boxVisAtt);

    //! Rotation matrix and translation of the strip
    //! Angles in the file are writtern in degrees, converting to radians
    G4RotationMatrix rot = G4RotationMatrix();
    rot.rotateX(TMath::DegToRad() * rot_x);
    rot.rotateY(TMath::DegToRad() * rot_y);
    rot.rotateZ(TMath::DegToRad() * rot_z);

    G4ThreeVector loc = G4ThreeVector(xcenter, ycenter, zcenter);
    G4Transform3D transform(rot, loc);

    //! Adding scintillator to detector construction
    new G4PVPlacement(transform, scinLog, name + "_phys", fWorldLogical, true, id, checkOverlaps);
    fStripsFromSetup.push_back(scinLog);

    //! Additional wrapping of the strips
    if (fLoadWrapping)
    {
      G4Box* scinBoxFree = new G4Box(name + "_box_free", height / 2.0 + DetectorConstants::wrappingShift,
                                     width / 2.0 + DetectorConstants::wrappingShift, length / 2.0);

      G4Box* wrappingBox = new G4Box(name + "_wrapping_box", height / 2.0 + DetectorConstants::wrappingThickness,
                                     width / 2.0 + DetectorConstants::wrappingThickness, length / 2.0 - 1 * cm);

      G4VSolid* unionSolid = new G4SubtractionSolid(name + "_wrapping_union", wrappingBox, scinBoxFree);
      G4LogicalVolume* wrappingLog = new G4LogicalVolume(unionSolid, fKapton, name + "_wrapping_logical");
      wrappingLog->SetVisAttributes(boxVisAttWrapping);
      new G4PVPlacement(transform, wrappingLog, name + "_wrapping_physical", fWorldLogical, true, id, checkOverlaps);
    }
  }
}

/**
 * Method for construction of setup used in Run 3
 * Please note that chamber is symmetric in z,
 * therefore only half of the table is presented.
 * Chamber has four different outer radii.
 *  z         |  r Inner | r Outer | Comment
 * -----------|----------|---------|-------
 *  -37.00*cm |  0*cm    | 3*cm    | chamber end
 *  -32.61*cm |  0*cm    | 3*cm    | start endcup
 *  -32.60*cm |  0*cm    | 10*cm   | end slope
 *  -31.10*cm |  0*cm    | 10*cm   | start slope
 *  -31.00*cm |  7.1*cm  | 7.5*cm  | end slope and start empty volume
 */
void DetectorConstruction::ConstructTargetRun3()
{
  const double chamber_radius_inner = 7.1 * cm;
  const double chamber_radius_outer = 7.5 * cm;
  const double chamber_endcup_min = 3.0 * cm;
  const double chamber_endcup_max = 10.0 * cm;
  const double vacuumChamber_halfLength = 31.0 * cm;
  const double vacuumChamber_outerRadius = chamber_radius_inner - 0.01 * cm;

  G4RotationMatrix rot = G4RotationMatrix();

  G4double z[] = {-37 * cm, -32.61 * cm, -32.6 * cm, -31.1 * cm, -31 * cm, 31 * cm, 31.1 * cm, 32.6 * cm, 32.61 * cm, 37 * cm};

  G4double rInner[] = {0 * cm, 0 * cm, 0 * cm, 0 * cm, chamber_radius_inner, chamber_radius_inner, 0 * cm, 0 * cm, 0 * cm, 0 * cm};

  G4double rOuter[] = {chamber_endcup_min,   chamber_endcup_min, chamber_endcup_max, chamber_endcup_max, chamber_radius_outer,
                       chamber_radius_outer, chamber_endcup_max, chamber_endcup_max, chamber_endcup_min, chamber_endcup_min};

  G4Polycone* bigChamber = new G4Polycone("bigChamber", 0 * degree, 360 * degree, 10, z, rInner, rOuter);

  G4LogicalVolume* bigChamber_logical = new G4LogicalVolume(bigChamber, fAluminiumMaterial, "bigChamber_logical");

  G4VisAttributes* detVisAtt = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));
  detVisAtt->SetForceWireframe(true);
  detVisAtt->SetForceSolid(true);
  bigChamber_logical->SetVisAttributes(detVisAtt);

  G4ThreeVector loc = DetectorConstants::GetChamberCenter();
  G4Transform3D transform(rot, loc);

  new G4PVPlacement(transform, bigChamber_logical, "bigChamberGeom", fWorldLogical, true, 0, checkOverlaps);

  G4Tubs* ringInner = new G4Tubs("ringInner", 15 * mm, 20.8 * mm, 0.8 * mm, 0 * degree, 360 * degree);

  G4Box* conn = new G4Box("conn", 25 * mm, 7. * mm, 0.8 * mm);
  G4LogicalVolume* conn_logical = new G4LogicalVolume(conn, fAluminiumMaterial, "conn_logical");
  conn_logical->SetVisAttributes(detVisAtt);

  G4ThreeVector loc2;
  G4Transform3D transform2;

  loc2 = G4ThreeVector(39.8 * mm, 0.0, 0.0) + DetectorConstants::GetChamberCenter();
  transform2 = G4Transform3D(rot, loc2);
  G4UnionSolid* unionSolid = new G4UnionSolid("c1", ringInner, conn, transform2);

  loc2 = G4ThreeVector(-39.8 * mm, 0.0, 0.0) + DetectorConstants::GetChamberCenter();
  transform2 = G4Transform3D(rot, loc2);
  unionSolid = new G4UnionSolid("c2", unionSolid, conn, transform2);

  loc2 = G4ThreeVector(0.0, 39.8 * mm, 0.0) + DetectorConstants::GetChamberCenter();
  transform2 = G4Transform3D(rot.rotateZ(90 * degree), loc2);
  unionSolid = new G4UnionSolid("c3", unionSolid, conn, transform2);

  loc2 = G4ThreeVector(0.0, -39.8 * mm, 0.0) + DetectorConstants::GetChamberCenter();
  transform2 = G4Transform3D(rot, loc2);
  unionSolid = new G4UnionSolid("c4", unionSolid, conn, transform2);

  G4Tubs* ringOuter = new G4Tubs("ringOuter", 60 * mm, 70 * mm, 0.8 * mm, 0 * degree, 360 * degree);
  unionSolid = new G4UnionSolid("c5", unionSolid, ringOuter);

  G4LogicalVolume* unionSolid_logical = new G4LogicalVolume(unionSolid, fAluminiumMaterial, "union_logical");
  unionSolid_logical->SetVisAttributes(detVisAtt);

  new G4PVPlacement(transform, unionSolid_logical, "bigChamberInnerStructure", fWorldLogical, true, 0, checkOverlaps);

  // Vacuum
  G4Tubs* bigChamberRun3_vac =
      new G4Tubs("bigChamberRun3_vac", 0 * cm, vacuumChamber_outerRadius, vacuumChamber_halfLength, 0 * degree, 360 * degree);

  G4LogicalVolume* Run3Vac_logical = new G4LogicalVolume(bigChamberRun3_vac, fVacuum, "Run3Vac_logical");

  new G4PVPlacement(transform, Run3Vac_logical, "bigChamberRun3_vacuum", bigChamber_logical, true, 0, checkOverlaps);
}

/**
 * Method for construction of setup used in Run 5
 * Parameters taken from http://koza.if.uj.edu.pl/petwiki/index.php/Small_annihilation_chamber
 * below main values are written. Please note that chamber is symmetric in z,
 * therefore only half of the table is presented.
 * Chamber has four different outer radii.
 *  z       |  r Inner | r Outer | Comment
 * ---------|----------|---------|-------
 *  -7.6*cm | 0*cm     | 2.5*cm  | chamber end
 *  -7.0*cm | 0*cm     | 2.5*cm  | start slope (R1->R2)
 *  -6.9*cm | 1.5*cm   | 2.0*cm  | end slope and start empty space
 *  -4.3*cm | 1.5*cm   | 2.0*cm  | start slope (R2->R3)
 *  -4.2*cm | 1.5*cm   | 1.8*cm  | end slope
 *  -2.7*cm | 1.5*cm   | 1.8*cm  | start slope (R3->R4)
 *  -2.6*cm | 1.5*cm   | 1.57*cm | end slope
 */
void DetectorConstruction::ConstructTargetRun5()
{
  const double chamber_radius_inner = 1.5 * cm;
  const double chamber_radius_outer_1 = 2.5 * cm;
  const double chamber_radius_outer_2 = 2.0 * cm;
  const double chamber_radius_outer_3 = 1.8 * cm;
  const double chamber_radius_outer_4 = 1.57 * cm;
  const double xadFilling_halfthickness = 0.6 * cm;
  const double vacuumChamber_halfLength = 2.6 * cm;
  const double vacuumChamber_outerRadius = chamber_radius_inner - 0.01 * cm;

  G4RotationMatrix rot = G4RotationMatrix();
  G4double z[] = {-7.6 * cm, -7.0 * cm, -6.9 * cm, -4.3 * cm, -4.2 * cm, -2.7 * cm, -2.6 * cm,
                  2.6 * cm,  2.7 * cm,  4.2 * cm,  4.3 * cm,  6.9 * cm,  7.0 * cm,  7.6 * cm};

  G4double rInner[] = {0 * cm,
                       0 * cm,
                       chamber_radius_inner,
                       chamber_radius_inner,
                       chamber_radius_inner,
                       chamber_radius_inner,
                       chamber_radius_inner,
                       chamber_radius_inner,
                       chamber_radius_inner,
                       chamber_radius_inner,
                       chamber_radius_inner,
                       chamber_radius_inner,
                       0 * cm,
                       0 * cm};

  G4double rOuter[] = {chamber_radius_outer_1, chamber_radius_outer_1, chamber_radius_outer_2, chamber_radius_outer_2, chamber_radius_outer_3,
                       chamber_radius_outer_3, chamber_radius_outer_4, chamber_radius_outer_4, chamber_radius_outer_3, chamber_radius_outer_3,
                       chamber_radius_outer_2, chamber_radius_outer_2, chamber_radius_outer_1, chamber_radius_outer_1};

  G4Polycone* smallChamber = new G4Polycone("bigChamber", 0 * degree, 360 * degree, 14, z, rInner, rOuter);

  G4LogicalVolume* smallChamber_logical = new G4LogicalVolume(smallChamber, fSmallChamberMaterial, "smallChamber_logical");

  G4VisAttributes* detVisAtt = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));
  detVisAtt->SetForceWireframe(true);
  detVisAtt->SetForceSolid(true);
  smallChamber_logical->SetVisAttributes(detVisAtt);

  G4ThreeVector loc = DetectorConstants::GetChamberCenter();
  G4Transform3D transform(rot, loc);
  new G4PVPlacement(transform, smallChamber_logical, "smallChamberGeom", fWorldLogical, true, 0, checkOverlaps);

  G4Tubs* xadFilling = new G4Tubs("xadFilling", 0 * cm, chamber_radius_inner - 0.01 * cm, xadFilling_halfthickness, 0 * degree, 360 * degree);
  G4LogicalVolume* xadFilling_logical = new G4LogicalVolume(xadFilling, fXADMaterial, "xadFilling_logical");
  G4VisAttributes* xadVisAtt = new G4VisAttributes(G4Colour(0.2, 0.3, 0.5));
  xadVisAtt->SetForceWireframe(true);
  xadVisAtt->SetForceSolid(true);
  xadFilling_logical->SetVisAttributes(xadVisAtt);

  new G4PVPlacement(transform, xadFilling_logical, "xadFillingGeom", fWorldLogical, true, 0, checkOverlaps);

  // Vacuum
  G4Tubs* smallChamber_vac = new G4Tubs("smallChamber_vac", 0 * cm, vacuumChamber_outerRadius, vacuumChamber_halfLength, 0 * degree, 360 * degree);

  G4LogicalVolume* Run5Vac_logical = new G4LogicalVolume(smallChamber_vac, fVacuum, "Run5Vac_logical");

  new G4PVPlacement(transform, Run5Vac_logical, "smallChamber_vacuum", smallChamber_logical, true, 0, checkOverlaps);
}

/**
 * Method for construction of setup used in Run 6
 * Parameters taken from http://koza.if.uj.edu.pl/petwiki/images/7/71/AnnihilationChamberRun6.pdf
 * below main values are written. Please note that chamber is symmetric in z,
 * therefore only half of the table is presented.
 *  z        | r Inner  | r Outer | Comment
 * ----------|----------|---------|-------
 *  -31.0*cm | 0*cm     | 12.7*cm | chamber end
 *  -29.0*cm | 0*cm     | 12.7*cm | start slope before endcup
 *  -28.9*cm | 0*cm     | 15*cm   | stop slope before endcup
 *  -28.0*cm | 0*cm     | 15*cm   | endcup
 *  -27.9*cm | 12.2*cm  | 12.5*cm | end slope after endcup and start empty inside
 */
void DetectorConstruction::ConstructTargetRun6()
{
  //! Annihilation Chamber part
  const double chamber_radius_inner = 12.2 * cm;
  const double chamber_radius_outer = 12.5 * cm;
  const double chamber_endcup_min = 12.7 * cm;
  const double chamber_endcup_max = 15.0 * cm;
  const double xad_z_coverage = 27.5 * cm;
  const double xad_halfthickness = 0.2 * cm;
  const double source_holder_connector_halfthickness = 1.125 * mm;
  const double source_holder_connector_width = 4.5 * mm;
  const double source_holder_connector_height = 44.5 * mm;
  const double source_holder_radius_inner = 24 * mm;
  const double source_holder_radius_outer = 31 * mm;
  const double source_holder_radius_halfthickness = 1.78 * mm;
  const double kapton_foil_halfthickness = 0.1 * cm;

  G4RotationMatrix rot = G4RotationMatrix();

  G4double z[] = {-31.0 * cm, -29.0 * cm, -28.9 * cm, -28 * cm, -27.9 * cm, 27.9 * cm, 28 * cm, 28.9 * cm, 29 * cm, 31 * cm};

  G4double rInner[] = {0 * cm, 0 * cm, 0 * cm, 0 * cm, chamber_radius_inner, chamber_radius_inner, 0 * cm, 0 * cm, 0 * cm, 0 * cm};

  G4double rOuter[] = {chamber_endcup_min,   chamber_endcup_min, chamber_endcup_max, chamber_endcup_max, chamber_radius_outer,
                       chamber_radius_outer, chamber_endcup_max, chamber_endcup_max, chamber_endcup_min, chamber_endcup_min};

  G4Polycone* bigChamber = new G4Polycone("bigChamber", 0 * degree, 360 * degree, 10, z, rInner, rOuter);

  G4LogicalVolume* bigChamber_logical = new G4LogicalVolume(bigChamber, fPlexiglass, "bigChamber_logical");

  G4VisAttributes* detVisAtt = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));
  detVisAtt->SetForceWireframe(true);
  detVisAtt->SetForceSolid(true);
  bigChamber_logical->SetVisAttributes(detVisAtt);

  G4ThreeVector loc = DetectorConstants::GetChamberCenter();
  G4Transform3D transform(rot, loc);
  new G4PVPlacement(transform, bigChamber_logical, "bigChamberGeom", fWorldLogical, true, 0, checkOverlaps);

  G4Tubs* ringInner =
      new G4Tubs("ringInner", source_holder_radius_inner, source_holder_radius_outer, source_holder_radius_halfthickness, 0 * degree, 360 * degree);

  G4Box* conn = new G4Box("conn", source_holder_connector_height, source_holder_connector_width, source_holder_connector_halfthickness);

  G4LogicalVolume* conn_logical = new G4LogicalVolume(conn, fAluminiumMaterial, "conn_logical");
  conn_logical->SetVisAttributes(detVisAtt);

  G4ThreeVector loc2;
  G4Transform3D transform2;

  loc2 = G4ThreeVector(75. * mm, 0.0, 0.0);
  transform2 = G4Transform3D(rot, loc2);
  G4UnionSolid* unionSolid = new G4UnionSolid("c1", ringInner, conn, transform2);

  loc2 = G4ThreeVector(-53. * mm, 53. * mm, 0.0);
  transform2 = G4Transform3D(rot.rotateZ(-45 * degree), loc2);
  unionSolid = new G4UnionSolid("c2", unionSolid, conn, transform2);

  loc2 = G4ThreeVector(-53. * mm, -53. * mm, 0.0);
  transform2 = G4Transform3D(rot.rotateZ(90 * degree), loc2);
  unionSolid = new G4UnionSolid("c3", unionSolid, conn, transform2);

  G4LogicalVolume* unionSolid_logical = new G4LogicalVolume(unionSolid, fAluminiumMaterial, "union_logical");
  unionSolid_logical->SetVisAttributes(detVisAtt);

  new G4PVPlacement(transform, unionSolid_logical, "bigChamberInnerStructure", fWorldLogical, true, 0, checkOverlaps);

  //! XAD filling part
  G4Tubs* xadFilling = new G4Tubs("xadFilling", chamber_radius_inner - 0.1 * cm - xad_halfthickness, chamber_radius_inner - 0.1 * cm, xad_z_coverage,
                                  0 * degree, 360 * degree);
  G4LogicalVolume* xadFilling_logical = new G4LogicalVolume(xadFilling, fXADMaterial, "xadFilling_logical");
  G4VisAttributes* xadVisAtt = new G4VisAttributes(G4Colour(0.2, 0.3, 0.5));
  xadVisAtt->SetForceWireframe(true);
  xadVisAtt->SetForceSolid(true);
  xadFilling_logical->SetVisAttributes(xadVisAtt);

  new G4PVPlacement(transform, xadFilling_logical, "xadFillingGeom", fWorldLogical, true, 0, checkOverlaps);

  //! Kapton foil part
  G4Tubs* kaptonFilling =
      new G4Tubs("kaptonFilling", 0. * cm, source_holder_radius_inner - 0.01 * mm, kapton_foil_halfthickness, 0 * degree, 360 * degree);
  G4LogicalVolume* kaptonFilling_logical = new G4LogicalVolume(kaptonFilling, fKapton, "kaptonFilling_logical");
  G4VisAttributes* kaptonVisAtt = new G4VisAttributes(G4Colour(0.2, 0.3, 0.5));
  kaptonVisAtt->SetForceWireframe(true);
  kaptonVisAtt->SetForceSolid(true);
  kaptonFilling_logical->SetVisAttributes(kaptonVisAtt);

  new G4PVPlacement(transform, kaptonFilling_logical, "kaptonFillingGeom", fWorldLogical, true, 0, checkOverlaps);

  // Vacuum
  G4Tubs* bigChamber_Vacuum = new G4Tubs("bigChamber_Vacuum", 0. * cm, chamber_radius_inner - 0.1 * cm - xad_halfthickness - 0.1 * cm, xad_z_coverage,
                                         0 * degree, 360 * degree);

  G4LogicalVolume* bigChamberVac_logical = new G4LogicalVolume(bigChamber_Vacuum, fVacuum, "bigChamberVac_logical");

  new G4PVPlacement(transform, bigChamberVac_logical, "bigChamberVac", bigChamber_logical, true, 0, checkOverlaps);
}

void DetectorConstruction::ConstructTargetRun7()
{
  G4RotationMatrix rot = G4RotationMatrix();

  G4double z[] = {-8.8 * cm, -7.8 * cm, -7.8 * cm, -6.6 * cm, -6.6 * cm, -5.1 * cm, -4.9 * cm, -3.1 * cm, -2.8 * cm, -0.57 * cm,
                  0.57 * cm, 2.8 * cm,  3.1 * cm,  4.9 * cm,  5.1 * cm,  6.6 * cm,  6.6 * cm,  7.8 * cm,  7.8 * cm,  8.8 * cm};

  G4double rInner[] = {0. * cm,  0.6 * cm,  0.6 * cm, 0.6 * cm, 0.6 * cm, 0.6 * cm, 0.6 * cm, 0.6 * cm, 0.54 * cm, 0.5 * cm,
                       0.5 * cm, 0.54 * cm, 0.6 * cm, 0.6 * cm, 0.6 * cm, 0.6 * cm, 0.6 * cm, 0.6 * cm, 0.6 * cm,  0. * cm};

  G4double rOuter[] = {1.75 * cm, 1.75 * cm, 1.35 * cm, 1.35 * cm, 1.0 * cm, 1.0 * cm, 1.0 * cm,  1.0 * cm,  0.6 * cm,  0.6 * cm,
                       0.6 * cm,  0.6 * cm,  1.0 * cm,  1.0 * cm,  1.0 * cm, 1.0 * cm, 1.35 * cm, 1.35 * cm, 1.75 * cm, 1.75 * cm};

  G4Polycone* smallChamberRun7 = new G4Polycone("smallchamberRun7", 0 * degree, 360 * degree, 19, z, rInner, rOuter);

  G4LogicalVolume* smallChamber_logical = new G4LogicalVolume(smallChamberRun7, fSmallChamberRun7Material, "smallChamberRun7_logical");

  G4VisAttributes* detVisAtt = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));
  detVisAtt->SetForceWireframe(true);
  detVisAtt->SetForceSolid(true);
  smallChamber_logical->SetVisAttributes(detVisAtt);

  G4ThreeVector loc = DetectorConstants::GetChamberCenter();
  G4Transform3D transform(rot, loc);
  new G4PVPlacement(transform, smallChamber_logical, "smallChamberRun7_logical", fWorldLogical, true, 0, checkOverlaps);

  G4Tubs* xadFilling = new G4Tubs("xadFilling", 0 * cm, 0.4 * cm, 0.6 * cm, 0 * degree, 360 * degree);

  G4LogicalVolume* xadFilling_logical = new G4LogicalVolume(xadFilling, fXADMaterial, "xadFilling_logical");

  G4VisAttributes* xadVisAtt = new G4VisAttributes(G4Colour(0.2, 0.3, 0.5));
  xadVisAtt->SetForceWireframe(true);
  xadVisAtt->SetForceSolid(true);
  xadFilling_logical->SetVisAttributes(xadVisAtt);

  new G4PVPlacement(transform, xadFilling_logical, "xadFillingGeom", fWorldLogical, true, 0, checkOverlaps);

  // Vacuum
  const double vacuumChamber_halfLength = 2.79 * cm;
  //! vacuumChamber_halfLength is slightly less than effective half length of target chamber (2.8 cm)
  const double vacuumChamber_outerRadius = 0.53 * cm;
  //! vacuumChamber_halfLength is slightly less than inner radius of target chamber (0.54 cm)

  G4Tubs* smallChamberRun7_vac =
      new G4Tubs("smallChamberRun7_vac", 0 * cm, vacuumChamber_outerRadius, vacuumChamber_halfLength, 0 * degree, 360 * degree);

  G4LogicalVolume* Run7Vac_logical = new G4LogicalVolume(smallChamberRun7_vac, fVacuum, "Run7Vac_logical");

  new G4PVPlacement(transform, Run7Vac_logical, "smallChamberRun7_vacuum", smallChamber_logical, true, 0, checkOverlaps);
}

/**
 * Method for construction of setup used for cylinder and endcaps
 * in sphericalChamber construction. Parameters taken from fig: 3.3.5
 * http://koza.if.uj.edu.pl/petwiki/images/2/25/PET_UJ_Report_no_13_2017_-_13_2017.pdf
 * below main values are written. Please note that chamber is symmetric in z,
 * therefore only half of the table is presented.
 *  z        | r Inner  | r Outer | Comment
 * ----------|----------|---------|-------
 *  -21.9*cm | 0*cm     | 12.7*cm | endcap end
 *  -21.8*cm | 0*cm     | 14*cm   | stop slope before endcap end
 *  -20.2*cm | 0*cm     | 14*cm   | start slope after endcap
 *  -20.1*cm | 0*cm     | 12.7*cm | endcap start
 *  -20.8*cm | 12*cm    | 12.3*cm | outer cylinder
 */
void DetectorConstruction::ConstructTargetRun12()
{
  const double cyd_radius_inner = 12.0 * cm;
  const double cyd_radius_outer = 12.3 * cm;
  const double cyd_endcap_min = 12.7 * cm;
  const double cyd_endcap_max = 14.0 * cm;

  G4double z_cyd[] = {-20.8 * cm, 20.8 * cm};
  G4double rInner_cyd[] = {cyd_radius_inner, cyd_radius_inner};
  G4double rOuter_cyd[] = {cyd_radius_outer, cyd_radius_outer};

  G4Polycone* cydChamber = new G4Polycone("cydChamber", 0 * degree, 360 * degree, 2, z_cyd, rInner_cyd, rOuter_cyd);

  G4LogicalVolume* cydChamber_logical = new G4LogicalVolume(cydChamber, fSiliconDioxide, "cydChamber_logical");

  G4VisAttributes* detVisAtt = new G4VisAttributes(G4Colour(0.8, 0.8, 0.0));
  detVisAtt->SetForceWireframe(true);
  detVisAtt->SetForceSolid(false);
  cydChamber_logical->SetVisAttributes(detVisAtt);

  G4RotationMatrix rot = G4RotationMatrix();
  G4ThreeVector loc = DetectorConstants::GetChamberCenter();
  G4Transform3D transform(rot, loc);

  new G4PVPlacement(transform, cydChamber_logical, "cydChamberGeom", fWorldLogical, true, 0, checkOverlaps);

  G4double z_cydVacuum[] = {-20. * cm, 20. * cm};
  G4double rOuter_cydVacuum[] = {cyd_radius_inner, cyd_radius_inner};
  G4double rInner_cydVacuum[] = {0 * cm, 0 * cm};

  G4Polycone* cydChamberVacuum = new G4Polycone("cydChamberVacuum", 0 * degree, 360 * degree, 2, z_cydVacuum, rInner_cydVacuum, rOuter_cydVacuum);

  G4LogicalVolume* cydChamberVac_logical = new G4LogicalVolume(cydChamberVacuum, fVacuum, "cydChamberVac_logical");

  new G4PVPlacement(transform, cydChamberVac_logical, "cydChamberVac", cydChamber_logical, true, 0, checkOverlaps);

  //  End Caps
  G4double z_endCap[] = {-21.9 * cm, -21.8 * cm, -20.2 * cm, -20.1 * cm};
  G4double rInner_endCap[] = {0. * cm, 0. * cm, 0. * cm, 0. * cm};
  G4double rOuter_endCap[] = {cyd_endcap_min, cyd_endcap_max, cyd_endcap_max, cyd_radius_outer};

  G4Polycone* endCap = new G4Polycone("endCap", 0 * degree, 360 * degree, 4, z_endCap, rInner_endCap, rOuter_endCap);

  G4LogicalVolume* endCap_logical = new G4LogicalVolume(endCap, fStainlessSteel, "endCap_logical");

  G4VisAttributes* endCapVisAtt = new G4VisAttributes(G4Colour(0.2, 0.4, 0.0));
  endCapVisAtt->SetForceWireframe(true);
  endCapVisAtt->SetForceSolid(false);
  endCap_logical->SetVisAttributes(endCapVisAtt);

  new G4PVPlacement(transform, endCap_logical, "endCapGeom", fWorldLogical, true, 0, checkOverlaps);

  G4RotationMatrix rot_endCap = G4RotationMatrix();
  rot_endCap.rotateY(180 * degree);
  G4Transform3D transform_endCap(rot_endCap, loc);

  new G4PVPlacement(transform_endCap, endCap_logical, "endCapGeom", fWorldLogical, true, 0, checkOverlaps);

  // SphericalChamber
  const double kapton_foil_radius_outer = 24 * mm;
  const double kapton_foil_halfthickness = 0.1 * cm;
  const double sphChamber_radius_inner = 97. * mm;
  const double sphChamber_radius_outer = 100. * mm;
  const double silica_filling_radius_inner = 95. * mm;

  G4Sphere* sphericalChamber =
      new G4Sphere("Sphere", sphChamber_radius_inner, sphChamber_radius_outer, 0 * degree, 360 * degree, 0 * degree, 360 * degree);

  G4LogicalVolume* sphericalChamber_logical = new G4LogicalVolume(sphericalChamber, fPlexiglass, "sphericalChamber_logical");

  G4ThreeVector loc_sphere = G4ThreeVector(0. * mm, 0. * mm, 0. * mm);
  G4Transform3D transform_sphere(rot, loc_sphere);

  new G4PVPlacement(transform_sphere, sphericalChamber_logical, "Sphere", fWorldLogical, true, 0, checkOverlaps);

  G4VisAttributes* sphChamberVisAtt = new G4VisAttributes(G4Colour(1., 0., 0.));
  sphChamberVisAtt->SetForceWireframe(true);
  sphChamberVisAtt->SetForceSolid(false);

  sphericalChamber_logical->SetVisAttributes(sphChamberVisAtt);

  G4Sphere* silicaFilling = new G4Sphere("silicaFilling", silica_filling_radius_inner, sphChamber_radius_inner - 0.1 * mm, 0 * degree, 360 * degree,
                                         0 * degree, 360 * degree);

  G4LogicalVolume* silicaFilling_logical = new G4LogicalVolume(silicaFilling, fSiliconDioxide, "silicaFilling_logical");

  new G4PVPlacement(transform_sphere, silicaFilling_logical, "silicaFillingGeom", fWorldLogical, true, 0, checkOverlaps);

  G4VisAttributes* silicaVisAtt = new G4VisAttributes(G4Colour(0.2, 0.3, 0.5));
  silicaVisAtt->SetForceWireframe(true);
  silicaVisAtt->SetForceSolid(false);
  silicaFilling_logical->SetVisAttributes(silicaVisAtt);

  G4Sphere* sphere_vacuum =
      new G4Sphere("vacuumSphere", 0 * mm, silica_filling_radius_inner - 0.1 * mm, 0 * degree, 360 * degree, 0 * degree, 360 * degree);

  G4LogicalVolume* sphereVacuum_logical = new G4LogicalVolume(sphere_vacuum, fVacuum, "vacuumSphere");

  new G4PVPlacement(transform_sphere, sphereVacuum_logical, "vacuumSphere", sphericalChamber_logical, true, 0, checkOverlaps);

  // 4 Chamber Bolts
  G4double z[] = {-14.0 * mm, 14.0 * mm};
  G4double rInner[] = {0 * mm, 0 * mm};
  G4double rOuter[] = {2.0 * mm, 2.0 * mm};

  G4Polycone* bolt = new G4Polycone("Bolts", 0 * degree, 360 * degree, 2, z, rInner, rOuter);

  G4LogicalVolume* bolt_logical = new G4LogicalVolume(bolt, fPolyoxymethylene, "bolt_logical");

  G4VisAttributes* boltsVisAtt = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8));
  boltsVisAtt->SetForceWireframe(true);
  boltsVisAtt->SetForceSolid(false);
  bolt_logical->SetVisAttributes(boltsVisAtt);

  G4ThreeVector loc1, loc2, loc3, loc4;
  G4Transform3D transform1, transform2, transform3, transform4;

  loc1 = G4ThreeVector(79. * mm, 78. * mm, 0.0);
  G4RotationMatrix rot1 = G4RotationMatrix();
  rot1.rotateX(90 * degree);
  rot1.rotateY(45 * degree);
  rot1.rotateZ(-45 * degree);
  transform1 = G4Transform3D(rot1, loc1);

  new G4PVPlacement(transform1, bolt_logical, "Bolts", fWorldLogical, true, 0, checkOverlaps);

  loc2 = G4ThreeVector(-79. * mm, 78. * mm, 0.0);
  G4RotationMatrix rot2 = G4RotationMatrix();
  rot2.rotateX(90 * degree);
  rot2.rotateY(45 * degree);
  rot2.rotateZ(45 * degree);
  transform2 = G4Transform3D(rot2, loc2);

  new G4PVPlacement(transform2, bolt_logical, "Bolts", fWorldLogical, true, 0, checkOverlaps);

  loc3 = G4ThreeVector(-79. * mm, -78. * mm, 0.0);
  G4RotationMatrix rot3 = G4RotationMatrix();
  rot3.rotateX(90 * degree);
  rot3.rotateY(45 * degree);
  rot3.rotateZ(-45 * degree);
  transform3 = G4Transform3D(rot3, loc3);

  new G4PVPlacement(transform3, bolt_logical, "Bolts", fWorldLogical, true, 0, checkOverlaps);

  loc4 = G4ThreeVector(79. * mm, -78. * mm, 0.0);
  G4RotationMatrix rot4 = G4RotationMatrix();
  rot4.rotateX(90 * degree);
  rot4.rotateY(45 * degree);
  rot4.rotateZ(45 * degree);
  transform4 = G4Transform3D(rot4, loc4);

  new G4PVPlacement(transform4, bolt_logical, "Bolts", fWorldLogical, true, 0, checkOverlaps);

  // Kapton Foil
  G4Tubs* kaptonFilling =
      new G4Tubs("kaptonFilling", 0. * cm, kapton_foil_radius_outer - 0.01 * mm, kapton_foil_halfthickness, 0 * degree, 360 * degree);

  G4LogicalVolume* kaptonFilling_logical = new G4LogicalVolume(kaptonFilling, fKapton, "kaptonFilling_logical");

  G4VisAttributes* kaptonVisAtt = new G4VisAttributes(G4Colour(0.65, 0.7, 0.0));
  kaptonVisAtt->SetForceWireframe(true);
  kaptonVisAtt->SetForceSolid(false);
  kaptonFilling_logical->SetVisAttributes(kaptonVisAtt);

  new G4PVPlacement(transform, kaptonFilling_logical, "kaptonFillingGeom", fWorldLogical, true, 0, checkOverlaps);

  // Ring between Spherical Chamber and outer cylinder
  auto cad_ring_mesh = CADMesh::TessellatedMesh::FromSTL((char*)"stl_geometry/Ring_SphericalChamber.ascii.stl");
  cad_ring_mesh->SetScale(cm);

  G4VSolid* cad_ring_solid = cad_ring_mesh->GetSolid();
  G4LogicalVolume* cadRing_logical = new G4LogicalVolume(cad_ring_solid, fPolyoxymethylene, "cadRing_logical");

  G4VisAttributes* ringVisAtt = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8));
  ringVisAtt->SetForceWireframe(true);
  ringVisAtt->SetForceSolid(false);
  cadRing_logical->SetVisAttributes(ringVisAtt);

  G4RotationMatrix rot_ring = G4RotationMatrix();
  rot_ring.rotateZ(45 * deg);
  G4ThreeVector loc_ring = DetectorConstants::GetChamberCenter();
  G4Transform3D transform_ring(rot_ring, loc_ring);

  new G4PVPlacement(transform_ring, cadRing_logical, "cadRingGeom", fWorldLogical, true, 0, checkOverlaps);
}

void DetectorConstruction::CreateGeometryFile()
{
  using namespace std;

  G4cout << "--- Saving simulated geometry to " << fGeometryFileName << " ---" << G4endl;
  auto runNumStr = to_string(fRunNumber + 90);
  pt::ptree jsonFile, full, partial, element;

  if (fGeometryFileType == "barrel")
  {
    G4cout << "--- Creating geometry file in Framework Big Barrel Format ---" << G4endl;

    //! Adding Frame object - only one, always the same
    element.put("id", 1);
    element.put("creator_id", 1);
    element.put("status", "OK");
    element.put("description", "Setup generated with J-PET Geant4 software");
    element.put("active", "true");
    element.put("version", 1);
    partial.push_back(make_pair("", element));
    full.add_child("frames", partial);

    //! Adding scintillators from collection of Scin objects added during construction
    partial.clear();
    for (unsigned i = 0; i < fScinContainer.size(); i++)
    {
      auto id = to_string(fScinContainer[i].fID);
      auto bsID = to_string(fScinContainer[i].fSlotID);
      auto height = to_string(fScinContainer[i].fHeight);
      height = height.substr(0, height.find(".", 0) + 3);
      auto width = to_string(fScinContainer[i].fWidth);
      width = width.substr(0, width.find(".", 0) + 3);
      auto length = to_string(fScinContainer[i].fLength);
      length = length.substr(0, length.find(".", 0) + 3);

      element.clear();
      element.put("id", id);
      element.put("barrelSlots_id", bsID);
      element.put("height", height);
      element.put("width", width);
      element.put("length", length);
      element.put("attenuation_length", "0");
      partial.push_back(make_pair("", element));
    }
    full.add_child("scintillators", partial);

    //! Adding slots
    partial.clear();
    for (unsigned i = 0; i < fSlotContainer.size(); i++)
    {
      auto id = to_string(fSlotContainer[i].fID);
      auto layerID = to_string(fSlotContainer[i].fLayerID);
      auto theta = to_string(fSlotContainer[i].fTheta);
      theta = theta.substr(0, theta.find(".", 0) + 3);

      element.clear();
      element.put("id", id);
      element.put("frame_id", "1");
      element.put("name", "1");
      element.put("layers_id", layerID);
      element.put("theta1", theta);
      element.put("active", "true");

      partial.push_back(make_pair("", element));
    }
    full.add_child("barrelSlots", partial);

    //! Adding layers
    partial.clear();
    for (unsigned i = 0; i < fLayerContainer.size(); i++)
    {
      auto id = to_string(fLayerContainer[i].fID);
      auto name = fLayerContainer[i].fName;
      auto radius = to_string(fLayerContainer[i].fRadius);
      radius = radius.substr(0, radius.find(".", 0) + 3);
      auto frameID = to_string(fLayerContainer[i].fSetupID);

      element.clear();
      element.put("id", id);
      element.put("name", name);
      element.put("radius", radius);
      element.put("frames_id", frameID);
      element.put("active", "true");

      partial.push_back(make_pair("", element));
    }
    full.add_child("layers", partial);

    jsonFile.add_child(runNumStr, full);
    pt::write_json(fGeometryFileName, jsonFile);
  }
  else if (fGeometryFileType == "modular")
  {

    G4cout << "--- Creating geometry file in Framework Modular Format ---" << G4endl;

    //! Adding Setup object - only one, always the same
    element.put("id", 1);
    element.put("description", "Setup generated with J-PET Geant4 software");
    partial.push_back(make_pair("", element));
    full.add_child("setup", partial);

    //! Adding Layers
    partial.clear();
    for (unsigned i = 0; i < fLayerContainer.size(); i++)
    {
      auto id = to_string(fLayerContainer[i].fID);
      auto name = fLayerContainer[i].fName;
      auto radius = to_string(fLayerContainer[i].fRadius);
      radius = radius.substr(0, radius.find(".", 0) + 3);
      auto setupID = to_string(fLayerContainer[i].fSetupID);

      element.clear();
      element.put("id", id);
      element.put("name", name);
      element.put("radius", radius);
      element.put("setup_id", setupID);
      partial.push_back(make_pair("", element));
    }
    full.add_child("layer", partial);

    //! Adding scintillators
    partial.clear();
    for (unsigned i = 0; i < fScinContainer.size(); i++)
    {
      auto id = to_string(fScinContainer[i].fID);
      auto slotID = to_string(fScinContainer[i].fSlotID);
      auto height = to_string(fScinContainer[i].fHeight);
      height = height.substr(0, height.find(".", 0) + 3);
      auto width = to_string(fScinContainer[i].fWidth);
      width = width.substr(0, width.find(".", 0) + 3);
      auto length = to_string(fScinContainer[i].fLength);
      length = length.substr(0, length.find(".", 0) + 3);
      auto x_center = to_string(fScinContainer[i].fX_center);
      x_center = x_center.substr(0, x_center.find(".", 0) + 3);
      auto y_center = to_string(fScinContainer[i].fY_center);
      y_center = y_center.substr(0, y_center.find(".", 0) + 3);
      auto z_center = to_string(fScinContainer[i].fZ_center);
      z_center = z_center.substr(0, z_center.find(".", 0) + 3);

      element.clear();
      element.put("id", id);
      element.put("slot_id", slotID);
      element.put("height", height);
      element.put("width", width);
      element.put("length", length);
      element.put("xcenter", x_center);
      element.put("ycenter", y_center);
      element.put("zcenter", z_center);
      partial.push_back(make_pair("", element));
    }
    full.add_child("scin", partial);

    //! Adding slots
    partial.clear();
    for (unsigned i = 0; i < fSlotContainer.size(); i++)
    {
      auto id = to_string(fSlotContainer[i].fID);
      auto layerID = to_string(fSlotContainer[i].fLayerID);
      auto theta = to_string(fSlotContainer[i].fTheta);
      theta = theta.substr(0, theta.find(".", 0) + 3);
      auto type = fSlotContainer[i].fType;

      element.clear();
      element.put("id", id);
      element.put("layer_id", layerID);
      element.put("theta", theta);
      element.put("type", type);
      partial.push_back(make_pair("", element));
    }
    full.add_child("slot", partial);

    jsonFile.add_child(runNumStr, full);
    pt::write_json(fGeometryFileName, jsonFile);
  }
  else
  {
    G4Exception("DetectorConstruction", "DC03", JustWarning, "Unrecognized geometry file type, accepted formats: barrel, modular.");
  }
}
