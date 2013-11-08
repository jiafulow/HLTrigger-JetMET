/** \class  HLTMETCleanerUsingJetID
 *
 *  \author Jia Fu Low <jia.fu.low@cern.ch>
 *
 *  See header file for more information.
 *
 */

#include "HLTrigger/JetMET/interface/HLTMETCleanerUsingJetID.h"
#include "DataFormats/Candidate/interface/Candidate.h"


// Constructor
template<class T, class J>
HLTMETCleanerUsingJetID<T, J>::HLTMETCleanerUsingJetID(const edm::ParameterSet& iConfig)
      : usePt_         (iConfig.getParameter<bool>("usePt")),
        excludePFMuons_(iConfig.getParameter<bool>("excludePFMuons")),
        minPt_         (iConfig.getParameter<double>("minPt")),
        maxEta_        (iConfig.getParameter<double>("maxEta")),
        metLabel_      (iConfig.getParameter<edm::InputTag>("metLabel")),
        jetsLabel_     (iConfig.getParameter<edm::InputTag>("jetsLabel")),
        goodJetsLabel_ (iConfig.getParameter<edm::InputTag>("goodJetsLabel")) {
    // Register your products
    produces<std::vector<T> > ();

}

// Fill descriptions
template<class T, class J>
void HLTMETCleanerUsingJetID<T, J>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<bool>("usePt", false);
    desc.add<bool>("excludePFMuons", false);
    desc.add<double>("minPt", 0.0);
    desc.add<double>("maxEta", 9999.0);
    desc.add<edm::InputTag>("metLabel", edm::InputTag("hltMet"));
    desc.add<edm::InputTag>("jetsLabel", edm::InputTag("hltAntiKT5CaloJets"));
    desc.add<edm::InputTag>("goodJetsLabel", edm::InputTag("hltCaloJetIDPassed"));
    descriptions.add(std::string("hlt")+std::string(typeid(HLTMETCleanerUsingJetID<T, J>).name()),desc);
}

// Produce the products
template<class T, class J>
void HLTMETCleanerUsingJetID<T, J>::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {

    std::auto_ptr<std::vector<T> > result(new std::vector<T>);

    edm::Handle<std::vector<T> > met;
    edm::Handle<std::vector<J> > jets;
    edm::Handle<std::vector<J> > goodJets;

    iEvent.getByLabel(metLabel_, met);
    iEvent.getByLabel(jetsLabel_, jets);
    iEvent.getByLabel(goodJetsLabel_, goodJets);

    double mex_jets = 0.;
    double mey_jets = 0.;
    double sumet_jets = 0.;
    if (jets->size() > 0 ) {
        for(typename std::vector<J>::const_iterator j = jets->begin(); j != jets->end(); ++j) {
            double pt = usePt_ ? j->pt() : j->et();
            double eta = j->eta();
            double phi = j->phi();
            double px = usePt_ ? j->px() : j->pt() * cos(phi);
            double py = usePt_ ? j->py() : j->pt() * sin(phi);

            if (pt > minPt_ && std::abs(eta) < maxEta_) {
                mex_jets -= px;
                mey_jets -= py;
                sumet_jets += pt;
            }
        }
    }

    double mex_goodJets = 0.;
    double mey_goodJets = 0.;
    double sumet_goodJets = 0.;
    if (goodJets->size() > 0) {
        for(typename std::vector<J>::const_iterator j = goodJets->begin(); j != goodJets->end(); ++j) {
            double pt = usePt_ ? j->pt() : j->et();
            double eta = j->eta();
            double phi = j->phi();
            double px = usePt_ ? j->px() : j->pt() * cos(phi);
            double py = usePt_ ? j->py() : j->pt() * sin(phi);

            if (pt > minPt_ && std::abs(eta) < maxEta_) {
                mex_goodJets -= px;
                mey_goodJets -= py;
                sumet_goodJets += pt;
            }
        }
    }

    if (met->size() > 0) {
        double mex_diff = mex_goodJets - mex_jets;
        double mey_diff = mey_goodJets - mey_jets;
        double sumet_diff = sumet_goodJets - sumet_jets;
        if (sumet_diff > 0)  std::cout << sumet_diff << std::endl;  // FIXME: remove this
        assert(sumet_diff <= 0.);  // FIXME: remove this
        reco::Candidate::LorentzVector p4_diff(mex_diff, mey_diff, 0, sqrt(mex_diff*mex_diff + mey_diff*mey_diff));

        T cleanmet = met->front();
        cleanmet.setP4(cleanmet.p4() + p4_diff);  // cannot set sumet...
        result->push_back(cleanmet);
    }

    iEvent.put( result );
}

