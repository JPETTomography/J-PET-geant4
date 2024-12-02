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
 *  @file VtxInformation.h
 */

#ifndef VTX_INFORMATION_H
#define VTX_INFORMATION_H 1

#include <G4ThreeVector.hh>
#include <G4VUserPrimaryVertexInformation.hh>
#include <globals.hh>

class VtxInformation : public G4VUserPrimaryVertexInformation
{
public:
  VtxInformation();
  virtual ~VtxInformation();
  void Clear();
  virtual void Print() const;

  void SetThreeGammaGen(G4bool tf) { fThreeGammaGen = tf; };
  void SetTwoGammaGen(G4bool tf) { fTwoGammaGen = tf; };
  void SetPromptGammaGen(G4bool tf) { fPromptGammaGen = tf; };
  void SetCosmicGen(G4bool tf) { fCosmicGen = tf; };
  void SetRunNr(G4int x) { fnRun = x; };
  void SetVtxPosition(G4ThreeVector position);
  void SetVtxPosition(G4double x, G4double y, G4double z);
  void SetLifetime(G4double x) { fLifetime = x; };
  void SetDensity(G4double x) { fDensity = x; };
  G4bool GetThreeGammaGen() { return fThreeGammaGen; };
  G4bool GetTwoGammaGen() { return fTwoGammaGen; };
  G4bool GetPromptGammaGen() { return fPromptGammaGen; };
  G4bool GetCosmicGammaGen() const { return fCosmicGen; };
  G4int GetRunNr() { return fnRun; };
  G4double GetVtxPositionX() { return fVtxPosition.x(); };
  G4double GetVtxPositionY() { return fVtxPosition.y(); };
  G4double GetVtxPositionZ() { return fVtxPosition.z(); };
  G4double GetLifetime() { return fLifetime; };
  G4double GetDensity() { return fDensity; };

private:
  G4ThreeVector fVtxPosition;
  G4bool fTwoGammaGen;
  G4bool fThreeGammaGen;
  G4bool fPromptGammaGen;
  G4bool fCosmicGen;
  G4int fnRun;
  G4double fLifetime;
  G4double fDensity;
};

#endif /* !VTX_INFORMATION_H */
