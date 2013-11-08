#ifndef HLTHtMhtProducer2_h_
#define HLTHtMhtProducer2_h_

/** \class HLTHtMhtProducer2
 *
 *  \brief  This produces a reco::MET object that stores HT and MHT
 *  \author Steven Lowette
 *  \author Michele de Gruttola, Jia Fu Low (Nov 2013)
 *
 *  HT & MHT can be calculated using one of the input collections:
 *    - Calo or PF jets (default)
 *    - reco tracks
 *    - PF tracks
 *    - PF charged candidates
 *    - PF candidates
 *  HT is stored as `sumet_`, MHT is stored as `p4_` in the output.
 *
 */

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


namespace edm {
    class ConfigurationDescriptions;
}

// Class declaration
class HLTHtMhtProducer2 : public edm::EDProducer {
  public:
    explicit HLTHtMhtProducer2(const edm::ParameterSet & iConfig);
    ~HLTHtMhtProducer2();
    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
    virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  private:
    /// Use pt; otherwise, use et.
    /// Ignored if jets are not used as input.
    bool usePt_;

    /// Use reco tracks as input instead of jets.
    /// If true, it overrides usePFRecTracks, usePFCandidatesCharged_ & usePFCandidates_.
    bool useTracks_;

    /// Use PF tracks as input instead of jets.
    /// If true, it overrides usePFCandidatesCharged_ & usePFCandidates_.
    bool usePFRecTracks_;

    /// Use PF charged candidates as input instead of jets.
    /// If true, it overrides usePFCandidates_.
    bool usePFCandidatesCharged_;

    /// Use PF candidates as input instead of jets.
    bool usePFCandidates_;

    /// Exclude PF muons in the MHT calculation (but not HT)
    /// Ignored if pfCandidatesLabel_ is empty.
    bool excludePFMuons_;

    /// Minimum number of jets passing pt and eta requirements
    /// Ignored if jets are not used as input
    int minNJetHt_;
    int minNJetMht_;

    /// Minimum pt requirement for jets (or objects used as input)
    double minPtJetHt_;
    double minPtJetMht_;

    /// Maximum (abs) eta requirement for jets (or objects used as input)
    double maxEtaJetHt_;
    double maxEtaJetMht_;

    /// Input jet, track, PFRecTrack, PFCandidate collections
    edm::InputTag jetsLabel_;
    edm::InputTag tracksLabel_;
    edm::InputTag pfRecTracksLabel_;
    edm::InputTag pfCandidatesLabel_;
};

#endif  // HLTHtMhtProducer2_h_

