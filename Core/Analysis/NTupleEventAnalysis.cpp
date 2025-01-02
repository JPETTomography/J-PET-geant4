#include "NTupleEventAnalysis.h"
#include "G4Run.hh"
#include <G4SDManager.hh>
#include <G4Event.hh>
#include <TTree.h>
#include <TFile.h>

bool NTupleEventAnalysis::NTupleMerging = true;


void NTupleEventAnalysis::ScinHitCollection::Reset(){
    ClearAndReserve<int>(ScinId);
    ClearAndReserve<int>(TrkId);
    ClearAndReserve<int>(TrkPDG);
    ClearAndReserve<int>(NumOfInteractions);
    ClearAndReserve<int>(GenGammaIndex);
    ClearAndReserve<int>(GenGammaMultiplicity);
    ClearAndReserve<double>(EneDep);
    ClearAndReserve<double>(Time);
    ClearAndReserve<double>(PositionX);
    ClearAndReserve<double>(PositionY);
    ClearAndReserve<double>(PositionZ);
    ClearAndReserve<double>(PolarizationInX);
    ClearAndReserve<double>(PolarizationInY);
    ClearAndReserve<double>(PolarizationInZ);
    ClearAndReserve<double>(PolarizationOutX);
    ClearAndReserve<double>(PolarizationOutY);
    ClearAndReserve<double>(PolarizationOutZ);
    ClearAndReserve<double>(MomentumInX);
    ClearAndReserve<double>(MomentumInY);
    ClearAndReserve<double>(MomentumInZ);
    ClearAndReserve<double>(MomentumOutX);
    ClearAndReserve<double>(MomentumOutY);
    ClearAndReserve<double>(MomentumOutZ);
}

////////////////////////////////////////////////////////////////////////////////
///
NTupleEventAnalysis *NTupleEventAnalysis::GetInstance() {
    static NTupleEventAnalysis instance = NTupleEventAnalysis();
    return &instance;
}

////////////////////////////////////////////////////////////////////////////////
///
void NTupleEventAnalysis::BeginOfRunAction(const G4Run* runPtr, G4bool isMaster){
    m_analysisManager.Put(G4AnalysisManager::Instance());
    m_scinHitCollection.Put(ScinHitCollection());
    m_scinHitCollection.Get().runId = runPtr->GetRunID();
    CreateNTuple();
    CreateHistograms();
}

////////////////////////////////////////////////////////////////////////////////
///
void NTupleEventAnalysis::CreateNTuple(){
    auto& threadLocalScinHitColl = m_scinHitCollection.Get();
    const auto& threadLocalAnaG4Mngr = m_analysisManager.Get();

    auto ntupleId = threadLocalAnaG4Mngr->CreateNtuple("T","J-PET TTree");
    threadLocalScinHitColl.ntupleId = ntupleId;

    auto createNtupleIColumn = [&](const char* name){
        m_treeColumnId.Insert(name, threadLocalAnaG4Mngr->CreateNtupleIColumn(ntupleId, name)); 
    };
    auto createNtupleDColumn = [&](const char* name){
        m_treeColumnId.Insert(name, threadLocalAnaG4Mngr->CreateNtupleDColumn(ntupleId, name)); 
    };

    auto createNtupleVecIColumn = [&](const char* name,std::vector<int>& vec){
        m_treeColumnId.Insert(name, threadLocalAnaG4Mngr->CreateNtupleIColumn(ntupleId, name, vec)); 
    };

    auto createNtupleVecDColumn = [&](const char* name,std::vector<double>& vec){
        m_treeColumnId.Insert(name, threadLocalAnaG4Mngr->CreateNtupleDColumn(ntupleId, name, vec)); 
    };

    // General TTree branches
    createNtupleIColumn("G4EvtId");
    createNtupleIColumn("ThreadId");
    createNtupleIColumn("G4RunId");

    // Scintilator Hits branches
    createNtupleVecIColumn("ScinId",threadLocalScinHitColl.ScinId);
    createNtupleVecIColumn("ScinHitTrackId",threadLocalScinHitColl.TrkId);
    createNtupleVecIColumn("ScinHitTrackPDG",threadLocalScinHitColl.TrkPDG);
    createNtupleVecIColumn("ScinHitNIteractions",threadLocalScinHitColl.NumOfInteractions);
    createNtupleVecIColumn("ScinHitGenGammaIndex",threadLocalScinHitColl.GenGammaIndex);
    createNtupleVecIColumn("ScinHitGenGammaMult",threadLocalScinHitColl.GenGammaMultiplicity);
    createNtupleVecDColumn("ScinHitEneDep",threadLocalScinHitColl.EneDep);
    createNtupleVecDColumn("ScinHitTime",threadLocalScinHitColl.Time);
    createNtupleVecDColumn("ScinHitPositionX",threadLocalScinHitColl.PositionX);
    createNtupleVecDColumn("ScinHitPositionY",threadLocalScinHitColl.PositionY);
    createNtupleVecDColumn("ScinHitPositionZ",threadLocalScinHitColl.PositionZ);
    createNtupleVecDColumn("ScinHitPolarizationInX",threadLocalScinHitColl.PolarizationInX);
    createNtupleVecDColumn("ScinHitPolarizationInY",threadLocalScinHitColl.PolarizationInY);
    createNtupleVecDColumn("ScinHitPolarizationInZ",threadLocalScinHitColl.PolarizationInZ);
    createNtupleVecDColumn("ScinHitPolarizationOutX",threadLocalScinHitColl.PolarizationOutX);
    createNtupleVecDColumn("ScinHitPolarizationOutY",threadLocalScinHitColl.PolarizationOutY);
    createNtupleVecDColumn("ScinHitPolarizationOutZ",threadLocalScinHitColl.PolarizationOutZ);
    createNtupleVecDColumn("ScinHitMomentumInX",threadLocalScinHitColl.MomentumInX);
    createNtupleVecDColumn("ScinHitMomentumInY",threadLocalScinHitColl.MomentumInY);
    createNtupleVecDColumn("ScinHitMomentumInZ",threadLocalScinHitColl.MomentumInZ);
    createNtupleVecDColumn("ScinHitMomentumOutX",threadLocalScinHitColl.MomentumOutX);
    createNtupleVecDColumn("ScinHitMomentumOutY",threadLocalScinHitColl.MomentumOutY);
    createNtupleVecDColumn("ScinHitMomentumOutZ",threadLocalScinHitColl.MomentumOutZ);

    //
    threadLocalAnaG4Mngr->FinishNtuple(ntupleId);
}


////////////////////////////////////////////////////////////////////////////////
///
void NTupleEventAnalysis::CreateHistograms(){
    const auto& threadLocalAnaG4Mngr = m_analysisManager.Get();

    auto createH1 = [&](const char* name, const char* title, const char* xLabel, const char* yLabel, int nBins, double min, double max){
        auto histId = threadLocalAnaG4Mngr->CreateH1(name,title,nBins,min,max);
        m_histId.Insert(name,histId);
        threadLocalAnaG4Mngr->SetH1XAxisTitle(histId,xLabel);
        threadLocalAnaG4Mngr->SetH1YAxisTitle(histId,yLabel);
    };

    auto createH2 = [&](const char* name, const char* title, const char* xLabel, const char* yLabel, int nBinsX, double minX, double maxX,
                                                                                                     int nBinsY, double minY, double maxY){
        auto histId = threadLocalAnaG4Mngr->CreateH2(name,title,nBinsX,minX,maxX,nBinsY,minY,maxY);
        m_histId.Insert(name,histId);
        threadLocalAnaG4Mngr->SetH2XAxisTitle(histId,xLabel);
        threadLocalAnaG4Mngr->SetH2YAxisTitle(histId,yLabel);
    };
    
    createH1("gen_gamma_multiplicity", "Generated gammas multiplicity. Bin size: 1", "Gamma quanta multiplicity: 1=prompt; 2=2g; 3=3g", "Entries", 10, -0.5, 9.5);
    
    createH2("gen_XY", "Generated XY coordinates of annihilation point. Bin size: 0.1 cm x 0.1 cm", "Annihilation point (2/3g) X [cm]", "Annihilation point (2/3g) Y [cm]", 500, -24.95, 25.05, 500, -24.95, 25.05);
}

////////////////////////////////////////////////////////////////////////////////
///
void NTupleEventAnalysis::EndOfEventAction(const G4Event *evt){
    // NOTE:
    // Related SensitiveDetector collection ID (Geant4 architecture)
    // collID==-1 the collection is not found
    // collID==-2 the collection name is ambiguous
    auto collection_id = G4SDManager::GetSDMpointer()->GetCollectionID(G4String("detectorCollection"));
    if(collection_id<0){
        G4cout<< "[ERROR]:: NTupleEventAnalysis::EndOfEventAction::G4SDManager err: " << collection_id  << G4endl;
        return;
    }
    auto hCofThisEvent = evt->GetHCofThisEvent();
    auto hitsColl = dynamic_cast<DetectorHitsCollection*>(hCofThisEvent->GetHC(collection_id));

    auto emplaceBackG4ThreeVector = []( std::vector<double>& x,
                                        std::vector<double>& y,
                                        std::vector<double>& z,
                                        const G4ThreeVector& vec,
                                        double unit = 0){
        unit = unit==0 ? 1:unit;
        x.emplace_back(static_cast<double>(vec.getX()/unit));
        y.emplace_back(static_cast<double>(vec.getY()/unit));
        z.emplace_back(static_cast<double>(vec.getZ()/unit));
    };

    if (hitsColl) {
        auto& threadLocalScinHitColl = m_scinHitCollection.Get();
        threadLocalScinHitColl.Reset();
        int n_hit = hitsColl->entries();
        for (int i = 0; i < n_hit; i++) {
            auto hit = dynamic_cast<DetectorHit*>(hitsColl->GetHit(i));

            // Forcefully cut the remnants from the cut on photon durinng first interaction   
            double EnergyDeposit = hit->GetEdep();
            // Removing remnants from the energy deposition cut on prim photon
            if (EnergyDeposit < .511 - m_EvtMessenger->GetEnergyCut() && 
                m_EvtMessenger->GetEnergyCutFlag()) 
                    continue;

            // Add new hit to output NTuple
            threadLocalScinHitColl.ScinId.emplace_back(hit->GetScinID());
            threadLocalScinHitColl.TrkId.emplace_back(hit->GetTrackID());
            threadLocalScinHitColl.TrkPDG.emplace_back(hit->GetTrackPDG());
            threadLocalScinHitColl.NumOfInteractions.emplace_back(hit->GetNumInteractions());
            threadLocalScinHitColl.GenGammaIndex.emplace_back(hit->GetGenGammaIndex());
            threadLocalScinHitColl.GenGammaMultiplicity.emplace_back(hit->GetGenGammaMultiplicity());
            threadLocalScinHitColl.EneDep.emplace_back(hit->GetEdep()/keV);
            threadLocalScinHitColl.Time.emplace_back(hit->GetTime()/ps);
            emplaceBackG4ThreeVector(threadLocalScinHitColl.PositionX,
                                     threadLocalScinHitColl.PositionY,
                                     threadLocalScinHitColl.PositionZ, hit->GetPosition(),cm);
            emplaceBackG4ThreeVector(threadLocalScinHitColl.PolarizationInX,
                                     threadLocalScinHitColl.PolarizationInY,
                                     threadLocalScinHitColl.PolarizationInZ, hit->GetPolarizationIn());
            emplaceBackG4ThreeVector(threadLocalScinHitColl.PolarizationOutX,
                                     threadLocalScinHitColl.PolarizationOutY,
                                     threadLocalScinHitColl.PolarizationOutZ, hit->GetPolarizationOut());
            emplaceBackG4ThreeVector(threadLocalScinHitColl.MomentumInX,
                                     threadLocalScinHitColl.MomentumInY,
                                     threadLocalScinHitColl.MomentumInZ, hit->GetMomentumIn(),keV);
            emplaceBackG4ThreeVector(threadLocalScinHitColl.MomentumOutX,
                                     threadLocalScinHitColl.MomentumOutY,
                                     threadLocalScinHitColl.MomentumOutZ, hit->GetMomentumOut(),keV);
        }
        if(threadLocalScinHitColl.ScinId.size()>0){
            FillNTupleEvent(evt->GetEventID()+1);
            const auto& threadLocalAnaG4Mngr = m_analysisManager.Get();
            
            threadLocalAnaG4Mngr->FillH1(m_histId.Get("gen_gamma_multiplicity"),2);

            threadLocalAnaG4Mngr->FillH2(m_histId.Get("gen_XY"),2,4);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
///
void NTupleEventAnalysis::FillNTupleEvent(const G4int& evtId){
    const auto& threadLocalAnaG4Mngr = m_analysisManager.Get();
    auto ntupleId = m_scinHitCollection.Get().ntupleId;

    auto fillNtupleIColumn = [&](const char* name, G4int val){
          threadLocalAnaG4Mngr->FillNtupleIColumn(ntupleId,m_treeColumnId.Get(name), val); // ,
        };

    auto fillNtupleDColumn = [&](const char* name, G4double val){
        threadLocalAnaG4Mngr->FillNtupleDColumn(ntupleId,m_treeColumnId.Get(name), val);// ,
    };

    fillNtupleIColumn("G4EvtId",evtId);
    fillNtupleIColumn("ThreadId",G4Threading::G4GetThreadId()); // 
    fillNtupleIColumn("G4RunId",m_scinHitCollection.Get().runId);

    // Note: For std::vector<> ROOT automatically manages the memory and data storage for the vector 
    // elements. This means that when you call tree->Fill(), the current state of the std::vector 
    // is saved directly into the tree without needing additional fill methods.
    // In Geant4 the AddNtupleRow method is a wrapper for the tree->Fill() function.
    threadLocalAnaG4Mngr->AddNtupleRow(ntupleId); // 

}

