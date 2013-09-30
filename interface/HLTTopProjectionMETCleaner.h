#ifndef HLTTopProjectionMETCleaner_h_
#define HLTTopProjectionMETCleaner_h_

/** \class  HLTTopProjectionMETCleaner
 * 
 *  \brief  This creates a MET object from the difference in MET between two 
 *          input collections.
 *  \author Jia Fu Low <jia.fu.low@cern.ch>
 *  \date   September 2013
 * 
 *  This code borrows the code of particle flow top projection: 
 *    CommonTools/ParticleFlow/plugins/TopProjector.h (CMSSW_6_1_1)
 *  so the top collection should be a subset of the bottom collection. It 
 *  creates a MET vector defined as:
 * 
 *    output MET = MET from top collection - MET from bottom collection
 *               = -1.0 * MET from (bottom - top) collection
 * 
 *  N.B. SpecificCaloMETData is not updated.
 * 
 *  \todo  Implement inverse masking
 */

// For >= CMSSW_6_1_1, the class TopProjectorDeltaROverlap can be included 
// by uncommenting the following. But note that it only works on candidate 
// pointers.
//#include "CommonTools/ParticleFlow/plugins/TopProjector.h"

// These are needed if TopProjector.h is not included
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
//#include "DataFormats/Candidate/interface/OverlapChecker.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETFwd.h"
#include "DataFormats/Math/interface/deltaR.h"


/// This checks matching based on delta R
template < class Top, class Bottom >
class TopProjectorDeltaROverlap : public std::unary_function< Top, bool > {
  public:
    explicit TopProjectorDeltaROverlap(const edm::ParameterSet& iConfig)
          : deltaR2_( iConfig.getParameter<double>("deltaR") ),
            botEta_(-999.), botPhi_(0.) {
        deltaR2_*=deltaR2_;
    }
    
    void setBottom(const Bottom& bottom) {
        botEta_ = bottom.eta();
        botPhi_ = bottom.phi();
    }

    bool operator() (const Top& top) const {
        double topEta = top.eta();
        double topPhi = top.phi();
        bool matched = reco::deltaR2(topEta, topPhi, botEta_, botPhi_) < deltaR2_;
        return matched;
    }

  protected:
    double deltaR2_;
    double botEta_;
    double botPhi_;
};


// Class declaration
template< class Top, class Bottom, class Matcher = TopProjectorDeltaROverlap<Top,Bottom> >
class HLTTopProjectionMETCleaner : public edm::EDProducer {
  public:
    typedef std::vector<Top> TopCollection;
    typedef edm::Handle< std::vector<Top> > TopHandle;
    //typedef edm::FwdPtr<Top> TopFwdPtr;
    //typedef std::vector<TopFwdPtr> TopFwdPtrCollection;
    //typedef edm::Handle< TopFwdPtrCollection > TopFwdPtrHandle;
    
    typedef std::vector<Bottom> BottomCollection;
    typedef edm::Handle< std::vector<Bottom> > BottomHandle;
    //typedef edm::Ptr<Bottom> BottomPtr;
    //typedef edm::Ref<BottomCollection> BottomRef;
    //typedef edm::FwdPtr<Bottom> BottomFwdPtr;
    //typedef std::vector<BottomFwdPtr> BottomFwdPtrCollection;
    //typedef edm::Handle< BottomFwdPtrCollection > BottomFwdPtrHandle;
  
    explicit HLTTopProjectionMETCleaner(const edm::ParameterSet&);
    ~HLTTopProjectionMETCleaner() {};
    
    static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
    virtual void produce(edm::Event&, const edm::EventSetup&);
    
  private:
    /// Matching method
    Matcher         match_;
    
    /// Input tag for the top (masking) collection
    edm::InputTag   inputTagTop_;

    /// Input tag for the masked collection
    edm::InputTag   inputTagBottom_;
    
    /// Input tag for the calo MET collection
    edm::InputTag   inputTagMET_;
    
    /// Use top collection as inverse mask instead of mask (currently unused)
    bool            useInverseMask_;
    
    /// Use pt instead of et
    bool            usePt_;
    
    double          minEt_;
    double          maxEta_;
};


#endif  // HLTTopProjectionMETCleaner_h_
