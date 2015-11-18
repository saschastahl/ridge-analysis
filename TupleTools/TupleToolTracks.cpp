// $Id: TupleToolTracks.cpp,v 1.11 2010-06-02 12:01:44 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolTracks.h"
#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Event/VertexBase.h"
#include "Event/Track.h"
#include <Linker/LinkedTo.h>
#include "MCInterfaces/ITrackGhostClassification.h"
#include "Event/MCTrackInfo.h"
#include "MCInterfaces/IMCReconstructed.h"
#include "MCInterfaces/IMCReconstructible.h"
#include "Event/GhostTrackInfo.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "Event/RecVertex.h"
#include <limits>
#include <Kernel/IDistanceCalculator.h>

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolTracks
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolTracks )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTracks::TupleToolTracks( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
: TupleToolBase ( type, name, parent )
  , m_dist(0)
  
{
  declareInterface<IEventTupleTool>(this);
  declareProperty("InputLocation", m_trLocation = LHCb::TrackLocation::Default, "Track location to be used.");
  declareProperty("IsMC", m_isMC = false, "Run over MC");
  declareProperty("FullDetails", m_fullDetail = false, "Switch on and off to save space");
  declareProperty( "MaxLifetime", m_maxLifetime = 1e-7
                     , "Maximum lifetime of short-lived particles (ns)" );
  declareProperty( "selectorName", m_selectorName = "TrackSelector"
                     , "name of trackselector" );
  declareProperty( "InputPrimaryVertices",
                   m_PVLocation = LHCb::RecVertexLocation::Primary );
  declareProperty( "InputMCParticles",
                   m_MCParticleLocation =  LHCb::MCParticleLocation::Default);

}

//=============================================================================
//=============================================================================

StatusCode TupleToolTracks::initialize()
{
  StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  //m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  //if (!m_dva) return Error("Couldn't get parent DVAlgorithm");
  info() << "Will be looking for Tracks at " << m_trLocation << endmsg ;
  m_ghostClassification = tool<ITrackGhostClassification>("LongGhostClassification",this);
  m_rected  = tool<IMCReconstructed>("MCReconstructed");
  m_recible = tool<IMCReconstructible>("MCReconstructible");
  sc = service( "LHCb::ParticlePropertySvc", m_ppSvc );
  info() << "service" << endmsg ;
  //m_dist = m_dva->distanceCalculator();
  m_dist = tool<IDistanceCalculator>( "LoKi::DistanceCalculator:PUBLIC", this);
  //m_dist = distanceCalculator();
  if ( !m_dist ){
    return Error("Unable to retrieve the IDistanceCalculator tool");
  }
  m_selector = tool<ITrackSelector>(m_selectorName, "TrackSelector", this);
  info() << "selector" << endmsg ;
  return sc;
}


//=============================================================================
//=============================================================================
bool isStableAndCharged(const LHCb::MCParticle * mc)
{
  if(!mc) return true;
  unsigned int anID=abs(mc->particleID().pid());
  if (anID == 22  || anID == 12 || anID == 111 || anID == 2112  || anID == 130)
    return false;
  
  if(mc->endVertices().empty()) return true;
  if(anID == 11 || anID == 13 
     ||  anID == 211 
     ||  anID == 321
     ||  anID == 2212) return true;
  return false;
}

StatusCode TupleToolTracks::fillMCParticles( Tuples::Tuple& tuple){
  const std::string prefix=fullName();
  MCTrackInfo trackInfo( evtSvc(), msgSvc() );
  LHCb::MCParticles* mcParts = getIfExists<LHCb::MCParticles>( m_MCParticleLocation );
  if(  mcParts == nullptr){
    error() << "Could not find MCParticles at " << m_MCParticleLocation << endmsg;
    return StatusCode::SUCCESS;
  }

  std::vector<float> particleID, mcKey, promptParticle, promptParticle2, inAcceptance,inAcceptance2, 
    isReconstructed,isReconstructible,isReconstructible2;
  std::vector<float> px, py, pz;
  
  for ( auto itP = mcParts->begin(); itP != mcParts->end(); ++itP){
    LHCb::MCParticle* part = *itP;
    if (!isStableAndCharged(part)) continue;
    if (part->momentum().Eta()<1.) continue;
    if (part->momentum().Eta()>6.) continue;
    if (part->momentum().P()<500.) continue;
    if (isPrompt2(part)){
      promptParticle2.push_back((float)1);
    }else{
      promptParticle2.push_back((float)0);
      //continue;
    }
    
    
    //if ( 0 == trackInfo.fullInfo( part ) )   continue;
    px.push_back(part->momentum().X());
    py.push_back(part->momentum().Y());
    pz.push_back(part->momentum().Z());
    int pID = 0;
    int key = -1;
    int prompt = -1;
    int catted = -10 ;
    pID = part->particleID().pid();
    key = part->key();
    prompt = isPrompt(part);
    catted  = m_rected->reconstructed(part);
    particleID.push_back((float)pID);
    mcKey.push_back((float)key);
    promptParticle.push_back((float)prompt);
    inAcceptance.push_back((float) m_recible->inAcceptance(part));
    inAcceptance2.push_back((float) trackInfo.accVeloAndT(part));
    isReconstructible.push_back((float) m_recible->reconstructible(part));
    isReconstructible2.push_back((float) trackInfo.hasVeloAndT(part));
    isReconstructed.push_back((float)catted);
  }
  
  bool test=true;
  test &= tuple->farray( prefix+"MCPPX", px, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPPY", py, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPPZ", pz, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPPID", particleID, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPIsPrompt", promptParticle, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPIsPrompt2", promptParticle2, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPInAcceptance", inAcceptance, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPInAcceptance2", inAcceptance2, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPIsReconstructed", isReconstructed, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPIsReconstructible", isReconstructible, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPIsReconstructible2", isReconstructible2, prefix+"nMCP",  10000 );
  test &= tuple->farray( prefix+"MCPMCKey", mcKey, prefix+"nMCP",  10000 );
  
  return StatusCode::SUCCESS;
}


StatusCode TupleToolTracks::fill( Tuples::Tuple& tuple )
{
  const std::string prefix=fullName();


  if (msgLevel(MSG::VERBOSE))
    verbose() << "Storing Tracks with prefix ``" << prefix << "''" << endmsg ;

  std::vector<float> px, py, pz;
  std::vector<float> chi2, chi2PerDof, clonedist, ghostprob,ndof,charge, type;
  std::vector<float> ghostType, particleID, mcKey, promptParticle,promptParticle2;
  std::vector<float> nVeloHits,nTTHits,nITHits,nOTHits;
  std::vector<float> ip,ipChi2;
  float prsMult,ecalMult,hcalMult;
 

  bool exists = exist<DataObject>(LHCb::TrackLocation::Default);
  if ( !exists){
    warning()<< "Track location does not exist" << endmsg;
    //return StatusCode(SUCCESS);
  }

  //Add these at some point
  auto digis = get<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Prs);
  prsMult = digis->size();
  digis = get<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Ecal);
  ecalMult = digis->size();
  digis = get<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Hcal);
  hcalMult = digis->size();
  

  auto tracks = getIfExists<LHCb::Track::Range>(LHCb::TrackLocation::Default);
  auto PVs = getIfExists<LHCb::RecVertex::Range>(m_PVLocation);
  //verbose() << "Tracks: "  << tracks()->size() << endmsg ;
  for(auto track : tracks ){
    if (msgLevel(MSG::VERBOSE)) verbose() << "Tracks: " <<  track->position() << endmsg ;
    if (m_selector && !m_selector->accept(*track)) continue;
    px.push_back( track->momentum().X() );
    py.push_back( track->momentum().Y() );
    pz.push_back( track->momentum().Z() );

    type.push_back((float)track->type());
    charge.push_back((float)track->charge());
    chi2.push_back(track->chi2());
    ndof.push_back((float)track->nDoF());
    chi2PerDof.push_back((float)track->chi2PerDoF());
    ghostprob.push_back(track->ghostProbability());
    clonedist.push_back(track->info(LHCb::Track::CloneDist,-1));

    double minIp = std::numeric_limits<double>::max(), minIpChi2= std::numeric_limits<double>::max();
    double tmpIp = std::numeric_limits<double>::max(), tmpIpChi2= std::numeric_limits<double>::max();
    for (auto pv : PVs){
      m_dist->distance ( track, pv, tmpIp, tmpIpChi2 );
      if (tmpIpChi2<minIpChi2){
        minIp = tmpIp;
        minIpChi2 = tmpIpChi2;
      }
    }
    ip.push_back((float) minIp);
    ipChi2.push_back((float) minIpChi2);

    if (m_fullDetail){
      auto ids = track->lhcbIDs();
      nVeloHits.push_back(std::count_if(ids.begin(), ids.end(),[](LHCb::LHCbID id){return id.isVelo();}));
      nTTHits.push_back(std::count_if(ids.begin(), ids.end(),[](LHCb::LHCbID id){return id.isTT();}));
      nITHits.push_back(std::count_if(ids.begin(), ids.end(),[](LHCb::LHCbID id){return id.isIT();}));
      nOTHits.push_back(std::count_if(ids.begin(), ids.end(),[](LHCb::LHCbID id){return id.isOT();}));
    }

    if (m_isMC){
      fillMCParticles( tuple );
      LinkedTo<LHCb::MCParticle, LHCb::Track> directLink( evtSvc(), msgSvc(), m_trLocation );
      auto mcp = directLink.first(track);
      int isGhost = 0;
      int pID = 0;
      int key = -1;
      int prompt = -1;
      if (mcp){
        isGhost = 0;
        pID = mcp->particleID().pid();
        key = mcp->key();
        prompt = isPrompt(mcp);
      }else{
        LHCb::GhostTrackInfo gInfo;
        m_ghostClassification->info(*track,gInfo);
        isGhost = gInfo.classification();
      }
      particleID.push_back((float)pID);
      ghostType.push_back((float)isGhost);
      mcKey.push_back((float)key);
      promptParticle.push_back((float)prompt);
      promptParticle2.push_back((float)isPrompt2(mcp));
    }
  }

  bool test=true;
  test &= tuple->column( prefix+"PRSMULT", prsMult );
  test &= tuple->column( prefix+"ECALMULT", ecalMult );
  test &= tuple->column( prefix+"HCALMULT", hcalMult );
  test &= tuple->farray( prefix+"TRPX", px, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRPY", py, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRPZ", pz, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRTYPE", type, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRCHARGE", charge, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRCHI2", chi2, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRCHI2PERDOF", chi2PerDof, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRNDOF", ndof, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRGHOSTPROB", ghostprob, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRCLONEDIST", clonedist, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRIP", ip, prefix+"nTr",  10000 );
  test &= tuple->farray( prefix+"TRIPChi2", ipChi2, prefix+"nTr",  10000 );

  if (m_fullDetail){
    test &= tuple->farray( prefix+"TRNVeloHits", nVeloHits , prefix+"nTr",  10000 );
    test &= tuple->farray( prefix+"TRNTTHits", nTTHits , prefix+"nTr",  10000 );
    test &= tuple->farray( prefix+"TRNITHits", nITHits , prefix+"nTr",  10000 );
    test &= tuple->farray( prefix+"TRNOTHits", nOTHits , prefix+"nTr",  10000 );
  }

  if (m_isMC){
    test &= tuple->farray( prefix+"TRGHOSTTYPE", ghostType, prefix+"nTr",  10000 );
    test &= tuple->farray( prefix+"TRPID", particleID, prefix+"nTr",  10000 );
    test &= tuple->farray( prefix+"TRIsPrompt", promptParticle, prefix+"nTr",  10000 );
    test &= tuple->farray( prefix+"TRIsPrompt2", promptParticle2, prefix+"nTr",  10000 );
    test &= tuple->farray( prefix+"TRMCKey", mcKey, prefix+"nTr",  10000 );
  }


  return StatusCode(test);
  //  return StatusCode::SUCCESS;
}

int TupleToolTracks::isPrompt2(const LHCb::MCParticle* mcp){
  const LHCb::MCVertex *ov = 0, *pv = 0;
  if (!mcp)
    return -1;
  pv = mcp->primaryVertex();
  ov = mcp->originVertex();
  if (pv && ov &&
      (pv->position() - Gaudi::XYZVector(ov->position())).R() < 1e-5){
    return 1;
  }else{
    return 0;
  }
}


int TupleToolTracks::isPrompt(const LHCb::MCParticle* mcp)
{
  //int longLivedPid=0;
  //int longLivedKey=0;

  int isPrompt=-1;

  //int mcParentPid=0;
  //int mcParentKey=0;

  double lcl_lifetime(0.);

  const LHCb::MCParticle *mcp_parent = mcp;

  const LHCb::ParticleProperty * lclPprop = NULL;

  if (mcp){
    isPrompt=1;
    while( isPrompt && mcp_parent->mother() )
    {
      mcp_parent = mcp_parent->mother();

      //mcParentPid=mcp_parent->particleID().pid();
      //mcParentKey=mcp_parent->key();
      
      lclPprop = m_ppSvc->find( mcp_parent->particleID() );

      // Some particle IDs are not known to the ppSvc
      // Log a warning if that's the case and assign a large negative lifetime
      if (lclPprop)
      {
        lcl_lifetime = lclPprop->lifetime();
        if(msgLevel(MSG::DEBUG))
        {
          debug() << "Particle: " << lclPprop->evtGenName()
                  << "  lifetime: " << lcl_lifetime << endmsg;
        }
      } else {
        debug() << "Particle ID '" << mcp_parent->particleID()
                  << "' not found in particle property service" << endmsg;
        lcl_lifetime = -999.0;
      }

      if ( lcl_lifetime > m_maxLifetime*Gaudi::Units::ns )
      {
        isPrompt=0;
        //longLivedPid=mcParentPid;
        //longLivedKey=mcParentKey;
      }
    }
  }
  return isPrompt;
}

