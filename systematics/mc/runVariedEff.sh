#! /usr/bin/env bash
# For Xi
directory="../data/efficiencies"
# Check if the directory exists
if [ -d "$directory" ]; then
    echo "Directory '$directory' exists."
else
    echo "Directory '$directory' does not exist."
    mkdir $directory
fi

workingDir="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data\""
pathToDATA="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/AnalysisResultsDataReduced.root\""
pathToMC="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/25apr_lhc24b1/AnalysisResults.root\""

# root -l -b -q "../../calibrationMC.C(
#                 $pathToMC,
#                 \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/22apr_lhc22o_pass6_medium/AnalysisResults.root\",
#                 $workingDir,
#                 \"\")"

# root -l -b -q "../../effCalc.C(2,
#                 $pathToMC,
#                 \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc/results/MC-LHC24b1-DEF.root\",
#                 \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/efficiencies/effCalibration.root\",
#                 $workingDir,
#                 \"_DEF\")"

# root -l -b -q "../../effCalc.C(2,
#                 $pathToMC,
#                 \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc/results/MC-LHC24b1-CURR.root\",
#                 \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/efficiencies/effCalibration.root\",
#                 $workingDir,
#                 \"_CURR\")"

root -l -b -q "../../effCalc.C(2,
                $pathToMC,
                \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc/results/MC-LHC24b1-LOOSEST.root\",
                \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/efficiencies/effCalibration.root\",
                $workingDir,
                \"_LOOSEST\")"

# #systematics
# for i in {1..500}
# do
#     root -l -b -q "../../effCalc.C(2,
#                     $pathToMC,
#                     \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc/results/MC-LHC24b1-$i.root\",
#                     \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/efficiencies/effCalibration.root\",
#                     $workingDir,
#                     \"_$i\")"
# done
