#ifndef HLTTrackMETProducer_h_
#define HLTTrackMETProducer_h_

/** \class  HLTTrackMETProducer
 * 
 *  \brief  This creates a PFMET object using only charged particles
 *  \author Michele de Gruttola <michele.de.gruttola@cern.ch>
 *  \date   August 2013
 * 
 *  (Descriptions go here...)
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
class HLTTrackMETProducer : public edm::EDProducer {
  public:
    explicit HLTTrackMETProducer(const edm::ParameterSet&);
    ~HLTTrackMETProducer();
    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
    virtual void produce(edm::Event&, const edm::EventSetup&);
    
  private:
    bool usePt_;  // currently unused
    bool useJets_;  // currently unused
    bool useTracks_;
    bool excludePFMuons_;  // currently unused
    double minPt_;
    double maxEta_;
    double scale_;
    edm::InputTag pfCandidatesLabel_;
    edm::InputTag pfJetsLabel_;
    edm::InputTag pfRecTracksLabel_;
};

#endif  //HLTTrackMETProducer_h_

