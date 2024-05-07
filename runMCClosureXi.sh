#!/bin/bash
# Xi MC Closure
workingDir="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM\""

pathToDATA="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/22apr_lhc22o_pass6_medium/AnalysisResults.root\""

pathToMC="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/25apr_lhc24b1/AnalysisResults.root\""
pathToMCPP="\"/Users/rnepeiv/workLund/PhD_work/run3omega/postprocessingo2physics/results/run3_13tev/xi/lhc24b1/27apr/AnalysisResults.root\""

pathToEfficiencyCalib="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/efficiencies/effCalibration_LHC24b1_mc-closure.root\""
pathToEfficiency="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/efficiencies/efficiency_XiPm_LHC24b1_mc-closure.root\""

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
