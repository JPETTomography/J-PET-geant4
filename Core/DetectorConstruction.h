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
 *  @file DetectorConstruction.h
 */

#ifndef DETECTORCONSTRUCTION_H
#define DETECTORCONSTRUCTION_H 1

#include "DetectorSD.h"
#include "MaterialExtension.h"

#include <G4Box.hh>
#include <G4Cache.hh>
#include <G4Colour.hh>
#include <G4Element.hh>
#include <G4GeometryManager.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4MaterialTable.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4VPhysicalVolume.hh>
#include <G4VUserDetectorConstruction.hh>
#include <G4VisAttributes.hh>
#include <globals.hh>
#include <vector>

class DetectorConstructionMessenger;
struct Layer;
struct Scin;
struct Slot;
struct PhantElem;

enum GeometryCombination {
  aUnion, aSubtraction, aIntersection
};

enum GeometryShape {
  aBox, aSphere, aOrb, aTube, aEllTube
};

enum PhantomMaterial {
  aWater, aPlastic, aAlumnium, aSiliconDioxide
};

//! Flag for debugging purposes
const G4bool checkOverlaps = false;

/**
 * @class DetectorConstruction
 * @brief creating detector; can read the CAD geometry
 */
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  enum GeometryKind
  {
    Unknown,
    Geo24ModulesLayer,
    Geo24ModulesLayerDistributed
  };

  //! only single instance can exist
  static DetectorConstruction* GetInstance();
  void SetHistoManager(HistoManager* histo);
  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();
  void LoadGeometryForRun(G4int nr);
  G4int getNumberOfScintillators();
  void UpdateGeometry();
  void ReloadMaterials(const G4String& material);
  void SetPressureInChamber(G4double pressure) { fPressure = pressure; };

  //! Basic geometry with 3 layers of scintillators
  void ConstructBasicGeometry(G4bool tf) { fLoadScintillators = tf; };
  void LoadFrame(G4bool tf) { fLoadCADFrame = tf; };
  void LoadWrapping(G4bool tf) { fLoadWrapping = tf; };

  //! Modular layer (known as 4th layer); 24 modules filled with scintillators
  void ConstructModularLayer(const G4String& module_name)
  {
    fLoadModularLayer = true;
    if (module_name == "Single")
    {
      fGeoKind = GeometryKind::Geo24ModulesLayer;
    }
    else if (module_name == "Double")
    {
      fGeoKind = GeometryKind::Geo24ModulesLayerDistributed;
    }
    else
    {
      fLoadModularLayer = false;
      fGeoKind = GeometryKind::Unknown;
    }
  }

  //! Writing out detector setup in json format
  void CreateGeometryFileFlag(G4bool tf) { fCreateGeometryFile = tf; };
  void SetGeometryFileName(G4String fileName) { fGeometryFileName = fileName; };
  void SetGeometryFileType(G4String type) { fGeometryFileType = type; };
  void CreateGeometryFile();
  // Reading JSON setup
  void readJSONSetup(G4bool readSetup) { fReadJSONSetup = readSetup; };
  void setJSONFileName(G4String fileName) { fJSONSetupFileName = fileName; };
  void setJSONSetupRunNum(G4int setJSONSetupRunNum) { fJSONSetupRunNum = setJSONSetupRunNum; };

  void setNemaPhantomFlag(G4bool tf) { fConstructNemaPhantom = tf; };

  void ConstructFromSetupFile(G4String fileName);

  G4int GetRunNumber() const { return fRunNumber; };

  void addPhantomElementWithShape(G4int id, G4String shape);
  void setContructionFlagTrue(G4int id);
  void setPhantomElementDimensions(G4int id, G4String stringWithParameters);
  void setPhantomElementLocation(G4int id, G4double x, G4double y, G4double z);
  void setPhantomElementRotation(G4int id, G4double xRot, G4double yRot, G4double zRot);
  void setPhantomElementAction(G4int id1, G4int id2, G4String action);
  void setPhantomElementMaterial(G4int id, G4String materialName, G4double density);

  G4VPhysicalVolume* GetPhantElement(G4int id)
  {
    auto search = idPhantElem.find(id);
    if (search != idPhantElem.end()) {
      return phantomElementsPhysVolumes.at(idPhantElem.at(id));
    } else
      return nullptr;
  }
private:
  static G4ThreadLocal G4bool fConstructedSDandField;
  static DetectorConstruction* fInstance;

  DetectorConstruction();
  virtual ~DetectorConstruction();
  DetectorConstructionMessenger* fMessenger = nullptr;
  HistoManager* fHistoManager;

  //! Load materials from NIST database
  void InitializeMaterials();
  //! Load detector elements from CAD files
  void ConstructFrameCAD();
  //! Create scintillators only; dimensions are right now fixed in code
  void ConstructScintillators();
  //! Construct modular layer inserted into detector (refered as 4th layer)
  void ConstructScintillatorsModularLayer();
  //! Create target used in run3 - big chamber no XAD material inside
  void ConstructTargetRun3();
  //! Create target used in run5 (and run4) - small chamber + XAD material inside
  void ConstructTargetRun5();
  //! Create target used in run6 - big chamber + XAD material inside tube
  void ConstructTargetRun6();
  //! Create target used in run7
  void ConstructTargetRun7();
  //! Create target for run12
  void ConstructTargetRun12();
  void ConstructNemaPhantom();

  void ConstructLayers(std::vector<G4double>& radius_dynamic, G4int& numberofModules, G4double& AngDisp_dynamic, G4int& icopyI);

  //! Corresponds to JPET measurements; run 0 = user setup
  G4int fRunNumber;
  //! Flag for loading standard 3 layers of the scintillators
  G4bool fLoadScintillators;
  //! Flag for loading frame from CAD file
  G4bool fLoadCADFrame;
  //! Flag for loading wrapping tf the scintillators
  G4bool fLoadWrapping;
  //! Flag for loading modular (4th) layer
  G4bool fLoadModularLayer;
  //! For creating file with geometry, by default not created, if yes, in big barrel format
  G4bool fCreateGeometryFile = false;
  G4String fGeometryFileName = "mc_geant_setup.json";
  G4String fGeometryFileType = "barrel";
  //! Reading JSON file with detector setup
  G4bool fReadJSONSetup = false;
  G4String fJSONSetupFileName = "detector_setup.json";
  G4int fJSONSetupRunNum = 999;
  G4int fMaxCreatedScinID = 0;
  std::vector<G4LogicalVolume*> fStripsFromSetup;

  G4bool fConstructNemaPhantom;

  G4Box* fWorldSolid = nullptr;
  G4LogicalVolume* fWorldLogical = nullptr;
  G4VPhysicalVolume* fWorldPhysical = nullptr;

  MaterialExtension* fAir = nullptr;
  MaterialExtension* fKapton = nullptr;
  MaterialExtension* fVacuum = nullptr;
  MaterialExtension* fPlexiglass = nullptr;
  MaterialExtension* fXADMaterial = nullptr;
  MaterialExtension* fScinMaterial = nullptr;
  MaterialExtension* fAluminiumMaterial = nullptr;
  MaterialExtension* fSmallChamberMaterial = nullptr;
  MaterialExtension* fSmallChamberRun7Material = nullptr;
  //! Vacuum
  G4Material* vacuum = nullptr;

  MaterialExtension* fPolycarbonate = nullptr;
  MaterialExtension* fPolyoxymethylene = nullptr;
  MaterialExtension* fSiliconDioxide = nullptr;
  MaterialExtension* fStainlessSteel = nullptr;

  G4LogicalVolume* fScinLog = nullptr;
  G4LogicalVolume* fScinLogInModule = nullptr;
  G4Cache<DetectorSD*> fDetectorSD;
  //! Geometry Kind for the modular layer
  GeometryKind fGeoKind = GeometryKind::Unknown;
  //! Maximum ID of the scintillators
  G4int fMaxScinID = 1;
  //! Pressure in chamber
  G4double fPressure = 1.e-19 * pascal;

  std::vector<Layer> fLayerContainer;
  std::vector<Scin> fScinContainer;
  std::vector<Slot> fSlotContainer;
  G4int fLayerNumber = 0;

  std::map<G4int, G4int> idPhantElem;
  std::vector<PhantElem> phantomElements;
  std::vector<G4VPhysicalVolume*> phantomElementsPhysVolumes;
};

struct Frame
{
  int fID;
  int fCreatorID;
  int fVersion;
  std::string fStatus;
  std::string fDescription;
  bool fActive;
};

struct Layer
{
  int fID;
  std::string fName;
  double fRadius;
  int fSetupID;
  Layer(int id, const std::string& name, double radius, int setupID) : fID(id), fName(name), fRadius(radius), fSetupID(setupID) {}
};

struct Slot
{
  int fID;
  int fLayerID;
  double fTheta;
  std::string fType;
  Slot(int id, int layerID, double theta, const std::string& type) : fID(id), fLayerID(layerID), fTheta(theta), fType(type) {}
};

struct Scin
{
  int fID;
  int fSlotID;
  float fHeight;
  float fWidth;
  float fLength;
  double fX_center;
  double fY_center;
  double fZ_center;
  Scin(int id, int slotID, double height, double width, double length, double x_center, double y_center, double z_center)
      : fID(id), fSlotID(slotID), fHeight(height), fWidth(width), fLength(length), fX_center(x_center), fY_center(y_center), fZ_center(z_center)
  {
  }
};

struct PhantElem
{
  GeometryShape fShape;
  bool fConstruct;
  std::vector<double> fDimensions;
  std::vector<double> fLocation;
  std::vector<double> fRotation;
  PhantomMaterial fMaterial;
  G4double fDensity;
  std::vector<std::pair<G4int, GeometryCombination>> fActionCombination;
};

void replace(std::string& json, const std::string& placeholder);
#endif /* !DETECTORCONSTRUCTION_H */
