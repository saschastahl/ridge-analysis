// $Id: mcparticle_selector_stable.cpp,v 1.15 2010-07-05 12:28:22 jhe Exp $
// Include files

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
// from Gaudi
//#include "GaudiKernel/AlgFactory.h"
#include <iostream>

// from DaVinci
#include "Kernel/ParticleFilters.h"

// local
#include "mcparticle_selector_stable.h"

using namespace Gaudi::Units ;
using namespace boost::lambda;
using namespace std;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : mcparticle_selector_stable
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( mcparticle_selector_stable )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
mcparticle_selector_stable::mcparticle_selector_stable( const std::string& name,
                                                        ISvcLocator* pSvcLocator)
: GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty("InputLocation",  m_inputLocation = LHCb::MCParticleLocation::Default);
  declareProperty("MCOutputLocation", m_outputLocation = "Phys/StableMCParticles/Particles");
  declareProperty("RejectNeutrinos", m_rejectNeutrinos = true);
}
//=============================================================================
// Destructor
//=============================================================================
mcparticle_selector_stable::~mcparticle_selector_stable() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode mcparticle_selector_stable::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) return sc;
  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;


  m_listofNeutrinos.push_back(12);
  m_listofNeutrinos.push_back(14);
  m_listofNeutrinos.push_back(16);

  m_listOfLongLived.push_back(321);
  m_listOfLongLived.push_back(211);
  m_listOfLongLived.push_back(130);
  m_listOfLongLived.push_back(3222);
  m_listOfLongLived.push_back(310);
  m_listOfLongLived.push_back(3122);
  m_listOfLongLived.push_back(3112);
  m_listOfLongLived.push_back(3312);
  m_listOfLongLived.push_back(3322);// f_0
  // m_listOfLongLived.push_back(30221);
  // m_listOfLongLived.push_back(9010221);

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode mcparticle_selector_stable::execute() {
  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.

  double evt_energy = 0.0;

  int nPVs = 0;

  LHCb::MCParticles* oParts = new LHCb::MCParticles();
  put( oParts, m_outputLocation );
  LHCb::MCParticles* iParts = getIfExists<LHCb::MCParticles>(m_inputLocation); // get particles
  LHCb::MCVertices* iVtxs   = getIfExists<LHCb::MCVertices>(LHCb::MCVertexLocation::Default);


  for (LHCb::MCVertices::iterator iv = iVtxs->begin() ; iv != iVtxs->end(); ++iv){
    if ((*iv)->type() == MCVertex::ppCollision) nPVs++;
  }

  for (LHCb::MCParticles::iterator it = iParts->begin(); it != iParts->end(); ++it){
    LHCb::MCParticle* mcparticle = (*it);

    //remove neutrinos if requested
    if(m_rejectNeutrinos && std::find(m_listofNeutrinos.begin(), m_listofNeutrinos.end(), mcparticle->particleID().abspid()) != m_listofNeutrinos.end())
    {
      continue;
    }
    // must come from a primary vertex
    if(!isFromPV(mcparticle)) continue;


    const SmartRefVector< LHCb::MCVertex >  evs = mcparticle->endVertices();
    bool isGenerator = false;
    // If is a long lived or do not has a decay at all, particle is kept
    if (evs.size()==0 || isALongLivedDecay( mcparticle )) isGenerator = true;
    // Otherwise only keep if its end end vertex is not a decay
    else{
      bool Decay = false;
      SmartRefVector< LHCb::MCVertex >::const_iterator itv;
      for(itv = evs.begin(); itv!= evs.end(); itv++){
        if ( ((*itv).target()->type() != 1) &&  ((*itv).target()->type() != 2) &&  ((*itv).target()->type() != 3) &&  ((*itv).target()->type() != 4) ){
          isGenerator = true;
        }
        if (((*itv).target()->type()  == 2) || ((*itv).target()->type()  == 3)){
          Decay = true;
        }
      }
      if(Decay) isGenerator = false;
    }
    if (isGenerator) {
      oParts->insert(mcparticle);
      evt_energy+= mcparticle->momentum().e();
    }
  }
  if (msgLevel(MSG::DEBUG) ) debug()<<"nPVs: "<<nPVs<<" Reconstructed energy is "<<evt_energy<<endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode mcparticle_selector_stable::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();
}

//=============================================================================

bool  mcparticle_selector_stable::isALongLivedDecay( const LHCb::MCParticle *mcparticle ){
  if(std::find( m_listOfLongLived.begin(),  m_listOfLongLived.end(), mcparticle->particleID().abspid()) !=  m_listOfLongLived.end()) {
    return true;
  } else {
    return false;
  }
}


bool mcparticle_selector_stable::isFromPV( const LHCb::MCParticle *mcparticle ){
  const LHCb::MCVertex * ov = mcparticle->originVertex();
  if (ov->isPrimary())
    return true;
  if ((ov->type() != 1) && (ov->type() != 2) && (ov->type() != 3) && (ov->type() != 4))
    return false;
  if(std::find(m_listOfLongLived.begin(), m_listOfLongLived.end(), mcparticle->mother()->particleID().abspid()) != m_listOfLongLived.end()) {
    return false;
  }
  return isFromPV( ov->mother() );
}


