/** \class  HLTMinDPhiMETFilter
 *
 *  \author Jia Fu Low <jia.fu.low@cern.ch>
 *
 *  See header file for more information.
 *
 */

#include "HLTrigger/JetMET/interface/HLTMinDPhiMETFilter.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
//#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
//#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
//#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METFwd.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/Math/interface/deltaPhi.h"


// Constructor
template<typename T>
HLTMinDPhiMETFilter<T>::HLTMinDPhiMETFilter(const edm::ParameterSet& iConfig) : HLTFilter(iConfig),
  usePt_          (iConfig.getParameter<bool>("usePt")),
  //excludePFMuons_ (iConfig.getParameter<bool>("excludePFMuons")),
  triggerType_    (iConfig.getParameter<int>("triggerType")),
  maxNJets_       (iConfig.getParameter<int>("maxNJets")),
  minPt_          (iConfig.getParameter<double>("minPt")),
  maxEta_         (iConfig.getParameter<double>("maxEta")),
  minDPhi_        (iConfig.getParameter<double>("minDPhi")),
  metLabel_       (iConfig.getParameter<edm::InputTag>("metLabel")),
  jetsLabel_      (iConfig.getParameter<edm::InputTag>("jetsLabel")) {
}

// Destructor
template<typename T>
HLTMinDPhiMETFilter<T>::~HLTMinDPhiMETFilter() {}

// Fill descriptions
template<typename T>
void HLTMinDPhiMETFilter<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    makeHLTFilterDescription(desc);
    desc.add<bool>("usePt", true);
    //desc.add<bool>("excludePFMuons", false);
    desc.add<int>("triggerType", trigger::TriggerJet);
    desc.add<int>("maxNJets", 999);
    desc.add<double>("minPt", 0.0);
    desc.add<double>("maxEta", 9999.0);
    desc.add<double>("minDPhi", 0.5);
    desc.add<edm::InputTag>("metLabel", edm::InputTag("hltPFMETProducer"));
    desc.add<edm::InputTag>("jetsLabel", edm::InputTag("hltAK5PFJetL1FastL2L3Corrected"));
    descriptions.add(std::string("hlt")+std::string(typeid(HLTMinDPhiMETFilter<T>).name()), desc);
}

// Make filter decision
template<typename T>
bool HLTMinDPhiMETFilter<T>::hltFilter(edm::Event& iEvent, const edm::EventSetup& iSetup, trigger::TriggerFilterObjectWithRefs & filterproduct) {

    // The filter object
    if (saveTags()) filterproduct.addCollectionTag(jetsLabel_);

    edm::Handle<reco::METCollection> mets;
    iEvent.getByLabel(metLabel_, mets);

    edm::Handle<TCollection> jets;  // assume to be sorted by pT
    iEvent.getByLabel(jetsLabel_, jets);

    double minDPhi = 9999.;
    int nJets = 0;  // nJets counts all jets in the events, not only those that pass pt, eta requirements

    if (mets->size() > 0 && jets->size() > 0) {
        const reco::MET& met = mets->at(0);
        for (typename TCollection::const_iterator j = jets->begin(); j != jets->end(); ++j) {
            if (nJets >= maxNJets_)
                break;

            double pt = usePt_ ? j->pt() : j->et();
            double eta = j->eta();
            double phi = j->phi();
            if (pt > minPt_ && std::abs(eta) < maxEta_) {
                double dPhi = std::abs(reco::deltaPhi(met.phi(), phi));
                if (minDPhi > dPhi) {
                    minDPhi = dPhi;
                }

                TRef ref(jets, distance(jets->begin(), j));
                filterproduct.addObject(triggerType_, ref);
            }

            nJets++;
        }
    }

    bool accept(minDPhi > minDPhi_);

    return accept;
}
