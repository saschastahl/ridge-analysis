from Gaudi.Configuration import *

from Configurables import DaVinci

from Configurables import FilterDesktop

from Configurables import CombineParticles

############# Global settings
down = False

evtmax = 100000

isMC = False

year = 2015

highMult = False

ns25 = True

#### Stripping filter 

from Configurables import LoKi__HDRFilter as StripFilter
stripFilter = StripFilter( 'stripPassFilter',\
                           Code = "HLT_PASS('StrippingMBNoBiasDecision')",\
                           Location= "/Event/Strip/Phys/DecReports")

from PhysConf.Filters import LoKi_Filters
if highMult:
    code  = " HLT_PASS_RE ( 'Hlt1HighVeloMult.*Decision')  "
    fltrs = LoKi_Filters (HLT1_Code  = code)
else:
    if not ns25:
        code  = " HLT_PASS_RE ( 'Hlt2MBNoBiasLeadingCrossingDecision')  "
        fltrs = LoKi_Filters (HLT2_Code  = code)
    else:
        code  ="HLT_PASS('Hlt1MBNoBiasDecision')"
        fltrs = LoKi_Filters (HLT1_Code  = code)


from Configurables import DeterministicPrescaler
if not ns25:
    nobias_prescale =  DeterministicPrescaler("PrescaleMinbias", AcceptFraction = 0.25)
else:
    nobias_prescale =  DeterministicPrescaler("PrescaleMinbias", AcceptFraction = 1.0)
from Configurables import LoKi__HDRFilter, CheckPV
checkPV=CheckPV("MoreThan1PV")

############# DecayTreeTuple
from DecayTreeTuple.Configuration import *
from Configurables import TupleToolTISTOS
from PhysSelPython.Wrappers import AutomaticData, Selection, SelectionSequence
from Configurables import FilterDesktop
from Configurables import PrintDecayTree, PrintDecayTreeTool, TupleToolTracks


from Configurables import TupleToolTrigger,TrackSelector

etuple=EventTuple("EvtTuple")
etuple.ToolList=[
    "TupleToolRecoStats",   
    "TupleToolTrigger",
    "TupleToolPrimaries",
    "TupleToolTracks"]

etuple.addTool(TupleToolTracks, name="TupleToolTracks")
etuple.TupleToolTracks.PropertiesPrint = False
etuple.TupleToolTracks.FullDetails = False
etuple.TupleToolTracks.addTool(TrackSelector,name="TrackSelector")
etuple.TupleToolTracks.TrackSelector.MaxGhostProbCut = 0.5
etuple.TupleToolTracks.TrackSelector.TrackTypes = ["Long"]
etuple.TupleToolTracks.TrackSelector.AcceptClones = False
etuple.TupleToolTracks.TrackSelector.MinCloneDistCut = 5000.
etuple.TupleToolTracks.TrackSelector.MaxCloneDistCut = 9e+99

if isMC:
    etuple.TupleToolTracks.IsMC = True
    etuple.ToolList +=["MCTupleToolPrimaries"]
else:
    etuple.ToolList +=["TupleToolEventInfo"]


etuple.addTool(TupleToolTrigger, name="TupleToolTrigger")
etuple.TupleToolTrigger.VerboseL0=True
etuple.TupleToolTrigger.VerboseHlt1=True
etuple.TupleToolTrigger.VerboseHlt2=True
etuple.TupleToolTrigger.TriggerList=["Hlt1MBNoBiasDecision",
                                     "Hlt1HighVeloMultDecision",
                                     "Hlt1HighVeloMultSinglePVDecision",
                                     "Hlt2PassThroughDecision",
                                     "Hlt2MBNoBiasLeadingCrossingDecision"] 



#main sequence
tupleseq = GaudiSequencer("TupleSeq")
if not highMult and not isMC:
    tupleseq.Members += [ nobias_prescale ]
if isMC:
    tupleseq.Members += [ checkPV ]
    from Configurables import mcparticle_selector_stable
    mcparticle_selector = mcparticle_selector_stable("SelectStableMCParticles")
    mcparticle_selector.RejectNeutrinos = True
    etuple.TupleToolTracks.InputMCParticles =  "Phys/StableMCParticles/Particles"
    tupleseq.Members += [ mcparticle_selector ]
tupleseq.Members += [etuple]
SeqMain = GaudiSequencer("MainSeq")
SeqMain.Members +=[tupleseq]
SeqMain.ModeOR = True;
SeqMain.ShortCircuit = False;

if not isMC and ( year==2012 or ns25):
    DaVinci().EventPreFilters = [stripFilter]

DaVinci().UserAlgorithms += [ SeqMain ]

if isMC:
    DaVinci().DataType = "2012"
else:
    DaVinci().DataType = "2015"
if isMC:
    DaVinci().Simulation = True
else:
    DaVinci().Simulation = False
    
DaVinci().EvtMax = evtmax
DaVinci().PrintFreq = 1000
DaVinci().TupleFile = "minbias.root"
#DaVinci().Lumi = True
DaVinci().IgnoreDQFlags = True
if not isMC:
    DaVinci().EventPreFilters = [fltrs.sequencer('MySeq')]

#2012 data tags
#--  DDDB : dddb-20130929-1 
#--  CONDDB : cond-20141107
# no db tags for data needed
#DaVinci().DDDBtag         = "dddb-20130929-1"  #Change me to the DB tag you want
#DaVinci().CondDBtag       = "cond-20141107"  #Change me to the DB tag you want
if isMC:
    DaVinci().Simulation = True
    DaVinci().DDDBtag = "dddb-20140729"
    DaVinci().CondDBtag = "sim-20140730-vc-mu100"
    if down:
        DaVinci().CondDBtag = "sim-20140730-vc-md100"
    DaVinci().DataType        = "2015"
    DaVinci().InputType = "DST"
else:
    from Configurables import CondDB
    CondDB( LatestGlobalTagByDataType = "2015" ) 
#from GaudiConf import IOHelper
#IOHelper().inputFiles( [
#"root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/MC/Dev/LDST/00042045/0000/00042045_00000328_1.ldst" #2015 MC
#])


