#!/bin/bash
# Xi analysis
workingDir="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM\""

pathToDATA="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/22apr_lhc22o_pass6_medium/AnalysisResults.root\""
pathToDATAPP="\"/Users/rnepeiv/workLund/PhD_work/run3omega/postprocessingo2physics/results/run3_13tev/xi/lhc22o_pass6/27apr/AnalysisResults.root\""

pathToMC="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/25apr_lhc24b1/AnalysisResults.root\""
pathToMCPP="\"/Users/rnepeiv/workLund/PhD_work/run3omega/postprocessingo2physics/results/run3_13tev/xi/lhc24b1/27apr/AnalysisResults.root\""

pathToEfficiencyCalib="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/efficiencies/effCalibration_LHC24b1.root\""
pathToEfficiency="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/efficiencies/efficiency_XiPm_LHC24b1_run2.root\""

pathToMBYield="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/yieldsOutEffCorr/yield_XiPm_MB_inel0_run2.root\""

# root -l "yield.C(2, 0, 0, 0,
#                $pathToDATA,
#                $pathToDATAPP,
#                $workingDir,
#                \"_run2\")"

root -l -b -q "calibrationMC.C(
                $pathToMC,
                $pathToDATA,
                $workingDir,
                \"_LHC24b1\")"

root -l -b -q "effCalc.C(2,
                $pathToMC,
                $pathToMCPP,
                $pathToEfficiencyCalib,
                $workingDir,
                \"_LHC24b1_run2\")"

root -l -b -q "yieldEffCorr.C(2, 0, 0,
                  $pathToEfficiency,
                  $workingDir,
                  \"_run2\")"

root -l "compPublXiMB.C($pathToMBYield,
                        $workingDir)"
