#include "../../help.h"

void topoStudy(const Int_t nParticle = 2, // 0-2 : xi, 3-5 : omega
               TString fileListDATA = "yieldsListDATA.txt",
               TString fileListMC = "yieldsListMC.txt",
               const TString outputDir = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/topoStudy",
               const TString postFix = "") // list of yields files
{
  // Start of Code
  std::cout << "\x1B[1;33m"; // Set text color to yellow
  std::cout << "\n************ Starting Topo/Kinem Study ************\n";
  std::cout << "\x1B[0m"; // Reset text color

  // Files (DATA) //
  std::vector<std::string> nameDATA;
  std::vector<std::string> nameLegendDATA;
  std::ifstream fileDATA(Form("%s", fileListDATA.Data()));

  std::string removeDATA = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/yieldsOut/yield_XiPm_MB_inel0";
  std::string remove2 = ".root";
  std::string remove3 = "_";
  cout << "Files:" << endl;

  if (fileDATA.is_open())
  {
    std::string line;
    while (std::getline(fileDATA, line))
    {
      nameDATA.push_back(line);
      cout << line << endl;
      size_t pos = line.find(removeDATA);
      if (pos != std::string::npos)
      {
        line.erase(pos, removeDATA.length());
      }

      size_t pos2 = line.find(remove2);
      if (pos2 != std::string::npos)
      {
        line.erase(pos2, remove2.length());
      }
      //nameLegendDATA.push_back(line);
      size_t pos3 = line.find(remove3);
      if (pos3 != std::string::npos)
      {
        line.erase(pos3, remove3.length());
      }
      nameLegendDATA.push_back(line);
    }
    fileDATA.close();
  }
  else
  {
    std::cerr << "Unable to open fileListDATA!" << std::endl;
  }

  cout << "List:" << endl;
  cout << fileListDATA.Data() << endl;

  const Int_t numFilesDATA = nameDATA.size();
  cout << "Number of files: " << numFilesDATA << endl;

  // Files (MC) //
  std::vector<std::string> nameMC;
  std::vector<std::string> nameLegendMC;
  std::ifstream fileMC(Form("%s", fileListMC.Data()));

  std::string removeMC = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc/yieldsOut/yield_XiPm_MB_inel0";
  cout << "Files:" << endl;

  if (fileMC.is_open())
  {
    std::string line;
    while (std::getline(fileMC, line))
    {
      nameMC.push_back(line);
      cout << line << endl;
      size_t pos = line.find(removeMC);
      if (pos != std::string::npos)
      {
        line.erase(pos, removeMC.length());
      }

      size_t pos2 = line.find(remove2);
      if (pos2 != std::string::npos)
      {
        line.erase(pos2, remove2.length());
      }
      //nameLegendMC.push_back(line);
      size_t pos3 = line.find(remove3);
      if (pos3 != std::string::npos)
      {
        line.erase(pos3, remove3.length());
      }
      nameLegendMC.push_back(line);
    }
    fileMC.close();
  }
  else
  {
    std::cerr << "Unable to open fileListMC!" << std::endl;
  }

  cout << "List:" << endl;
  cout << fileListMC.Data() << endl;

  const Int_t numFilesMC = nameMC.size();
  cout << "Number of files: " << numFilesMC << endl;

  TFile *fileInMC[numFilesMC];
  TFile *fileInDATA[numFilesDATA];

  // output file
  TString outputfilePath;
  outputfilePath = outputDir + "/results/" + "topoStudy" + postFix + ".root";
  TFile *outputfile = new TFile(outputfilePath, "RECREATE");

  // Create vectors for DATA
  std::vector<std::tuple<Double_t, std::string, Int_t>> yieldDATA; // value, config #, bin number
  std::vector<std::tuple<Double_t, std::string, Int_t>> varsDATA; // value, config #, bin number
  std::vector<std::pair<Double_t, std::string>> purityDATA; // value, config #, bin number

  // Create vectors for MC
  std::vector<std::tuple<Double_t, std::string, Int_t>> yieldMC; // value, config #, bin number
  std::vector<std::tuple<Double_t, std::string, Int_t>> varsMC; // value, config #, bin number

  // Setup pt binning
  Int_t numPtBins = 0;
  Int_t partType;
  if (nParticle <= 2) {
    numPtBins = numPtXi; // Xi
    partType = 0;
  } else {
    numPtBins = numPtOmega; // Omega
    partType = 1;
  }

  // DATA //
  for (Int_t iFile = 0; iFile < numFilesDATA; iFile++)
  {
    // Open input file
    fileInDATA[iFile] = TFile::Open(Form("%s", nameDATA[iFile].c_str()));
    if (!fileInDATA[iFile] || fileInDATA[iFile]->IsZombie()) {
        std::cerr << "Error opening input file!" << std::endl;
        return;
    }

    fileInDATA[iFile]->cd();

    TDirectory* dirFitParams = fileInDATA[iFile]->GetDirectory("fitParams");
    if (!dirFitParams)
    {
      std::cerr << "`fitParams` directory is not found!" << std::endl;
      return;
    }

    dirFitParams->cd();

    TH1F* hPurity = (TH1F *)dirFitParams->Get("Purity_" + particleNames[nParticle]);
    if (!hPurity)
    {
      std::cerr << "Histogram `hPurity` is not found!" << std::endl;
      return;
    }

    TH1F* hYield = (TH1F *)dirFitParams->Get("Yield_" + particleNames[nParticle]);
    if (!hYield)
    {
      std::cerr << "Histogram `hYield` is not found!" << std::endl;
      return;
    }

    Double_t purityValue = 0;
    for (Int_t b = 1; b <= numPtBins; b++) {
      Double_t yieldValue = hYield->GetBinContent(b);
      purityValue += hPurity->GetBinContent(b);
      yieldDATA.push_back(std::make_tuple(yieldValue, nameLegendDATA[iFile], b));
    }
    purityDATA.push_back(std::make_pair(purityValue/numPtBins, nameLegendMC[iFile]));
  }

  // Find the loosest config in DATA
  std::vector<std::pair<Double_t, Int_t>> yieldLoosestDATA; // yield, bin number
  for (const auto& tuple : yieldDATA) {
      if (std::get<1>(tuple) == "LOOSEST") {
        yieldLoosestDATA.push_back(std::make_pair(std::get<0>(tuple), std::get<2>(tuple)));
      }
  }

  // Make signal loss fraction
  for (const auto& tuple : yieldDATA) {
    for (const auto& pair : yieldLoosestDATA) {
      if (pair.second == std::get<2>(tuple)) { // match bin number
        Double_t recYield = std::get<0>(tuple);
        Double_t yeildVariation = 1 - recYield / pair.first;
        varsDATA.push_back(std::make_tuple(yeildVariation, std::get<1>(tuple), std::get<2>(tuple)));
      }
    }
  }

  // MC //
  for (Int_t iFile = 0; iFile < numFilesMC; iFile++)
  {
    // Open input file
    fileInMC[iFile] = TFile::Open(Form("%s", nameMC[iFile].c_str()));
    if (!fileInMC[iFile] || fileInMC[iFile]->IsZombie()) {
        std::cerr << "Error opening input file!" << std::endl;
        return;
    }

    fileInMC[iFile]->cd();

    TDirectory* dirFitParams = fileInMC[iFile]->GetDirectory("fitParams");
    if (!dirFitParams)
    {
      std::cerr << "`fitParams` directory is not found!" << std::endl;
      return;
    }

    dirFitParams->cd();

    TH1F* hPurity = (TH1F *)dirFitParams->Get("Purity_" + particleNames[nParticle]);
    if (!hPurity)
    {
      std::cerr << "Histogram `hPurity` is not found!" << std::endl;
      return;
    }

    TH1F* hYield = (TH1F *)dirFitParams->Get("Yield_" + particleNames[nParticle]);
    if (!hYield)
    {
      std::cerr << "Histogram `hYield` is not found!" << std::endl;
      return;
    }

    for (Int_t b = 1; b <= numPtBins; b++) {
      Double_t yieldValue = hYield->GetBinContent(b);
      yieldMC.push_back(std::make_tuple(yieldValue, nameLegendMC[iFile], b));
    }
  }

  // Find the loosest config in MC
  std::vector<std::pair<Double_t, Int_t>> yieldLoosestMC; // yield, bin number
  for (const auto& tuple : yieldMC) {
      if (std::get<1>(tuple) == "LOOSEST") {
        yieldLoosestMC.push_back(std::make_pair(std::get<0>(tuple), std::get<2>(tuple)));
      }
  }

  // Make signal loss fraction
  for (const auto& tuple : yieldMC) {
    for (const auto& pair : yieldLoosestMC) {
      if (pair.second == std::get<2>(tuple)) { // match bin number
        Double_t recYield = std::get<0>(tuple);
        Double_t yeildVariation = 1 - recYield / pair.first;
        varsMC.push_back(std::make_tuple(yeildVariation, std::get<1>(tuple), std::get<2>(tuple)));
      }
    }
  }

  TH1F* signaLossDATA[numPtBins];
  TH1F* signaLossMC[numPtBins];
  for (Int_t b = 1; b <= numPtBins; b++) {
    signaLossDATA[b - 1] = new TH1F(Form("signalLossFractionDATA_%i", b), Form("signalLossFractionDATA_%i", b), numFilesDATA, 0.0, 1.0);
    signaLossMC[b - 1] = new TH1F(Form("signalLossFractionMC_%i", b), Form("signalLossFractionMC_%i", b), numFilesMC, 0.0, 1.0);
  }

  Double_t signalLossBestPurity[numPtBins];

  for (Int_t b = 1; b <= numPtBins; b++) {
    // DATA //
    std::vector<std::pair<Double_t, std::string>> varsInBinDATA; // yield, config #
    for (const auto& tuple : varsDATA) {
      if (b == std::get<2>(tuple)) { // match bin number
        varsInBinDATA.push_back(std::make_pair(std::get<0>(tuple), std::get<1>(tuple)));
        if ("451" == std::get<1>(tuple)) {
          signalLossBestPurity[b - 1] = std::get<0>(tuple);
        }
      }
    }
    // Sort the vector by keys using a lambda comparator
    std::sort(varsInBinDATA.begin(), varsInBinDATA.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    for (Int_t bin = 1; bin <= signaLossDATA[b - 1]->GetNbinsX(); bin++) {
      signaLossDATA[b - 1]->SetBinContent(bin, varsInBinDATA[bin-1].first);
      signaLossDATA[b - 1]->SetBinError(bin, 0.);

      // MC //
      for (const auto& tuple : varsMC) {
        if (varsInBinDATA[bin-1].second == std::get<1>(tuple) && b == std::get<2>(tuple)) { // match the tag and pt bin
          signaLossMC[b - 1]->SetBinContent(bin, std::get<0>(tuple));
          signaLossMC[b - 1]->SetBinError(bin, 0.);
        }
      }
    }
  }

  Int_t n = 5;
  gStyle->SetOptStat(0);
  // signaLossDATA[n]->Rebin(20);
  // signaLossMC[n]->Rebin(20);
  // signaLossDATA[n]->Scale(1./20);
  // signaLossMC[n]->Scale(1./20);
  signaLossMC[n]->SetLineColor(kCyan+2); //{kBlack, kRed+1 , kBlue+1, kGreen+3, kMagenta+1, kOrange-1,kCyan+2,kYellow+2};
  signaLossDATA[n]->SetLineColor(kBlack); //{kBlack, kRed+1 , kBlue+1, kGreen+3, kMagenta+1, kOrange-1,kCyan+2,kYellow+2};
  signaLossDATA[n]->Draw();
  DrawHorLine(1.0, signalLossBestPurity[n]);
  signaLossMC[n]->Draw("SAME");



  // Sort the vector by keys using a lambda comparator
  std::sort(purityDATA.begin(), purityDATA.end(), [](const auto& a, const auto& b) {
      return a.first > b.first;
  });

  for (const auto& pair : purityDATA) {
      std::cout << pair.first << ": " << pair.second << std::endl;
  }

  for (const auto& value : signalLossBestPurity) {
      std::cout << "signal loss in purity-best config: " << value << std::endl;
  }



  // End of Code
  std::cout << "\x1B[1;32m"; // Set text color to green
  std::cout << "\n************* Topo/Kinem Study Finished Successfully! *************\n";
  std::cout << "\x1B[0m"; // Reset text color
}
