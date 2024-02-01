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
 *  @file PrimaryGeneratorConstants.h
 */

#ifndef PRIMARY_GENERATOR_CONSTANTS
#define PRIMARY_GENERATOR_CONSTANTS

#include <G4SystemOfUnits.hh>

namespace primary_generator_constants {

//Constants that should in future be changed by the user
    
// Cosmics muons parameters
  static const G4double MUON_CHARGE_RATIO = 1.2766;
  static const G4double MUON_MEAN_ENERGY_GEV = 4*GeV;

// Primary expected fractions of annihilation of positron by different decay modes
  static const G4double DIRECT_PS_ANNIHILATION = 0.6;
  static const G4double PARA_PS_CREATION = 0.25;
  
// Constants for a dumy fit to the expected lifetime of direct annihilation as a function of the density of the material
  static const G4double DIRECT_LF_DENSITY_PARAM_A = 0.819151;
  static const G4double DIRECT_LF_DENSITY_PARAM_B = 0.517436;

// Constants for a dumy fit to the expected range of positron as a function of the density of the material
  static const G4double DIRECT_RANGE_DENSITY_PARAM_A = 19.378;
  static const G4double DIRECT_RANGE_DENSITY_PARAM_B = -1.53977;
}

#endif /* !PRIMARY_GENERATOR_CONSTANTS */
