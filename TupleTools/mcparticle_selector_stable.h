#ifndef MCPARTICLESELECTOR_H
#define MCPARTICLESELECTOR_H 1 

// Include files
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/ITupleTool.h"
#include "Kernel/IParticle2State.h"
#include "Event/Particle.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "Event/MCParticle.h"




class mcparticle_selector_stable : public GaudiTupleAlg {
public: 
  /// Standard constructor
  mcparticle_selector_stable( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~mcparticle_selector_stable( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_outputLocation;
  std::string m_inputLocation;
  bool m_rejectNeutrinos;
  bool isALongLivedDecay( const LHCb::MCParticle *mcparticle );
  bool isFromPV( const LHCb::MCParticle *mcparticle );

  std::vector<int> m_listofNeutrinos;
  std::vector<int> m_listOfLongLived;


  };
#endif
