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
 *  @file PrimaryGenerator.h
 */

#ifndef PRIMARYGENERATOR_H
#define PRIMARYGENERATOR_H 1

#include "../Objects/Framework/JPetGeantDecayTree.h"
#include "MaterialExtension.h"
#include "NemaGenerator.h"
#include "HistoManager.h"
#include "SourceParams.h"
#include "BeamParams.h"

#include <G4TransportationManager.hh>
#include <G4VPrimaryGenerator.hh>
#include <G4PrimaryVertex.hh>
#include <TGenPhaseSpace.h>
#include <TLorentzVector.h>
#include <G4Navigator.hh>
#include <TRotation.h>
#include <G4Event.hh>
#include <TVector3.h>
#include <TGraph.h>
#include <vector>
#include <TF1.h>
#include <map>
#include <TH1F.h>
#include <TH2F.h>

class PrimaryGenerator : public G4VPrimaryGenerator
{
public:
  PrimaryGenerator();
  ~PrimaryGenerator();
  void GenerateBeam(BeamParams*, G4Event*);
  void GenerateIsotope(SourceParams*, G4Event*);
  void GenerateNema(G4Event*, NemaGenerator*);
  void GenerateEvtSmallChamber(G4Event* event, const G4double);
  void GenerateEvtLargeChamber(G4Event* event);
  void GenerateCosmicVertex(SourceParams* sourceParams, G4Event* event, HistoManager* histo);
  virtual void GeneratePrimaryVertex(G4Event*){};
  DecayChannel GetDecayChannel() { return fDecayChannel; };

private:
  DecayChannel fDecayChannel = DecayChannel::kUnknown;
  //! return: vtx position, 2/3g ratio, meanlifetime;
  //! as input the maximal dimension(/2) of annihilation chamber are taken (to speed up simulatons)
  std::tuple<G4ThreeVector, MaterialExtension*> GetVerticesDistributionInFilledSphere(
    const G4ThreeVector center, G4double radius
  );
  std::tuple<G4ThreeVector, MaterialExtension*> GetVerticesDistributionAlongStepVector(
    const G4ThreeVector center, const G4ThreeVector direction
  );
  std::tuple<G4ThreeVector, MaterialExtension*> GetVerticesDistributionAlongStepVectorExponential(
    const G4ThreeVector center, const G4ThreeVector direction
  );
  G4PrimaryVertex* GenerateTwoGammaVertex(
    const G4ThreeVector vtxPosition, const G4double T0, const G4double lifetime2g
  );
  G4PrimaryVertex* GenerateThreeGammaVertex(
    DecayChannel channel, const G4ThreeVector vtxPosition, const G4double T0, const G4double lifetime3g
  );
  G4PrimaryVertex* GeneratePromptGammaVertex(
    const G4ThreeVector vtxPosition, const G4double T0, const G4double lifetimePrompt, const G4double energy
  );
  G4ThreeVector GenerateVertexUniformInCylinder(G4double, G4double);
  G4ThreeVector GenerateVertexUniformInCuboid(G4double xMax, G4double yMax, G4double zMax);
  G4PrimaryVertex* projectPointToWorldRoof(
    const G4ThreeVector& posInDetector, G4double theta, G4double phi
  );
  G4double calculate_mQED(
    DecayChannel channel, Double_t mass_e, Double_t w1, Double_t w2, Double_t w3
  );
  const G4ThreeVector GetRandomPointInFilledSphere(G4double radius);
  const G4ThreeVector GetRandomPointOnSphere(G4double radius);
  
  G4Navigator* theNavigator =  G4TransportationManager::GetTransportationManager()
  ->GetNavigatorForTracking();
};

#endif /* !PRIMARYGENERATOR_H */
