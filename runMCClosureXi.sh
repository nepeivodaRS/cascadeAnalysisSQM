#!/bin/bash
# Xi MC Closure
workingDir="\"${PWD}\""

date="2024-05-07"

pathToDATA="\"${PWD}/data/22apr_lhc22o_pass6_medium/AnalysisResults.root\""

pathToMC="\"${PWD}/data/25apr_lhc24b1/AnalysisResults.root\""
pathToMCPP="\"${PWD}/O2PhysicsRuns/results/run3_13tev/xi/lhc24b1/$date/AnalysisResults.root\""

pathToEfficiencyCalib="\"${PWD}/efficiencies/effCalibration_LHC24b1_mc-closure.root\""
pathToEfficiency="\"${PWD}/efficiencies/efficiency_XiPm_LHC24b1_mc-closure.root\""

root -l "yield.C(2, 0, 0, 1,
               $pathToMC,
               $pathToMCPP,
               $workingDir,
               \"_mc-closure\")"

# root -l -b -q "calibrationMC.C(
#                 $pathToMC,
#                 $pathToDATA,
#                 $workingDir,
#                 \"_LHC24b1_mc-closure\")"

# root -l -b -q "effCalc.C(2,
#                 $pathToMC,
#                 $pathToMCPP,
#                 $pathToEfficiencyCalib,
#                 $workingDir,
#                 \"_LHC24b1_mc-closure\")"

# root -l -b -q "yieldEffCorr.C(2, 0, 0,
#                   $pathToEfficiency,
#                   $workingDir,
#                   \"_mc-closure\")"

# root -l "mcClosure.C(2,
#          $workingDir,
#          $pathToMC,
#          $pathToEfficiency)"
