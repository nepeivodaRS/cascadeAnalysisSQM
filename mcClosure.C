#include "help.h"
#include "effHelp.h"

void mcClosure(const  Int_t nParticle = 2, // 0-2 : xi, 3-5 : omega
               const TString workingDir = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM",
               const TString normPath = "/Users/rnepeiv/workLund/PhD_work/run3omega/data/used_in_analysis_note/18apr_lhc24b1/AnalysisResults.root",
               const TString effAccPath = "/Users/rnepeiv/workLund/PhD_work/run3omega/efficiencyCalculation/results/effChiaraXi_inelgt0_lhc24b1_18apr.root")
{
  // Start of Code
  std::cout << "\x1B[1;33m"; // Set text color to yellow
  std::cout << "\n************ Starting Making MC Closure ************\n";
  std::cout << "\x1B[0m"; // Reset text color

  TString outputfilePath = workingDir + "/mc-closures/" + particleNames[nParticle] + ".root";
  TFile *outputfile = new TFile(outputfilePath, "RECREATE");

  // Efficiency Corrected Data File
  TString inputDataPath = workingDir + "/yieldsOutEffCorr/yield_" + particleNames[nParticle] + "_MB_inel0_mc-closure.root";
  TFile* fileDataIn = TFile::Open(inputDataPath);
  if (!fileDataIn || fileDataIn->IsZombie()) {
      std::cerr << "Error opening eff. corr. data file!" << std::endl;
      return;
  }

  TDirectory* yieldEffCorrDir = fileDataIn->GetDirectory("effCorrYield");
  if (!yieldEffCorrDir)
  {
    std::cerr << "`effCorrYield` directory in data is not found!" << std::endl;
    return;
  }

  TH1F* hEffCorrYield = (TH1F *)yieldEffCorrDir->Get("Yield_" + particleNames[nParticle]);
  if (!hEffCorrYield)
  {
    std::cerr << "Histogram `Yield_XiPm` is not found!" << std::endl;
    return;
  }

  // Open efficiency x acceptance file
  TFile *effAccFile = TFile::Open(effAccPath);
  if (!effAccFile || effAccFile->IsZombie()) {
    std::cerr << "Error opening efficiency x acceptance file!" << std::endl;
    return;
  } else {
    cout << "efficiency x acceptance file is opened"<< std::endl;
  }
  TDirectory* supportHistDir = effAccFile->GetDirectory("supportHistos");
  if (!supportHistDir)
  {
    std::cerr << "`supportHistos` directory is not found!" << std::endl;
    return;
  }

  TH1F* hTrueSpectrum;
  if (nParticle == 2 || nParticle == 5) {
    hTrueSpectrum = (TH1F *)supportHistDir->Get("SumTrueClone_xy_py");
  } else if (nParticle == 0 || nParticle == 3) {
    hTrueSpectrum = (TH1F *)supportHistDir->Get("MinusTrueClone_xy_py");
  } else if (nParticle == 1 || nParticle == 4) {
    hTrueSpectrum = (TH1F *)supportHistDir->Get("PlusTrueClone_xy_py");
  }

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

  hTrueSpectrum = (TH1F*)hTrueSpectrum->Rebin(numPtBins, "hTrueSpectrumRebin", binpt);

  TFile* fileNormMC = TFile::Open(normPath);
  TDirectory* cascqaanalysisDir = fileNormMC->GetDirectory("lf-cascqaanalysis");
  if (!cascqaanalysisDir)
  {
    std::cerr << "`lf-cascqaanalysis` directory is not found!" << std::endl;
    return;
  }

  cascqaanalysisDir->cd();

  TH1F* hNeventsMC = (TH1F *)cascqaanalysisDir->Get("hNEventsMC");
  if (!hNeventsMC)
  {
    std::cerr << "Histogram `hNEventsMC` is not found!" << std::endl;
    return;
  }
  long int nEvents = hNeventsMC->GetBinContent(4); // get total number of INEL > 0 events
  std::cout << "Total number of generated INEL > 0 events is: " << nEvents << std::endl;
  TDirectory* baseHistsOutDir = outputfile->mkdir("baseHists");
  baseHistsOutDir->cd();
  hNeventsMC->Write();

  // normalize MC gen.
  for (Int_t b = 1; b <= hTrueSpectrum->GetNbinsX(); b++) {
    Int_t nCounts = hTrueSpectrum->GetBinContent(b);
    Int_t k = hTrueSpectrum->GetBinContent(b);
    Int_t n = nEvents;
    Double_t errorInBin = sqrt(((Double_t)k+1)*((Double_t)k+2)/(n+2)/(n+3) - pow((Double_t)(k+1),2)/pow(n+2,2));
    hTrueSpectrum->SetBinContent(b, (Double_t)k/(Double_t)n * 1./hTrueSpectrum->GetBinWidth(b));
    hTrueSpectrum->SetBinError(b, errorInBin * 1./hTrueSpectrum->GetBinWidth(b));
  }
  hTrueSpectrum->GetXaxis()->SetRangeUser(binpt[0], binpt[numPtBins]);

  TH1F* hYieldsDenom = (TH1F *)hTrueSpectrum->Clone("hTrueSpectrumClone");
  TH1F* hYieldsRatio = (TH1F *)hEffCorrYield->Clone("YieldClone");
  hYieldsRatio->Divide(hYieldsDenom);

  Double_t yieldYLow[2] = {0.1*1e-5, 0.2*1e-8};
  Double_t yieldYUp[2] = {0.04, 3*1e4};

  TCanvas* canvasComparison = new TCanvas("canvasComparison", "canvasComparison", 0, 70, 620, 850);
  StyleCanvas(canvasComparison, 0.15, 0.05, 0.05, 0.15);
  // Yield Pads
  TPad *padYieldUp = new TPad("padYieldUp_" + particleNames[2], "padYieldUp_" + particleNames[2], 0, 0.36, 1, 1);
  TPad *padYieldLow = new TPad("padYieldLow_" + particleNames[2], "padYieldLow_" + particleNames[2], 0, 0, 1, 0.36);
  StylePad(padYieldUp, 0.15, 0.05, 0.05, 0.);
  StylePad(padYieldLow, 0.15, 0.05, 0.02, 0.2);

  canvasComparison->cd();
  padYieldUp->Draw();
  padYieldLow->Draw();
  gStyle->SetOptStat(0);

  // Yield Legend
  TLegend *legYield = new TLegend(0.186, 0.0625, 0.893, 0.3125);
  legYield->SetHeader("MC Closure LHC24b1");
  //legYield->SetNColumns(3);
  TLegendEntry *legYieldHeader = (TLegendEntry *)legYield->GetListOfPrimitives()->First();
  legYieldHeader->SetTextSize(0.04);
  //legYield->SetTextAlign(13); // adjust text wrt markers
  StyleLegend(legYield, 0.0, 0.0);
  legYield->SetTextSize(0.035);

  TLegend *legendTitle = new TLegend(0.609, 0.693913, 0.908, 0.893);
  StyleLegend(legendTitle, 0.0, 0.0);
  legendTitle->SetTextAlign(33);
  legendTitle->SetTextSize(0.05);
  legendTitle->SetTextFont(42);
  legendTitle->SetLineColorAlpha(0.,0.);
  legendTitle->SetFillColorAlpha(0.,0.);
  legendTitle->AddEntry("", "#bf{ALICE Work In Progress}", "");
  legendTitle->AddEntry("", "pp, #sqrt{#it{s}} = 13.6 TeV", "");
  legendTitle->AddEntry("", particleSymnbols[nParticle] + ", |y| < 0.5", "");

  Int_t compStyle = 3;
  StyleHistoMultiPlot(hEffCorrYield, yieldYLow[partType], yieldYUp[partType], color[0], MarkerMult[0], "", hEffCorrYield->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[0], 0.0, 0.05, 0.0, 0.035, 0.005);
  StyleHistoMultiPlot(hTrueSpectrum, yieldYLow[partType], yieldYUp[partType], color[compStyle], MarkerMult[compStyle], "", hEffCorrYield->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[compStyle], 0.0, 0.05, 0.0, 0.035, 0.005);

  // Dummy histograms to extend x-axis
  TH1F *hDummy = new TH1F("hDummy", "hDummy", 10000, 0, 8.5);
  // Up
  for (Int_t i = 1; i <= hDummy->GetNbinsX(); i++)
    hDummy->SetBinContent(i, 1e-12);
  canvasComparison->cd();
  padYieldUp->cd();
  StyleHistoMultiPlot(hDummy, yieldYLow[partType], yieldYUp[partType], 1, 1, "#it{p}_{T} (GeV/#it{c})", sdNdPtdY, "", 0, 0, 0, 1.5, 1.0, 0, 0.0, 0.05, 0.0, 0.035, 0.005);
  SetTickLength(hDummy, 0.025, 0.03);
  TAxis *axisYieldDummy = hDummy->GetYaxis();
  axisYieldDummy->ChangeLabel(1, -1, -1, -1, -1, -1, " ");
  hDummy->GetXaxis()->SetRangeUser(0, hEffCorrYield->GetXaxis()->GetBinUpEdge(hEffCorrYield->GetNbinsX()) + 0.5);
  hDummy->Draw("same");
  // Low
  TH1F *hDummyLow = new TH1F("hDummyLow", "hDummyLow", 10000, 0, 8.5);
  for (Int_t i = 1; i <= hDummyLow->GetNbinsX(); i++)
    hDummyLow->SetBinContent(i, 1);
  padYieldLow->cd();
  StyleHistoMultiPlot(hDummyLow, 0.5 , 1.5, 1, 1, "#it{p}_{T} (GeV/#it{c})", "Ratio to generated", "", 0, 0, 0, 1.0, 0.7, 0, 0.08, 0.08, 0.08, 0.07, 0.01);
  SetTickLength(hDummyLow, 0.025, 0.03);
  hDummyLow->GetXaxis()->SetRangeUser(0, hYieldsRatio->GetXaxis()->GetBinUpEdge(hYieldsRatio->GetNbinsX()) + 0.5);
  hDummyLow->GetYaxis()->CenterTitle();
  TAxis *axisYieldDummyLow = hDummyLow->GetYaxis();
  axisYieldDummyLow->ChangeLabel(1, -1, -1, -1, -1, -1, " ");
  axisYieldDummyLow->ChangeLabel(-1, -1, -1, -1, -1, -1, " ");
  hDummyLow->Draw("same");

  padYieldUp->cd();
  hEffCorrYield->Draw("same ex0");
  hEffCorrYield->SetFillStyle(0);
  hEffCorrYield->Draw("same e2");
  legYield->AddEntry(hEffCorrYield, "reconstructed", "pef");
  hTrueSpectrum->Draw("same ex0");
  hTrueSpectrum->SetFillStyle(0);
  hTrueSpectrum->Draw("same e2");
  legYield->AddEntry(hTrueSpectrum, "generated", "pef");
  padYieldUp->SetLogy();

  padYieldLow->cd();
  StyleHistoMultiPlot(hYieldsRatio, 0.5 , 1.1, color[0], MarkerMult[0], "#it{p}_{T} (GeV/#it{c})", "Ratio to Published", "", 0, 0, 0, 1.0, 0.7, SizeMult[0], 0.08, 0.08, 0.08, 0.08, 0.005);
  hYieldsRatio->Draw("same ex0");
  hYieldsRatio->SetFillStyle(0);
  hYieldsRatio->Draw("same e2");
  //padYieldLow->SetLogy();

  padYieldUp->cd();
  legendTitle->Draw();
  legYield->Draw();
  gPad->Update();
  padYieldLow->cd();
  DrawHorLine(hYieldsRatio->GetXaxis()->GetBinUpEdge(hYieldsRatio->GetNbinsX()) + 0.5, 1.0);
  canvasComparison->Update();

  outputfile->cd();
  canvasComparison->Write();

  // End of Code
  std::cout << "\x1B[1;32m"; // Set text color to green
  std::cout << "\n************* MC Closure is Finished! *************\n";
  std::cout << "\x1B[0m"; // Reset text color
}