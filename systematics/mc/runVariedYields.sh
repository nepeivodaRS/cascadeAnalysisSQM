#! /usr/bin/env bash

directory="yieldsOut"
# Check if the directory exists
if [ -d "$directory" ]; then
    echo "Directory '$directory' exists."
else
    echo "Directory '$directory' does not exist."
    mkdir $directory
fi

workingDir="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc\""

pathToMC="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/25apr_lhc24b1/AnalysisResults.root\""
pathToMCPP="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc/results/MC-LHC24b1-DEF.root\""

#default
root -l -b -q "../../yield.C(2, 0, 0, 1,
               $pathToMC,
               $pathToMCPP,
               $workingDir,
               \"_DEF\")"

#current
root -l -b -q "../../yield.C(2, 0, 0, 1,
               $pathToMC,
               \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc/results/MC-LHC24b1-CURR.root\",
               $workingDir,
               \"_CURR\")"

#loosest
root -l -b -q "../../yield.C(2, 0, 0, 1,
               $pathToMC,
               \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc/results/MC-LHC24b1-LOOSEST.root\",
               $workingDir,
               \"_LOOSEST\")"

#systematics
for i in {1..500}
do
    root -l -b -q "../../yield.C(2, 0, 0, 1,
                   $pathToMC,
                   \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc/results/MC-LHC24b1-$i.root\",
                   $workingDir,
                   \"_$i\")"        
done
