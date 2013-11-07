#ifndef HLTMhtProducer2_h_
#define HLTMhtProducer2_h_

/** \class HLTMhtProducer2
 *
 *  \brief  This produces a reco::MET object that stores MHT (or MET)
 *  \author Steven Lowette
 *  \author Michele de Gruttola, Jia Fu Low (Nov 2013)
 *
 *  MHT (or MET) can be calculated using one of the input collections:
 *    - Calo or PF jets (default)
 *    - reco tracks
 *    - PF tracks
 *    - PF charged candidates
 *    - PF candidates
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
class HLTMhtProducer2 : public edm::EDProducer {
  public:

    explicit HLTMhtProducer2(const edm::ParameterSet & iConfig);
    ~HLTMhtProducer2();
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

    /// Minimum pt requirement for jets (or objects used as input)
    double minPtJet_;

    /// Maximum (abs) eta requirement for jets (or objects used as input)
    double maxEtaJet_;

    /// Input jet, track, PFRecTrack, PFCandidate collections
    edm::InputTag jetsLabel_;
    edm::InputTag tracksLabel_;
    edm::InputTag pfRecTracksLabel_;
    edm::InputTag pfCandidatesLabel_;
};

#endif  // HLTMhtProducer2_h_
