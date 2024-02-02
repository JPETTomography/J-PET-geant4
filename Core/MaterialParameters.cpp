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
 *  @file MaterialParameters.cpp
 */

#include "MaterialParameters.h"
#include "MaterialExtension.h"

MaterialConstants::MaterialConstants(
  const std::vector<G4double>& oPsLF, const std::vector<G4double>& oPsProb,
  G4double pPsLF, G4double pPsFrac, const std::vector<G4double>& dirLF,
  const std::vector<G4double>& dirProb
) : oPsLifetimes(oPsLF), oPsProbabilities(oPsProb), pPsLifetime(pPsLF),
pPsFraction(pPsFrac), directLifetimes(dirLF), directProbabilities(dirProb) {}

const G4double MaterialParameters::fpPsTauVaccum = 0.125 * ns;
const G4double MaterialParameters::foPsTauVaccum = 142.0 * ns;
const G4double MaterialParameters::fDirectTauMax = 0.6 * ns;
const G4double MaterialParameters::fpPsTauMax = 0.2 * ns;
const G4double MaterialParameters::fTauBulk = 0.3;
const G4double MaterialParameters::fDirect3Gfraction = 0.002688;
const G4double MaterialParameters::fSodiumGammaEnergy = 1.2770 * MeV;
const G4double MaterialParameters::fSodiumGammaTau = 3.7 * ps;
const G4double MaterialParameters::fSodiumChanceEC = 0.095;
const G4double MaterialParameters::fSodiumChanceNoPrompt = 0.005;
const G4double MaterialParameters::fScandiumGammaEnergy = 1.1570 * MeV;
const G4double MaterialParameters::fScandiumGammaTau = 2.61 * ps;
const G4double MaterialParameters::fScandiumChanceEC = 0.09427;
const G4double MaterialParameters::fScandiumChanceNoPrompt = 0.00225;

G4String MaterialParameters::fAnnihlationMode = "";
G4double MaterialParameters::fpPs3Gfraction = 0;
MaterialConstants MaterialParameters::fTemp = MaterialConstants({}, {}, 0., 0., {}, {});

const MaterialConstants MaterialParameters::fXAD4 = MaterialConstants(
  {2.5 * ns, 10 * ns, 90 * ns}, {3.3, 2.8, 44.8}, 0.125 * ns, 1.0 / 3.0, {0.45 * ns}, {100}
);
const MaterialConstants MaterialParameters::fAl = MaterialConstants(
  {142 * ns}, {0}, 0.125 * ns, 0, {0.32 * ns}, {100}
);
const MaterialConstants MaterialParameters::fKapton = MaterialConstants(
  {142 * ns}, {0}, 0.125 * ns, 0, {0.374 * ns}, {100}
);
const MaterialConstants MaterialParameters::fPlexiglass = MaterialConstants(
  {1.8 * ns}, {28}, 0.125 * ns, 1.0 / 3.0, {0.4 * ns}, {100}
);
// DOI 10.1063/1.475876 for temperature 293 K
const MaterialConstants MaterialParameters::fScin = MaterialConstants(
  {1.95 * ns}, {35}, 0.125 * ns, 1.0 / 3.0, {0.45 * ns}, {100}
);
// DOI 10.1515/nuka-2015-0140 for temperature 293 K
const MaterialConstants MaterialParameters::fPA6 = MaterialConstants(
  {1.7 * ns}, {19.5}, 0.125 * ns, 1.0 / 3.0, {0.356 * ns}, {100}
);
// DOI 10.1002/app.10319 for temperature 293 K
const MaterialConstants MaterialParameters::fAir = MaterialConstants(
  {142 * ns}, {75}, 0.125 * ns, 1.0 / 3.0, {0.5 * ns}, {0}
);

const MaterialConstants MaterialParameters::fPolycarbonate = MaterialConstants( 
  {3 * ns}, {25}, 0.125 * ns, 1.0/3.0, {0.3 * ns}, {100}
);

const MaterialConstants MaterialParameters::fPolyoxymethylene = MaterialConstants(
  {1.6 * ns}, {20}, 0.125 * ns, 1.0/3.0, {0.3 * ns}, {100}
);

const MaterialConstants MaterialParameters::fSiliconDioxide = MaterialConstants( 
  {2 * ns}, {30}, 0.125 * ns, 1.0/3.0, {0.35 * ns}, {100}
);

const MaterialConstants MaterialParameters::fStainlessSteel = MaterialConstants(
  {}, {}, 0.125 * ns, 1.0/3.0, {0.18 * ns, 0.22 * ns}, {40, 60}
);

MaterialParameters::MaterialParameters()
{
  foPsLifetimes.clear();
  foPsProbabilities.clear();
  foPs2GIntensities.clear();
  foPs3GIntensities.clear();
  fpPsLifetime = 0;
  fpPsFraction = 0;
  fpPsIntensity = 0;
  fDirectLifetimes.clear();
  fDirectIntensities.clear();
  fAnnihlationMode = "";
  fpPs3Gfraction = 0.;
}

MaterialParameters::MaterialParameters(
  const std::vector<G4double>& oPsLF, const std::vector<G4double>& oPsProb,
  G4double pPsLF, G4double pPsFrac, const std::vector<G4double>& directLF,
  const std::vector<G4double>& directProb
) : foPsLifetimes(oPsLF), foPsProbabilities(oPsProb), fpPsLifetime(pPsLF),
fpPsFraction(pPsFrac), fDirectLifetimes(directLF),
fDirectProbabilities(directProb)
{
  SetComponentsIntensities();
}

void MaterialParameters::ClearTemp()
{
  fTemp.oPsLifetimes.clear();
  fTemp.oPsProbabilities.clear();
  fTemp.pPsLifetime = 0.;
  fTemp.pPsFraction = 0.;
  fTemp.directLifetimes.clear();
  fTemp.directProbabilities.clear();
}

void MaterialParameters::SetMaterial(MaterialConstants mat)
{
  foPsLifetimes = mat.oPsLifetimes;
  foPsProbabilities = mat.oPsProbabilities;
  fpPsLifetime = mat.pPsLifetime;
  fpPsFraction = mat.pPsFraction;
  fDirectLifetimes = mat.directLifetimes;
  fDirectProbabilities = mat.directProbabilities;
  SetComponentsIntensities();
}

void MaterialParameters::SetMaterialByName(MaterialID materialID)
{
  if (materialID == MaterialID::mXAD4) SetMaterial(fXAD4);
  else if (materialID == MaterialID::mAl) SetMaterial(fAl);
  else if (materialID == MaterialID::mKapton) SetMaterial(fKapton);
  else if (materialID == MaterialID::mPlexiglass) SetMaterial(fPlexiglass);
  else if (materialID == MaterialID::mScin) SetMaterial(fScin);
  else if (materialID == MaterialID::mPA6) SetMaterial(fPA6);
  else if (materialID == MaterialID::mAir) SetMaterial(fAir);
  else if (materialID == MaterialID::mPolycarbonate) SetMaterial(fPolycarbonate);
  else if (materialID == MaterialID::mPolyoxymethylene) SetMaterial(fPolyoxymethylene);
  else if (materialID == MaterialID::mSiliconDioxide) SetMaterial(fSiliconDioxide);
  else if (materialID == MaterialID::mStainlessSteel) SetMaterial(fStainlessSteel);

  else SetMaterial(fTemp);
}

void MaterialParameters::SetAnnihilationMode(G4String mode)
{
  fAnnihlationMode = mode;
}

void MaterialParameters::SetpPs3GFraction(double pPs3Gfraction)
{
  fpPs3Gfraction = pPs3Gfraction;
}

void MaterialParameters::AddoPsComponent(G4double lifetime, G4double probability)
{
  foPsLifetimes.push_back(lifetime);
  foPsProbabilities.push_back(probability);
}

// cppcheck-suppress unusedFunction
void MaterialParameters::SetoPsComponents(
  const std::vector<G4double>& oPsLF, const std::vector<G4double>& oPsProb
) {
  foPsLifetimes = oPsLF;
  foPsProbabilities = oPsProb;
}

void MaterialParameters::SetpPsComponent(G4double lifetime, G4double fraction)
{
  fpPsLifetime = lifetime;
  fpPsFraction = fraction;
}

void MaterialParameters::AddDirectComponent(G4double lifetime, G4double probability)
{
  fDirectLifetimes.push_back(lifetime);
  fDirectProbabilities.push_back(probability);
}

// cppcheck-suppress unusedFunction
void MaterialParameters::SetDirectComponents(
  const std::vector<G4double>& directLF, const std::vector<G4double>& directProb
) {
  fDirectLifetimes = directLF;
  fDirectProbabilities = directProb;
}

void MaterialParameters::SetComponentsIntensities()
{
  foPs2GIntensities.clear();
  foPs3GIntensities.clear();
  fDirectIntensities.clear();
  G4double oPsTotalProbability = 0., totalIntensity = 0.;
  for (unsigned i = 0; i < foPsProbabilities.size(); i++) {
    oPsTotalProbability += foPsProbabilities[i];
    foPs2GIntensities.push_back(GetoPsIntensity2G(foPsLifetimes[i], foPsProbabilities[i]));
    foPs3GIntensities.push_back(GetoPsIntensity3G(foPsLifetimes[i], foPsProbabilities[i]));
  }
  totalIntensity += oPsTotalProbability / 100.;
  fpPsIntensity = oPsTotalProbability * fpPsFraction / 100; // Fraction from 0 to whatever;
  totalIntensity += fpPsIntensity;
  G4double directTotalProbability = 0., temp = 0.;
  for (unsigned j = 0; j < fDirectProbabilities.size(); j++) {
    directTotalProbability = fDirectProbabilities[j];
  }
  for (unsigned j = 0; j < fDirectProbabilities.size(); j++) {
    temp = (100 - oPsTotalProbability - 100 * fpPsIntensity)
    * fDirectProbabilities[j] / (100 * directTotalProbability);
    if (temp > 0. && temp <= 1.) {
      fDirectIntensities.push_back(temp);
      totalIntensity += temp;
    } else {
      fDirectIntensities.push_back(0);
    }
  }
  if (foPsProbabilities.size() == 0) {
    foPs2GIntensities.push_back(0.);
    foPs3GIntensities.push_back(0.);
  }
  if (fDirectProbabilities.size() == 0){
    fDirectIntensities.push_back(0.);
  }
  if (totalIntensity > 1) {
    RenormalizeIntensities(totalIntensity);
  }
}

void MaterialParameters::RenormalizeIntensities(G4double totalIntensity)
{
  for (unsigned i = 0; i < foPs2GIntensities.size(); i++) {
    foPs2GIntensities[i] = foPs2GIntensities[i] / totalIntensity;
    foPs3GIntensities[i] = foPs3GIntensities[i] / totalIntensity;
  }
  fpPsIntensity = fpPsIntensity / totalIntensity;
  for (unsigned i = 0; i < fDirectIntensities.size(); i++) {
    fDirectIntensities[i] = fDirectIntensities[i] / totalIntensity;
  }
}

G4double MaterialParameters::GetoPs2GTotalIntensity() const
{
  G4double totalIntensity = 0.;
  for (unsigned i = 0; i < foPs2GIntensities.size(); i++){
    totalIntensity += foPs2GIntensities[i];
  }
  return totalIntensity;
}

G4double MaterialParameters::GetoPs3GTotalIntensity() const
{
  G4double totalIntensity = 0.;
  for (unsigned i = 0; i < foPs3GIntensities.size(); i++){
    totalIntensity += foPs3GIntensities[i];
  }
  return totalIntensity;
}

G4double MaterialParameters::GetDirect2GTotalIntensity() const
{
  G4double totalIntensity = 0.;
  for (unsigned i = 0; i < fDirectIntensities.size(); i++) {
    totalIntensity += fDirectIntensities[i];
  }
  return totalIntensity * (1. - fDirect3Gfraction);
}

G4double MaterialParameters::GetDirect3GTotalIntensity() const
{
  G4double totalIntensity = 0.;
  for (unsigned i = 0; i < fDirectIntensities.size(); i++){
    totalIntensity += fDirectIntensities[i];
  }
  return totalIntensity * fDirect3Gfraction;
}

G4double MaterialParameters::GetpPs2GTotalIntensity() const
{
  return (1-fpPs3Gfraction) * fpPsIntensity;
}

G4double MaterialParameters::GetpPs3GTotalIntensity() const
{
  return fpPs3Gfraction * fpPsIntensity;
}

G4double MaterialParameters::GetoPsIntensity2G(G4double oPsLifetime, G4double oPsProbability)
{
  return ((foPsTauVaccum - oPsLifetime) / foPsTauVaccum) * oPsProbability / 100.;
}

G4double MaterialParameters::GetoPsIntensity3G(G4double oPsLifetime, G4double oPsProbability)
{
  return (oPsLifetime / foPsTauVaccum) * oPsProbability / 100.;
}

G4double MaterialParameters::GetoPs2GLifetimeFromVector(double randNumber)
{
  double randNumberNorm = randNumber;
  if (fAnnihlationMode != "") {
    randNumberNorm = randNumber * GetoPs2GTotalIntensity();
  }
  double intensitySum = 0.;
  for (unsigned i = 0; i < foPsLifetimes.size(); i++) {
    intensitySum += foPs2GIntensities[i];
    if (randNumberNorm < intensitySum){
      return foPsLifetimes[i];
    }
  }
  return GetLifetimeVector(foPsLifetimes);
}

G4double MaterialParameters::GetoPs3GLifetimeFromVector(double randNumber)
{
  double randNumberNorm = randNumber;
  if (fAnnihlationMode != "") {
    randNumberNorm = randNumber * GetoPs3GTotalIntensity();
  }
  double intensitySum = 0.;
  for (unsigned i = 0; i < foPsLifetimes.size(); i++) {
    intensitySum += foPs3GIntensities[i];
    if (randNumberNorm < intensitySum){
      return foPsLifetimes[i];
    }
  }
  return GetLifetimeVector(foPsLifetimes);
}

G4double MaterialParameters::GetDirectLifetimeFromVector(double randNumber)
{
  double randNumberNorm = randNumber;
  if (fAnnihlationMode != ""){
    randNumberNorm = randNumber * GetDirect2GTotalIntensity();
  }
  double intensitySum = 0.;
  for (unsigned i = 0; i < fDirectLifetimes.size(); i++) {
    intensitySum += fDirectIntensities[i];
    if (randNumberNorm < intensitySum){
      return fDirectLifetimes[i];
    }
  }
  return GetLifetimeVector(fDirectLifetimes);
}

G4double MaterialParameters::GetpPsLifetime() const { return fpPsLifetime; }

G4double MaterialParameters::GetLifetimeVector(std::vector<G4double> vectorToCheck)
{
  if (vectorToCheck.size()){
    return vectorToCheck[vectorToCheck.size() - 1];
  } else {
    return foPsTauVaccum;
  }
}
