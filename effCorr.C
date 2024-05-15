#include "help.h"
#include "effHelp.h"

void effCorr(const  Int_t nParticle = 2, // 0-2 : xi, 3-5 : omega
            const Int_t inel = 0, // inel > N (0/1)
            const TString fileMC = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/25apr_lhc24b1b/AnalysisResults.root",
            const TString fileMCPP = "/Users/rnepeiv/workLund/PhD_work/run3omega/postprocessingo2physics/results/run3_13tev/xi/lhc24b1b/25apr/AnalysisResults.root",
            const TString fileCalibration = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/efficiencies/effCalibration_LHC24b1b.root",
            const TString outputDir = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM",
            const TString yieldPostFix = "", // postfix of yields (needed to compute MB signal loss)
            const TString postFix = "_LHC24b1b_run2") // postfix to the output file
{
  // Start of Code
  std::cout << "\x1B[1;33m"; // Set text color to yellow
  std::cout << "\n************ Starting Computing Eff. x Acc.,  Signal Loss and Event Factor ************\n";
  std::cout << "\x1B[0m"; // Reset text color

  // Set Style
  //gROOT->SetBatch(kTRUE);
  gStyle->SetOptStat("me");
  gStyle->SetStatX(0.9);
  gStyle->SetStatY(0.9);

  // File with calibration histograms (cascqaanalysis output of MC)
  TFile* fileMCin = TFile::Open(fileMC);
  if (!fileMCin || fileMCin->IsZombie()) {
      std::cerr << "Error opening `fileMC` data file!" << std::endl;
      return;
  }

  // File with main histograms (postprocessing output of MC)
  TFile* fileMCPPIn = TFile::Open(fileMCPP);
  if (!fileMCPPIn || fileMCPPIn->IsZombie()) {
      std::cerr << "Error opening input `fileMCPP` file!" << std::endl;
      return;
  }

  // File with calibration
  TFile* fileCalibrationIn = TFile::Open(fileCalibration);
  if (!fileCalibrationIn || fileCalibrationIn->IsZombie()) {
      std::cerr << "Error opening input `fileCalibration` file!" << std::endl;
      return;
  }

  // Open yield files
  TFile *inputFile[numMult + 1];
  TString inputFilePath;
  for (Int_t iFile = 0; iFile < numMult + 1; iFile++) {
    if (iFile == 0) {
      inputFilePath = outputDir + "/yieldsOut/" + "yield_" + particleNames[nParticle] + "_MB_inel" + inel + yieldPostFix + ".root";
      inputFile[0] = TFile::Open(inputFilePath);
      if (!inputFile[0] || inputFile[0]->IsZombie()) {
        std::cerr << "Error opening input data file for MB!" << std::endl;
        return;
      } else {
        cout << "file for MB yield is opened"<< std::endl;
      }
    } else {
      inputFilePath = outputDir + "/yieldsOut/" + "yield_" + particleNames[nParticle] + "_" + multiplicityPerc[iFile - 1] + "-" + multiplicityPerc[iFile] + "_inel" + inel + yieldPostFix + ".root";
      inputFile[iFile] = TFile::Open(inputFilePath);
      if (!inputFile[iFile] || inputFile[iFile]->IsZombie()) {
        std::cerr << "Error opening input data file for mult. class: " << multiplicityPerc[iFile - 1] <<  " - " << multiplicityPerc[iFile] << std::endl;
      } else {
        cout << "file for mult. class: " << multiplicityPerc[iFile - 1] <<  " - " << multiplicityPerc[iFile] << " is opened"<< std::endl;
      }
    }
  }

  TH1F* hYield[numMult + 1];
  TH1F* hYieldCorrected[numMult + 1];
  TDirectory* yieldDir[numMult + 1];

  // Get yields
  for (Int_t iFile = 0; iFile < numMult + 1; iFile++) {
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

  // Retrieve the TTree from the ROOT file
  TTree* calibValuesTree = (TTree*)fileCalibrationIn->Get("MCcalibration/calibValuesTree");
  std::cout << "\n************* Calibration Values *************\n";
  Double_t NChFT0MCalib[numMult + 1];
  Double_t NChFT0MCalibEff[numMultEff + 1];
  calibValuesTree->SetBranchAddress("NChFT0MCalib", NChFT0MCalib);
  calibValuesTree->GetEntry(0);
  Double_t NChFT0MCalibReversed[numMult + 1] = {0};
  Double_t NChFT0MCalibReversedEff[numMultEff + 1] = {0};
  for (Int_t i = 0; i < numMult + 1; i++) {
    NChFT0MCalibReversed[numMult - i] = NChFT0MCalib[i];
  }
  for (int i = 0; i < numMult + 1; ++i) {
    std::cout << "mult. % " << multiplicityPerc[i] << ": " << NChFT0MCalib[i] << "; reversed element: " << NChFT0MCalibReversed[i] << std::endl;
  }

  std::cout << "\n************* Calibration Values For Efficiencies *************\n";
  for (int i = 0; i < numMult + 1; ++i) {
    for (int j = 0; j < numMultEff + 1; ++j) {
      if (multiplicityPerc[i] == multiplicityPercEff[j]) {
        NChFT0MCalibReversedEff[numMultEff - j] = NChFT0MCalib[i];
        NChFT0MCalibEff[j] = NChFT0MCalib[i];
      }
    }
  }

  for (int i = 0; i < numMultEff + 1; ++i) {
    std::cout << "mult. % " << multiplicityPercEff[i] << ": " << NChFT0MCalibEff[i] << "; reversed element: " << NChFT0MCalibReversedEff[i] << std::endl;
  }

  // Output File
  TString outputfilePath = outputDir + "/efficiencies/" + "efficiency_" + particleNames[nParticle] + postFix + ".root";
  TFile *outputfile = new TFile(outputfilePath, "RECREATE");

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

  Double_t* binpt = new Double_t[numPtBins + 1];

  if (nParticle <= 2) {
    for (int i = 0; i < (numPtBins + 1); ++i) {
        binpt[i] = binptXi[i]; // Xi
      }
  } else {
    for (int i = 0; i < (numPtBins + 1); ++i) {
        binpt[i] = binptOmega[i]; // Omega
      }
  }

  // Histos of generated cascades from generated events with accepted z vrtx + chosen event type (evSelFlag)
  TH3F* hCascadePlusTrue;
  TH3F* hCascadeMinusTrue;
  TH3F* hCascadeSumTrue;

  // Histos of generated cascades from generated events with accepted z vrtx + chosen event type (evSelFlag) + associated to the accepted reconstructed event of the same type
  TH3F* hCascadePlusTrueAssoc;
  TH3F* hCascadeMinusTrueAssoc;
  TH3F* hCascadeSumTrueAssoc;

  if(partType == 1){
    hCascadePlusTrueAssoc = (TH3F*)fileMCPPIn->Get("lf-cascpostprocessing/hPtOmegaPlusTrueAssocWithSelColl");
    hCascadeMinusTrueAssoc = (TH3F*)fileMCPPIn->Get("lf-cascpostprocessing/hPtOmegaMinusTrueAssocWithSelColl");
    hCascadePlusTrue = (TH3F*)fileMCPPIn->Get("lf-cascpostprocessing/hPtOmegaPlusTrue");
    hCascadeMinusTrue = (TH3F*)fileMCPPIn->Get("lf-cascpostprocessing/hPtOmegaMinusTrue");
  } else {
    hCascadePlusTrueAssoc = (TH3F*)fileMCPPIn->Get("lf-cascpostprocessing/hPtXiPlusTrueAssocWithSelColl");
    hCascadeMinusTrueAssoc = (TH3F*)fileMCPPIn->Get("lf-cascpostprocessing/hPtXiMinusTrueAssocWithSelColl");
    hCascadePlusTrue = (TH3F*)fileMCPPIn->Get("lf-cascpostprocessing/hPtXiPlusTrue");
    hCascadeMinusTrue = (TH3F*)fileMCPPIn->Get("lf-cascpostprocessing/hPtXiMinusTrue");
  }

  if (!hCascadePlusTrueAssoc || !hCascadeMinusTrueAssoc || !hCascadePlusTrue || !hCascadeMinusTrue)
  {
    std::cerr << "At least one of the histograms is not found in `fileMCPPIn`!" << std::endl;
    return;
  }

  // Reconstructed cascades (only true ones)
  TH3F* hCascadePlusTrueRec = (TH3F*)fileMCPPIn->Get("lf-cascpostprocessing/hPtCascPlusTrueRec");
  TH3F* hCascadeMinusTrueRec = (TH3F*)fileMCPPIn->Get("lf-cascpostprocessing/hPtCascMinusTrueRec");
  TH3F* hCascadeRecSum = (TH3F*)hCascadePlusTrueRec->Clone("SumRec");
  hCascadeRecSum->Add(hCascadeMinusTrueRec);
  hCascadeRecSum->SetName("hPtRecSum");

  hCascadeSumTrue = (TH3F*)hCascadePlusTrue->Clone("SumTrue");
  hCascadeSumTrue->Add(hCascadeMinusTrue);
  hCascadeSumTrue->SetName("hPtSumTrue");

  hCascadeSumTrueAssoc = (TH3F*)hCascadePlusTrueAssoc->Clone("SumTrueAssoc");
  hCascadeSumTrueAssoc->Add(hCascadeMinusTrueAssoc);
  hCascadeSumTrueAssoc->SetName("hPtSumTrueAssoc");

  TDirectory* dir = outputfile->mkdir("supportHistos");
  dir->cd();

  Double_t leftRap = -0.5;
  Double_t rightRap = 0.5;

  // True lvl Sum Not Assoc
  TH3F* hCascadeSumTrueClone = (TH3F*)hCascadeSumTrue->Clone("SumTrueClone");
  hCascadeSumTrueClone->Write();

  TH2F* hProjYPtCascadeSumTrueClone = static_cast<TH2F*>(hCascadeSumTrueClone->Project3D("xy"));
  hProjYPtCascadeSumTrueClone->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
  hProjYPtCascadeSumTrueClone->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
  hProjYPtCascadeSumTrueClone->Write();

  TH1D* hProjPtCascadeSumTrueClone = (hProjYPtCascadeSumTrueClone->ProjectionY());
  hProjPtCascadeSumTrueClone->Sumw2();
  hProjPtCascadeSumTrueClone->Write();

  // True lvl Plus Not Assoc
  TH3F* hCascadePlusTrueClone = (TH3F*)hCascadePlusTrue->Clone("PlusTrueClone");
  hCascadePlusTrueClone->Write();

  TH2F* hProjYPtCascadePlusTrueClone = static_cast<TH2F*>(hCascadePlusTrueClone->Project3D("xy"));
  hProjYPtCascadePlusTrueClone->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
  hProjYPtCascadePlusTrueClone->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
  hProjYPtCascadePlusTrueClone->Write();

  TH1D* hProjPtCascadePlusTrueClone = (hProjYPtCascadePlusTrueClone->ProjectionY());
  hProjPtCascadePlusTrueClone->Sumw2();
  hProjPtCascadePlusTrueClone->Write();

  // True lvl Minus Not Assoc
  TH3F* hCascadeMinusTrueClone = (TH3F*)hCascadeMinusTrue->Clone("MinusTrueClone");
  hCascadeMinusTrueClone->Write();

  TH2F* hProjYPtCascadeMinusTrueClone = static_cast<TH2F*>(hCascadeMinusTrueClone->Project3D("xy"));
  hProjYPtCascadeMinusTrueClone->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
  hProjYPtCascadeMinusTrueClone->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
  hProjYPtCascadeMinusTrueClone->Write();

  TH1D* hProjPtCascadeMinusTrueClone = (hProjYPtCascadeMinusTrueClone->ProjectionY());
  hProjPtCascadeMinusTrueClone->Sumw2();
  hProjPtCascadeMinusTrueClone->Write();

  // True lvl Sum Assoc
  TH3F* hCascadeSumTrueAssocClone = (TH3F*)hCascadeSumTrueAssoc->Clone("SumAssocClone");
  hCascadeSumTrueAssocClone->Write();

  TH2F* hProjYPtCascadeSumTrueAssocClone = static_cast<TH2F*>(hCascadeSumTrueAssocClone->Project3D("xy"));
  hProjYPtCascadeSumTrueAssocClone->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
  hProjYPtCascadeSumTrueAssocClone->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
  hProjYPtCascadeSumTrueAssocClone->Write();

  TH1D* hProjPtCascadeSumTrueAssocClone = (hProjYPtCascadeSumTrueAssocClone->ProjectionY());
  hProjPtCascadeSumTrueAssocClone->Sumw2();
  hProjPtCascadeSumTrueAssocClone->Write();

  // True lvl Plus Assoc
  TH3F* hCascadePlusTrueAssocClone = (TH3F*)hCascadePlusTrueAssoc->Clone("PlusTrueAssocClone");
  hCascadePlusTrueAssocClone->Write();

  TH2F* hProjYPtCascadePlusTrueAssocClone = static_cast<TH2F*>(hCascadePlusTrueAssocClone->Project3D("xy"));
  hProjYPtCascadePlusTrueAssocClone->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
  hProjYPtCascadePlusTrueAssocClone->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
  hProjYPtCascadePlusTrueAssocClone->Write();

  TH1D* hProjPtCascadePlusTrueAssocClone = (hProjYPtCascadePlusTrueAssocClone->ProjectionY());
  hProjPtCascadePlusTrueAssocClone->Sumw2();
  hProjPtCascadePlusTrueAssocClone->Write();

  // True lvl Minus Assoc
  TH3F* hCascadeMinusTrueAssocClone = (TH3F*)hCascadeMinusTrueAssoc->Clone("MinusTrueAssocClone");
  hCascadeMinusTrueAssocClone->Write();

  TH2F* hProjYPtCascadeMinusTrueAssocClone = static_cast<TH2F*>(hCascadeMinusTrueAssocClone->Project3D("xy"));
  hProjYPtCascadeMinusTrueAssocClone->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
  hProjYPtCascadeMinusTrueAssocClone->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
  hProjYPtCascadeMinusTrueAssocClone->Write();

  TH1D* hProjPtCascadeMinusTrueAssocClone = (hProjYPtCascadeMinusTrueAssocClone->ProjectionY());
  hProjPtCascadeMinusTrueAssocClone->Sumw2();
  hProjPtCascadeMinusTrueAssocClone->Write();

  // Rec lvl Sum
  TH3F* hCascadeRecSumClone = (TH3F*)hCascadeRecSum->Clone("SumRecClone");
  hCascadeRecSumClone->Write();

  TH2F* hProjYPtCascadeRecSumClone = static_cast<TH2F*>(hCascadeRecSumClone->Project3D("xy"));
  hProjYPtCascadeRecSumClone->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
  hProjYPtCascadeRecSumClone->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
  hProjYPtCascadeRecSumClone->Write();

  TH1D* hProjPtCascadeRecSumClone = (hProjYPtCascadeRecSumClone->ProjectionY());
  hProjPtCascadeRecSumClone->Sumw2();
  hProjPtCascadeRecSumClone->Write();

  // Rec lvl Plus
  TH3F* hCascadePlusTrueRecClone = (TH3F*)hCascadePlusTrueRec->Clone("PlusTrueRecClone");
  hCascadePlusTrueRecClone->Write();

  TH2F* hProjYPtCascadePlusTrueRecClone = static_cast<TH2F*>(hCascadePlusTrueRecClone->Project3D("xy"));
  hProjYPtCascadePlusTrueRecClone->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
  hProjYPtCascadePlusTrueRecClone->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
  hProjYPtCascadePlusTrueRecClone->Write();

  TH1D* hProjPtCascadePlusTrueRecClone = (hProjYPtCascadePlusTrueRecClone->ProjectionY());
  hProjPtCascadePlusTrueRecClone->Sumw2();
  hProjPtCascadePlusTrueRecClone->Write();

  // Rec lvl Minus
  TH3F* hCascadeMinusTrueRecClone = (TH3F*)hCascadeMinusTrueRec->Clone("MinusTrueRecClone");
  hCascadeMinusTrueRecClone->Write();

  TH2F* hProjYPtCascadeMinusTrueRecClone = static_cast<TH2F*>(hCascadeMinusTrueRecClone->Project3D("xy"));
  hProjYPtCascadeMinusTrueRecClone->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
  hProjYPtCascadeMinusTrueRecClone->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
  hProjYPtCascadeMinusTrueRecClone->Write();

  TH1D* hProjPtCascadeMinusTrueRecClone = (hProjYPtCascadeMinusTrueRecClone->ProjectionY());
  hProjPtCascadeMinusTrueRecClone->Sumw2();
  hProjPtCascadeMinusTrueRecClone->Write();

  // Rebin to match analysis binning
  hProjPtCascadePlusTrueRecClone = (TH1D*)hProjPtCascadePlusTrueRecClone->Rebin(numPtBins, "hProfilePtCascadeYZRecPlusRebinned", binpt);
  hProjPtCascadePlusTrueAssocClone = (TH1D*)hProjPtCascadePlusTrueAssocClone->Rebin(numPtBins, "hProfilePtCascadeYZPlusRebinned", binpt);
  hProjPtCascadePlusTrueClone = (TH1D*)hProjPtCascadePlusTrueClone->Rebin(numPtBins, "hProfilePtCascadeYZPlusTrueRebinned", binpt);

  hProjPtCascadeMinusTrueRecClone = (TH1D*)hProjPtCascadeMinusTrueRecClone->Rebin(numPtBins, "hProfilePtCascadeYZRecMinusRebinned", binpt);
  hProjPtCascadeMinusTrueAssocClone = (TH1D*)hProjPtCascadeMinusTrueAssocClone->Rebin(numPtBins, "hProfilePtCascadeYZMinusRebinned", binpt);
  hProjPtCascadeMinusTrueClone = (TH1D*)hProjPtCascadeMinusTrueClone->Rebin(numPtBins, "hProfilePtCascadeYZMinusTrueRebinned", binpt);

  hProjPtCascadeRecSumClone = (TH1D*)hProjPtCascadeRecSumClone->Rebin(numPtBins, "hProfilePtCascadeYZRecSumRebinned", binpt);
  hProjPtCascadeSumTrueAssocClone = (TH1D*)hProjPtCascadeSumTrueAssocClone->Rebin(numPtBins, "hProfilePtCascadeYZSumRebinned", binpt);
  hProjPtCascadeSumTrueClone = (TH1D*)hProjPtCascadeSumTrueClone->Rebin(numPtBins, "hProfilePtCascadeYZRecSumTrueRebinned", binpt);

  TDirectory* dirEffAcc = outputfile->mkdir("effAcc");
  dirEffAcc->cd();

  // Efficiency x Acceptance
  // Eff Plus
  TH1D* hEffCascadePlus = (TH1D*)hProjPtCascadePlusTrueRecClone->Clone("EffPlus");
  hEffCascadePlus->Divide(hProjPtCascadePlusTrueAssocClone);
  SetErrorDivide(hEffCascadePlus, hProjPtCascadePlusTrueRecClone, hProjPtCascadePlusTrueAssocClone);
  hEffCascadePlus->SetStats(0);
  StyleHisto(hEffCascadePlus, 0, 1.2 * hEffCascadePlus->GetBinContent(hEffCascadePlus->GetMaximumBin()), kBlack, 20, "#it{p}_{T} (GeV/#it{c})", "eff. x acc.", "EffAccPlus", 0, 0, 0, 1.0, 1.25, 1, 0.04, 0.04);
  hEffCascadePlus->Write();

  // Eff Minus
  TH1D* hEffCascadeMinus = (TH1D*)hProjPtCascadeMinusTrueRecClone->Clone("EffMinus");
  hEffCascadeMinus->Divide(hProjPtCascadeMinusTrueAssocClone);
  SetErrorDivide(hEffCascadeMinus, hProjPtCascadeMinusTrueRecClone, hProjPtCascadeMinusTrueAssocClone);
  hEffCascadeMinus->SetStats(0);
  StyleHisto(hEffCascadeMinus, 0, 1.2 * hEffCascadeMinus->GetBinContent(hEffCascadeMinus->GetMaximumBin()), kBlack, 20, "#it{p}_{T} (GeV/#it{c})", "eff. x acc.", "EffAccMinus", 0, 0, 0, 1.0, 1.25, 1, 0.04, 0.04);
  hEffCascadeMinus->Write();

  // Eff Sum
  TH1D* hEffCascadeSum = (TH1D*)hProjPtCascadeRecSumClone->Clone("EffSum");
  hEffCascadeSum->Divide(hProjPtCascadeSumTrueAssocClone);
  SetErrorDivide(hEffCascadeSum, hProjPtCascadeRecSumClone, hProjPtCascadeSumTrueAssocClone);
  hEffCascadeSum->SetStats(0);
  StyleHisto(hEffCascadeSum, 0, 1.2 * hEffCascadeSum->GetBinContent(hEffCascadeSum->GetMaximumBin()), kBlack, 20, "#it{p}_{T} (GeV/#it{c})", "eff. x acc.", "EffAccSum", 0, 0, 0, 1.0, 1.25, 1, 0.04, 0.04);
  hEffCascadeSum->Write();

  TDirectory* dirEffSignal = outputfile->mkdir("effSignal");
  dirEffSignal->cd();

  // Signal loss
  // Eff Plus
  TH1D* hEffCascadePlusSignal = (TH1D*)hProjPtCascadePlusTrueAssocClone->Clone("SignalLossPlus");
  hEffCascadePlusSignal->Divide(hProjPtCascadePlusTrueClone);
  SetErrorDivide(hEffCascadePlusSignal, hProjPtCascadePlusTrueAssocClone, hProjPtCascadePlusTrueClone);
  hEffCascadePlusSignal->SetStats(0);
  StyleHisto(hEffCascadePlusSignal, 0, 1.2 * hEffCascadePlusSignal->GetBinContent(hEffCascadePlusSignal->GetMaximumBin()), kBlack, 20, "#it{p}_{T} (GeV/#it{c})", "Signal loss", "SignalLossPlus", 0, 0, 0, 1.0, 1.25, 1, 0.04, 0.04);
  hEffCascadePlusSignal->Write();

  // Eff Minus
  TH1D* hEffCascadeMinusSignal = (TH1D*)hProjPtCascadeMinusTrueAssocClone->Clone("SignalLossMinus");
  hEffCascadeMinusSignal->Divide(hProjPtCascadeMinusTrueClone);
  SetErrorDivide(hEffCascadeMinusSignal, hProjPtCascadeMinusTrueAssocClone, hProjPtCascadeMinusTrueClone);
  hEffCascadeMinusSignal->SetStats(0);
  StyleHisto(hEffCascadeMinusSignal, 0, 1.2 * hEffCascadeMinusSignal->GetBinContent(hEffCascadeMinusSignal->GetMaximumBin()), kBlack, 20, "#it{p}_{T} (GeV/#it{c})", "Signal loss", "SignalLossMinus", 0, 0, 0, 1.0, 1.25, 1, 0.04, 0.04);
  hEffCascadeMinusSignal->Write();

  // Eff Sum
  TH1D* hEffCascadeSumSignal = (TH1D*)hProjPtCascadeSumTrueAssocClone->Clone("SignalLossSum");
  hEffCascadeSumSignal->Divide(hProjPtCascadeSumTrueClone);
  SetErrorDivide(hEffCascadeSumSignal, hProjPtCascadeSumTrueAssocClone, hProjPtCascadeSumTrueClone);
  hEffCascadeSumSignal->SetStats(0);
  StyleHisto(hEffCascadeSumSignal, 0, 1.2 * hEffCascadeSumSignal->GetBinContent(hEffCascadeSumSignal->GetMaximumBin()), kBlack, 20, "#it{p}_{T} (GeV/#it{c})", "Signal loss", "SignalLossSum", 0, 0, 0, 1.0, 1.25, 1, 0.04, 0.04);

  // Eff. x Acc. in classes //
  TDirectory* dirEffAccInClasses = outputfile->mkdir("effAccInClasses");
  dirEffAccInClasses->cd();

  TCanvas* canvasEffAccInClasses = new TCanvas("canvasEffAccInClasses", "canvasEffAccInClasses", 0, 70, 620, 850);
  StyleCanvas(canvasEffAccInClasses, 0.15, 0.05, 0.05, 0.15);
  TPad *padEffAccInClassesUp = new TPad("padEffAccInClassesUp", "padEffAccInClassesUp", 0, 0.36, 1, 1);
  TPad *padEffAccInClassesLow = new TPad("padEffAccInClassesLow", "padEffAccInClassesLow", 0, 0, 1, 0.36);
  StylePad(padEffAccInClassesUp, 0.15, 0.05, 0.05, 0.);
  StylePad(padEffAccInClassesLow, 0.15, 0.05, 0.02, 0.2);
  canvasEffAccInClasses->cd();
  padEffAccInClassesUp->Draw();
  padEffAccInClassesLow->Draw();

  // Legend
  TLegend *legEffAccInClasses = new TLegend(0.186, 0.0625, 0.893, 0.3125);
  legEffAccInClasses->SetHeader("FT0M Multiplicity Percentile");
  legEffAccInClasses->SetNColumns(3);
  TLegendEntry *legEffAccInClassesHeader = (TLegendEntry *)legEffAccInClasses->GetListOfPrimitives()->First();
  legEffAccInClassesHeader->SetTextSize(0.04);
  StyleLegend(legEffAccInClasses, 0.0, 0.0);

  TLegend *legendTitleMC = new TLegend(0.608, 0.636, 0.907, 0.893);
  StyleLegend(legendTitleMC, 0.0, 0.0);
  legendTitleMC->SetTextAlign(33);
  legendTitleMC->SetTextSize(0.05);
  legendTitleMC->SetTextFont(42);
  legendTitleMC->SetLineColorAlpha(0.,0.);
  legendTitleMC->SetFillColorAlpha(0.,0.);
  legendTitleMC->AddEntry("", "#bf{ALICE Work In Progress}", "");
  legendTitleMC->AddEntry("", "pp, #sqrt{#it{s}} = 13.6 TeV", "");
  legendTitleMC->AddEntry("", particleSymnbols[nParticle] + ", |y| < 0.5", "");
  legendTitleMC->AddEntry("", "LHC24b1b", "");

  // Dummy histograms
  TH1F *hDummyEffAccInClasses = new TH1F("hDummyEffAccInClasses", "hDummyEffAccInClasses", 10000, 0, binpt[numPtBins] + 0.5);
  // Up
  for (Int_t i = 1; i <= hDummyEffAccInClasses->GetNbinsX(); i++)
    hDummyEffAccInClasses->SetBinContent(i, 1000);
  canvasEffAccInClasses->cd();
  padEffAccInClassesUp->cd();
  //StyleHistoMultiPlot(hDummyEffAccInClasses, 1e-6, 1e2, 1, 1, "#it{p}_{T} (GeV/#it{c})", "eff. x. acc.", "", 0, 0, 0, 1.5, 1.0, 0, 0.0, 0.05, 0.0, 0.035, 0.005); // for log
  StyleHistoMultiPlot(hDummyEffAccInClasses, -0.1, 0.22, 1, 1, "#it{p}_{T} (GeV/#it{c})", "eff. x. acc.", "", 0, 0, 0, 1.5, 1.0, 0, 0.0, 0.05, 0.0, 0.035, 0.005);
  SetTickLength(hDummyEffAccInClasses, 0.025, 0.03);
  TAxis *axisEffAccInClassesDummy = hDummyEffAccInClasses->GetYaxis();
  axisEffAccInClassesDummy->ChangeLabel(1, -1, -1, -1, -1, -1, " ");
  axisEffAccInClassesDummy->ChangeLabel(2, -1, -1, -1, -1, -1, " ");
  //axisEffAccInClassesDummy->ChangeLabel(-1, -1, -1, -1, -1, -1, " ");
  hDummyEffAccInClasses->GetXaxis()->SetRangeUser(0, binpt[numPtBins] + 0.5);
  //padEffAccInClassesUp->SetLogy();
  hDummyEffAccInClasses->Draw("same");
  // Low
  TH1F *hDummyEffAccInClassesLow = new TH1F("hDummyEffAccInClassesLow", "hDummyEffAccInClassesLow", 10000, 0, binpt[numPtBins] + 0.5);
  for (Int_t i = 1; i <= hDummyEffAccInClassesLow->GetNbinsX(); i++)
    hDummyEffAccInClassesLow->SetBinContent(i, 1);
  padEffAccInClassesLow->cd();
  StyleHistoMultiPlot(hDummyEffAccInClassesLow, 0.2, 1.8, 1, 1, "#it{p}_{T} (GeV/#it{c})", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, 0, 0.08, 0.08, 0.08, 0.07, 0.01);
  SetTickLength(hDummyEffAccInClassesLow, 0.025, 0.03);
  TAxis *axisDummyEffAccInClassesLow = hDummyEffAccInClassesLow->GetYaxis();
  //padEffAccInClassesLow->SetGridy(); // Only grid on x-axis
  axisDummyEffAccInClassesLow->ChangeLabel(-1, -1, -1, -1, -1, -1, " "); // last 
  axisDummyEffAccInClassesLow->ChangeLabel(1, -1, -1, -1, -1, -1, " "); // first
  hDummyEffAccInClassesLow->GetXaxis()->SetRangeUser(0, binpt[numPtBins] + 0.5);
  hDummyEffAccInClassesLow->GetYaxis()->CenterTitle();
  hDummyEffAccInClassesLow->Draw("same");

  //Plotting
  padEffAccInClassesUp->cd();

  TString SmoltBis[numMultEff + 1];
  for (Int_t m = 0; m < numMultEff + 1; m++) {
    if (m == 0) { 
      SmoltBis[m] += Form("%.1f#minus%.1f%s", 0.0, 100.0, "%");
    } else {
      SmoltBis[m] += Form("%.1f#minus%.1f%s", multiplicityPercEff[m-1], multiplicityPercEff[m], "%");
    }
  }

  StyleHistoMultiPlot(hEffCascadeSum, 1e-6, hEffCascadeSum->GetMaximum()*1e2, color[0], MarkerMult[0], "", hEffCascadeSum->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[0], 0.0, 0.05, 0.0, 0.035, 0.005);
  hEffCascadeSum->Draw("same");
  legEffAccInClasses->AddEntry(hEffCascadeSum, SmoltBis[0], "pef");

  TH3F* hCascadeRecSumInClasses[numMultEff];
  TH2F* hProjYPtCascadeRecSumInClasses[numMultEff];
  TH1D* hProjPtCascadeRecSumInClasses[numMultEff];

  TH3F* hCascadeSumTrueAssocInClasses[numMultEff];
  TH2F* hProjYPtCascadeSumTrueAssocInClasses[numMultEff];
  TH1D* hProjPtCascadeSumTrueAssocInClasses[numMultEff];

  TH1D* hEffCascadeSumInClasses[numMultEff];
  TH1D* hEffCascadeSumInClassesRatio[numMultEff];

  for (Int_t i = 0; i < numMultEff; i++) {
    // Rec lvl Sum
    padEffAccInClassesUp->cd();
    hCascadeRecSumInClasses[i] = (TH3F*)hCascadeRecSum->Clone(Form("SumRecClone_%i", i));
    hCascadeRecSumInClasses[i]->GetZaxis()->SetRangeUser(NChFT0MCalibReversedEff[numMultEff - i - 1], NChFT0MCalibReversedEff[numMultEff - i]);
    hCascadeRecSumInClasses[i]->Write();

    hProjYPtCascadeRecSumInClasses[i] = static_cast<TH2F*>(hCascadeRecSumInClasses[i]->Project3D("xy"));
    hProjYPtCascadeRecSumInClasses[i]->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
    hProjYPtCascadeRecSumInClasses[i]->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
    hProjYPtCascadeRecSumInClasses[i]->Write();

    hProjPtCascadeRecSumInClasses[i] = (hProjYPtCascadeRecSumInClasses[i]->ProjectionY());
    hProjPtCascadeRecSumInClasses[i]->Sumw2();
    hProjPtCascadeRecSumInClasses[i] = (TH1D*)hProjPtCascadeRecSumInClasses[i]->Rebin(numPtBins, "hProjPtCascadeRecSumInClassesRebinned", binpt);
    hProjPtCascadeRecSumInClasses[i]->Write();

    // True lvl Sum Assoc
    hCascadeSumTrueAssocInClasses[i] = (TH3F*)hCascadeSumTrueAssoc->Clone(Form("SumAssocClone_%i", i));
    hCascadeSumTrueAssocInClasses[i]->GetZaxis()->SetRangeUser(NChFT0MCalibReversedEff[numMultEff - i - 1], NChFT0MCalibReversedEff[numMultEff - i]);
    hCascadeSumTrueAssocInClasses[i]->Write();

    hProjYPtCascadeSumTrueAssocInClasses[i] = static_cast<TH2F*>(hCascadeSumTrueAssocInClasses[i]->Project3D("xy"));
    hProjYPtCascadeSumTrueAssocInClasses[i]->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
    hProjYPtCascadeSumTrueAssocInClasses[i]->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
    hProjYPtCascadeSumTrueAssocInClasses[i]->Write();

    hProjPtCascadeSumTrueAssocInClasses[i] = (hProjYPtCascadeSumTrueAssocInClasses[i]->ProjectionY());
    hProjPtCascadeSumTrueAssocInClasses[i]->Sumw2();
    hProjPtCascadeSumTrueAssocInClasses[i] = (TH1D*)hProjPtCascadeSumTrueAssocInClasses[i]->Rebin(numPtBins, "hProjPtCascadeSumTrueAssocInClassesRebinned", binpt);
    hProjPtCascadeSumTrueAssocInClasses[i]->Write();

    // Eff Sum
    hEffCascadeSumInClasses[i] = (TH1D*)hProjPtCascadeRecSumInClasses[i]->Clone(Form("EffSum_%i", i));
    hEffCascadeSumInClasses[i]->Divide(hProjPtCascadeSumTrueAssocInClasses[i]);
    SetErrorDivide(hEffCascadeSumInClasses[i], hProjPtCascadeRecSumInClasses[i], hProjPtCascadeSumTrueAssocInClasses[i]);
    StyleHistoMultiPlot(hEffCascadeSumInClasses[i], 1e-6, hEffCascadeSumInClasses[i]->GetMaximum()*1e2, color[i + 1], MarkerMult[i + 1], "", hEffCascadeSumInClasses[i]->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[i + 1], 0.0, 0.05, 0.0, 0.035, 0.005);
    hEffCascadeSumInClasses[i]->Draw("same");
    hEffCascadeSumInClasses[i]->Write();
    legEffAccInClasses->AddEntry(hEffCascadeSumInClasses[i], SmoltBis[i+1], "pef");

    padEffAccInClassesLow->cd();
    hEffCascadeSumInClassesRatio[i] = (TH1D*)hEffCascadeSumInClasses[i]->Clone(Form("hEffCascadeSumRatio_%i", i));
    hEffCascadeSumInClassesRatio[i]->Divide(hEffCascadeSum);
    StyleHistoMultiPlot(hEffCascadeSumInClassesRatio[i], 0.1 , 1.9, color[i+1], MarkerMult[i+1], "#it{p}_{T} (GeV/#it{c})", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, SizeMult[i+1], 0.08, 0.08, 0.08, 0.08, 0.005);
    hEffCascadeSumInClassesRatio[i]->Draw("same");
  }

  padEffAccInClassesUp->cd();
  legendTitleMC->Draw("same");
  legEffAccInClasses->Draw("same");

  canvasEffAccInClasses->Write();

  // Signal Loss in classes //
  TDirectory* dirEffSignalInClasses = outputfile->mkdir("effSignalInClasses");
  dirEffSignalInClasses->cd();

  TCanvas* canvasEffSignalInClasses = new TCanvas("canvasEffSignalInClasses", "canvasEffSignalInClasses", 0, 70, 620, 850);
  StyleCanvas(canvasEffSignalInClasses, 0.15, 0.05, 0.05, 0.15);
  TPad *padEffSignalInClassesUp = new TPad("padEffSignalInClassesUp", "padEffSignalInClassesUp", 0, 0.36, 1, 1);
  TPad *padEffSignalInClassesLow = new TPad("padEffSignalInClassesLow", "padEffSignalInClassesLow", 0, 0, 1, 0.36);
  StylePad(padEffSignalInClassesUp, 0.15, 0.05, 0.05, 0.);
  StylePad(padEffSignalInClassesLow, 0.15, 0.05, 0.02, 0.2);
  canvasEffSignalInClasses->cd();
  padEffSignalInClassesUp->Draw();
  padEffSignalInClassesLow->Draw();

  // Legend
  TLegend *legEffSignalInClasses = new TLegend(0.186, 0.0625, 0.893, 0.3125);
  legEffSignalInClasses->SetHeader("FT0M Multiplicity Percentile");
  legEffSignalInClasses->SetNColumns(3);
  TLegendEntry *legEffSignalInClassesHeader = (TLegendEntry *)legEffSignalInClasses->GetListOfPrimitives()->First();
  legEffSignalInClassesHeader->SetTextSize(0.04);
  StyleLegend(legEffSignalInClasses, 0.0, 0.0);

  // Dummy histograms
  TH1F *hDummyEffSignalInClasses = new TH1F("hDummyEffSignalInClasses", "hDummyEffSignalInClasses", 10000, 0, binpt[numPtBins] + 0.5);
  // Up
  for (Int_t i = 1; i <= hDummyEffSignalInClasses->GetNbinsX(); i++)
    hDummyEffSignalInClasses->SetBinContent(i, 1e-12);
  canvasEffSignalInClasses->cd();
  padEffSignalInClassesUp->cd();
  StyleHistoMultiPlot(hDummyEffSignalInClasses, 0.65, 1.10, 1, 1, "#it{p}_{T} (GeV/#it{c})", "Signal loss", "", 0, 0, 0, 1.5, 1.0, 0, 0.0, 0.05, 0.0, 0.035, 0.005);
  SetTickLength(hDummyEffSignalInClasses, 0.025, 0.03);
  TAxis *axisEffSignalInClassesDummy = hDummyEffSignalInClasses->GetYaxis();
  axisEffSignalInClassesDummy->ChangeLabel(1, -1, -1, -1, -1, -1, " ");
  //axisEffSignalInClassesDummy->ChangeLabel(-1, -1, -1, -1, -1, -1, " ");
  hDummyEffSignalInClasses->GetXaxis()->SetRangeUser(0, binpt[numPtBins] + 0.5);
  //padEffSignalInClassesUp->SetLogy();
  hDummyEffSignalInClasses->Draw("same");
  // Low
  TH1F *hDummyEffSignalInClassesLow = new TH1F("hDummyEffSignalInClassesLow", "hDummyEffSignalInClassesLow", 10000, 0, binpt[numPtBins] + 0.5);
  for (Int_t i = 1; i <= hDummyEffSignalInClassesLow->GetNbinsX(); i++)
    hDummyEffSignalInClassesLow->SetBinContent(i, 1);
  padEffSignalInClassesLow->cd();
  StyleHistoMultiPlot(hDummyEffSignalInClassesLow, 0.8, 1.2, 1, 1, "#it{p}_{T} (GeV/#it{c})", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, 0, 0.08, 0.08, 0.08, 0.07, 0.01);
  SetTickLength(hDummyEffSignalInClassesLow, 0.025, 0.03);
  TAxis *axisDummyEffSignalInClassesLow = hDummyEffSignalInClassesLow->GetYaxis();
  //padEffSignalInClassesLow->SetGridy(); // Only grid on x-axis
  axisDummyEffSignalInClassesLow->ChangeLabel(-1, -1, -1, -1, -1, -1, " "); // last 
  axisDummyEffSignalInClassesLow->ChangeLabel(1, -1, -1, -1, -1, -1, " "); // first
  hDummyEffSignalInClassesLow->GetXaxis()->SetRangeUser(0, binpt[numPtBins] + 0.5);
  hDummyEffSignalInClassesLow->GetYaxis()->CenterTitle();
  hDummyEffSignalInClassesLow->Draw("same");

  //Plotting
  padEffSignalInClassesUp->cd();

  TH3F* hCascadeSumTrueCloneInClasses[numMultEff];
  TH2F* hProjYPtCascadeSumTrueCloneInClasses[numMultEff];
  TH1D* hProjPtCascadeSumTrueCloneInClasses[numMultEff];

  TH1D* hEffCascadeSumSignalInClasses[numMultEff];
  TH1D* hEffCascadeSumSignalInClassesRatio[numMultEff];
  for (Int_t i = 0; i < numMultEff; i++) {
    padEffSignalInClassesUp->cd();
    // True lvl Sum Not Assoc
    hCascadeSumTrueCloneInClasses[i] = (TH3F*)hCascadeSumTrue->Clone(Form("SumTrueClone_%i", i));
    hCascadeSumTrueCloneInClasses[i]->GetZaxis()->SetRangeUser(NChFT0MCalibReversedEff[numMultEff - i - 1], NChFT0MCalibReversedEff[numMultEff - i]);
    hCascadeSumTrueCloneInClasses[i]->Write();

    hProjYPtCascadeSumTrueCloneInClasses[i] = static_cast<TH2F*>(hCascadeSumTrueCloneInClasses[i]->Project3D("xy"));
    hProjYPtCascadeSumTrueCloneInClasses[i]->GetXaxis()->SetRangeUser(leftRap, rightRap); // Set rapidity interval
    hProjYPtCascadeSumTrueCloneInClasses[i]->GetYaxis()->SetRangeUser(binpt[0], binpt[numPtBins]); // Set pt interval
    hProjYPtCascadeSumTrueCloneInClasses[i]->Write();

    hProjPtCascadeSumTrueCloneInClasses[i] = (hProjYPtCascadeSumTrueCloneInClasses[i]->ProjectionY());
    hProjPtCascadeSumTrueCloneInClasses[i]->Sumw2();
    hProjPtCascadeSumTrueCloneInClasses[i] = (TH1D*)hProjPtCascadeSumTrueCloneInClasses[i]->Rebin(numPtBins, "hProjPtCascadeSumTrueCloneInClassesRebinned", binpt);
    hProjPtCascadeSumTrueCloneInClasses[i]->Write();

    // Signal Loss Sum
    hEffCascadeSumSignalInClasses[i] = (TH1D*)hProjPtCascadeSumTrueAssocInClasses[i]->Clone(Form("EffSignalSum_%i", i));
    hEffCascadeSumSignalInClasses[i]->Divide(hProjPtCascadeSumTrueCloneInClasses[i]);
    SetErrorDivide(hEffCascadeSumSignalInClasses[i], hProjPtCascadeSumTrueAssocInClasses[i], hProjPtCascadeSumTrueCloneInClasses[i]);
    StyleHistoMultiPlot(hEffCascadeSumSignalInClasses[i], 1e-6, hEffCascadeSumSignalInClasses[i]->GetMaximum()*1.2, color[i + 1], MarkerMult[i + 1], "", hEffCascadeSumSignalInClasses[i]->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[i + 1], 0.0, 0.05, 0.0, 0.035, 0.005);
    hEffCascadeSumSignalInClasses[i]->Draw("same");
    hEffCascadeSumSignalInClasses[i]->Write();
    legEffSignalInClasses->AddEntry(hEffCascadeSumSignalInClasses[i], SmoltBis[i+1], "pef");
  }

  // AT THIS POINT WE HAVE EFF. x ACC. and SIGNAL LOSS IN CLASSES

  padEffSignalInClassesUp->cd();
  legendTitleMC->Draw("same");
  legEffSignalInClasses->Draw("same");

  // Event Correction //
  TDirectory* dirEventCorr = outputfile->mkdir("effEvent");
  dirEventCorr->cd();

  TLegend *LegendTitleEventCorr = new TLegend(0.497, 0.626, 0.918, 0.833);
  LegendTitleEventCorr->SetFillStyle(0);
  LegendTitleEventCorr->SetTextAlign(33);
  LegendTitleEventCorr->SetTextSize(0.04);
  LegendTitleEventCorr->SetTextFont(42);
  LegendTitleEventCorr->SetLineColorAlpha(0.,0.);
  LegendTitleEventCorr->SetFillColorAlpha(0.,0.);
  LegendTitleEventCorr->SetBorderSize(0.);
  LegendTitleEventCorr->AddEntry("", "#bf{ALICE Work In Progress}", "");
  LegendTitleEventCorr->AddEntry("", "Event Correction", "");
  LegendTitleEventCorr->AddEntry("", "pp, #sqrt{#it{s}} = 13.6 TeV", "");
  LegendTitleEventCorr->AddEntry("", "LHC24b1b, rec. true INEL > 0", "");
  
  TH3F* hNchFT0MPVContr = (TH3F*)fileMCin->Get("lf-cascqaanalysis/hNchFT0MPVContr");
  TH3F* hRecCollisions3D = (TH3F*)hNchFT0MPVContr->Clone("hNchFT0MPVContrCloneEventCorr");
  hRecCollisions3D->GetZaxis()->SetRange(2, 3); // INEL>0
  hRecCollisions3D->Write();
  TH2D* hRecCollisions2D = static_cast<TH2D*>(hRecCollisions3D->Project3D("yx"));
  hRecCollisions2D->Write();
  TH1D* hRecCollisions1D = (hRecCollisions2D->ProjectionX());
  hRecCollisions1D->Sumw2();
  hRecCollisions1D = (TH1D*)hRecCollisions1D->Rebin(numMult, "hRecCollisions1DRebinned", NChFT0MCalibReversed);
  hRecCollisions1D->Write();

  TH2D* hGenCollisions2D = (TH2D*)fileMCin->Get("lf-cascqaanalysis/hNchFT0MGenEvType");
  hGenCollisions2D->Write();
  hGenCollisions2D->GetYaxis()->SetRange(2, 3); // INEL>0
  TH1D* hGenCollisions1D = (hGenCollisions2D->ProjectionX());
  hGenCollisions1D->Sumw2();
  hGenCollisions1D = (TH1D*)hGenCollisions1D->Rebin(numMult, "hGenCollisions1DRebinned", NChFT0MCalibReversed);
  hGenCollisions1D->Write();

  TCanvas *canvasEventCorr = new TCanvas("canvasEventCorr","canvasEventCorr", 800, 600);
  canvasEventCorr->cd();
  StyleCanvas(canvasEventCorr, 0.14, 0.05, 0.11, 0.15);

  Double_t multiplicityPercEventFacotr[numMult + 1 + 1] = {0}; 
  for (int i = 0; i < numMult + 1; ++i) {
      multiplicityPercEventFacotr[i] = multiplicityPerc[i];
  }
  multiplicityPercEventFacotr[numMult + 1] = 150;

  TH1D* eventCorr = new TH1D("eventCorr", "eventCorr", numMult + 1, multiplicityPercEventFacotr);
  std::cout << "\n************* Calibration Values For Efficiencies *************\n";
  long int totalRec = 0;
  long int totalGen = 0;
  for (int i = 1; i <= eventCorr->GetNbinsX(); ++i) {
      double k = hRecCollisions1D->GetBinContent(numMult + 1 - i);
      double n = hGenCollisions1D->GetBinContent(numMult + 1 - i);
      totalRec += k;
      totalGen += n;
      if (i == eventCorr->GetNbinsX()) { // MB
        k = totalRec;
        n = totalGen;
      }
      double ratio = (n != 0) ? (k / n) : 0.0;
      Double_t errorInBin = sqrt(((Double_t)k+1)*((Double_t)k+2)/(n+2)/(n+3) - pow((Double_t)(k+1),2)/pow(n+2,2));
      std::cout << "mult. %: " << multiplicityPercEventFacotr[i-1] << " -- " << multiplicityPercEventFacotr[i] <<  " k = " << k << " n = " << n << " ratio: " << ratio << " error: " << errorInBin << std::endl;
      eventCorr->SetBinContent(i, ratio);
      eventCorr->SetBinError(i, errorInBin);
  }

  // MB efficiency is a weighted by the class width
  Double_t totWeights = 0;
  Double_t MBeventCorr = 0;
  Double_t errorMBeventCorr = 0;
  for (int j = 1; j <= eventCorr->GetNbinsX() - 1; ++j) {
    MBeventCorr += eventCorr->GetBinContent(j)*eventCorr->GetBinWidth(j);
    totWeights += eventCorr->GetBinWidth(j);
    errorMBeventCorr += pow(eventCorr->GetBinError(j)*eventCorr->GetBinWidth(j), 2);
  }
  MBeventCorr = MBeventCorr / totWeights;
  std::cout << "total weights: " << totWeights << std::endl;
  eventCorr->SetBinContent(eventCorr->GetNbinsX(), MBeventCorr);
  eventCorr->SetBinError(eventCorr->GetNbinsX(), sqrt(errorMBeventCorr));

  TAxis *axisEventCorr = eventCorr->GetXaxis();
  axisEventCorr->ChangeLabel(-1, -1, -1, -1, -1, -1, " "); // last
  axisEventCorr->ChangeLabel(-2, -1, -1, -1, -1, -1, " "); // pre-last 

  StyleHisto(eventCorr, 0.5, 1.2 * eventCorr->GetBinContent(eventCorr->GetMaximumBin()), color[0], MarkerMult[0], "FT0M Multiplicity percentile", "Event Factor", "", 0, 0, 0, 1.0, 1.25, SizeMult[0], 0.04, 0.04);
  eventCorr->Draw("same HIST");
  TLatex labelMB;
  labelMB.SetTextSize(0.05);
  labelMB.SetTextAlign(12);
  labelMB.DrawLatex(103.3,.5329,"Minimum Bias");

  LegendTitleEventCorr->Draw("same");
  canvasEventCorr->Write();

  // Weights rescaling //
  gROOT->SetBatch(kTRUE);
  TCanvas *canvasEventCorrInverse = new TCanvas("canvasEventCorrInverse","canvasEventCorrInverse", 800, 600);
  canvasEventCorrInverse->cd();
  StyleCanvas(canvasEventCorrInverse, 0.14, 0.05, 0.11, 0.15);
  TH1D* eventCorrInverse = new TH1D("eventCorrInverse", "eventCorrInverse", numMult, multiplicityPerc);
  Double_t totEventCorrInverse = 0;
  for (int j = 1; j <= eventCorr->GetNbinsX() - 1; ++j) {
    eventCorrInverse->SetBinContent(j, 1. / eventCorr->GetBinContent(j));
    totEventCorrInverse += eventCorrInverse->GetBinContent(j);
  }
  StyleHisto(eventCorrInverse, 0., 1.2 * eventCorrInverse->GetBinContent(eventCorrInverse->GetMaximumBin()), color[0], MarkerMult[0], "FT0M Multiplicity percentile", "1. / Event Factor", "", 0, 0, 0, 1.0, 1.25, SizeMult[0], 0.04, 0.04);
  eventCorrInverse->Draw();

  gROOT->SetBatch(kFALSE);
  TCanvas *canvasEventCorrCumulative = new TCanvas("canvasEventCorrCumulative","canvasEventCorrCumulative", 800, 600);
  canvasEventCorrCumulative->cd();
  StyleCanvas(canvasEventCorrCumulative, 0.14, 0.05, 0.11, 0.15);
  TH1D* eventCorrCumulative = new TH1D("eventCorrCumulative", "eventCorrCumulative", numMult, multiplicityPerc);
  Double_t cumulativeSum = 0;
  for (int j = 1; j <= eventCorrCumulative->GetNbinsX(); ++j) {
    cumulativeSum += eventCorrInverse->GetBinContent(j);
    eventCorrCumulative->SetBinContent(j, cumulativeSum / totEventCorrInverse);
  }
  StyleHisto(eventCorrCumulative, 0., 1.1, color[0], MarkerMult[0], "FT0M Multiplicity percentile", "Cumulative", "", 0, 0, 0, 1.0, 1.25, SizeMult[0], 0.04, 0.04);
  eventCorrCumulative->Draw("same");
  eventCorrCumulative->Write();
  // HERE WE HAVE TO FULLY CORRECT THE SPECTRA IN CLASSES, COMPUTE MB SIGNAL LOSS AND CORRECT THE MB YIELD
  // Correct spectra in classes for Efficiency x Acceptance, Signal Loss and Event Factor
  // Create output files
  TFile *outputfileCorrYield[numMult + 1];
  for (Int_t iFile = 0; iFile < numMult + 1; iFile++) {
    if (iFile == 0) {
      outputfilePath = outputDir + "/yieldsOutEffCorr/" + "yield_" + particleNames[nParticle] + "_MB_inel" + inel + yieldPostFix + ".root";
    } else {
      outputfilePath = outputDir + "/yieldsOutEffCorr/" + "yield_" + particleNames[nParticle] + "_" + multiplicityPerc[iFile - 1] + "-" + multiplicityPerc[iFile] + "_inel" + inel + yieldPostFix + ".root";
    }
    outputfileCorrYield[iFile] = new TFile(outputfilePath, "RECREATE");
  }

  TDirectory* baseHistsOutDir[numMult + 1];
  TDirectory* corrYieldOutDir[numMult + 1];
  Double_t totYieldCorrected[numPtBins][numMult + 1]; // 0 - MB
  for (Int_t iClass = 0; iClass < numMult + 1; iClass++) {
    for (Int_t b = 0; b <= numPtBins - 1; b++) {
      totYieldCorrected[b][iClass] = 0;
    }
  }

  TH1F* hSignalLoss;
  TH1F* hEffAcc = (TH1F*)hEffCascadeSum->Clone("hEffAcc");

  for (Int_t iFile = 1; iFile < numMult + 1; iFile++) {
    cout << "yield in mult. interval: " << multiplicityPerc[iFile - 1] << "-" << multiplicityPerc[iFile] << endl; 
    Int_t multBin = -1;
    // Choose signal loss histogram
    if (multiplicityPerc[iFile - 1] < 29.) {
      hSignalLoss = (TH1F *)hEffCascadeSumSignalInClasses[0]->Clone("EffSignalSum_0");
      multBin = 0;
    } else if (multiplicityPerc[iFile - 1] < 69.) {
      hSignalLoss = (TH1F *)hEffCascadeSumSignalInClasses[1]->Clone("EffSignalSum_1");
      multBin = 1;
    } else {
      hSignalLoss = (TH1F *)hEffCascadeSumSignalInClasses[2]->Clone("EffSignalSum_2");
      multBin = 2;
    }
    hSignalLoss->SetName("hSignalLoss");
    TF1 *fitExp = new TF1("fitExp", "[0]*exp([1]*x-[2]) + [3]", binpt[0], binpt[numPtBins]);
    fitExp->SetParameters(-0.4, -5.7, -0.1, 1);
    TF1 *fitPol1 = new TF1("fitPol1", "[0]*x + [1]", binpt[0], binpt[numPtBins]);
    fitPol1->SetParameters(0.01, 0.9);
    padEffSignalInClassesUp->cd();
    if (multiplicityPerc[iFile - 1] > 29.) {
      hSignalLoss->Fit(fitExp, "QR0"); // Fit low-mult. with exponent
      fitExp->Draw("same");
      //cout << "Chi2/NDF " << fitExp->GetChisquare() / fitExp->GetNDF() << endl;
    } else {
      hSignalLoss->Fit(fitPol1, "QR0"); // Fit wil pol1
      fitPol1->Draw("same");
      //cout << "Chi2/NDF " << fitPol1->GetChisquare() / fitPol1->GetNDF() << endl;      
    }

    for (Int_t b = 1; b <= numPtBins; b++) {
      Double_t relErr = 0;
      Double_t relErrEffAcc = 0;
      // Eff. x Acc.
      relErr = hYieldCorrected[iFile]->GetBinError(b) / hYieldCorrected[iFile]->GetBinContent(b);
      relErrEffAcc = hEffAcc->GetBinError(hEffAcc->FindBin(hYieldCorrected[iFile]->GetBinCenter(b))) / hEffAcc->GetBinContent(hEffAcc->FindBin(hYieldCorrected[iFile]->GetBinCenter(b)));
      hYieldCorrected[iFile]->SetBinContent(b, hYieldCorrected[iFile]->GetBinContent(b) / hEffAcc->GetBinContent(hEffAcc->FindBin(hYieldCorrected[iFile]->GetBinCenter(b))));
      hYieldCorrected[iFile]->SetBinError(b, sqrt(pow(relErr, 2) + pow(relErrEffAcc, 2)) * hYieldCorrected[iFile]->GetBinContent(b));
      //cout << "eff. x acc.: " << hEffAcc->GetBinContent(hEffAcc->FindBin(hYield[iFile]->GetBinCenter(b))) << " relErrEffAcc: " << relErrEffAcc << endl;
      std::cout << "yield after eff. x acc.: " << hYieldCorrected[iFile]->GetBinContent(b) << std::endl;
      // Signal Loss
      relErr = hYieldCorrected[iFile]->GetBinError(b) / hYieldCorrected[iFile]->GetBinContent(b);
      Double_t relErrSigLoss = 0;
      Double_t sigLoss = 0;

      Double_t xValue = hSignalLoss->GetXaxis()->GetBinCenter(b);

      if (multiplicityPerc[iFile - 1] > 69.) {
        sigLoss = fitExp->Eval(xValue);

        Double_t withoutConst = sigLoss - fitExp->GetParameter(3);
        Double_t sig0 = (withoutConst / fitExp->GetParameter(0)) * fitExp->GetParError(0);
        Double_t sig1 = withoutConst * xValue * fitExp->GetParError(1);
        Double_t sig2 = withoutConst * fitExp->GetParError(2);
        Double_t sig3 = fitExp->GetParError(3);

        relErrSigLoss = sqrt(pow(sig0,2) + pow(sig1,2) + pow(sig2,2) + pow(sig3,2)) / sigLoss;
      } else {
        // Take values from histogram
        //sigLoss = hSignalLoss->GetBinContent(b);
        //relErrSigLoss = hSignalLoss->GetBinError(hSignalLoss->FindBin(hYieldCorrected[iFile]->GetXaxis()->GetBinCenter(b))) / sigLoss;
        // Take values from fit
        sigLoss = fitPol1->Eval(xValue);

        Double_t sig0 = xValue * fitPol1->GetParError(0);
        Double_t sig1 = fitPol1->GetParError(1);

        relErrSigLoss = sqrt(pow(sig0,2) + pow(sig1,2)) / sigLoss;
      }

      hYieldCorrected[iFile]->SetBinContent(b, hYieldCorrected[iFile]->GetBinContent(b) / sigLoss);
      std::cout << "yield after signal loss: " << hYieldCorrected[iFile]->GetBinContent(b) << std::endl;
      hYieldCorrected[iFile]->SetBinError(b, sqrt(pow(relErr, 2) + pow(relErrSigLoss, 2)) * hYieldCorrected[iFile]->GetBinContent(b));
      //cout << "sigLoss: " << sigLoss << " relErrSigLoss: " << relErrSigLoss<< endl;

      // Event splitting/loss correction
      relErr = hYieldCorrected[iFile]->GetBinError(b) / hYieldCorrected[iFile]->GetBinContent(b);
      Double_t relErrEventFactor = eventCorr->GetBinError(iFile)/eventCorr->GetBinContent(iFile);
      hYieldCorrected[iFile]->SetBinContent(b, hYieldCorrected[iFile]->GetBinContent(b) * eventCorr->GetBinContent(iFile));
      std::cout << "yield after event corr. in bin " << b << " is: " << hYieldCorrected[iFile]->GetBinContent(b) << std::endl;
      hYieldCorrected[iFile]->SetBinError(b, sqrt(pow(relErr, 2) + pow(relErrEventFactor, 2)) * hYieldCorrected[iFile]->GetBinContent(b));
      //cout << "event correction: " << eventCorr[iFile] << " relErrEventFactor: " << relErrEventFactor<< endl;

      // Compute "integrated yiled"
      totYieldCorrected[b-1][iFile] += hYieldCorrected[iFile]->GetBinContent(b);
    }
    corrYieldOutDir[iFile] = outputfileCorrYield[iFile]->mkdir("effCorrYield");
    corrYieldOutDir[iFile]->cd();
    hYieldCorrected[iFile]->Write();

    baseHistsOutDir[iFile] = outputfileCorrYield[iFile]->mkdir("baseHists");
    baseHistsOutDir[iFile]->cd();
    hYield[iFile]->Write();
    eventCorr->Write();
    hSignalLoss->Write();
    hEffAcc->Write();
  }

  // Compute MB Signal Loss
  Double_t cascadesInClass[numPtBins][numMultEff]; // cascades in class per percentile of mult. class of efficiencies
  for (Int_t iClass = 0; iClass < numMultEff; iClass++) {
    for (Int_t b = 0; b <= numPtBins - 1; b++) {
      cascadesInClass[b][iClass] = 0;
    }
  }
  TH1F* hCalibFT0MPerc = (TH1F*)fileCalibrationIn->Get("results/hCalibFT0Mperc");
  for (Int_t iFile = 1; iFile < numMult + 1; iFile++) {
    for (Int_t b = 1; b <= numPtBins; b++) {
      if (iFile <= 6) {
        cascadesInClass[b-1][0] += totYieldCorrected[b-1][iFile] * hCalibFT0MPerc->GetBinWidth(iFile);
      }
      if (iFile >= 7 && iFile <= 9) {
        cascadesInClass[b-1][1] += totYieldCorrected[b-1][iFile] * hCalibFT0MPerc->GetBinWidth(iFile);
      }
      if (iFile >= 10) {
        cascadesInClass[b-1][2] += totYieldCorrected[b-1][iFile] * hCalibFT0MPerc->GetBinWidth(iFile);
      }
      std::cout << "totYieldCorrected in bin " << b-1 << " is: " << totYieldCorrected[b-1][iFile] << " " << multiplicityPerc[iFile-1] << " -- " << multiplicityPerc[iFile] <<  " class width: " << hCalibFT0MPerc->GetBinWidth(iFile) << std::endl;
    }
  }

  hEffCascadeSumSignal->Reset();
  for (Int_t b = 1; b <= numPtBins; b++) {
    Double_t signalLossInBin = 0; 
    Double_t sumWeights = 0;
    Double_t errorInBin = 0;
    for (Int_t i = 0; i < numMultEff; i++) {
      TH1F* hSignalLossInClass = (TH1F*)hEffCascadeSumSignalInClasses[i]->Clone(Form("hSignalLossInClass_%i_ptbin_%i", i, b));
      signalLossInBin += hSignalLossInClass->GetBinContent(b)*cascadesInClass[b-1][i];
      sumWeights += cascadesInClass[b-1][i];
      errorInBin += pow(hSignalLossInClass->GetBinError(b)*cascadesInClass[b-1][i], 2);
    }
    std::cout << "tot number of cascades in pt bin " << b-1 << " is: " <<  sumWeights << std::endl;
    hEffCascadeSumSignal->SetBinContent(b, signalLossInBin / sumWeights);
    hEffCascadeSumSignal->SetBinError(b, sqrt(errorInBin) / sumWeights);
  }

  padEffSignalInClassesUp->cd();
  StyleHistoMultiPlot(hEffCascadeSumSignal, 1e-6, hEffCascadeSumSignal->GetMaximum()*1e2, color[0], MarkerMult[0], "", hEffCascadeSumSignal->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[0], 0.0, 0.05, 0.0, 0.035, 0.005);
  hEffCascadeSumSignal->Draw("same");
  dirEffSignal->cd();
  hEffCascadeSumSignal->Write();

  dirEffSignalInClasses->cd();
  for (Int_t i = 0; i < numMultEff; i++) {
    padEffSignalInClassesLow->cd();
    hEffCascadeSumSignalInClassesRatio[i] = (TH1D*)hEffCascadeSumSignalInClasses[i]->Clone(Form("hEffCascadeSumSignalRatio_%i", i));
    hEffCascadeSumSignalInClassesRatio[i]->Divide(hEffCascadeSumSignal);
    StyleHistoMultiPlot(hEffCascadeSumSignalInClassesRatio[i], 0.1 , 1.9, color[i+1], MarkerMult[i+1], "#it{p}_{T} (GeV/#it{c})", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, SizeMult[i+1], 0.08, 0.08, 0.08, 0.08, 0.005);
    hEffCascadeSumSignalInClassesRatio[i]->Draw("same");
  }
  legEffSignalInClasses->AddEntry(hEffCascadeSumSignal, SmoltBis[0], "pef");
  canvasEffSignalInClasses->Write();

  // NOW WE CAN COMPUTE YIELD IN MB
  {
    cout << "MB yield" << endl;
    hSignalLoss = (TH1F*)hEffCascadeSumSignal->Clone("hSignalLossMB");
    // TF1 *fitPol1 = new TF1("fitPol1", "[0]*x + [1]", binpt[0], binpt[numPtBins]);
    // fitPol1->SetParameters(0.01, 0.9);
    // padEffSignalInClassesUp->cd();
    // hSignalLoss->Fit(fitPol1, "QR0"); // Fit wil pol1
    // fitPol1->Draw("same");
    //cout << "Chi2/NDF " << fitPol1->GetChisquare() / fitPol1->GetNDF() << endl;
    // Compute it like a separate class //
    for (Int_t b = 1; b <= hYield[0]->GetNbinsX(); b++) {
      Double_t relErr = 0;
      Double_t relErrEffAcc = 0;
      // Eff. x Acc.
      relErr = hYieldCorrected[0]->GetBinError(b) / hYieldCorrected[0]->GetBinContent(b);
      relErrEffAcc = hEffAcc->GetBinError(hEffAcc->FindBin(hYieldCorrected[0]->GetBinCenter(b))) / hEffAcc->GetBinContent(hEffAcc->FindBin(hYieldCorrected[0]->GetBinCenter(b)));
      hYieldCorrected[0]->SetBinContent(b, hYieldCorrected[0]->GetBinContent(b) / hEffAcc->GetBinContent(hEffAcc->FindBin(hYieldCorrected[0]->GetBinCenter(b))));
      hYieldCorrected[0]->SetBinError(b, sqrt(pow(relErr, 2) + pow(relErrEffAcc, 2)) * hYieldCorrected[0]->GetBinContent(b));
      //cout << "eff. x acc.: " << hEffAcc->GetBinContent(hEffAcc->FindBin(hYield[0]->GetBinCenter(b))) << " relErrEffAcc: " << relErrEffAcc << endl;
      std::cout << "yield after eff. x acc.: " << hYieldCorrected[0]->GetBinContent(b) << std::endl;
      // Signal Loss
      relErr = hYieldCorrected[0]->GetBinError(b) / hYieldCorrected[0]->GetBinContent(b);
      Double_t relErrSigLoss = 0;
      Double_t sigLoss = 0;

      Double_t xValue = hSignalLoss->GetXaxis()->GetBinCenter(b);
      // Take values from histogram
      sigLoss = hSignalLoss->GetBinContent(b);
      relErrSigLoss = hSignalLoss->GetBinError(hSignalLoss->FindBin(hYieldCorrected[0]->GetXaxis()->GetBinCenter(b))) / sigLoss;
      // Take values from fit
      // sigLoss = fitPol1->Eval(xValue);

      // Double_t sig0 = xValue * fitPol1->GetParError(0);
      // Double_t sig1 = fitPol1->GetParError(1);

      // relErrSigLoss = sqrt(pow(sig0,2) + pow(sig1,2)) / sigLoss;

      hYieldCorrected[0]->SetBinContent(b, hYieldCorrected[0]->GetBinContent(b) / sigLoss);
      std::cout << "yield after signal loss: " << hYieldCorrected[0]->GetBinContent(b) << std::endl;
      hYieldCorrected[0]->SetBinError(b, sqrt(pow(relErr, 2) + pow(relErrSigLoss, 2)) * hYieldCorrected[0]->GetBinContent(b));
      //cout << "sigLoss: " << sigLoss << " relErrSigLoss: " << relErrSigLoss<< endl;

      // Event splitting/loss correction
      relErr = hYieldCorrected[0]->GetBinError(b) / hYieldCorrected[0]->GetBinContent(b);
      Double_t relErrEventFactor = eventCorr->GetBinError(numMult + 1)/eventCorr->GetBinContent(numMult + 1);
      hYieldCorrected[0]->SetBinContent(b, hYieldCorrected[0]->GetBinContent(b) * eventCorr->GetBinContent(numMult + 1));
      std::cout << "yield after event corr.: " << hYieldCorrected[0]->GetBinContent(b) << std::endl;
      hYieldCorrected[0]->SetBinError(b, sqrt(pow(relErr, 2) + pow(relErrEventFactor, 2)) * hYieldCorrected[0]->GetBinContent(b));
      //cout << "event correction: " << eventCorr[numMult + 1] << " relErrEventFactor: " << relErrEventFactor<< endl;
    }
    corrYieldOutDir[0] = outputfileCorrYield[0]->mkdir("effCorrYield");
    corrYieldOutDir[0]->cd();
    hYieldCorrected[0]->Write();

    baseHistsOutDir[0] = outputfileCorrYield[0]->mkdir("baseHists");
    baseHistsOutDir[0]->cd();
    hYield[0]->Write();
    eventCorr->Write();
    hSignalLoss->Write();
    hEffAcc->Write();
  }

  delete[] binpt;

  // End of Code
  std::cout << "\x1B[1;32m"; // Set text color to green
  std::cout << "\n************* Eff. x Acc.,  Signal Loss and Event Factor are Obtained Successfully! *************\n";
  std::cout << "\x1B[0m"; // Reset text color
}
