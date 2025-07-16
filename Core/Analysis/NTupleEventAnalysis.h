/**
 *  @copyright Copyright 2024 The J-PET Monte Carlo Authors. All rights reserved.
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
 *  @file NTupleEventAnalysis.h
 *  @date 2023-10-23
 *  @author Bartlomiej Rachwal (brachwal@agh.edu.pl)
 */

#ifndef EVENT_ANALYSIS_HH
#define EVENT_ANALYSIS_HH 1

#define _USE_MATH_DEFINES
#include <cmath>
#include "globals.hh"
#include "G4Cache.hh"
#include "DetectorHit.h"
#include "G4AnalysisManager.hh"
#include "EventMessenger.h"
#include "JPetG4EventGenInformation.h"

class G4PrimaryParticle;
class VtxInformation;
class G4Event;
class G4Run;


/// A wrapper for storing a double value while tracking 
/// whether it has been modified. 
struct TrackedDouble {
  bool isChanged = false;
  double value = 0.;
  TrackedDouble() {}
  explicit TrackedDouble(double newValue){
    value = newValue;
    isChanged = true;
  }
};

class NTupleEventAnalysis {

  private:
    class ScinHitCollection {
      public:
        G4int ntupleId = -1;
        //!

        G4int runId = -1;
        //!

        std::vector<int> ScinId, TrkId, TrkPDG;
        //! Number of interaction taking place in single scintillator,
        //! which were classified as a single hit;
        //! it may be a big number since electron deposits energy in many steps

        std::vector<int> NumOfInteractions;
        //! unequivocal number identifying gamma quanta

        std::vector<int> GenGammaIndex;
        //! 2 - back-to-back; 3 - 3gamma annihilation

        std::vector<int> GenGammaMultiplicity;
        //! deposited energy (a sum if more than one gamma quanta interacted)

        std::vector<double> EneDep;
        //! Time of interaction in the scintillator
        //! with respect to the beta+ decay (start of simulation)

        std::vector<double> Time;

        std::vector<double> PositionX,PositionY,PositionZ;
        std::vector<double> PolarizationInX,PolarizationInY,PolarizationInZ;
        std::vector<double> PolarizationOutX,PolarizationOutY,PolarizationOutZ;

        std::vector<double> MomentumInX, MomentumInY, MomentumInZ;
        std::vector<double> MomentumOutX, MomentumOutY, MomentumOutZ;

        void Reset();
        
      private:
        template <typename T>
        void ClearAndReserve(std::vector<T>& data){
          data.clear();
          data.reserve(10);
        }
    };
  
    ///
    NTupleEventAnalysis() = default;

    ///
    ~NTupleEventAnalysis() = default;

    /// Delete the copy and move constructors
    NTupleEventAnalysis(const NTupleEventAnalysis &) = delete;
    NTupleEventAnalysis &operator=(const NTupleEventAnalysis &) = delete;
    NTupleEventAnalysis(NTupleEventAnalysis &&) = delete;
    NTupleEventAnalysis &operator=(NTupleEventAnalysis &&) = delete;

    ///
    G4Cache<ScinHitCollection> m_scinHitCollection;
    
    ///
    G4Cache<G4AnalysisManager*> m_analysisManager;

    /// The G4AnalysisManager::CreateNtuple(X)Column method assigns a unique ID to each column, 
    /// incrementing it with each successive call. To explicitly set a value using FillNtuple(X)Column, 
    /// it is essential to know the ID associated with the corresponding column. To simplify this process 
    /// and ensure correct mapping, we create a map that links each column to its respective ID.
    G4MapCache<const char*, G4int> m_treeColumnId;
    G4MapCache<const char*, G4int> m_histId;

    ///
    G4Cache<std::set<const char*>> m_errorCounts;

    ///
    G4Cache<int> m_parentIDofPhoton;

    ///
    G4Cache<bool> m_emptyEvent;

    ///
    G4Cache<JPetG4EventGenInformation> m_g4EventGenInfo;

    ///
    void WriteError(const char* nameOfHistogram, const std::string& messageEnd);

    ///
    EventMessenger* m_EvtMessenger = EventMessenger::GetEventMessenger();

    ///
    G4int GetNTupleId() { return m_scinHitCollection.Get().ntupleId; }

    ///
    void FillNTupleEvent(const G4int& evtId);

    ///
    void FillG4EventGenInfo(const G4Event* anEvent);
    void FillG4EventGenVtxInfo(VtxInformation* info);
    void FillG4EventGenParticleInfo(G4PrimaryParticle* particle);

    ///
    void CreateNTuple();

    ///
    void CreateHistograms();


    public:
      ///
      static NTupleEventAnalysis* GetInstance();

      ///
      static bool NTupleMerging;

      ///
      static bool Cosmic;
      static bool ControlHisto;

      ///
      void BeginOfRunAction(const G4Run* runPtr, G4bool isMaster);

      ///
      void EndOfEventAction(const G4Event *evt);

      ///
      void SetParentIDofPhoton(int x) { m_parentIDofPhoton.Put(x); };
      int GetParentIDofPhoton() const { return m_parentIDofPhoton.Get(); };

      ///
      void AddNodeToDecayTree(int nodeID, int trackID);
};

#endif /* !EVENT_ANALYSIS_HH */