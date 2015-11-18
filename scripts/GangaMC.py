NAME = "Minbias1PV"

#from tempfile import mkstemp
#from shutil import move
#from os import remove, close
#
#def replace(file_path, pattern, subst):
#    #Create temp file
#    fh, abs_path = mkstemp()
#    with open(abs_path,'w') as new_file:
#        with open(file_path) as old_file:
#            for line in old_file:
#                new_file.write(line.replace(pattern, subst))
#    close(fh)
#    #Remove original file
#    remove(file_path)
#    #Move new file
#    move(abs_path, file_path)

    
for polarity in ["MagUp","MagDown"]:
    #script = "/afs/cern.ch/user/s/sstahl/work/analysis/PVscriptMC.py"
    script = "/afs/cern.ch/user/s/sstahl/work/analysis/evt_tuple.py"
    optsfile = [File(script)]
    script_opts = {"YEAR":"MC2015","POL":polarity}
    PATH = "/MC/Dev/Beam6500GeV-RunII-%(POL)s-Nu1.6-Pythia8/Sim08f/Trig0x40b10033/Reco15DEV/30000000/LDST/" % script_opts
    j = Job(
        name           = NAME + '_%(YEAR)s%(POL)s' %script_opts,
        application    = DaVinci(version='v36r7p6',optsfile=optsfile),
        splitter       = SplitByFiles(filesPerJob = 8),
        inputdata      = BKQuery(path=PATH).getDataset(),
        outputfiles     = [DiracFile("minbias.root")],
        do_auto_resubmit = True,
        backend        = Dirac()
        )

    j.submit()
    #queues.add(j.submit)
