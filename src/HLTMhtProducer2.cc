/** \class HLTMhtProducer2
 *
 * See header file for documentation
 *
 *  \author Steven Lowette
 *
 */

#include "HLTrigger/JetMET/interface/HLTMhtProducer2.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecTrackFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"


// Constructor
HLTMhtProducer2::HLTMhtProducer2(const edm::ParameterSet & iConfig) :
  usePt_                  ( iConfig.getParameter<bool>("usePt") ),
  useTracks_              ( iConfig.getParameter<bool>("useTracks") ),
  usePFRecTracks_         ( iConfig.getParameter<bool>("usePFRecTracks") ),
  usePFCandidatesCharged_ ( iConfig.getParameter<bool>("usePFCandidatesCharged") ),
  usePFCandidates_        ( iConfig.getParameter<bool>("usePFCandidates") ),
  excludePFMuons_         ( iConfig.getParameter<bool>("excludePFMuons") ),
  minPtJet_               ( iConfig.getParameter<double>("minPtJet") ),
  maxEtaJet_              ( iConfig.getParameter<double>("maxEtaJet") ),
  jetsLabel_              ( iConfig.getParameter<edm::InputTag>("jetsLabel") ),
  tracksLabel_            ( iConfig.getParameter<edm::InputTag>("tracksLabel") ),
  pfRecTracksLabel_       ( iConfig.getParameter<edm::InputTag>("pfRecTracksLabel") ),
  pfCandidatesLabel_      ( iConfig.getParameter<edm::InputTag>("pfCandidatesLabel") ) {

    // Register the products
    produces<reco::METCollection>();
}

// Destructor
HLTMhtProducer2::~HLTMhtProducer2() {}

// Fill descriptions
void HLTMhtProducer2::fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<bool>("usePt", true);
    desc.add<bool>("useTracks", false);
    desc.add<bool>("usePFRecTracks", false);
    desc.add<bool>("usePFCandidatesCharged", false);
    desc.add<bool>("usePFCandidates", false);
    desc.add<bool>("excludePFMuons", false);
    desc.add<double>("minPtJet", 0);
    desc.add<double>("maxEtaJet", 999);
    desc.add<edm::InputTag>("jetsLabel", edm::InputTag("hltAntiKT5PFJets"));
    desc.add<edm::InputTag>("tracksLabel",  edm::InputTag(""));  // set to hltL3Muons?
    desc.add<edm::InputTag>("pfRecTracksLabel",  edm::InputTag(""));  // set to hltLightPFTracks?
    desc.add<edm::InputTag>("pfCandidatesLabel",  edm::InputTag(""));  // set to hltParticleFlow?
    descriptions.add("hltMhtProducer2", desc);
}

// Produce the products
void HLTMhtProducer2::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {

    // Create a pointer to the products
    std::auto_ptr<reco::METCollection> result(new reco::METCollection());

    if (pfCandidatesLabel_.label() == "")
        excludePFMuons_ = false;

    bool useJets = !useTracks_ && !usePFRecTracks_ && !usePFCandidatesCharged_ && !usePFCandidates_;

    edm::Handle<edm::View<reco::Jet> > jets;
    if (useJets) iEvent.getByLabel(jetsLabel_, jets);

    edm::Handle<reco::TrackCollection> tracks;
    if (useTracks_) iEvent.getByLabel(tracksLabel_, tracks);

    edm::Handle<reco::PFRecTrackCollection> pfRecTracks;
    if (usePFRecTracks_) iEvent.getByLabel(pfRecTracksLabel_, pfRecTracks);

    edm::Handle<reco::PFCandidateCollection> pfCandidates;
    if (excludePFMuons_ || usePFCandidatesCharged_ || usePFCandidates_)
        iEvent.getByLabel(pfCandidatesLabel_, pfCandidates);

    double mhx = 0., mhy = 0.;

    if (useJets && jets->size() > 0) {
        for(edm::View<reco::Jet>::const_iterator j = jets->begin(); j != jets->end(); ++j) {
            double pt = usePt_ ? j->pt() : j->et();
            double eta = j->eta();
            double phi = j->phi();
            double px = usePt_ ? j->px() : j->pt() * cos(phi);
            double py = usePt_ ? j->py() : j->pt() * sin(phi);

            if (pt > minPtJet_ && std::abs(eta) < maxEtaJet_) {
                mhx -= px;
                mhy -= py;
            }
        }

    } else if (useTracks_ && tracks->size() > 0) {
        for (reco::TrackCollection::const_iterator j = tracks->begin(); j != tracks->end(); ++j) {
            double pt = j->pt();
            double px = j->px();
            double py = j->py();
            double eta = j->eta();

            if (pt > minPtJet_ && std::abs(eta) < maxEtaJet_) {
                mhx -= px;
                mhy -= py;
            }
        }

    } else if (usePFRecTracks_ && pfRecTracks->size() > 0) {
        for (reco::PFRecTrackCollection::const_iterator j = pfRecTracks->begin(); j != pfRecTracks->end(); ++j) {
            double pt = j->trackRef()->pt();
            double px = j->trackRef()->px();
            double py = j->trackRef()->py();
            double eta = j->trackRef()->eta();

            if (pt > minPtJet_ && std::abs(eta) < maxEtaJet_) {
                mhx -= px;
                mhy -= py;
            }
        }

    } else if ((usePFCandidatesCharged_ || usePFCandidates_) && pfCandidates->size() > 0) {
        for (reco::PFCandidateCollection::const_iterator j = pfCandidates->begin(); j != pfCandidates->end(); ++j) {
            if (usePFCandidatesCharged_ && j->charge() == 0)  continue;
            double pt = j->pt();
            double px = j->px();
            double py = j->py();
            double eta = j->eta();

            if (pt > minPtJet_ && std::abs(eta) < maxEtaJet_) {
                mhx -= px;
                mhy -= py;
            }
        }
    }

    if (excludePFMuons_) {
        for (reco::PFCandidateCollection::const_iterator j = pfCandidates->begin(); j != pfCandidates->end(); ++j) {
            if (std::abs(j->pdgId()) == 13) {
                mhx += j->px();
                mhy += j->py();
            }
        }
    }

    reco::MET::LorentzVector p4(mhx, mhy, 0, sqrt(mhx*mhx + mhy*mhy));
    reco::MET::Point vtx(0, 0, 0);
    reco::MET mht(p4, vtx);
    result->push_back(mht);

    // Put the products into the Event
    iEvent.put(result);
}
