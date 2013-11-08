/** \class  HLTMinDPhiMETFilter
 * 
 *  \author Jia Fu Low <jia.fu.low@cern.ch>
 * 
 *  See header file for more information.
 * 
 */

#include "HLTrigger/JetMET/interface/HLTMinDPhiMETFilter.h"

//#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METFwd.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Math/interface/deltaPhi.h"


// Constructor
HLTMinDPhiMETFilter::HLTMinDPhiMETFilter(const edm::ParameterSet& iConfig) : HLTFilter(iConfig),
  excludePFMuons_(iConfig.getParameter<bool>("excludePFMuons")),
  maxNJets_      (iConfig.getParameter<int>("maxNJets")),
  minPt_         (iConfig.getParameter<double>("minPt")),
  maxEta_        (iConfig.getParameter<double>("maxEta")),
  minDPhi_       (iConfig.getParameter<double>("minDPhi")),
  metLabel_      (iConfig.getParameter<edm::InputTag>("metLabel")),
  jetsLabel_     (iConfig.getParameter<edm::InputTag>("jetsLabel")) {
}

// Destructor
HLTMinDPhiMETFilter::~HLTMinDPhiMETFilter() {}

// Fill descriptions
void HLTMinDPhiMETFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<bool>("saveTags", false);
    desc.add<bool>("excludePFMuons", false);
    desc.add<int>("maxNJets", 999);
    desc.add<double>("minPt", 0.0);
    desc.add<double>("maxEta", 9999.0);
    desc.add<double>("minDPhi", 0.5);
    desc.add<edm::InputTag>("metLabel", edm::InputTag("hltPFMETProducer"));
    desc.add<edm::InputTag>("jetsLabel", edm::InputTag("hltAK5PFJetL1FastL2L3Corrected"));
    descriptions.add("hltMinDPhiFilter",desc);
}

bool HLTMinDPhiMETFilter::hltFilter(edm::Event& iEvent, const edm::EventSetup& iSetup, trigger::TriggerFilterObjectWithRefs & filterproduct) {
    // The filter object
    if (saveTags()) filterproduct.addCollectionTag(jetsLabel_);
    
    edm::Handle<reco::METCollection> mets;
    iEvent.getByLabel(metLabel_, mets);
    
    edm::Handle<reco::PFJetCollection> jets;
    iEvent.getByLabel(jetsLabel_, jets);
    
    double minDPhi = 9999.;
    int nJets = 0;
    
    if (mets->size() > 0 && jets->size() > 0) {
        const reco::MET& met = mets->at(0);
        for (reco::PFJetCollection::const_iterator jet = jets->begin(); jet != jets->end(); ++jet) {
            double pt = jet->pt();
            double eta = jet->eta();
            double phi = jet->phi();
            if (pt > minPt_ && std::abs(eta) < maxEta_) {
                double dPhi = std::abs(reco::deltaPhi(met.phi(), phi));
                if (minDPhi > dPhi) {
                    minDPhi = dPhi;
                }
                reco::PFJetRef ref(jets,distance(jets->begin(),jet));
                filterproduct.addObject(trigger::TriggerJet, ref);  // perhaps only save the jet with minDPhi?
                nJets += 1;
            }
        }
    }
    
    bool accept(minDPhi > minDPhi_);
    
    return accept;
}

