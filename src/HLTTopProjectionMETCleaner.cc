/** \class  HLTTopProjectionMETCleaner
 * 
 *  \author Jia Fu Low <jia.fu.low@cern.ch>
 * 
 *  See header file for more information.
 * 
 */

#include "HLTrigger/JetMET/interface/HLTTopProjectionMETCleaner.h"


// Constructor
template< class Top, class Bottom, class Matcher >
HLTTopProjectionMETCleaner< Top, Bottom, Matcher >::HLTTopProjectionMETCleaner(const edm::ParameterSet& iConfig)
      : match_(iConfig),
        inputTagTop_(iConfig.getParameter<edm::InputTag>("topCollection")),
        inputTagBottom_(iConfig.getParameter<edm::InputTag>("bottomCollection")), 
        inputTagMET_(iConfig.getParameter<edm::InputTag>("metLabel")), 
        useInverseMask_(iConfig.getParameter<bool>("useInverseMask")),
        usePt_(iConfig.getParameter<bool>("usePt")),
        minEt_(iConfig.getParameter<double>("minEt")),
        maxEta_(iConfig.getParameter<double>("maxEta")) {
    // Register your products
    produces<reco::CaloMETCollection> ();

}

// Fill descriptions
template< class Top, class Bottom, class Matcher >
void HLTTopProjectionMETCleaner< Top, Bottom, Matcher >::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<double>("deltaR",0.001);
    desc.add<edm::InputTag>("topCollection", edm::InputTag("hltCaloJetIDPassed"));
    desc.add<edm::InputTag>("bottomCollection", edm::InputTag("hltAntiKT5CaloJets"));
    desc.add<edm::InputTag>("metLabel", edm::InputTag("hltMet"));
    desc.add<bool>("useInverseMask", true);
    desc.add<bool>("usePt", false);
    desc.add<double>("minEt", 0.0);
    desc.add<double>("maxEta", 9999.0);
    descriptions.add(std::string("hlt")+std::string(typeid(HLTTopProjectionMETCleaner< Top, Bottom, Matcher >).name()),desc);
}

// Produce the products
template< class Top, class Bottom, class Matcher >
void HLTTopProjectionMETCleaner< Top, Bottom, Matcher >::produce(edm::Event& iEvent,
                                                                 const edm::EventSetup& iSetup) {
    // Access the masking collection
    TopHandle tops;
//NO_FWDPTR    TopFwdPtrHandle tops;
    iEvent.getByLabel( inputTagTop_, tops );
    std::list< Top > topsList;
//NO_FWDPTR    std::list< TopFwdPtr > topsList;

    for ( typename TopCollection::const_iterator ibegin = tops->begin(),    
//NO_FWDPTR    for ( typename TopFwdPtrCollection::const_iterator ibegin = tops->begin(),
          iend = tops->end(), i = ibegin; i != iend; ++i ) {
        if ((usePt_ ? i->pt() : i->et()) > minEt_ && std::abs(i->eta()) < maxEta_) {
            topsList.push_back( *i );
        }
    }
    
    // Access the collection to 
    // be masked by the other ones
    BottomHandle bottoms;
//NO_FWDPTR    BottomFwdPtrHandle bottoms;
    iEvent.getByLabel( inputTagBottom_, bottoms );
    
    // Access the input CaloMET collection
    edm::Handle< reco::CaloMETCollection > mets;
    iEvent.getByLabel( inputTagMET_, mets );
    reco::CaloMET calomet;
    if (mets->size() > 0)
        calomet = mets->at(0);
    
    double mex = calomet.px();
    double mey = calomet.py();
    double sumet = calomet.sumEt();
    
    // Create the output CaloMET collection
    std::auto_ptr< reco::CaloMETCollection > result( new reco::CaloMETCollection );
    
    for ( typename BottomCollection::const_iterator ibegin = bottoms->begin(),
//NO_FWDPTR    for ( typename BottomFwdPtrCollection::const_iterator ibegin = bottoms->begin(),
          iend = bottoms->end(), i = ibegin; i != iend; ++i ) {
        
        if ((usePt_ ? i->pt() : i->et()) > minEt_ && std::abs(i->eta()) < maxEta_) {
            Bottom const & bottom = *i;
            //BottomFwdPtr const & bottom = *i;
            match_.setBottom(bottom);
            typename std::list< Top >::iterator found = 
//NO_FWDPTR            typename std::list< TopFwdPtr >::iterator found = 
                    std::find_if( topsList.begin(), topsList.end(), match_ );
            
            // If this is masked in the top projection
            if ( found != topsList.end() ) {
                topsList.erase(found);
                //continue;
            
            // Otherwise, we keep it.
            } else {
                double et = i->et();
                double phi = i->phi();
                mex += (usePt_ ? i->px() : et * cos(phi));
                mey += (usePt_ ? i->py() : et * sin(phi));
                sumet -= (usePt_ ? i->pt() : et);
            }
        }
    }
    
    reco::CaloMET::LorentzVector p4(mex, mey, 0, sqrt(mex*mex + mey*mey));
    calomet.setP4(p4);
    result->push_back(calomet);
    
    iEvent.put( result );
}
