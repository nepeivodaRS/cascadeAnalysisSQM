// The macro generates N different jsons with N different cut settings
// when given as input the limits for each variable

// Usage: root -l -b -q generatejson_systematics.cpp

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <random>

void saveJsonToFile(const std::string& json, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << json;
        file.close();
        std::cout << "JSON saved to " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

std::string generateJson(float bachBaryonCosPA, float bachBaryonDCAxyToPV, float casccospa, float cascradius,
                         float dcabachtopv, float dcacascdau, float dcanegtopv, float dcapostopv, float dcav0dau,
                         float dcav0topv, float lambdamasswin, float masswin, float masswintpc, float minpt,
                         int mintpccrrows, float nsigmatpcKa, float nsigmatpcPi, float nsigmatpcPr,
                         float nsigmatofKa, float nsigmatofPi, float nsigmatofPr, float proplifetime, float ptthrtof,
                         float rejcomp, float v0cospa, float v0radius, float hastof, int isMC,
                         float isSelectBachBaryon, int isXi, int evSelFlag, int processGen, int processRec)
{
    std::ostringstream json;

    json << "{" << std::endl;
    json << "    \"internal-dpl-clock\": \"\"," << std::endl;
    json << "    \"internal-dpl-aod-reader\": {" << std::endl;
    json << "        \"time-limit\": \"0\"," << std::endl;
    json << "        \"orbit-offset-enumeration\": \"0\"," << std::endl;
    json << "        \"orbit-multiplier-enumeration\": \"0\"," << std::endl;
    json << "        \"start-value-enumeration\": \"0\"," << std::endl;
    json << "        \"end-value-enumeration\": \"-1\"," << std::endl;
    json << "        \"step-value-enumeration\": \"1\"," << std::endl;
    json << "        \"aod-file\": \"@input_data.txt\"" << std::endl;
    json << "    }," << std::endl;
    json << "    \"lf-cascpostprocessing\": {" << std::endl;
    json << "        \"bachBaryonCosPA\": \"" << std::fixed << std::setprecision(1) << bachBaryonCosPA << "\"," << std::endl;
    json << "        \"bachBaryonDCAxyToPV\": \"" << std::fixed << std::setprecision(3) << bachBaryonDCAxyToPV << "\"," << std::endl;
    json << "        \"casccospa\": \"" << std::fixed << std::setprecision(4) << casccospa << "\"," << std::endl;
    json << "        \"cascradiusMin\": \"" << std::fixed << std::setprecision(2) << cascradius << "\"," << std::endl;
    json << "        \"cascradiusMax\": \"" << std::fixed << std::setprecision(2) << 1000 << "\"," << std::endl;
    json << "        \"dcabachtopv\": \"" << std::fixed << std::setprecision(2) << dcabachtopv << "\"," << std::endl;
    json << "        \"dcacascdau\": \"" << std::fixed << std::setprecision(1) << dcacascdau << "\"," << std::endl;
    json << "        \"dcanegtopv\": \"" << std::fixed << std::setprecision(2) << dcanegtopv << "\"," << std::endl;
    json << "        \"dcapostopv\": \"" << std::fixed << std::setprecision(2) << dcapostopv << "\"," << std::endl;
    json << "        \"dcav0dau\": \"" << std::fixed << std::setprecision(1) << dcav0dau << "\"," << std::endl;
    json << "        \"dcav0topv\": \"" << std::fixed << std::setprecision(2) << dcav0topv << "\"," << std::endl;
    json << "        \"evSelFlag\": \"" << std::fixed << std::setprecision(0) << evSelFlag << "\"," << std::endl;
    json << "        \"hastof\": \"" << std::fixed << std::setprecision(0) << hastof << "\"," << std::endl;
    json << "        \"isMC\": \"" << std::fixed << std::setprecision(0) << isMC << "\"," << std::endl;
    json << "        \"isSelectBachBaryon\": \"" << std::fixed << std::setprecision(0) << isSelectBachBaryon << "\"," << std::endl;
    json << "        \"isXi\": \"" << std::fixed << std::setprecision(0) << isXi << "\"," << std::endl;
    json << "        \"lambdamasswin\": \"" << std::fixed << std::setprecision(4) << lambdamasswin << "\"," << std::endl;
    json << "        \"masswin\": \"" << std::fixed << std::setprecision(3) << masswin << "\"," << std::endl;
    json << "        \"masswintpc\": \"" << std::fixed << std::setprecision(3) << masswintpc << "\"," << std::endl;
    json << "        \"minpt\": \"" << std::fixed << std::setprecision(2) << minpt << "\"," << std::endl;
    json << "        \"mintpccrrows\": \"" << std::fixed << std::setprecision(1) << mintpccrrows << "\"," << std::endl;
    json << "        \"nsigmatofKa\": \"" << std::fixed << std::setprecision(1) << nsigmatofKa << "\"," << std::endl;
    json << "        \"nsigmatofPi\": \"" << std::fixed << std::setprecision(1) << nsigmatofPi << "\"," << std::endl;
    json << "        \"nsigmatofPr\": \"" << std::fixed << std::setprecision(1) << nsigmatofPr << "\"," << std::endl;
    json << "        \"nsigmatpcKa\": \"" << std::fixed << std::setprecision(1) << nsigmatpcKa << "\"," << std::endl;
    json << "        \"nsigmatpcPi\": \"" << std::fixed << std::setprecision(1) << nsigmatpcPi << "\"," << std::endl;
    json << "        \"nsigmatpcPr\": \"" << std::fixed << std::setprecision(1) << nsigmatpcPr << "\"," << std::endl;
    json << "        \"processGen\": \"" << std::fixed << std::setprecision(0) << processGen << "\"," << std::endl;
    json << "        \"processRec\": \"" << std::fixed << std::setprecision(0) << processRec << "\"," << std::endl;
    json << "        \"proplifetime\": \"" << std::fixed << std::setprecision(1) << proplifetime << "\"," << std::endl;
    json << "        \"ptthrtof\": \"" << std::fixed << std::setprecision(1) << ptthrtof << "\"," << std::endl;
    json << "        \"rejcomp\": \"" << std::fixed << std::setprecision(3) << rejcomp << "\"," << std::endl;
    json << "        \"v0cospa\": \"" << std::fixed << std::setprecision(2) << v0cospa << "\"," << std::endl;
    json << "        \"v0radiusMin\": \"" << std::fixed << std::setprecision(2) << v0radius << "\"," << std::endl;
    json << "        \"v0radiusMax\": \"" << std::fixed << std::setprecision(2) << 1000 << "\"," << std::endl;
    json << "        \"dooobrej\": \"" << std::fixed << std::setprecision(0) << 0 << "\"" << std::endl;
    json << "    }," << std::endl;
    json << "    \"internal-dpl-injected-dummy-sink\": \"\"," << std::endl;
    json << "    \"internal-dpl-aod-global-analysis-file-sink\": \"\"" << std::endl;
    json << "}" << std::endl;

    return json.str();
}

double generateRandomX(double min, double max)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> dis(min,max);

    return dis(gen);
}

void generatejson_systematics()
{

    int nsigmamc = 100;

    // Default values (Xi)
    float bachBaryonCosPA = 2;
    float bachBaryonDCAxyToPV = 0.0199999996;
    float casccospa = 0.995;
    float cascradius = 0.5;
    float dcabachtopv = 0.04;
    float dcacascdau = 0.8;
    float dcanegtopv = 0.03;
    float dcapostopv = 0.03;
    float dcav0dau = 0.4;
    float dcav0topv = 0;
    float etadau = 0.8;
    float evSelFlag = 2;
    float hastof = 0;
    float isSelectBachBaryon = 1;
    float isXi = 1;
    float lambdamasswin = 0.0075;
    float masswin = 0.05;
    float masswintpc = 0.012;
    float minpt = 0;
    int mintpccrrows = 50;
    float nsigmatofKa = 100;
    float nsigmatofPi = 100;
    float nsigmatofPr = 100;
    float nsigmatpcKa = 4;
    float nsigmatpcPi = 4;
    float nsigmatpcPr = 4;
    float proplifetime = 7;
    float ptthrtof = 0;
    float rap = 0.5;
    float rejcomp = 0;
    float v0cospa = 0.97;
    float v0radius = 1.2;

    // DEFAULT JSON
    std::string jsondef = generateJson(bachBaryonCosPA, bachBaryonDCAxyToPV, casccospa, cascradius, dcabachtopv,
                                       dcacascdau, dcanegtopv, dcapostopv, dcav0dau, dcav0topv, lambdamasswin, masswin,
                                       masswintpc, minpt, mintpccrrows, nsigmatpcKa, nsigmatpcPi, nsigmatpcPr,
                                       nsigmatofKa, nsigmatofPi, nsigmatofPr, proplifetime, ptthrtof, rejcomp, v0cospa,
                                       v0radius, hastof, 0, isSelectBachBaryon, isXi, evSelFlag, 0,
                                       1);

    std::string jsondefMC = generateJson(bachBaryonCosPA, bachBaryonDCAxyToPV, casccospa, cascradius, dcabachtopv,
                                         dcacascdau, dcanegtopv, dcapostopv, dcav0dau, dcav0topv, lambdamasswin, masswin,
                                         masswintpc, minpt, mintpccrrows, nsigmamc, nsigmamc, nsigmamc,
                                         nsigmatofKa, nsigmatofPi, nsigmatofPr, proplifetime, ptthrtof, rejcomp, v0cospa,
                                         v0radius, hastof, 1, isSelectBachBaryon, isXi, evSelFlag, 1,
                                         1);

    std::string filenameDEF;
    std::string filenameDEFMC;
    filenameDEF = "data/config/configDEF.json";
    filenameDEFMC = "mc/config/configDEF.json";

    saveJsonToFile(jsondef, filenameDEF);
    saveJsonToFile(jsondefMC, filenameDEFMC);

    //Limits:
    double casccospalim[2] = {0.970, 0.999};
    double cascradiuslim[2] = {0.4, 0.6};
    double dcabachtopvlim[2] = {0.03, 0.1};
    double dcacascdaulim[2] = {0.7, 1.5};
    double dcanegtopvlim[2] = {0.02, 0.1};
    double dcapostopvlim[2] = {0.02, 0.1};
    double dcav0daulim[2] = {0.3, 1.0};
    double dcav0topvlim[2] = {0., 0.};
    double lambdamasswinlim[2] = {0.006, 0.01};
    double nsigmatpcKlim[2] = {3, 5};
    double nsigmatpcPilim[2] = {3, 5};
    double nsigmatpcPrlim[2] = {3, 5};
    double proplifetimelim[2] = {4, 8};
    double v0cospalim[2] = {0.96, 0.99};
    double v0radiuslim[2] = {1.1, 1.3};
    double tpccrrowslim[2] = {40, 80};
    double rejcomplim[2] = {0., 0.};

    const int njson = 500;

    std::string filename[njson];
    std::string filenameMC[njson];

    for (int i = 0; i < njson; i++)
    {
        // Assign to the variables random values within their limits
        casccospa = generateRandomX(casccospalim[0], casccospalim[1]);
        cascradius = generateRandomX(cascradiuslim[0], cascradiuslim[1]);
        dcabachtopv = generateRandomX(dcabachtopvlim[0], dcabachtopvlim[1]);
        dcacascdau = generateRandomX(dcacascdaulim[0], dcacascdaulim[1]);
        dcanegtopv = generateRandomX(dcanegtopvlim[0], dcanegtopvlim[1]);
        dcapostopv = generateRandomX(dcapostopvlim[0], dcapostopvlim[1]);
        dcav0dau = generateRandomX(dcav0daulim[0], dcav0daulim[1]);
        dcav0topv = generateRandomX(dcav0topvlim[0], dcav0topvlim[1]);
        lambdamasswin = generateRandomX(lambdamasswinlim[0], lambdamasswinlim[1]);
        nsigmatpcKa = generateRandomX(nsigmatpcKlim[0], nsigmatpcKlim[1]);
        nsigmatpcPi = generateRandomX(nsigmatpcPilim[0], nsigmatpcPilim[1]);
        nsigmatpcPr = generateRandomX(nsigmatpcPrlim[0], nsigmatpcPrlim[1]);
        proplifetime = generateRandomX(proplifetimelim[0], proplifetimelim[1]);
        rejcomp = generateRandomX(rejcomplim[0], rejcomplim[1]);
        v0cospa = generateRandomX(v0cospalim[0], v0cospalim[1]);
        v0radius = generateRandomX(v0radiuslim[0], v0radiuslim[1]);
        mintpccrrows = generateRandomX(tpccrrowslim[0], tpccrrowslim[1]);

        std::string json = generateJson(bachBaryonCosPA, bachBaryonDCAxyToPV, casccospa, cascradius, dcabachtopv,
                                        dcacascdau, dcanegtopv, dcapostopv, dcav0dau, dcav0topv, lambdamasswin, masswin,
                                        masswintpc, minpt, mintpccrrows, nsigmatpcKa, nsigmatpcPi, nsigmatpcPr,
                                        nsigmatofKa, nsigmatofPi, nsigmatofPr, proplifetime, ptthrtof, rejcomp, v0cospa,
                                        v0radius, hastof, 0, isSelectBachBaryon, isXi, evSelFlag, 0,
                                        1);

        std::string jsonMC = generateJson(bachBaryonCosPA, bachBaryonDCAxyToPV, casccospa, cascradius, dcabachtopv,
                                          dcacascdau, dcanegtopv, dcapostopv, dcav0dau, dcav0topv, lambdamasswin, masswin,
                                          masswintpc, minpt, mintpccrrows, nsigmamc, nsigmamc, nsigmamc,
                                          nsigmatofKa, nsigmatofPi, nsigmatofPr, proplifetime, ptthrtof, rejcomp, v0cospa,
                                          v0radius, hastof, 1, isSelectBachBaryon, isXi, evSelFlag, 1,
                                          1);

        filename[i] = "data/config/systjson" + std::to_string(i+1) + ".json";
        filenameMC[i] = "mc/config/systjson" + std::to_string(i+1) + ".json";
        saveJsonToFile(json, filename[i]);
        saveJsonToFile(jsonMC, filenameMC[i]);
    }
}
