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
 *  @file JPetGeantEventInformation.h
 */

#ifndef JPET_GEANT_EVENT_INFORMATION_H
#define JPET_GEANT_EVENT_INFORMATION_H 1

#include <TVector3.h>
#include <TObject.h>
#include <TBits.h>
#include <vector>

/**
 * @class JPetGeantEventInformation
 * @brief keeps information about initial simulation parameters
 * e.g. vertices and times distributions for annihilation
 * and prompt gamma photons
 */
class JPetGeantEventInformation// : public TObject
{
public:
  JPetGeantEventInformation();
  ~JPetGeantEventInformation();

  void Clear(Option_t* = "");
  void SetThreeGammaGen(bool tf) { fGenGammaNum.SetBitNumber(2, tf); };
  void SetTwoGammaGen(bool tf) { fGenGammaNum.SetBitNumber(1, tf); };
  void SetPromptGammaGen(bool tf) { fGenGammaNum.SetBitNumber(0, tf); };
  void SetRunNr(int x) { fnRun = x; };
  void SetVtxPosition(double x, double y, double z) { fVtxPosition.SetXYZ(x, y, z); };
  void SetVtxPromptPosition(double x, double y, double z) { fVtxPromptPosition.SetXYZ(x, y, z); };
  void SetLifetime(double x) { fLifetime = x; };
  void SetPromptLifetime(double x) { fPromptLifetime = x; };
  bool GetThreeGammaGen() const { return fGenGammaNum.TestBitNumber(2); };
  bool GetTwoGammaGen() const { return fGenGammaNum.TestBitNumber(1); };
  bool GetPromptGammaGen() const { return fGenGammaNum.TestBitNumber(0); };
  int GetRunNr() const { return fnRun; };
  double GetVtxPositionX() const { return fVtxPosition.X(); };
  double GetVtxPositionY() const { return fVtxPosition.Y(); };
  double GetVtxPositionZ() const { return fVtxPosition.Z(); };
  double GetVtxPromptPositionX() const { return fVtxPromptPosition.X(); };
  double GetVtxPromptPositionY() const { return fVtxPromptPosition.Y(); };
  double GetVtxPromptPositionZ() const { return fVtxPromptPosition.Z(); };
  TVector3 GetVtxPromptPosition() const { return fVtxPromptPosition; }
  TVector3 GetVtxPosition() const { return fVtxPosition; }
  double GetLifetime() const { return fLifetime; };
  double GetPromptLifetime() const { return fPromptLifetime; };
  void SetMomentumGamma(int index, double x, double y, double z) {
    fMomentumGamma[index].SetXYZ(x, y, z);
  };
  TVector3 GetMomentumGamma(int index) const { return fMomentumGamma[index]; };
  void setCosmicEventTag(bool cosmicTag) { fCosmicEventTag = cosmicTag; };

  bool getCosmicEventTag() const { return fCosmicEventTag; };

private:
  const unsigned int fMaxGammaNumberIndex = 3;
  //! Position of annihilation
  TVector3 fVtxPosition;
  //! Position of prompt photon emmision
  TVector3 fVtxPromptPosition;
  //! bitNR 0-prompt; 1-back-to-back; 2- oPs
  TBits fGenGammaNum;
  //! Number should follow the J-PET measurements run numbering scheme
  int fnRun = -1;
  //! Lifetime of generated bound state or direct annihilation;
  //! see specific simulation details
  double fLifetime = -1.0;
  //! generated lifetime of emmited prompt photon; filled only if prompt gamma is generated
  double fPromptLifetime = -1.0;
  std::vector<TVector3> fMomentumGamma;
  bool fCosmicEventTag = false;

private:
  ClassDef(JPetGeantEventInformation, 8)
};

#endif /* !JPET_GEANT_EVENT_INFORMATION_H */
