/** \class HLTCandidateDiffChecker
 *
 * See header file for documentation
 *
 *  \author Steven Lowette
 *
 */

#include "HLTrigger/JetMET/interface/HLTCandidateDiffChecker.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETFwd.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METFwd.h"
#include "DataFormats/Math/interface/deltaPhi.h"


// Constructor
HLTCandidateDiffChecker::HLTCandidateDiffChecker(const edm::ParameterSet & iConfig) : HLTFilter(iConfig),
  checkPt_      ( iConfig.getParameter<bool>("checkPt") ),
  triggerType_  ( iConfig.getParameter<int>("triggerType") ),
  inputTag1_    ( iConfig.getParameter<edm::InputTag>("inputTag1") ),
  inputTag2_    ( iConfig.getParameter<edm::InputTag>("inputTag2") ) {

    moduleLabel_ = iConfig.getParameter<std::string>("@module_label");
}

// Destructor
HLTCandidateDiffChecker::~HLTCandidateDiffChecker() {}

// Fill descriptions
void HLTCandidateDiffChecker::fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
    edm::ParameterSetDescription desc;
    //makeHLTFilterDescription(desc);
    desc.add<bool>("saveTags", false);
    desc.add<bool>("checkPt", true);
    desc.add<int>("triggerType", 90);
    desc.add<edm::InputTag>("inputTag1", edm::InputTag("hltPFMETProducer"));
    desc.add<edm::InputTag>("inputTag2", edm::InputTag("hltPFMETProducer2"));
    descriptions.add("hltCandidateDiffChecker", desc);
}

// Make filter decision
bool HLTCandidateDiffChecker::hltFilter(edm::Event & iEvent, const edm::EventSetup & iSetup, trigger::TriggerFilterObjectWithRefs & filterproduct) {

    // Create the reference to the output filter objects
    if (saveTags())  filterproduct.addCollectionTag(moduleLabel_);

    bool accept = true;
    double epsilon = 1e-3;

    if (triggerType_ == trigger::TriggerJet) {
        edm::Handle<reco::JetView> jets1;
        iEvent.getByLabel(inputTag1_, jets1);

        edm::Handle<reco::JetView> jets2;
        iEvent.getByLabel(inputTag2_, jets2);

        if (jets1->size() != jets2->size()) {
            edm::LogError("HLTCandidateDiffChecker")
                << "Size (" << inputTag1_.label() << "): " << jets1->size() << "\n"
                << "Size (" << inputTag2_.label() << "): " << jets2->size() << std::endl;

        } else {
            bool print = false;
            for (unsigned int i = 0; i < jets1->size(); ++i) {
                const reco::Jet& j1 = jets1->at(i);
                const reco::Jet& j2 = jets2->at(i);

                if (fabs(j1.eta() - j2.eta()) > epsilon || fabs(reco::deltaPhi(j1.phi(), j2.phi())) > epsilon)
                    print = true;
                if (checkPt_ && fabs(j1.pt() - j2.pt()) > epsilon)
                    print = true;
                if (print)  break;
            }

            if (print) {
                for (unsigned int i = 0; i < jets1->size(); ++i) {
                    const reco::Jet& j1 = jets1->at(i);
                    const reco::Jet& j2 = jets2->at(i);

                edm::LogError("HLTCandidateDiffChecker")
                    << i << ". pt, eta, phi (" << inputTag1_.label() << "): " << j1.pt() << ", " << j1.eta() << ", " << j1.phi() << "\n"
                    << i << ". pt, eta, phi (" << inputTag2_.label() << "): " << j2.pt() << ", " << j2.eta() << ", " << j2.phi() << std::endl;
                }
            }
        }

    } else if (triggerType_ == trigger::TriggerMET) {
        edm::Handle<reco::CaloMETCollection> calomets1;
        iEvent.getByLabel(inputTag1_, calomets1);

        edm::Handle<reco::CaloMETCollection> calomets2;
        iEvent.getByLabel(inputTag2_, calomets2);

        if (calomets1->size() != calomets2->size()) {
            edm::LogError("HLTCandidateDiffChecker")
                << "Size (" << inputTag1_.label() << "): " << calomets1->size() << "\n"
                << "Size (" << inputTag2_.label() << "): " << calomets2->size() << std::endl;

        } else {
            if (calomets1->size() > 0) {
                bool print = false;
                const reco::CaloMET& calomet1 = calomets1->front();
                const reco::CaloMET& calomet2 = calomets2->front();
                if (fabs(calomet1.pt() - calomet2.pt()) > epsilon ||
                    fabs(reco::deltaPhi(calomet1.phi(), calomet2.phi())) > epsilon)
                    print = true;

                if (print) {
                    edm::LogError("HLTCandidateDiffChecker")
                        << "pt, phi (" << inputTag1_.label() << "): " << calomet1.pt() << ", " << calomet1.phi() << "\n"
                        << "pt, phi (" << inputTag2_.label() << "): " << calomet2.pt() << ", " << calomet2.phi() << std::endl;
                }
            }
        }

    } else if (triggerType_ == trigger::TriggerMHT) {
        edm::Handle<reco::METCollection> mets1;
        iEvent.getByLabel(inputTag1_, mets1);

        edm::Handle<reco::METCollection> mets2;
        iEvent.getByLabel(inputTag2_, mets2);

        if (mets1->size() != mets2->size()) {
            edm::LogError("HLTCandidateDiffChecker")
                << "Size (" << inputTag1_.label() << "): " << mets1->size() << "\n"
                << "Size (" << inputTag2_.label() << "): " << mets2->size() << std::endl;

        } else {
            if (mets1->size() > 0) {
                bool print = false;
                const reco::MET& met1 = mets1->front();
                const reco::MET& met2 = mets2->front();
                if (fabs(met1.pt() - met2.pt()) > epsilon ||
                    fabs(reco::deltaPhi(met1.phi(), met2.phi())) > epsilon ||
                    fabs(met1.sumEt() - met2.sumEt()) > epsilon)
                    print = true;

                if (print) {
                    edm::LogError("HLTCandidateDiffChecker")
                        << "pt, phi, sumEt (" << inputTag1_.label() << "): " << met1.pt() << ", " << met1.phi() << ", " << met1.sumEt() << "\n"
                        << "pt, phi, sumEt (" << inputTag2_.label() << "): " << met2.pt() << ", " << met2.phi() << ", " << met2.sumEt() << std::endl;
                }
            }
        }
    }

    return accept;
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HLTCandidateDiffChecker);
