#!/bin/bash
# Xi analysis
workingDir="\"${PWD}\""

date="2024-05-14"

pathToDATA="\"${PWD}/data/9may_lhc22o_pass6_small/AnalysisResults.root\""
pathToDATAPP="\"${PWD}/O2PhysicsRuns/results/run3_13tev/xi/lhc22o_pass6/$date/AnalysisResults.root\""

pathToMC="\"${PWD}/data/10may_lhc24b1b/AnalysisResults.root\""
pathToMCPP="\"${PWD}/O2PhysicsRuns/results/run3_13tev/xi/lhc24b1b/$date/AnalysisResults.root\""

pathToEfficiencyCalib="\"${PWD}/efficiencies/effCalibration_LHC24b1b.root\""
pathToEfficiency="\"${PWD}/efficiencies/efficiency_XiPm_LHC24b1b.root\""

# for i in `seq 0 10`
#   do
#     root -l -b -q "yield.C(2, $i, 0, 0,
#                    $pathToDATA,
#                    $pathToDATAPP,
#                    $workingDir,
#                    \"\")"
#   done

# root -l "calibrationMC.C(
#                 $pathToMC,
#                 $pathToDATA,
#                 $workingDir,
#                 \"_LHC24b1b\")"

# root -l "effCorr.C(2, 0,
#                 $pathToMC,
#                 $pathToMCPP,
#                 $pathToEfficiencyCalib,
#                 $workingDir,
#                 \"\",
#                 \"_LHC24b1b\")"

# root -l "yieldInMult.C(2, 0, $workingDir, \"\",  \"_LHC24b1b\")"
# root -l "meanInMult.C(2, 0, $workingDir)"
# root -l "sigmaInMult.C(2, 0, $workingDir)"
# root -l "purityInMult.C(2, 0, $workingDir)"
root -l "yieldInMultFitted.C(2, 0, 3, $workingDir, \"\")"
