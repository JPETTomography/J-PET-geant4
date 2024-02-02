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
 *  @file MaterialParameters.h
 */

#ifndef MATERIALPARAMETERS_H
#define MATERIALPARAMETERS_H 1

#include <G4PhysicalConstants.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <globals.hh>
#include <vector>

struct MaterialConstants
{
  MaterialConstants(
    const std::vector<G4double>& oPsLF, const std::vector<G4double>& oPsProb,
    G4double pPsLF, G4double pPsFrac, const std::vector<G4double>& dirLF,
    const std::vector<G4double>& dirProb
  );
  std::vector<G4double> oPsLifetimes;
  std::vector<G4double> oPsProbabilities;
  G4double pPsLifetime;
  G4double pPsFraction;
  std::vector<G4double> directLifetimes;
  std::vector<G4double> directProbabilities;
};

class MaterialParameters
{
public:
  enum MaterialID {
    mUnknown, mXAD4, mAl, mKapton, mPlexiglass, mScin, mPA6, mAir,
    mPolycarbonate, mPolyoxymethylene, mSiliconDioxide, mStainlessSteel,
    mWater
  };

  MaterialParameters();
  MaterialParameters(
    const std::vector<G4double>& oPsLF, const std::vector<G4double>& oPsProb,
    G4double pPsLF, G4double pPsFrac, const std::vector<G4double>& directLF,
    const std::vector<G4double>& directProb
  );

  static void ClearTemp();
  static void SetAnnihilationMode(G4String mode);
  static void SetpPs3GFraction(double pPs3Gfraction);

  void SetMaterial(MaterialConstants mat);
  void SetMaterialByName(MaterialID materialID);
  void AddoPsComponent(G4double Lifetime, G4double Probability);
  void SetoPsComponents(
    const std::vector<G4double>& oPsLF, const std::vector<G4double>& oPsProb
  );
  void SetpPsComponent(G4double Lifetime, G4double Fraction);
  void AddDirectComponent(G4double Lifetime, G4double Probability);
  void SetDirectComponents(
    const std::vector<G4double>& directLF, const std::vector<G4double>& directProb
  );

  static G4double GetoPsIntensity2G(G4double oPsLifetime, G4double oPsProbability);
  static G4double GetoPsIntensity3G(G4double oPsLifetime, G4double oPsProbability);
  void SetComponentsIntensities();
  void RenormalizeIntensities(G4double TotalIntensity);

  G4double GetoPs2GTotalIntensity() const;
  G4double GetoPs3GTotalIntensity() const;
  G4double GetDirect2GTotalIntensity() const;
  G4double GetDirect3GTotalIntensity() const;
  G4double GetpPs2GTotalIntensity() const;
  G4double GetpPs3GTotalIntensity() const;
  G4double GetoPs2GLifetimeFromVector(double randNumber);
  G4double GetoPs3GLifetimeFromVector(double randNumber);
  G4double GetDirectLifetimeFromVector(double randNumber);
  G4double GetpPsLifetime() const;
  static G4double GetLifetimeVector(std::vector<G4double> vectorToCheck);

  static const G4double foPsTauVaccum;
  static const G4double fDirectTauMax;
  static const G4double fpPsTauVaccum;
  static const G4double fpPsTauMax;
  static const G4double fTauBulk;
  static const G4double fDirect3Gfraction;
  //! doi 10.2478/umcschem-2013-0012 1/372
  static const G4double fSodiumGammaEnergy;
  static const G4double fSodiumGammaTau;
  static const G4double fSodiumChanceEC;
  static const G4double fSodiumChanceNoPrompt;
  static const G4double fScandiumGammaEnergy;
  static const G4double fScandiumGammaTau;
  static const G4double fScandiumChanceEC;
  static const G4double fScandiumChanceNoPrompt;
  static G4String fAnnihlationMode;
  static G4double fpPs3Gfraction;

  static const MaterialConstants fXAD4;
  static const MaterialConstants fAl;
  static const MaterialConstants fKapton;
  static const MaterialConstants fPlexiglass;
  static const MaterialConstants fScin;
  static const MaterialConstants fPA6;
  static const MaterialConstants fAir;
  static const MaterialConstants fPolycarbonate;
  static const MaterialConstants fPolyoxymethylene;
  static const MaterialConstants fSiliconDioxide;
  static const MaterialConstants fStainlessSteel;
  static MaterialConstants fTemp;

private:
  //! There can be more than 1 component of oPs so that is why vector is used
  //! Different types of pores/free volumes can give different mean lifetime of o-Ps in them
  std::vector<G4double> foPsLifetimes;
  //! Probability of positron decay with given lifetime.
  //! From Probabilities intensities of decay into 2/3 for each component, can be calculated
  std::vector<G4double> foPsProbabilities;
  std::vector<G4double> foPs2GIntensities;
  std::vector<G4double> foPs3GIntensities;

  G4double fpPsLifetime;
  //! pPs Intensity defined as a fraction of pPs to oPs
  G4double fpPsFraction;
  G4double fpPsIntensity;

  //! There can be more than 1 component of direct annihilation
  //! of positron so that is why vector is used
  //! Different defects in material can give different mean lifetime of positron in it
  std::vector<G4double> fDirectLifetimes;
  std::vector<G4double> fDirectProbabilities;
  std::vector<G4double> fDirectIntensities;
};

#endif /* !MATERIALPARAMETERS_H */
