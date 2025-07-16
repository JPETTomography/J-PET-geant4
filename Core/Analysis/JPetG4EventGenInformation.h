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
 *  @file JPetG4EventGenInformation.h
 */

#ifndef JPET_G4EVENT_GEN_INFORMATION_H
#define JPET_G4EVENT_GEN_INFORMATION_H 1


#include "G4ThreeVector.hh"
#include <vector>

/**
 * @class JPetG4EventGenInformation
 * @brief keeps information about initial simulation parameters
 * e.g. vertices and times distributions for annihilation
 * and prompt gamma photons
 */
class JPetG4EventGenInformation
{
public:
  JPetG4EventGenInformation() = default;
  ~JPetG4EventGenInformation() = default;
  void SetThreeGammaGen(bool tf) { fThreeGammaGen = tf; };
  void SetTwoGammaGen(bool tf) { fTwoGammaGen = tf; };
  void SetPromptGammaGen(bool tf) { fPromptGammaGen = tf; };
  void SetRunNr(int x) { fnRun = x; };
  void SetVtxPosition(double x, double y, double z) { fVtxPosition.set(x, y, z); };
  void SetVtxPromptPosition(double x, double y, double z) { fVtxPromptPosition.set(x, y, z); };
  void SetLifetime(double x) { fLifetime = x; };
  void SetPromptLifetime(double x) { fPromptLifetime = x; };
  bool GetThreeGammaGen() const { return fThreeGammaGen; };
  bool GetTwoGammaGen() const { return fTwoGammaGen; };
  bool GetPromptGammaGen() const { return fPromptGammaGen; };
  int GetRunNr() const { return fnRun; };
  double GetVtxPositionX() const { return fVtxPosition.getX(); };
  double GetVtxPositionY() const { return fVtxPosition.getY(); };
  double GetVtxPositionZ() const { return fVtxPosition.getZ(); };
  double GetVtxPromptPositionX() const { return fVtxPromptPosition.getX(); };
  double GetVtxPromptPositionY() const { return fVtxPromptPosition.getY(); };
  double GetVtxPromptPositionZ() const { return fVtxPromptPosition.getZ(); };
  G4ThreeVector GetVtxPromptPosition() const { return fVtxPromptPosition; }
  G4ThreeVector GetVtxPosition() const { return fVtxPosition; }
  double GetLifetime() const { return fLifetime; };
  double GetPromptLifetime() const { return fPromptLifetime; };
  void SetMomentumGamma(int index, double x, double y, double z) {
    fMomentumGammaX.at(index) =x;
    fMomentumGammaY.at(index) =y;
    fMomentumGammaZ.at(index) =z;
  };
  double GetMomentumGammaX(int index) const { return fMomentumGammaX.at(index); };
  double GetMomentumGammaY(int index) const { return fMomentumGammaY.at(index); };
  double GetMomentumGammaZ(int index) const { return fMomentumGammaZ.at(index); };
  void setCosmicEventTag(bool cosmicTag) { fCosmicEventTag = cosmicTag; };

  bool GetCosmicEventTag() const { return fCosmicEventTag; };

  void Reset(){
    fPromptGammaGen = false;
    fTwoGammaGen = false;
    fThreeGammaGen = false;
    fnRun = 0;
    fLifetime = 0.0;
    fPromptLifetime = 0.0;
    fVtxPosition.set(0.0, 0.0, 0.0);
    fVtxPromptPosition.set(0.0, 0.0, 0.0);
    fMomentumGammaX.clear();
    fMomentumGammaX.resize(4);
    fMomentumGammaY.clear();
    fMomentumGammaY.resize(4);
    fMomentumGammaZ.clear();
    fMomentumGammaZ.resize(4);
    fCosmicEventTag = false;
  }

public: // TEMPORARY!!!
  const unsigned int fMaxGammaNumberIndex = 3;
  //! Position of annihilation
  G4ThreeVector fVtxPosition;
  //! Position of prompt photon emmision
  G4ThreeVector fVtxPromptPosition;
  bool fPromptGammaGen = false; // prompt
  bool fTwoGammaGen = false;    // back-to-back
  bool fThreeGammaGen = false;  // oPs
  //! Number should follow the J-PET measurements run numbering scheme
  int fnRun = -1;
  //! Lifetime of generated bound state or direct annihilation;
  //! see specific simulation details
  double fLifetime = -1.0;
  //! generated lifetime of emmited prompt photon; filled only if prompt gamma is generated
  double fPromptLifetime = -1.0;
  std::vector<double> fMomentumGammaX{4};
  std::vector<double> fMomentumGammaY{4};
  std::vector<double> fMomentumGammaZ{4};
  bool fCosmicEventTag = false;
};


#endif /* !JPET_G4EVENT_GEN_INFORMATION_H */
