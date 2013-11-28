/** \class HLTJetVBFFilter2
 *
 *
 *  \author Monica Vazquez Acosta (CERN)
 *  \author Phat Srimanobhas (srimanob@mail.cern.ch)
 *
 *  See header file for more information.
 *
 */

#include "HLTrigger/JetMET/interface/HLTJetVBFFilter2.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
//#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
//#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
//#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"


// Constructor
template<typename T>
HLTJetVBFFilter2<T>::HLTJetVBFFilter2(const edm::ParameterSet& iConfig) : HLTFilter(iConfig),
  etaOpposite_    (iConfig.getParameter<bool>("etaOpposite")),
  leadingJetOnly_ (iConfig.getParameter<bool>("leadingJetOnly")),
  triggerType_    (iConfig.getParameter<int> ("triggerType")),
  minPtLow_       (iConfig.getParameter<double>("minPtLow")),
  minPtHigh_      (iConfig.getParameter<double>("minPtHigh")),
  maxEta_         (iConfig.getParameter<double>("maxEta")),
  minDeltaEta_    (iConfig.getParameter<double>("minDeltaEta")),
  minInvMass_     (iConfig.getParameter<double>("minInvMass")),
  inputTag_       (iConfig.getParameter<edm::InputTag>("inputTag")) {
    LogDebug("") << "HLTJetVBFFilter2: inputTag_/minPtLow_/minPtHigh_/triggerType_: "
                 << inputTag_.encode() << " "
                 << minPtLow_  << " "
                 << minPtHigh_ << " "
                 << triggerType_;
}

// Destructor
template<typename T>
HLTJetVBFFilter2<T>::~HLTJetVBFFilter2() {}

// Fill descriptions
template<typename T>
void HLTJetVBFFilter2<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    makeHLTFilterDescription(desc);
    desc.add<bool>("etaOpposite", false);
    desc.add<bool>("leadingJetOnly", false);
    desc.add<double>("minPtLow", 40.);
    desc.add<double>("minPtHigh", 40.);
    desc.add<double>("maxEta", 5.0);
    desc.add<double>("minDeltaEta", 4.0);
    desc.add<double>("minInvMass", 1000.);
    desc.add<int>("triggerType", trigger::TriggerJet);
    desc.add<edm::InputTag>("inputTag", edm::InputTag("hltAK5PFJetL1FastL2L3Corrected"));
    descriptions.add(std::string("hlt")+std::string(typeid(HLTJetVBFFilter2<T>).name()), desc);
}

// Make filter decision
template<typename T>
bool HLTJetVBFFilter2<T>::hltFilter(edm::Event& iEvent, const edm::EventSetup& iSetup, trigger::TriggerFilterObjectWithRefs & filterproduct) {

    // The filter object
    if (saveTags()) filterproduct.addCollectionTag(inputTag_);

    edm::Handle<TCollection> jets;  // assumed to be sorted by pT
    iEvent.getByLabel(inputTag_, jets);

    // Look at all candidates, check cuts and add to filter object
    int n(0);
    if(jets->size() > 1) {

        // Loop on all jets
        int countJet1 = 0;
        int countJet2 = 0;
        for (typename TCollection::const_iterator j1 = jets->begin(); j1 != jets->end(); ++j1) {
            ++countJet1;
            double ptj1 = j1->pt();
            double etaj1 = j1->eta();

            if (leadingJetOnly_ && countJet1 > 2)  break;
            if (ptj1 < minPtHigh_)  break;  // No need to go to the next jets (lower pT)
            if (std::abs(etaj1) > maxEta_)  continue;

            countJet2 = countJet1-1;
            for (typename TCollection::const_iterator j2 = j1+1; j2 != jets->end(); ++j2) {
                ++countJet2;
                double ptj2 = j2->pt();
                double etaj2 = j2->eta();

                if (leadingJetOnly_ && countJet2 > 2)  break;
                if (ptj2 < minPtLow_)  break;  // No need to go to the next jets (lower pT)
                if (std::abs(etaj2) > maxEta_)  continue;

                double deltaEta = std::abs(etaj1 - etaj2);
                double invMass = (j1->p4() + j2->p4()).mass();

                // VBF cuts
                if ( (ptj1 > minPtHigh_) &&
                     (ptj2 > minPtLow_) &&
                     ( (etaOpposite_ && (etaj1 * etaj2) < 0) || !etaOpposite_ ) &&
                     (deltaEta > minDeltaEta_) &&
                     (invMass > minInvMass_) ) {
                    ++n;

                    TRef ref1(jets, distance(jets->begin(), j1));
                    TRef ref2(jets, distance(jets->begin(), j2));
                    filterproduct.addObject(triggerType_, ref1);
                    filterproduct.addObject(triggerType_, ref2);
                }
                //if(n>=1) break;  // Store all possible pairs
            }
        }  // loop on all jets
    }  // events with two or more jets

    // filter decision
    bool accept(n>=1);

    return accept;
}
