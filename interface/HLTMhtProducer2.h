#ifndef HLTMhtProducer2_h_
#define HLTMhtProducer2_h_

/** \class HLTMhtProducer2
 *
 *  \brief  This produces a reco::MET object that stores MHT (or MET)
 *  \author Steven Lowette
 *  \author Michele de Gruttola, Jia Fu Low (Nov 2013)
 *
 *  MHT (or MET) is calculated using input CaloJet or PFJet collection.
 *  MHT can include or exclude the contribution from muons.
 *
 */

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"


namespace edm {
    class ConfigurationDescriptions;
}

// Class declaration
class HLTMhtProducer2 : public edm::EDProducer {
  public:
    explicit HLTMhtProducer2(const edm::ParameterSet & iConfig);
    ~HLTMhtProducer2();
    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
    virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  private:
    /// Use pt; otherwise, use et.
    bool usePt_;

    /// Exclude PF muons in the MHT calculation (but not HT)
    /// Ignored if pfCandidatesLabel_ is empty.
    bool excludePFMuons_;

    /// Minimum number of jets passing pt and eta requirements
    int minNJet_;

    /// Minimum pt requirement for jets
    double minPtJet_;

    /// Maximum (abs) eta requirement for jets
    double maxEtaJet_;

    /// Input jet, PFCandidate collections
    edm::InputTag jetsLabel_;
    edm::InputTag pfCandidatesLabel_;

    edm::EDGetTokenT<reco::JetView> m_theJetToken;
    edm::EDGetTokenT<reco::PFCandidateCollection> m_thePFCandidateToken;
};

#endif  // HLTMhtProducer2_h_

