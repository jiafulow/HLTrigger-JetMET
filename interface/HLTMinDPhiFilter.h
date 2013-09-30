#ifndef HLTMinDPhiFilter_h_
#define HLTMinDPhiFilter_h_

/** \class  HLTMinDPhiFilter
 * 
 *  \brief  This rejects events using the minimum delta phi between a jet and MET.
 *  \author Jia Fu Low <jia.fu.low@cern.ch>
 *  \date   September 2013
 * 
 *  (Descriptions go here...)
 * 
 *  \todo   Also allow minDPhi using CaloJet & CaloMET?
 * 
 */

#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

// Class declaration
class HLTMinDPhiFilter : public HLTFilter {
  public:
    explicit HLTMinDPhiFilter(const edm::ParameterSet&);
    ~HLTMinDPhiFilter();
    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
    virtual bool hltFilter(edm::Event&, const edm::EventSetup&, trigger::TriggerFilterObjectWithRefs & filterproduct);
    
  private:
    /// Input tag for particle flow candidates
    bool excludePFMuons_;  // currently unused
    int maxNJets_;
    double minPt_;
    double maxEta_;
    double minDPhi_;
    edm::InputTag metLabel_;
    edm::InputTag jetsLabel_;
};


#endif  // HLTMinDPhiFilter_h_
