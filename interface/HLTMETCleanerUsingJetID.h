#ifndef HLTMETCleanerUsingJetID_h_
#define HLTMETCleanerUsingJetID_h_

/** \class  HLTMETCleanerUsingJetID
 *
 *  \brief  This creates a MET object from the difference in MET between two
 *          input jet collections.
 *  \author Jia Fu Low <jia.fu.low@cern.ch>
 *  \date   November 2013
 *
 *  This code creates a new MET vector defined as:
 *
 *    output MET = input MET + MET from 'good jets' - MET from 'all jets'
 *
 *  N.B. SpecificCaloMETData is not updated.
 *
 */

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


// Class declaration
template<class T, class J>
class HLTMETCleanerUsingJetID : public edm::EDProducer {
  public:
    explicit HLTMETCleanerUsingJetID(const edm::ParameterSet&);
    ~HLTMETCleanerUsingJetID() {};

    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
    virtual void produce(edm::Event&, const edm::EventSetup&);

  private:
    bool            usePt_;
    bool            excludePFMuons_;  // currently unused, be careful when dealing with PFJets
    double          minPt_;
    double          maxEta_;

    /// Input tag for the MET collection
    edm::InputTag   metLabel_;

    /// Input tag for the 'all jets' collection
    edm::InputTag   jetsLabel_;

    /// Input tag for the 'good jets' collection
    edm::InputTag   goodJetsLabel_;
};

#endif  // HLTMETCleanerUsingJetID_h_

