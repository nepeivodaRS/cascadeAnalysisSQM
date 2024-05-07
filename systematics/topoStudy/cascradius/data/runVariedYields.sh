#! /usr/bin/env bash

directory="yieldsOut"
# Check if the directory exists
if [ -d "$directory" ]; then
    echo "Directory '$directory' exists."
else
    echo "Directory '$directory' does not exist."
    mkdir $directory
fi

workingDir=\"${PWD}\"

pathToDATA="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/AnalysisResultsDataReduced.root\""

#default
root -l -b -q "../../../../yield.C(2, 0, 0, 0,
               $pathToDATA,
               \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/topoStudy/cascradius/data/results/DATA-LHC22o_apass6-DEF.root\",
               $workingDir,
               \"_DEF\")"

#systematics
for i in {1..20}
do
    root -l -b -q "../../../../yield.C(2, 0, 0, 0,
                   $pathToDATA,
                   \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/topoStudy/cascradius/data/results/DATA-LHC22o_apass6-$i.root\",
                   $workingDir,
                   \"_$i\")"        
done
