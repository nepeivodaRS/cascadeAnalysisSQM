#!/bin/bash
# Xi analysis
workingDir="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM\""

pathToDATA="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/22apr_lhc22o_pass6_medium/AnalysisResults.root\""
pathToDATAPP="\"/Users/rnepeiv/workLund/PhD_work/run3omega/postprocessingo2physics/results/run3_13tev/xi/lhc22o_pass6/27apr/AnalysisResults.root\""

pathToMC="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/25apr_lhc24b1/AnalysisResults.root\""
pathToMCPP="\"/Users/rnepeiv/workLund/PhD_work/run3omega/postprocessingo2physics/results/run3_13tev/xi/lhc24b1/27apr/AnalysisResults.root\""

pathToEfficiencyCalib="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/efficiencies/effCalibration_LHC24b1.root\""
pathToEfficiency="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/efficiencies/efficiency_XiPm_LHC24b1.root\""

for i in `seq 0 9`
  do
    root -l -b -q "yield.C(2, $i, 0, 0,
                   $pathToDATA,
                   $pathToDATAPP,
                   $workingDir,
                   \"\")"
  done

root -l "calibrationMC.C(
                $pathToMC,
                $pathToDATA,
                $workingDir,
                \"_LHC24b1\")"

root -l -b -q "effCalc.C(2,
                $pathToMC,
                $pathToMCPP,
                $pathToEfficiencyCalib,
                $workingDir,
                \"_LHC24b1\")"

root -l -b -q "yieldEffCorr.C(2, 1, 0,
                  $pathToEfficiency,
                  $workingDir,
                  \"\")"

root -l "yieldInMult.C(2, 0, $workingDir, \"\")"
root -l "meanInMult.C(2, 0, $workingDir)"
root -l "sigmaInMult.C(2, 0, $workingDir)"
root -l "purityInMult.C(2, 0, $workingDir)"
root -l "yieldInMultFitted.C(2, 0, 3, $workingDir, \"\")"
