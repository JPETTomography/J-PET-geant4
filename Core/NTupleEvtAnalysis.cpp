#include "NTupleEvtAnalysis.h"

std::vector<NTupleEventAnalisys::TTreeCollection> NTupleEventAnalisys::m_ttree_collection = std::vector<TTreeCollection>();

////////////////////////////////////////////////////////////////////////////////
///
NTupleEventAnalisys *NTupleEventAnalisys::GetInstance() {
    static NTupleEventAnalisys instance = NTupleEventAnalisys();
    return &instance;
}