
// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IEventTupleTool.h"            // Interface
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Event/MCParticle.h"

class IDistanceCalculator;
class IDVAlgorithm ;
class ITrackGhostClassification;
class ITrackSelector;
class IMCReconstructed ;
class IMCReconstructible;

namespace LHCb
{
  class Particle;
  class VertexBase;
  
}

class TupleToolTracks : public TupleToolBase, virtual public IEventTupleTool{
public:
  /// Standard constructor
  TupleToolTracks( const std::string& type,
                      const std::string& name,
                      const IInterface* parent);
  int isPrompt(const LHCb::MCParticle* mcp);
  int isPrompt2(const LHCb::MCParticle* mcp);
  
  virtual ~TupleToolTracks( ){}; ///< Destructor
  virtual StatusCode initialize();

  virtual StatusCode fill( Tuples::Tuple& );
  StatusCode fillMCParticles( Tuples::Tuple& tuple);
  

private:
  std::string m_PVLocation;
  std::string m_trLocation ;
  std::string m_MCParticleLocation;
  bool m_isMC;
  bool m_fullDetail;
  ITrackGhostClassification* m_ghostClassification; 
  double m_maxLifetime;
  LHCb::IParticlePropertySvc *m_ppSvc;
  const IDistanceCalculator* m_dist;
  ITrackSelector* m_selector;
  std::string m_selectorName;
  IDVAlgorithm* m_dva;
  IMCReconstructed* m_rected;
  IMCReconstructible* m_recible ; ///< Reconstructible tool
};
