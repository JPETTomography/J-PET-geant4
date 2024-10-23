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
 *  @file NTupleEvtAnalysis.h
 *  @date 2023-10-23
 *  @author Bartlomiej Rachwal (brachwal@agh.edu.pl)
 */

#ifndef EVENT_ANALYSIS_HH
#define EVENT_ANALYSIS_HH 1

#include "globals.hh"
#include "G4Cache.hh"
#include "DetectorHit.h"

class G4Event;
class G4Run;

class NTupleEventAnalisys {
  private:
    ///
    NTupleEventAnalisys() = default;

    ///
    ~NTupleEventAnalisys() = default;

    /// Delete the copy and move constructors
    NTupleEventAnalisys(const NTupleEventAnalisys &) = delete;
    NTupleEventAnalisys &operator=(const NTupleEventAnalisys &) = delete;
    NTupleEventAnalisys(NTupleEventAnalisys &&) = delete;
    NTupleEventAnalisys &operator=(NTupleEventAnalisys &&) = delete;

    class TTreeEventCollection {
      public:
        G4int m_evtId = -1;
        G4int m_ntupleId = -1;
        std::map<G4String, G4int> m_colId; // variable index within NTuples structure
    };

    ///
    class TTreeCollection {
      public:
        G4String m_name;
        G4String m_description;
        /// Single or many HitsCollections can be related to single TTree,
        /// see NTupleEventAnalisys::DefineTTree definition
        std::vector<G4String> m_hc_names;
    };

    ///
    G4int m_runId = -1;

    ///
    static std::vector<TTreeCollection> m_ttree_collection;

    ///
    G4MapCache<G4String,TTreeEventCollection> m_ntuple_collection;

    ///
    void CreateNTuple(const TTreeCollection& treeColl) {/* TODO */};

    ///
    G4int GetNTupleId(const G4String& treeName) {/* TODO */ return 0;};

    ///
    void FillEventCollection(const G4String& treeName, const G4Event *evt, DetectorHitsCollection* hitsColl) {/* TODO */};

    ///
    void FillNTupleEvent() {/* TODO */};

    ///
    void ClearEventCollections() {/* TODO */};


    public:
      ///
      static NTupleEventAnalisys* GetInstance();

      ///
      void BeginOfRun(const G4Run* runPtr, G4bool isMaster) {/* TODO */};

      ///
      void EndOfEventAction(const G4Event *evt) {/* TODO */};
};

#endif /* !EVENT_ANALYSIS_HH */