#!/bin/bash
# Xi MC Closure
workingDir="\"${PWD}\""

date="2024-05-10"

pathToDATA="\"${PWD}/data/9may_lhc22o_pass6_small/AnalysisResults.root\""

pathToMC="\"${PWD}/data/10may_lhc24b1b/AnalysisResults.root\""
pathToMCPP="\"${PWD}/O2PhysicsRuns/results/run3_13tev/xi/lhc24b1b/$date/AnalysisResults.root\""

pathToEfficiencyCalib="\"${PWD}/efficiencies/effCalibration_LHC24b1b_mc-closure.root\""
pathToEfficiency="\"${PWD}/efficiencies/efficiency_XiPm_LHC24b1b_mc-closure.root\""


# THINK HOW TO DO IT FOR MC CLOSURE WITH NEW SIGNAL LOSS CALC
root -l "yield.C(2, 0, 0, 1,
               $pathToMC,
               $pathToMCPP,
               $workingDir,
               \"_mc-closure\")"

# # for i in `seq 0 9`
# #   do
# #     root -l -b -q "yield.C(2, $i, 0, 1,
# #                    $pathToMC,
# #                    $pathToDATAPP,
# #                    $workingDir,
# #                    \"_mc-closure\")"
# #   done

# root -l "calibrationMC.C(
#                 $pathToMC,
#                 $pathToDATA,
#                 $workingDir,
#                 \"_LHC24b1b_mc-closure\")"

# root -l "effCorr.C(2, 0,
#                 $pathToMC,
#                 $pathToMCPP,
#                 $pathToEfficiencyCalib,
#                 $workingDir,
#                 \"_mc-closure\",
#                 \"_LHC24b1b_mc-closure\")"

# root -l -b -q "yieldEffCorr.C(2, 0, 0,
#                   $pathToEfficiency,
#                   $workingDir,
#                   \"_mc-closure\")"

# root -l "mcClosure.C(2,
#          $workingDir,
#          $pathToMC,
#          $pathToEfficiency)"
