#ifndef HLTMinDPhiMETFilter_h_
#define HLTMinDPhiMETFilter_h_

/** \class  HLTMinDPhiMETFilter
 *
 *  \brief  This rejects events using the minimum delta phi between a jet and MET.
 *  \author Jia Fu Low (Nov 2013)
 *
 *  (Descriptions go here...)
 *
 *  \todo   Also allow minDPhi using CaloJet & CaloMET?
 *
 */

#include "HLTrigger/HLTcore/interface/HLTFilter.h"


namespace edm {
    class ConfigurationDescriptions;
}

// Class declaration
template<typename T>
class HLTMinDPhiMETFilter : public HLTFilter {
  public:
    typedef std::vector<T> TCollection;
    typedef edm::Ref<TCollection> TRef;

    explicit HLTMinDPhiMETFilter(const edm::ParameterSet&);
    ~HLTMinDPhiMETFilter();
    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
    virtual bool hltFilter(edm::Event&, const edm::EventSetup&, trigger::TriggerFilterObjectWithRefs & filterproduct);

    static bool returnHigherPt(const TRef& lhs, const TRef& rhs) {
        return (lhs->pt() > rhs->pt());
    }

  private:
    /// Use pt; otherwise, use et.
    bool usePt_;

    //bool excludePFMuons_;  // currently unused

    /// Output trigger type
    int triggerType_;

    /// Consider only n leading-pt (or et) jets, n = maxNJets_
    int maxNJets_;

    /// Minimum pt requirement for jets
    double minPt_;

    /// Maximum (abs) eta requirement for jets
    double maxEta_;

    /// Minium delta phi between a jet and MET
    double minDPhi_;

    /// Input jet, MET collections
    edm::InputTag metLabel_;
    edm::InputTag jetsLabel_;
};

#endif  // HLTMinDPhiMETFilter_h_

