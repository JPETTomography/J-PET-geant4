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
 *  @file ActionInitialization.h
 */

#ifndef ACTIONINITIALIZATION_H
#define ACTIONINITIALIZATION_H 1

#include <G4VUserActionInitialization.hh>
#include "G4Cache.hh"

class HistoManager;
/**
 * @class ActionInitialization
 * @brief function inherited from GEANT4; be careful while implementing multithread mode
 */
class ActionInitialization : public G4VUserActionInitialization
{
private:
  G4Cache<HistoManager*> fHistoManager;
public:
  ActionInitialization();
  virtual ~ActionInitialization();
  //! Put here functions called only once during multithread mode as histograms
  virtual void BuildForMaster() const;
  //! Functions called for each thread
  virtual void Build() const;
};

#endif /* !ACTIONINITIALIZATION_H */
