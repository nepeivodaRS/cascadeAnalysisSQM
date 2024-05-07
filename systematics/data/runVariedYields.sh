#! /usr/bin/env bash

directory="yieldsOut"
# Check if the directory exists
if [ -d "$directory" ]; then
    echo "Directory '$directory' exists."
else
    echo "Directory '$directory' does not exist."
    mkdir $directory
fi

workingDir="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data\""

pathToDATA="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/AnalysisResultsDataReduced.root\""
pathToDATAPP="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/results/DATA-LHC22o_apass6-DEF.root\""

#default
root -l -b -q "../../yield.C(2, 0, 0, 0,
               $pathToDATA,
               $pathToDATAPP,
               $workingDir,
               \"_DEF\")"

#current
root -l -b -q "../../yield.C(2, 0, 0, 0,
               $pathToDATA,
               \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/results/DATA-LHC22o_apass6-CURR.root\",
               $workingDir,
               \"_CURR\")"

#loosest
root -l -b -q "../../yield.C(2, 0, 0, 0,
               $pathToDATA,
               \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/results/DATA-LHC22o_apass6-LOOSEST.root\",
               $workingDir,
               \"_LOOSEST\")"

#systematics
for i in {1..500}
do
    root -l -b -q "../../yield.C(2, 0, 0, 0,
                   $pathToDATA,
                   \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/results/DATA-LHC22o_apass6-$i.root\",
                   $workingDir,
                   \"_$i\")"        
done
