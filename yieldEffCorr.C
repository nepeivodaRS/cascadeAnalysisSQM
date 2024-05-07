#include "help.h"
#include "effHelp.h"

void yieldEffCorr(const Int_t nParticle = 2, // 0-2 : xi, 3-5 : omega
                  const Int_t nMultInterval = 1, // 0 : MB, 1 - in multiplicity intervals
                  const Int_t inel = 0, // inel > N (0/1)
                  const TString effPath = "/Users/rnepeiv/workLund/PhD_work/run3omega/efficiencyCalculation/results/effChiaraXi_inelgt0_lhc24b1_18mar.root",
                  const TString workingDir = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM",
                  const TString postFix = "")
{
  gROOT->SetBatch(kTRUE);
  // Start of Code
  std::cout << "\x1B[1;33m"; // Set text color to yellow
  std::cout << "\n************ Starting Making Efficiency Corrected Spectra Plot ************\n";
  std::cout << "\x1B[0m"; // Reset text color

  // For systematics, when we need only MB efficiency corrected spectra
  Int_t numMultUsed = nMultInterval ? numMult : 0; // 0 = MB

  // Create output files
  TFile *outputfile[numMultUsed + 1];
  TString outputfilePath;
  for (Int_t iFile = 0; iFile < numMultUsed + 1; iFile++) {
    if (iFile == 0) {
      outputfilePath = workingDir + "/yieldsOutEffCorr/" + "yield_" + particleNames[nParticle] + "_MB_inel" + inel + postFix + ".root";
    } else {
      outputfilePath = workingDir + "/yieldsOutEffCorr/" + "yield_" + particleNames[nParticle] + "_" + multiplicityPerc[iFile - 1] + "-" + multiplicityPerc[iFile] + "_inel" + inel + postFix + ".root";
    }
    outputfile[iFile] = new TFile(outputfilePath, "RECREATE");
  }

  // Open input files
  TFile *inputFile[numMultUsed + 1];
  TString inputFilePath;
  for (Int_t iFile = 0; iFile < numMultUsed + 1; iFile++) {
    if (iFile == 0) {
      inputFilePath = workingDir + "/yieldsOut/" + "yield_" + particleNames[nParticle] + "_MB_inel" + inel + postFix + ".root";
      inputFile[0] = TFile::Open(inputFilePath);
      if (!inputFile[0] || inputFile[0]->IsZombie()) {
        std::cerr << "Error opening input data file for MB!" << std::endl;
        return;
      } else {
        cout << "file for MB yield is opened"<< std::endl;
      }
    } else {
      inputFilePath = workingDir + "/yieldsOut/" + "yield_" + particleNames[nParticle] + "_" + multiplicityPerc[iFile - 1] + "-" + multiplicityPerc[iFile] + "_inel" + inel + postFix + ".root";
      inputFile[iFile] = TFile::Open(inputFilePath);
      if (!inputFile[iFile] || inputFile[iFile]->IsZombie()) {
        std::cerr << "Error opening input data file for mult. class: " << multiplicityPerc[iFile - 1] <<  " - " << multiplicityPerc[iFile] << std::endl;
      } else {
        cout << "file for mult. class: " << multiplicityPerc[iFile - 1] <<  " - " << multiplicityPerc[iFile] << " is opened"<< std::endl;
      }
    }
  }

  TH1F* hYield[numMultUsed + 1];
  TH1F* hYieldCorrected[numMultUsed + 1];
  TDirectory* yieldDir[numMultUsed + 1];

  // Get yields
  for (Int_t iFile = 0; iFile < numMultUsed + 1; iFile++) {
    yieldDir[iFile] = inputFile[iFile]->GetDirectory("fitParams");
    if (!yieldDir[iFile])
    {
      std::cerr << "`fitParams` directory is not found!" << std::endl;
      return;
    }
    hYield[iFile] = (TH1F *)yieldDir[iFile]->Get("Yield_" + particleNames[nParticle]);
    if (!hYield[iFile])
    {
      std::cerr << "Yield histogram is not found!" << std::endl;
      return;
    }
    hYieldCorrected[iFile] = (TH1F*)hYield[iFile]->Clone(Form("hYields_%d", iFile));
    hYieldCorrected[iFile]->SetName("Yield_" + particleNames[nParticle]);
  }

  // Open efficiency file
  TFile *effFile = TFile::Open(effPath);
  if (!effFile || effFile->IsZombie()) {
    std::cerr << "Error opening efficiency file!" << std::endl;
    return;
  } else {
    cout << "efficiency file is opened"<< std::endl;
  }
  TDirectory* effAccDir = effFile->GetDirectory("effAcc");
  if (!effAccDir)
  {
    std::cerr << "`effAcc` directory is not found!" << std::endl;
    return;
  }

  TDirectory* effSignalMBDir = effFile->GetDirectory("effSignal");
  if (!effAccDir)
  {
    std::cerr << "`effSignal` directory is not found!" << std::endl;
    return;
  }

  TH1F* hEffAccFromFile;
  if (nParticle == 2 || nParticle == 5) {
    hEffAccFromFile = (TH1F *)effAccDir->Get("EffSum");
  } else if (nParticle == 0 || nParticle == 3) {
    hEffAccFromFile = (TH1F *)effAccDir->Get("EffMinus");
  } else if (nParticle == 1 || nParticle == 4) {
    hEffAccFromFile = (TH1F *)effAccDir->Get("EffPlus");
  }

  //ListHistogramsInDirectory(effAccDir);
  TH1F* hEffAcc = (TH1F*)hEffAccFromFile->Clone("hEffAcc");

  TH1F* hSignalLossMBFromFile;
  if (nParticle == 2 || nParticle == 5) {
    hSignalLossMBFromFile = (TH1F *)effSignalMBDir->Get("SignalLossSum");
  } else if (nParticle == 0 || nParticle == 3) {
    hSignalLossMBFromFile = (TH1F *)effSignalMBDir->Get("SignalLossMinus");
  } else if (nParticle == 1 || nParticle == 4) {
    hSignalLossMBFromFile = (TH1F *)effSignalMBDir->Get("SignalLossPlus");
  }
  TH1F* hSignalLossMB = (TH1F*)hSignalLossMBFromFile->Clone("hSignalLossMB");

  TDirectory* signalLossDir = effFile->GetDirectory("effSignalInClasses");
  if (!signalLossDir)
  {
    std::cerr << "`effSignalLossInClasses` directory is not found!" << std::endl;
    return;
  }

  TH1F* hSignalLoss;

  TCanvas *cEventFactor = (TCanvas *)effFile->Get("effEvent/canvasEventCorr");
  if (!cEventFactor)
  {
    std::cerr << "`canvasEventCorr` canvas is not found!" << endl;
    return;
  }

  TH1F* hEventFactor = (TH1F *)cEventFactor->GetPrimitive("eventCorr");
  if (!hEventFactor)
  {
    std::cerr << "`eventCorr` histogram is not found!" << std::endl;
    return;
  }

  Double_t eventFactor[numMultEff + 1]; // the last one is for MB
  Double_t errEventFactor[numMultEff + 1]; 

  for (Int_t iBin = 1; iBin <= numMultEff + 1; iBin++) {
    eventFactor[iBin-1] = hEventFactor->GetBinContent(iBin);
    errEventFactor[iBin-1] = hEventFactor->GetBinError(iBin);
  }
  
  TDirectory* baseHistsOutDir[numMultUsed + 1];
  TDirectory* corrYieldOutDir[numMultUsed + 1];

  // Correct for Efficiency x Acceptance and Signal Loss
  for (Int_t iFile = 0; iFile < numMultUsed + 1; iFile++) {
    if (iFile != 0) {
      cout << "yield in mult. interval: " << multiplicityPerc[iFile - 1] << "-" << multiplicityPerc[iFile] << endl; 
    } else {
      cout << "MB yield" << endl;
    }
    Int_t multBin = -1;
    // Choose signal loss histogram
    if (iFile == 0) {
      hSignalLoss = (TH1F*)hSignalLossMB->Clone("hSignalLossMB");;
      multBin = 3;
    } else {
      if (multiplicityPerc[iFile - 1] < 29.) {
        hSignalLoss = (TH1F *)signalLossDir->Get("EffSignalSum_0");
        multBin = 0;
      } else if (multiplicityPerc[iFile - 1] < 69.) {
        hSignalLoss = (TH1F *)signalLossDir->Get("EffSignalSum_1");
        multBin = 1;
      } else {
        hSignalLoss = (TH1F *)signalLossDir->Get("EffSignalSum_2");
        multBin = 2;
      }
    }
    hSignalLoss->SetName("hSignalLoss");
    TF1 *fitExp = new TF1("fitExp", "[0]*exp([1]*x-[2]) + [3]", 0, 8);
    fitExp->SetParameters(-0.4, -5.7, -0.1, 1);
    TF1 *fitPol1 = new TF1("fitPol1", "[0]*x + [1]", 0, 8);
    fitPol1->SetParameters(0.01, 0.9);
    if (multiplicityPerc[iFile - 1] > 69. || iFile == 0) {
      hSignalLoss->Fit(fitExp, "QR"); // Fit low-mult. and MB with exponent
      //cout << "Chi2/NDF " << fitExp->GetChisquare() / fitExp->GetNDF() << endl;
    } else {
      hSignalLoss->Fit(fitPol1, "QR"); // Fit wil pol1
      //cout << "Chi2/NDF " << fitPol1->GetChisquare() / fitPol1->GetNDF() << endl;      
    }

    for (Int_t b = 1; b <= hYield[iFile]->GetNbinsX(); b++) {
      Double_t relErr = 0;
      Double_t relErrEffAcc = 0;
      // Eff. x Acc.
      relErr = hYield[iFile]->GetBinError(b) / hYield[iFile]->GetBinContent(b);
      relErrEffAcc = hEffAcc->GetBinError(hEffAcc->FindBin(hYield[iFile]->GetBinCenter(b))) / hEffAcc->GetBinContent(hEffAcc->FindBin(hYield[iFile]->GetBinCenter(b)));
      hYieldCorrected[iFile]->SetBinContent(b, hYield[iFile]->GetBinContent(b) / hEffAcc->GetBinContent(hEffAcc->FindBin(hYield[iFile]->GetBinCenter(b))));
      hYieldCorrected[iFile]->SetBinError(b, sqrt(pow(relErr, 2) + pow(relErrEffAcc, 2)) * hYieldCorrected[iFile]->GetBinContent(b));
      //cout << "eff. x acc.: " << hEffAcc->GetBinContent(hEffAcc->FindBin(hYield[iFile]->GetBinCenter(b))) << " relErrEffAcc: " << relErrEffAcc << endl;

      // Signal Loss
      relErr = 0;
      Double_t sigLoss = 0;
      Double_t relErrSigLoss = 0;

      relErr = hYieldCorrected[iFile]->GetBinError(b) / hYieldCorrected[iFile]->GetBinContent(b);
      Double_t xValue = hSignalLoss->GetXaxis()->GetBinCenter(b);

      if (multiplicityPerc[iFile - 1] > 69. || iFile == 0) {
        sigLoss = fitExp->Eval(xValue);

        Double_t withoutConst = sigLoss - fitExp->GetParameter(3);
        Double_t sig0 = (withoutConst / fitExp->GetParameter(0)) * fitExp->GetParError(0);
        Double_t sig1 = withoutConst * xValue * fitExp->GetParError(1);
        Double_t sig2 = withoutConst * fitExp->GetParError(2);
        Double_t sig3 = fitExp->GetParError(3);

        relErrSigLoss = sqrt(pow(sig0,2) + pow(sig1,2) + pow(sig2,2) + pow(sig3,2)) / sigLoss;
      } else {
        // Take values from histogram
        //sigLoss = hSignalLoss->GetBinContent(hSignalLoss->FindBin(hSignalLoss->GetXaxis()->GetBinCenter(b)));
        //relErrSigLoss = hSignalLoss->GetBinError(hSignalLoss->FindBin(hYieldCorrected[iFile]->GetXaxis()->GetBinCenter(b))) / sigLoss;
        // Take values from fit
        sigLoss = fitPol1->Eval(xValue);

        Double_t sig0 = xValue * fitPol1->GetParError(0);
        Double_t sig1 = fitPol1->GetParError(1);

        relErrSigLoss = sqrt(pow(sig0,2) + pow(sig1,2)) / sigLoss;
      }

      hYieldCorrected[iFile]->SetBinContent(b, hYieldCorrected[iFile]->GetBinContent(b) / sigLoss);
      hYieldCorrected[iFile]->SetBinError(b, sqrt(pow(relErr, 2) + pow(relErrSigLoss, 2)) * hYieldCorrected[iFile]->GetBinContent(b));
      //cout << "sigLoss: " << sigLoss << " relErrSigLoss: " << relErrSigLoss<< endl;

      // Event splitting/loss correction 
      relErr = hYieldCorrected[iFile]->GetBinError(b) / hYieldCorrected[iFile]->GetBinContent(b);
      Double_t relErrEventFactor = errEventFactor[multBin]/eventFactor[multBin];
      hYieldCorrected[iFile]->SetBinContent(b, hYieldCorrected[iFile]->GetBinContent(b) * eventFactor[multBin]);
      hYieldCorrected[iFile]->SetBinError(b, sqrt(pow(relErr, 2) + pow(relErrEventFactor, 2)) * hYieldCorrected[iFile]->GetBinContent(b));
      //cout << "event correction: " << eventFactor[multBin] << " relErrEventFactor: " << relErrEventFactor<< endl;
    }
    corrYieldOutDir[iFile] = outputfile[iFile]->mkdir("effCorrYield");
    corrYieldOutDir[iFile]->cd();
    hYieldCorrected[iFile]->Write();

    baseHistsOutDir[iFile] = outputfile[iFile]->mkdir("baseHists");
    baseHistsOutDir[iFile]->cd();
    hYield[iFile]->Write();
    hEventFactor->Write();

    hSignalLoss->Write();
    hEffAcc->Write();
  }
  gROOT->SetBatch(kFALSE);
  // End of Code
  std::cout << "\x1B[1;32m"; // Set text color to green
  std::cout << "\n************* Efficiency Corrected Plot is Created Successfully! *************\n";
  std::cout << "\x1B[0m"; // Reset text color
}