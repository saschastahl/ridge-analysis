#!/bin/bash
#(use bash as shell)
#Start with bsub -J moreLLToys[1-N] < createToys.sh
#BSUB -L /bin/bash
#or use lhcbt3
#BSUB -q 8nh
#(run on SLC6 and require at least 16 GB of RAM)
#BSUB -R "select[type=SLC6_64] rusage[mem=8000]"
#BSUB -F 5000000
#(some shell magic to strip the annoying [1-36] from the job name)
JOBNAME=$(echo $LSB_JOBNAME | rev | cut -c 6- | rev)
#(run the program)
echo "================================================================================================================================"
echo "Starting on : $(date)"
echo "Running on node : $(hostname)"
echo "Current directory : $(pwd)"
echo "Working directory : $LS_SUBCWD"
echo "Current job ID : $LSB_JOBID"
echo "Current job name : $JOBNAME"
echo "Job index number : $LSB_JOBINDEX"
echo
source /cvmfs/lhcb.cern.ch/group_login.sh;
SetupProject.sh Urania v3r0;
which gcc
echo "######### Copy files and compile"
ls

echo "$LSB_JOBINDEX    start">>$LS_SUBCWD/../../../log/minbias/pt0_m3_sig_log.txt

cp -a $LS_SUBCWD/../../../codes/* .
g++ main.cpp  ./source/rewrite_data.cpp ./source/signal.cpp ./source/background.cpp ./source/general.cpp  `root-config --libs --cflags` -o execute

rm -r preprocessed
mkdir preprocessed
mkdir histograms0

ls

./execute $LSB_JOBINDEX 0 0 3 1

date

cp -R histograms0/* /afs/cern.ch/work/r/rkopecna/minbias/60_100/histograms0/

echo "$LSB_JOBINDEX    end">>$LS_SUBCWD/../../../log/minbias/pt0_m3_sig_log.txt

