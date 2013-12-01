#ifndef HLTCandidateDiffChecker_h_
#define HLTCandidateDiffChecker_h_

/** \class HLTCandidateDiffChecker
 *
 *  \brief  This checks the difference between two HLT candidates (pt, eta, phi)
 *  \author Jia Fu Low (Nov 2013)
 *
 */

#include "HLTrigger/HLTcore/interface/HLTFilter.h"

#include "HLTrigger/HLTcore/interface/HLTFilter.h"


namespace edm {
    class ConfigurationDescriptions;
}

// Class declaration
class HLTCandidateDiffChecker : public HLTFilter {
  public:
    explicit HLTCandidateDiffChecker(const edm::ParameterSet & iConfig);
    ~HLTCandidateDiffChecker();
    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
    virtual bool hltFilter(edm::Event & iEvent, const edm::EventSetup & iSetup, trigger::TriggerFilterObjectWithRefs & filterproduct);

  private:
    bool checkPt_;
    int triggerType_;
    edm::InputTag inputTag1_;
    edm::InputTag inputTag2_;

    std::string moduleLabel_;
};


#endif  // HLTCandidateDiffChecker_h_