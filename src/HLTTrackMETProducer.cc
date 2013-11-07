/** \class  HLTTrackMETProducer
 * 
 *  \author Michele de Gruttola <michele.de.gruttola@cern.ch>
 * 
 *  See header file for more information.
 * 
 */

#include "HLTrigger/JetMET/interface/HLTTrackMETProducer.h"

#include <vector>
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecTrackFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"


// Constructor
HLTTrackMETProducer::HLTTrackMETProducer(const edm::ParameterSet& iConfig)
      : usePt_(iConfig.getParameter<bool>("usePt")),
        useJets_(iConfig.getParameter<bool>("useJets")),
        useTracks_(iConfig.getParameter<bool>("useTracks")),
        excludePFMuons_(iConfig.getParameter<bool>("excludePFMuons")),
        minPt_(iConfig.getParameter<double>("minPt")),
        maxEta_(iConfig.getParameter<double>("maxEta")),
        scale_(iConfig.getParameter<double>("scale")),
        pfCandidatesLabel_(iConfig.getParameter<edm::InputTag>("pfCandidatesLabel")),
        pfJetsLabel_(iConfig.getParameter<edm::InputTag>("pfJetsLabel")),
        pfRecTracksLabel_(iConfig.getParameter<edm::InputTag>("pfRecTracksLabel")) {
    // Register your products
    produces<reco::METCollection>();
}

// Destructor
HLTTrackMETProducer::~HLTTrackMETProducer() {}

// Fill descriptions
void HLTTrackMETProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<bool>("usePt", true);
    desc.add<bool>("useJets", false);
    desc.add<bool>("useTracks", false);
    desc.add<bool>("excludePFMuons", false);
    desc.add<double>("minPt", 0.0);
    desc.add<double>("maxEta", 9999.0);
    desc.add<double>("scale", 1.0);
    desc.add<edm::InputTag>("pfCandidatesLabel", edm::InputTag("hltPFNoPileUp"));
    desc.add<edm::InputTag>("pfJetsLabel", edm::InputTag("hltAK5PFJetL1FastL2L3Corrected"));
    desc.add<edm::InputTag>("pfRecTracksLabel", edm::InputTag("hltLightPFTracks"));
    descriptions.add("hltTrackMetProducer",desc);
}

// Produce the products
void HLTTrackMETProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace std;
    using namespace edm;
    using namespace reco;
    
    std::auto_ptr<reco::METCollection> result(new reco::METCollection()); 
    
    edm::Handle<reco::PFRecTrackCollection> pfRecTracks;
    edm::Handle<reco::PFCandidateCollection> pfCandidates;
    
    if (useTracks_) {
        iEvent.getByLabel(pfRecTracksLabel_, pfRecTracks);
    } else {
        iEvent.getByLabel(pfCandidatesLabel_, pfCandidates);
    }

    double mpx=0., mpy=0.;
    if (useTracks_ && pfRecTracks->size() > 0) {
        for(reco::PFRecTrackCollection::const_iterator track = pfRecTracks->begin(); track != pfRecTracks->end(); ++track) {
            const reco::TrackRef& trackRef = track->trackRef();
            double px = trackRef->px();
            double py = trackRef->py();
            double pt = trackRef->pt();
            double eta = trackRef->eta();

            if (pt > minPt_ && std::abs(eta) < maxEta_) {
                mpx -= px;
                mpy -= py;
            }
        }
    } else if (pfCandidates->size() > 0) {
        for(reco::PFCandidateCollection::const_iterator cand = pfCandidates->begin(); cand != pfCandidates->end(); ++cand) {
            if (cand->charge() == 0)  continue;
            double px = cand->px();
            double py = cand->py();
            double pt = cand->pt();
            double eta = cand->eta();
            
            if (pt > minPt_ && std::abs(eta) < maxEta_) {
                mpx -= px;
                mpy -= py;
            }
        }
    }
    
    reco::MET::LorentzVector p4(mpx, mpy, 0, sqrt(mpx*mpx + mpy*mpy));
    reco::MET::Point vtx(0, 0, 0);
    reco::MET mpt(p4, vtx);
    result->push_back(mpt);
    
    // Put your products into the Event
    iEvent.put(result);

}
