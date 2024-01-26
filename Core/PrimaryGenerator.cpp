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
 *  @file PrimaryGenerator.cpp
 */

#include "../Info/PrimaryParticleInformation.h"
#include "../Info/VtxInformation.h"
#include "PrimaryGeneratorConstants.h"
#include "DetectorConstruction.h"
#include "MaterialParameters.h"
#include "DetectorConstants.h"
#include "MaterialExtension.h"
#include "PrimaryGenerator.h"

#include <G4HadPhaseSpaceGenbod.hh>
#include <G4ParticleDefinition.hh>
#include <G4PhysicalConstants.hh>
#include <G4RandomDirection.hh>
#include <G4SystemOfUnits.hh>
#include <G4ParticleTable.hh>
#include <Randomize.hh>
#include <geomdefs.hh>
#include <globals.hh>
#include <TF1.h>

PrimaryGenerator::PrimaryGenerator() : G4VPrimaryGenerator() {}

PrimaryGenerator::~PrimaryGenerator() {}

G4PrimaryVertex* PrimaryGenerator::GenerateThreeGammaVertex( 
  DecayChannel channel, const G4ThreeVector vtxPosition, 
  const G4double T0, const G4double lifetime3g
) {

  G4PrimaryVertex* vertex = new G4PrimaryVertex();
  VtxInformation* info = new VtxInformation();

  G4double lifetime = G4RandExponential::shoot(lifetime3g);
  info->SetThreeGammaGen(true);
  info->SetLifetime((T0 + lifetime));
  info->SetVtxPosition(vtxPosition.x(), vtxPosition.y(), vtxPosition.z());
  vertex->SetUserInformation(info);
  vertex->SetT0(T0 + lifetime);
  vertex->SetPosition(vtxPosition.x(), vtxPosition.y(), vtxPosition.z());

  theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  MaterialExtension* material = dynamic_cast<MaterialExtension*>(
    theNavigator->LocateGlobalPointAndSetup(vtxPosition)->GetLogicalVolume()->GetMaterial()
  );
  info->SetDensity(material->GetDensity());

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDefinition = particleTable->FindParticle("gamma");
  Double_t mass_secondaries[3] = {0., 0., 0.};

  TGenPhaseSpace event;
  TLorentzVector positonium(0.0, 0.0, 0.0, 1022 * keV);
  Bool_t test = event.SetDecay(positonium, 3, mass_secondaries);
  if (!test) {
    G4cout << "error: generate_gamma : createThreeEvts:" << test << G4endl;
  }

  Double_t weight = 1;
  Double_t weight_max = event.GetWtMax() * pow(10, -1);
  Double_t rwt = 1;
  Double_t M_max = 1;
  //Manual how to obtain amplitude for the 3G decays are in the manual_v4.pdf in appendix section
  if (channel == DecayChannel::kPara3G) {
    M_max = 2.00967 * pow(10, 25);
  } else {
    M_max = 7.65928 * pow(10, -6);
  }
  do {
    weight = event.Generate();
    weight = weight * calculate_mQED( channel, 511., event.GetDecay(0)->E() / keV, event.GetDecay(1)->E() / keV, event.GetDecay(2)->E() / keV );
    rwt = M_max * weight_max * (G4UniformRand());
  } while (rwt > weight);

  G4PrimaryParticle* particle[3];
  for (int i = 0; i < 3; i++) {
    TLorentzVector* out = event.GetDecay(i);
    particle[i] = new G4PrimaryParticle(
      particleDefinition, out->Px(), out->Py(), out->Pz(), out->E()
    );
    PrimaryParticleInformation* infoParticle = new PrimaryParticleInformation();
    infoParticle->SetGammaMultiplicity(PrimaryParticleInformation::koPsGamma);
    infoParticle->SetGeneratedGammaMultiplicity(PrimaryParticleInformation::koPsGamma);
    infoParticle->SetIndex(i + 1);
    infoParticle->SetGenMomentum(out->Px(), out->Py(), out->Pz());
    particle[i]->SetUserInformation(infoParticle);
    vertex->SetPrimary(particle[i]);
  }
  return vertex;
}

G4PrimaryVertex* PrimaryGenerator::GenerateTwoGammaVertex(
  const G4ThreeVector vtxPosition, const G4double T0, const G4double lifetime2g
) {
  G4PrimaryVertex* vertex = new G4PrimaryVertex();
  VtxInformation* info = new VtxInformation();

  G4double lifetime = G4RandExponential::shoot(lifetime2g);
  info->SetTwoGammaGen(true);
  info->SetLifetime((T0 + lifetime));
  info->SetVtxPosition(vtxPosition.x(), vtxPosition.y(), vtxPosition.z());
  vertex->SetUserInformation(info);
  vertex->SetT0(T0 + lifetime);
  vertex->SetPosition(vtxPosition.x(), vtxPosition.y(), vtxPosition.z());

  theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  MaterialExtension* material = dynamic_cast<MaterialExtension*>(
    theNavigator->LocateGlobalPointAndSetup(vtxPosition)->GetLogicalVolume()->GetMaterial()
  );
  info->SetDensity(material->GetDensity());

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDefinition = particleTable->FindParticle("gamma");

  Double_t mass_secondaries[2] = {0., 0.};

  TGenPhaseSpace event;
  TLorentzVector positonium(0.0, 0.0, 0.0, 1022 * keV);
  Bool_t test = event.SetDecay(positonium, 2, mass_secondaries);
  if (!test) {
    G4cout << "Error: generate_gamma : createTwoEvts" << test << G4endl;
  }

  event.Generate();
  G4PrimaryParticle* particle[2];

  for (int i = 0; i < 2; i++) {
    TLorentzVector* out = event.GetDecay(i);
    particle[i] = new G4PrimaryParticle(
      particleDefinition, out->Px(), out->Py(), out->Pz(), out->E()
    );

    PrimaryParticleInformation* infoParticle = new PrimaryParticleInformation();
    infoParticle->SetGammaMultiplicity(PrimaryParticleInformation::kBackToBackGamma);
    infoParticle->SetGeneratedGammaMultiplicity(PrimaryParticleInformation::kBackToBackGamma);
    infoParticle->SetIndex(i + 1);
    infoParticle->SetGenMomentum(out->Px(), out->Py(), out->Pz());
    particle[i]->SetUserInformation(infoParticle);
    vertex->SetPrimary(particle[i]);
  }

  G4ThreeVector gammaMom = particle[0]->GetMomentum();
  G4ThreeVector polarization1 = gammaMom.orthogonal().unit();
  polarization1 = polarization1.rotate(twopi * G4UniformRand(), gammaMom);
  particle[1]->SetPolarization(polarization1);

  G4ThreeVector polarization2 = polarization1;
  polarization2 = polarization2.rotate(pi / 2.0, gammaMom);
  particle[0]->SetPolarization(polarization2);
  return vertex;
}

G4PrimaryVertex* PrimaryGenerator::GeneratePromptGammaVertex(
  const G4ThreeVector vtxPosition, const G4double T0,
  const G4double lifetimePrompt, const G4double energy
) {
  G4PrimaryVertex* vertex = new G4PrimaryVertex();
  VtxInformation* info = new VtxInformation();

  G4double lifetime = G4RandExponential::shoot(lifetimePrompt);
  info->SetPromptGammaGen(true);
  info->SetLifetime((T0 + lifetime));
  info->SetVtxPosition(vtxPosition.x(), vtxPosition.y(), vtxPosition.z());
  vertex->SetUserInformation(info);
  vertex->SetT0(T0 + lifetime);
  vertex->SetPosition(vtxPosition.x(), vtxPosition.y(), vtxPosition.z());

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDefinition = particleTable->FindParticle("gamma");
  G4ThreeVector momentum = GetRandomPointOnSphere(energy);
  G4PrimaryParticle* particle1 = new G4PrimaryParticle(
    particleDefinition, momentum.x(), momentum.y(), momentum.z(), energy
  );
  PrimaryParticleInformation* infoParticle = new PrimaryParticleInformation();
  infoParticle->SetGammaMultiplicity(PrimaryParticleInformation::kPromptGamma);
  infoParticle->SetGeneratedGammaMultiplicity(PrimaryParticleInformation::kPromptGamma);
  infoParticle->SetIndex(0);
  infoParticle->SetGenMomentum(momentum.x(), momentum.y(), momentum.z());
  particle1->SetUserInformation(infoParticle);
  vertex->SetPrimary(particle1);
  return vertex;
}

void PrimaryGenerator::GenerateEvtSmallChamber(
  G4Event* event, const G4double effectivePositronRadius
) {
  G4ThreeVector chamberCenter = DetectorConstants::GetChamberCenter();
  G4ThreeVector vtxPosition;
  MaterialExtension* material;
  std::tie(vtxPosition, material) = GetVerticesDistributionInFilledSphere(
    chamberCenter, effectivePositronRadius
  );
  std::vector<G4double> evtFractions = material->GetEventsFraction();
  G4double random = G4UniformRand();

  //! for sodium: emitted positrons have up to 100~keV velocity
  //! therefore their speed v=sqrt(2*e/m) = 0.6c
  G4double T0 = (vtxPosition - chamberCenter).mag() / (0.6 * c_light);

  G4double decayRandom = G4UniformRand();
  //Not all Na decays lead to the emission of positron
  if (decayRandom > MaterialParameters::fSodiumChanceEC + MaterialParameters::fSodiumChanceNoPrompt) {
    if (evtFractions[0] > random) {
      // pPs 2G
      event->AddPrimaryVertex(GenerateTwoGammaVertex(
        vtxPosition, T0,
        material->GetLifetime(random, DecayChannel::kPara2G)
        ));
      fDecayChannel = DecayChannel::kPara2G;
    } else if (evtFractions[0] + evtFractions[1] > random) {
      // Direct 2G
      event->AddPrimaryVertex(GenerateTwoGammaVertex(
        vtxPosition, T0,
        material->GetLifetime(random - evtFractions[0], DecayChannel::kDirect2G)
        ));
      fDecayChannel = DecayChannel::kDirect2G;
    } else if (evtFractions[0] + evtFractions[1] + evtFractions[2] > random) {
      // oPs 2G
      event->AddPrimaryVertex(GenerateTwoGammaVertex(
        vtxPosition, T0,
        material->GetLifetime(random - evtFractions[0] - evtFractions[1], DecayChannel::kOrtho2G)
        ));
      fDecayChannel = DecayChannel::kOrtho2G;
    } else if (evtFractions[0] + evtFractions[1] + evtFractions[2] + evtFractions[3] > random) {
      // pPs 3G
      event->AddPrimaryVertex(GenerateThreeGammaVertex(
        DecayChannel::kPara3G, vtxPosition, T0,
        material->GetLifetime(random - evtFractions[0] - evtFractions[1] - evtFractions[2], DecayChannel::kPara3G)
        ));
      fDecayChannel = DecayChannel::kPara3G;
    } else if (evtFractions[0] + evtFractions[1] + evtFractions[2] + evtFractions[3] + evtFractions[4] > random) {
      // Direct 3G
      event->AddPrimaryVertex(GenerateThreeGammaVertex(
        DecayChannel::kDirect3G, vtxPosition, T0,
        material->GetLifetime(random - evtFractions[0] - evtFractions[1] - evtFractions[2] - evtFractions[3], DecayChannel::kDirect3G)
        ));
      fDecayChannel = DecayChannel::kDirect3G;
    } else {
      // oPs 3G
      event->AddPrimaryVertex(GenerateThreeGammaVertex(
        DecayChannel::kOrtho3G, vtxPosition, T0,
        material->GetLifetime(random - evtFractions[0] - evtFractions[1] - evtFractions[2] - evtFractions[3] - evtFractions[4], DecayChannel::kOrtho3G)
        ));
      fDecayChannel = DecayChannel::kOrtho3G;
    } 
  }

  //Not all Na decays lead to the emission of prompt photon
  if (decayRandom > MaterialParameters::fSodiumChanceNoPrompt) {
    //! Add prompt gamma from sodium
    G4ThreeVector promptVtxPosition = GenerateVertexUniformInCylinder(0.2 * cm, 0.2 * cm) + chamberCenter;
    event->AddPrimaryVertex(GeneratePromptGammaVertex(
        promptVtxPosition, 0.0f, MaterialParameters::fSodiumGammaTau,
        MaterialParameters::fSodiumGammaEnergy
    ));
  }
}

/**
 * @param maximal dimension(/2) of annihilation chamber are taken (to speed up simulatons)
 * @return: vtx position, 2/3g ratio, meanlifetime;
 */
std::tuple<G4ThreeVector, MaterialExtension*>
PrimaryGenerator::GetVerticesDistributionInFilledSphere(
  const G4ThreeVector center, G4double radius
) {
  G4bool vertexFound = false;
  G4ThreeVector vertex(0 * cm, 0 * cm, 0 * cm);
  MaterialExtension* material;

  //! annihilation will occure only in materials where it was allowed
  //! @see MaterialExtension
  //! annihilation rate 2g/3g also depends on the material type
  while (!vertexFound) {
    vertex = GetRandomPointInFilledSphere(radius) + center;
    theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    material = dynamic_cast<MaterialExtension*>(
      theNavigator->LocateGlobalPointAndSetup(vertex)->GetLogicalVolume()->GetMaterial()
    );
    vertexFound = material->IsTarget();
  };
  return std::make_tuple(vertex, material);
}

std::tuple<G4ThreeVector, MaterialExtension*>
PrimaryGenerator::GetVerticesDistributionAlongStepVector(
  const G4ThreeVector center, const G4ThreeVector direction
) {
  G4bool lookForVtx = false;
  G4ThreeVector myPoint;
  G4ThreeVector myNextPoint = center;
  MaterialExtension* mat;
  while (!lookForVtx) {
    myPoint = myNextPoint;
    theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    mat = dynamic_cast<MaterialExtension*>(
      theNavigator->LocateGlobalPointAndSetup(myPoint)->GetLogicalVolume()->GetMaterial()
    );
    lookForVtx = mat->IsTarget();
    myNextPoint = myPoint + direction;
  };
  return std::make_tuple(myPoint, mat);
}

std::tuple<G4ThreeVector, MaterialExtension*>
PrimaryGenerator::GetVerticesDistributionAlongStepVectorExponential(
  const G4ThreeVector center, const G4ThreeVector direction
) {
  G4bool lookForVtx = false;
  G4ThreeVector myPoint;
  MaterialExtension* mat;
  G4double reach = 1 * mm;
  G4double stepNumber = 0.;
  G4double density, probFunction;
  G4double directionMag = direction.mag();
  G4double minDens = 0.1;
  theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  while (!lookForVtx) {
    stepNumber++;
    myPoint = center + direction*stepNumber;
    mat = dynamic_cast<MaterialExtension*>(
      theNavigator->LocateGlobalPointAndSetup(myPoint)->GetLogicalVolume()->GetMaterial()
    );

    if (mat) {
      density = mat->GetDensity() / (g/cm3);
      if (density < minDens)
        density = minDens;
    } else {
      density = minDens;
    }

    reach = 19.378*std::pow(density, -1.53977); // test function of reach (in mm)
    probFunction = 1 - exp(-directionMag*stepNumber/reach);
    if (G4UniformRand() < probFunction)
      lookForVtx = true;
  };
  return std::make_tuple(myPoint, mat);
}

G4ThreeVector PrimaryGenerator::GenerateNemaVertex(G4int phantomElementID, G4ThreeVector boxCoveringElement)
{
  G4VPhysicalVolume* nemaVolume = DetectorConstruction::GetInstance()->GetPhantElement(phantomElementID);
  if (nemaVolume == nullptr)
    return G4ThreeVector(0,0,0);
  
  G4String desiredName = nemaVolume->GetName();
  G4ThreeVector translation = nemaVolume->GetObjectTranslation();
  G4bool lookForVtx = false;
  G4ThreeVector myPoint;
  theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  
  while (!lookForVtx) {
    G4double rand1 = G4UniformRand();
    G4double rand2 = G4UniformRand();
    G4double rand3 = G4UniformRand();
    myPoint.setX((2*rand1 - 1)*boxCoveringElement.getX());
    myPoint.setY((2*rand2 - 1)*boxCoveringElement.getY());
    myPoint.setZ((2*rand3 - 1)*boxCoveringElement.getZ());
    myPoint = myPoint + translation;

    if (theNavigator->LocateGlobalPointAndSetup(myPoint)->GetName() == desiredName)
      lookForVtx = true;
  }
  return myPoint;
}

void PrimaryGenerator::GenerateEvtLargeChamber(G4Event* event)
{
  G4ThreeVector chamberCenter = DetectorConstants::GetChamberCenter();
  G4ThreeVector vtxPosition;
  MaterialExtension* material;
  std::tie(vtxPosition, material) = GetVerticesDistributionAlongStepVector(
    chamberCenter, GetRandomPointInFilledSphere(1.0f * mm)
  );
  std::vector<G4double> evtFractions = material->GetEventsFraction();
  G4double random = G4UniformRand();

  //! for sodium: emitted positrons have up to 100~keV velocity
  //! therefore their speed v=sqrt(2*e/m) = 0.6c
  G4double T0 = (vtxPosition - chamberCenter).mag() / (0.6 * c_light);
  
  G4double decayRandom = G4UniformRand();
  //Not all Na decays lead to the emission of positron
  if (decayRandom > MaterialParameters::fSodiumChanceEC + MaterialParameters::fSodiumChanceNoPrompt) {
    if (evtFractions[0] > random) {
      // pPs 2G
      event->AddPrimaryVertex(GenerateTwoGammaVertex(
        vtxPosition, T0,
        material->GetLifetime(random, DecayChannel::kPara2G)
        ));
      fDecayChannel = DecayChannel::kPara2G;
    } else if (evtFractions[0] + evtFractions[1] > random) {
      // Direct 2G
      event->AddPrimaryVertex(GenerateTwoGammaVertex(
        vtxPosition, T0,
        material->GetLifetime(random - evtFractions[0], DecayChannel::kDirect2G)
        ));
      fDecayChannel = DecayChannel::kDirect2G;
    } else if (evtFractions[0] + evtFractions[1] + evtFractions[2] > random) {
      // oPs 2G
      event->AddPrimaryVertex(GenerateTwoGammaVertex(
        vtxPosition, T0,
        material->GetLifetime(random - evtFractions[0] - evtFractions[1], DecayChannel::kOrtho2G)
        ));
      fDecayChannel = DecayChannel::kOrtho2G;
    } else if (evtFractions[0] + evtFractions[1] + evtFractions[2] + evtFractions[3] > random) {
      // pPs 3G
      event->AddPrimaryVertex(GenerateThreeGammaVertex(
        DecayChannel::kPara3G, vtxPosition, T0,
        material->GetLifetime(random - evtFractions[0] - evtFractions[1] - evtFractions[2], DecayChannel::kPara3G)
        ));
      fDecayChannel = DecayChannel::kPara3G;
    } else if (evtFractions[0] + evtFractions[1] + evtFractions[2] + evtFractions[3] + evtFractions[4] > random) {
      // Direct 3G
      event->AddPrimaryVertex(GenerateThreeGammaVertex(
        DecayChannel::kDirect3G, vtxPosition, T0,
        material->GetLifetime(random - evtFractions[0] - evtFractions[1] - evtFractions[2] - evtFractions[3], DecayChannel::kDirect3G)
        ));
      fDecayChannel = DecayChannel::kDirect3G;
    } else {
      // oPs 3G
      event->AddPrimaryVertex(GenerateThreeGammaVertex(
        DecayChannel::kOrtho3G, vtxPosition, T0,
        material->GetLifetime(random - evtFractions[0] - evtFractions[1] - evtFractions[2] - evtFractions[3] - evtFractions[4], DecayChannel::kOrtho3G)
        ));
      fDecayChannel = DecayChannel::kOrtho3G;
    } 
  }

  //Not all Na decays lead to the emission of prompt photon
  if (decayRandom > MaterialParameters::fSodiumChanceNoPrompt) {
    //! Add prompt gamma from sodium
    G4ThreeVector promptVtxPosition = GenerateVertexUniformInCylinder(0.2 * cm, 0.2 * cm) + chamberCenter;
    event->AddPrimaryVertex(GeneratePromptGammaVertex(
        promptVtxPosition, 0.0f, MaterialParameters::fSodiumGammaTau,
        MaterialParameters::fSodiumGammaEnergy
    ));
  }
}

/**
 * Generating vertex for cosmic radiation particle
 * - choose randomly some point inside the cuboid covering the detector,
 * as cosmic particle has to fly through the detector anyway
 * - choose particle momentum
 * - choose muon charge with ratio of positive to negative muons
 * in cosmic radiation at the sea level from https://arxiv.org/pdf/1005.5332.pdf
 * - trace back the flight track of the particle to a point onto a cosmic roof,
 * that is simulation worlds top dimension
 * - add particle and vertex to an event with energy of 4 GeV, which is
 * mean for muons at sea level, according to PDG (30.3.1)
 * http://pdg.lbl.gov/2017/reviews/rpp2017-rev-cosmic-rays.pdf
 */
void PrimaryGenerator::GenerateCosmicVertex(SourceParams* sourceParams, G4Event* event, HistoManager* histo)
{
  using namespace primary_generator_constants;

  // Generating particle
  G4ParticleDefinition* muonDefinition = nullptr;
  G4double muonFrac = MUON_CHARGE_RATIO/(1+MUON_CHARGE_RATIO);
  if (muonFrac < G4UniformRand()){
    muonDefinition = G4ParticleTable::GetParticleTable()->FindParticle("mu+");
  } else {
    muonDefinition = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
  }

  // Generating angles
  TF1 cos2Func("cos2", "pow(cos(x),2)", -M_PI/2, M_PI/2);
  G4double theta = cos2Func.GetRandom();
  G4double phi = G4UniformRand()*2*M_PI;

  G4ThreeVector posInDetector;
  if (sourceParams->GetShape() == "cylinder") {
    // Cylinder of radius of third layer and half of scintillator dimension
    posInDetector = GenerateVertexUniformInCylinder(DetectorConstants::radius[2], DetectorConstants::scinDim[2]/2);
  } else {
    // Cuboid with dimension that covering whole detector
    posInDetector = GenerateVertexUniformInCuboid(DetectorConstants::radius[2] + DetectorConstants::scinDim[0]/2, 
                                                  DetectorConstants::radius[2] + DetectorConstants::scinDim[0]/2, 
                                                  DetectorConstants::scinDim[2]/2);
  }
  auto cosmicVertex = projectPointToWorldRoof(posInDetector, theta, phi);

  histo->FillCosmicInfo(theta, posInDetector, cosmicVertex->GetPosition());
  VtxInformation* info = new VtxInformation();
  info->SetCosmicGen(true);
  info->SetVtxPosition(cosmicVertex->GetPosition());
  cosmicVertex->SetUserInformation(info);
  G4PrimaryParticle* muon = new G4PrimaryParticle(
    muonDefinition,
    -cos(theta)*MUON_MEAN_ENERGY_GEV,
    sin(theta)*cos(phi)*MUON_MEAN_ENERGY_GEV,
    sin(theta)*sin(phi)*MUON_MEAN_ENERGY_GEV
  );

  PrimaryParticleInformation* infoParticle = new PrimaryParticleInformation();
  infoParticle->SetGammaMultiplicity(PrimaryParticleInformation::kBackground);
  infoParticle->SetGeneratedGammaMultiplicity(PrimaryParticleInformation::kBackground);
  infoParticle->SetIndex(0);
  infoParticle->SetGenMomentum(-cos(theta), -sin(theta)*cos(phi), -sin(theta)*sin(phi));
  muon->SetUserInformation(infoParticle);

  cosmicVertex->SetPrimary(muon);
  event->AddPrimaryVertex(cosmicVertex);
}

void PrimaryGenerator::GenerateBeam(BeamParams* beamParams, G4Event* event)
{
  G4ThreeVector vtxCoor = beamParams->GetVtx();
  G4PrimaryVertex* vertex = new G4PrimaryVertex(vtxCoor, 0);
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDefinition = particleTable->FindParticle("gamma");
  const G4double ene = beamParams->GetEnergy();
  G4ThreeVector momentum = beamParams->GetMomentum();
  G4double px = ene * momentum.x();
  G4double py = ene * momentum.y();
  G4double pz = ene * momentum.z();
  G4PrimaryParticle* particle1 = new G4PrimaryParticle(particleDefinition, px, py, pz, ene);
  PrimaryParticleInformation* infoParticle = new PrimaryParticleInformation();
  infoParticle->SetGammaMultiplicity(PrimaryParticleInformation::kPromptGamma);
  infoParticle->SetGeneratedGammaMultiplicity(PrimaryParticleInformation::kPromptGamma);
  infoParticle->SetIndex(0);
  infoParticle->SetGenMomentum(px, py, pz);
  particle1->SetUserInformation(infoParticle);
  vertex->SetPrimary(particle1);
  VtxInformation* infoPrompt = new VtxInformation();
  vertex->SetUserInformation(infoPrompt);
  infoPrompt->SetPromptGammaGen(true);
  infoPrompt->SetVtxPosition(vtxCoor.x(), vtxCoor.y(), vtxCoor.z());
  event->AddPrimaryVertex(vertex);
}

void PrimaryGenerator::GenerateIsotope(SourceParams* sourceParams, G4Event* event)
{
  G4ThreeVector vtxPosition;
  if (sourceParams->GetShape() == "cylinder") {
    vtxPosition =
      GenerateVertexUniformInCylinder(sourceParams->GetShapeDim(0), sourceParams->GetShapeDim(1))
      + sourceParams->GetShapeCenterPosition();
  }
  if (sourceParams->GetGammasNumber() == 1) {
    event->AddPrimaryVertex(GeneratePromptGammaVertex(
      vtxPosition, 0.0f, MaterialParameters::fSodiumGammaTau,
      MaterialParameters::fSodiumGammaEnergy
    ));
  } else if (sourceParams->GetGammasNumber() == 2) {
    //! generate 2g
    event->AddPrimaryVertex(GenerateTwoGammaVertex(
      vtxPosition, 0.0f, MaterialParameters::fTauBulk
    ));
  } else if (sourceParams->GetGammasNumber() == 3) {
    //! generate 3g
    event->AddPrimaryVertex(GenerateThreeGammaVertex(
      DecayChannel::kOrtho3G ,vtxPosition, 0.0f, MaterialParameters::foPsTauVaccum
    ));
  } else {
    G4Exception(
      "PrimaryGenerator", "PG01", FatalException,
      "program does not know how many gamma quanta is needed to be simulated"
    );
  }
}

/**
 * Generation for NEMA studies
 * Positions - coordinate system centered in the middle of the detector
 *  for(z={0,-3/4L}){
 *    y = 1, 10, 20 cm
 *    x =0
 *  }
 *  20      3       6
 *  10      2       5
 *  1       1       4
 *  z ------0------3/4L ------
 */
void PrimaryGenerator::GenerateNema(G4Event* event, NemaGenerator* nemaGen)
{
  NemaPoint nemaPoint = nemaGen->GetRandomPoint();

  G4ThreeVector nemaPosition = nemaPoint.position;
  PointShape shape = nemaPoint.shape;
  G4ThreeVector vtxPosition;
  if (shape == PointShape::aCylinder) {
    vtxPosition = GenerateVertexUniformInCylinder(nemaPoint.sizeOfPoint.getX(), nemaPoint.sizeOfPoint.getY());
    vtxPosition = nemaGen->GetPointShapedInY(vtxPosition, nemaPoint);
    vtxPosition = nemaGen->GetRotatedPoint(vtxPosition, nemaPoint);
    vtxPosition = vtxPosition + nemaPosition;
  } else if (shape == PointShape::aBall) {
    vtxPosition = GetRandomPointInFilledSphere(nemaPoint.sizeOfPoint.getX());
    vtxPosition = vtxPosition + nemaPosition;
  } else {
    vtxPosition = GenerateNemaVertex(nemaPoint.phantomElementID, nemaPoint.sizeOfPoint);
  }
  
  G4ThreeVector vtxPromptPosition = vtxPosition; //Connecting prompt with the annihilation

  MaterialExtension* material;
  double stepSize = 0.1 * mm;
  PositronReachOption reachOpt = nemaPoint.isPositronReachExp;
  G4double posReach = nemaPoint.positronReach;

  if (reachOpt == PositronReachOption::aNo) {
    theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    material = dynamic_cast<MaterialExtension*>(
      theNavigator->LocateGlobalPointAndSetup(vtxPosition)->GetLogicalVolume()->GetMaterial()
    );
  } else if (reachOpt == PositronReachOption::aFixedUniform) {
    vtxPosition = vtxPosition + GetRandomPointInFilledSphere(posReach * cm);
    theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    material = dynamic_cast<MaterialExtension*>(
      theNavigator->LocateGlobalPointAndSetup(vtxPosition)->GetLogicalVolume()->GetMaterial()
    );
  } else {
    std::tie(vtxPosition, material) = GetVerticesDistributionAlongStepVectorExponential(
      vtxPosition, GetRandomPointOnSphere(stepSize)
    );
  }

  double DirectLF = nemaPoint.directLifetime;
  G4double density = 0.1*g/cm3;

  if (material)
    density = material->GetDensity() / (g/cm3);

  if (nemaPoint.directLFDensityDependent) {
    DirectLF = 0.819151*exp(-0.517436*density);   //dummy function for test in ns
  }

  double lifetime = nemaPoint.lifetime;
  bool is3GAllowed = nemaPoint.is3GAllowed;
  bool alreadyPrimVertexAdded = false;
  double randDecChannel = G4UniformRand();

  using namespace primary_generator_constants;

  if (nemaPoint.isDirectAllowed) {
    if (randDecChannel > 1.0 - DIRECT_PS_ANNIHILATION) {
      randDecChannel = randDecChannel/DIRECT_PS_ANNIHILATION;
      if (is3GAllowed && randDecChannel < MaterialParameters::fDirect3Gfraction) {
        event->AddPrimaryVertex(GenerateThreeGammaVertex(
          DecayChannel::kDirect3G, vtxPosition, 0.0f, DirectLF
        ));
      } else {
        event->AddPrimaryVertex(GenerateTwoGammaVertex(
            vtxPosition, 0.0f, DirectLF
        ));
      }
      alreadyPrimVertexAdded = true;
    } else {
      randDecChannel = randDecChannel/(1.0 - DIRECT_PS_ANNIHILATION);
    }
  }

  if (nemaPoint.isParaPSAllowed && !alreadyPrimVertexAdded) {
    if (randDecChannel < PARA_PS_CREATION) {
      event->AddPrimaryVertex(GenerateTwoGammaVertex(
        vtxPosition, 0.0f, nemaPoint.pPsLifetime
      ));
      alreadyPrimVertexAdded = true;
    }
  }

  if (is3GAllowed && !alreadyPrimVertexAdded) {
    double randLF = G4UniformRand();
    double lfTest = MaterialParameters::GetoPsIntensity3G(lifetime * ns, 100);
    if (randLF > lfTest) {
      event->AddPrimaryVertex(GenerateTwoGammaVertex(
        vtxPosition, 0.0f, lifetime
      ));
    } else {
      event->AddPrimaryVertex(GenerateThreeGammaVertex(
        DecayChannel::kOrtho3G, vtxPosition, 0.0f, lifetime
      ));
    }
  } else if (!alreadyPrimVertexAdded) {
    event->AddPrimaryVertex(GenerateTwoGammaVertex(
      vtxPosition, 0.0f, lifetime
    ));
  }

  bool isPromptAllowed = nemaPoint.isPromptAllowed;

  if (isPromptAllowed) {
/*   if (shape == PointShape::aCylinder) {
      vtxPromptPosition = GenerateVertexUniformInCylinder(nemaPoint.sizeOfPointPrompt.getX(), nemaPoint.sizeOfPointPrompt.getY());
      vtxPromptPosition = nemaGen->GetPointShapedInY(vtxPromptPosition, nemaPoint);
      vtxPromptPosition = nemaGen->GetRotatedPoint(vtxPromptPosition, nemaPoint);
      vtxPromptPosition = vtxPromptPosition + nemaPosition;
    } else if (shape == PointShape::aBall) */
   // vtxPromptPosition = GetRandomPointInFilledSphere(nemaPoint.sizeOfPointPrompt.getX());
      
//    vtxPromptPosition = vtxPromptPosition + vtxPosition; // moving prompt from the annihilation position
    if (nemaPoint.isotope == IsotopeType::i22Na) {
      event->AddPrimaryVertex(GeneratePromptGammaVertex(
        vtxPromptPosition, 0.0f, MaterialParameters::fSodiumGammaTau,
        MaterialParameters::fSodiumGammaEnergy
      ));
    } else {
      event->AddPrimaryVertex(GeneratePromptGammaVertex(
        vtxPromptPosition, 0.0f, MaterialParameters::fScandiumGammaTau,
        MaterialParameters::fScandiumGammaEnergy
      ));
    }
  }
}

G4ThreeVector PrimaryGenerator::GenerateVertexUniformInCylinder(G4double rIn, G4double zmax)
{
  G4double r = std::sqrt(pow(rIn, 2) * G4UniformRand());
  //! alpha uniform in (0, 2*pi)
  G4double alpha = twopi * G4UniformRand();
  G4double ux = std::cos(alpha);
  G4double uy = std::sin(alpha);
  //! z uniform in (-zmax, +zmax)
  G4double z = zmax * (2 * G4UniformRand() - 1);
  G4ThreeVector positionA(r * ux, r * uy, z);
  return positionA;
}

G4ThreeVector PrimaryGenerator::GenerateVertexUniformInCuboid(G4double xMax, G4double yMax, G4double zMax)
{
  G4double x = 2 * xMax * G4UniformRand() - xMax;
  G4double y = 2 * yMax * G4UniformRand() - yMax;
  G4double z = 2 * zMax * G4UniformRand() - zMax;
  G4ThreeVector position(x, y, z);
  return position;
}

/**
 * Projecting point from the cylinder onto "roof" of the simulation world
 */
G4PrimaryVertex* PrimaryGenerator::projectPointToWorldRoof(
  const G4ThreeVector& posInDetector, G4double theta, G4double phi) 
{
  G4double heightDiff = DetectorConstants::world_size[0]-posInDetector.x();
  G4double yzDiff = heightDiff*tan(theta);
  G4double vtx_y = posInDetector.y()-yzDiff*cos(phi);
  G4double vtx_z = posInDetector.z()-yzDiff*sin(phi);

  G4PrimaryVertex* vertex = new G4PrimaryVertex();
  vertex->SetPosition(DetectorConstants::world_size[0], vtx_y, vtx_z);
  return vertex;
}

const G4ThreeVector PrimaryGenerator::GetRandomPointInFilledSphere(G4double radius)
{
  G4double theta = 2 * M_PI * G4UniformRand();
  G4double phi = acos(1 - 2 * G4UniformRand());
  G4double r = radius * cbrt(G4UniformRand());
  G4double x = r * sin(phi) * cos(theta);
  G4double y = r * sin(phi) * sin(theta);
  G4double z = r * cos(phi);
  return G4ThreeVector(x, y, z);
}

const G4ThreeVector PrimaryGenerator::GetRandomPointOnSphere(G4double radius)
{
  G4double theta = 2 * M_PI * G4UniformRand();
  G4double phi = acos(1 - 2 * G4UniformRand());
  G4double x = radius * sin(phi) * cos(theta);
  G4double y = radius * sin(phi) * sin(theta);
  G4double z = radius * cos(phi);
  return G4ThreeVector(x, y, z);
}

G4double PrimaryGenerator::calculate_mQED(DecayChannel channel, Double_t mass_e, Double_t w1, Double_t w2, Double_t w3)
{
  if (channel == DecayChannel::kPara3G) {
    return pow(w1*w2*w3,2)* pow(sin(acos((-pow(w1,2) - pow(w2,2) + pow(w3,2))/(2*w1*w2))) +
				       sin(acos(( pow(w1,2) - pow(w2,2) - pow(w3,2))/(2*w3*w2))) +
				       sin(acos((-pow(w1,2) + pow(w2,2) - pow(w3,2))/(2*w1*w3))), 2)* 
				       (pow(mass_e-w3,2)*pow(w1-w2,2) + pow(mass_e-w1,2)*pow(w2-w3,2) + pow(mass_e-w2,2)*pow(w3-w1,2));
  } else {
    return pow((mass_e - w1) / (w2 * w3), 2) + pow((mass_e - w2) / (w1 * w3), 2) + pow((mass_e - w3) / (w1 * w2), 2);
  }
  return 0;
}
