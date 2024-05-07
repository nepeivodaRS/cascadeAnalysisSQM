#!/bin/bash
# log file where the terminal output will be saved
LOGFILE="cascpostprocessingLog.txt"

# directory of this script
DIR_THIS="$(dirname "$(realpath "$0")")"

OPTION="-b --configuration json:/${PWD}/jsonConfigs/ppRun3xi13tevData.json"

o2-analysis-lf-cascpostprocessing ${OPTION} \
> "./logFiles/$LOGFILE" 2>&1

#--aod-file /Users/rnepeiv/workLund/PhD_work/run3omega/data/used_in_analysis_note/AO2D_500khz_without_inelgt0.root

# report status
rc=$?
if [ $rc -eq 0 ]; then
  echo "No problems!"
  resultsDir="${PWD}/results"
  # Check if the directory exists
  if [ -d "$resultsDir" ]; then
      echo "Directory '$resultsDir' exists."
  else
      echo "Directory '$resultsDir' does not exist."
      mkdir $resultsDir
  fi
  #mv "AnalysisResults.root" "${PWD}/results/run3_13tev/xi/lhc24b1/7may/AnalysisResults.root"
  mv "AnalysisResults.root" "${PWD}/results/run3_13tev/xi/lhc22o_pass6/7may/AnalysisResults.root"
  mv "dpl-config.json" "${PWD}/jsonConfigs/latestUsed.json"
else
  echo "Error: Exit code $rc"
  echo "Check the log file $LOGFILE"
  exit $rc
fi