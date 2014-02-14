#ifndef HLTJetVBFFilter2_h_
#define HLTJetVBFFilter2_h_

/** \class HLTJetVBFFilter2
 *
 *  \brief
 *  \author Monica Vazquez Acosta (CERN)
 *  \author Phat Srimanobhas (srimanob@mail.cern.ch)
 *
 *  (Descriptions go here...)
 *
 */

#include "HLTrigger/HLTcore/interface/HLTFilter.h"


namespace edm {
   class ConfigurationDescriptions;
}

// Class decleration
template<typename T>
class HLTJetVBFFilter2 : public HLTFilter {
  public:
    typedef std::vector<T> TCollection;
    typedef edm::Ref<TCollection> TRef;

    explicit HLTJetVBFFilter2(const edm::ParameterSet&);
    ~HLTJetVBFFilter2();
    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
    virtual bool hltFilter(edm::Event&, const edm::EventSetup&, trigger::TriggerFilterObjectWithRefs & filterproduct);

  private:
    bool   etaOpposite_;
    bool   leadingJetOnly_;
    int    triggerType_;
    double minPtLow_;
    double minPtHigh_;
    double maxEta_;
    double minDeltaEta_;
    double minInvMass_;

    /// Input jet collection
    edm::InputTag inputTag_;
};

#endif // HLTJetVBFFilter2_h_
