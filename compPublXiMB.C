#include "help.h"

void StyleHisto(TH1F *histo, Double_t Low, Double_t Up, Int_t color, Int_t style, TString titleX, TString titleY, TString title, 
                Bool_t XRange, Double_t XLow, Double_t XUp, Double_t xOffset, Double_t yOffset, Double_t mSize, 
                Double_t xTitleSize, Double_t yTitleSize, Double_t xLabelSize,  Double_t yLabelSize, Double_t yLabelOffset)
{
  histo->GetYaxis()->SetRangeUser(Low, Up);
  if (XRange)
    histo->GetXaxis()->SetRangeUser(XLow, XUp);
  histo->SetLineColor(color);
  histo->SetMarkerColor(color);
  histo->SetMarkerStyle(style);
  histo->SetMarkerSize(mSize);
  histo->GetXaxis()->SetTitle(titleX);
  histo->GetXaxis()->SetLabelSize(xLabelSize);
  histo->GetXaxis()->SetTitleSize(xTitleSize);
  histo->GetXaxis()->SetTitleOffset(xOffset);
  histo->GetYaxis()->SetTitle(titleY);
  histo->GetYaxis()->SetTitleSize(yTitleSize);
  histo->GetYaxis()->SetLabelSize(yLabelSize);
  histo->GetYaxis()->SetTitleOffset(yOffset);
  histo->SetTitle(title);
  histo->GetYaxis()->SetLabelOffset(yLabelOffset);
}

void compPublXiMB(const Int_t nParticle = 2, // 0-2 : xi, 3-5 : omega
                 const Int_t inel = 0, // inel > N (0/1)
                 const TString workingDir = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM",
                 const TString postFix = "")
{
  // Start of Code
  std::cout << "\x1B[1;33m"; // Set text color to yellow
  std::cout << "\n************ Starting Making Comparison with Published Results ************\n";
  std::cout << "\x1B[0m"; // Reset text color

  TString outputfilePath = workingDir + "/comparisons/" + "mbXi" + ".root";
  TFile *outputfile = new TFile(outputfilePath, "RECREATE");

  // HEP Data File 
  TString inputHEPDataPath = workingDir + "/data/published/xi.root";
  TFile* fileHEPDataIn = TFile::Open(inputHEPDataPath);
  if (!fileHEPDataIn || fileHEPDataIn->IsZombie()) {
      std::cerr << "Error opening HEP data file!" << std::endl;
      return;
  }
  
  TDirectory* yieldHEPDir = fileHEPDataIn->GetDirectory("Table 3");
  if (!yieldHEPDir)
  {
    std::cerr << "`Table 3` directory in HEP data file is not found!" << std::endl;
    return;
  }

  TH1F* hHEPYield = (TH1F *)yieldHEPDir->Get("Hist1D_y11");
  TH1F* hHEPYielde1 = (TH1F *)yieldHEPDir->Get("Hist1D_y11_e1");
  TH1F* hHEPYielde2 = (TH1F *)yieldHEPDir->Get("Hist1D_y11_e2");
  TH1F* hHEPYielde3 = (TH1F *)yieldHEPDir->Get("Hist1D_y11_e3");
  if (!hHEPYield || !hHEPYielde1 || !hHEPYielde2)
  {
    std::cerr << "Histogram `Hist1D_y11` is not found!" << std::endl;
    return;
  }

  Double_t e1 = 0;
  Double_t e2 = 0;
  Double_t e3 = 0;

  for (Int_t iBin = 1; iBin <= hHEPYield->GetNbinsX(); iBin++) {
    e1 = hHEPYielde1->GetBinContent(iBin);
    e2 = hHEPYielde2->GetBinContent(iBin);
    e3 = hHEPYielde3->GetBinContent(iBin); // uncorrelated
    //hHEPYield->SetBinError(iBin, sqrt(pow(e1, 2) + pow(e2, 2)) + e3);
    hHEPYield->SetBinError(iBin, sqrt(pow(e1, 2))); // only statistical uncert.
  }

  // Files with yields in all mult. classes + MB
  TFile* fileDataIn[numMult + 1];
  // MB
  fileDataIn[0] = TFile::Open(workingDir + "/yieldsOutEffCorr" +  "/yield_" + particleNames[nParticle] + "_MB_inel" + inel + postFix + ".root");
  if (!fileDataIn[0] || fileDataIn[0]->IsZombie()) {
    std::cerr << "Error opening input data file for MB!" << std::endl;
    return;
  } else {
    cout << "file for MB yield is opened"<< std::endl;
  }
  // in mult. classes
  for (Int_t iFile = 1; iFile < numMult + 1; iFile++) {
    TString fileInPath = workingDir + "/yieldsOutEffCorr" + "/yield_" + particleNames[nParticle] + "_" + multiplicityPerc[iFile - 1] + "-" + multiplicityPerc[iFile] + "_inel" + inel + postFix  + ".root";
    fileDataIn[iFile] = TFile::Open(fileInPath);
    if (!fileDataIn[iFile] || fileDataIn[iFile]->IsZombie()) {
      std::cerr << "Error opening input data file for mult. class: " << multiplicityPerc[iFile - 1] <<  " - " << multiplicityPerc[iFile] << std::endl;
      return;
    } else {
      cout << "file for mult. class: " << multiplicityPerc[iFile - 1] <<  " - " << multiplicityPerc[iFile] << " is opened"<< std::endl;
    }
  }

  TH1F* hYield[numMult + 1];
  TDirectory* yieldDir[numMult + 1];

  // Get yields
  for (Int_t iFile = 0; iFile < numMult + 1; iFile++) {
    yieldDir[iFile] = fileDataIn[iFile]->GetDirectory("effCorrYield");
    if (!yieldDir[iFile])
    {
      std::cerr << "`effCorrYield` directory is not found!" << std::endl;
      return;
    }

    hYield[iFile] = (TH1F *)yieldDir[iFile]->Get("Yield_" + particleNames[nParticle]);
  }

  TH1F* hYieldMBsummed;
  hYieldMBsummed = (TH1F*)hYield[0]->Clone("hYieldMBsummed");
  hYieldMBsummed->Reset();
  for (Int_t iFile = 1; iFile < numMult + 1; iFile++) {
    TH1F* hYieldPerPerc = (TH1F*)hYield[iFile]->Clone("hYieldPerPerc");
    hYieldPerPerc->Scale(multiplicityPerc[iFile] - multiplicityPerc[iFile - 1]);
    std::cout << "class width: " << (multiplicityPerc[iFile] - multiplicityPerc[iFile - 1]) << std::endl;
    hYieldMBsummed->Add(hYieldPerPerc);
  }
  hYieldMBsummed->Scale(1. / 100);

  TDirectory* yieldEffCorrDir = fileDataIn[0]->GetDirectory("effCorrYield");
  if (!yieldEffCorrDir)
  {
    std::cerr << "`effCorrYield` directory in data is not found!" << std::endl;
    return;
  }

  TH1F* hEffCorrYield = (TH1F *)yieldEffCorrDir->Get("Yield_XiPm");
  if (!hEffCorrYield)
  {
    std::cerr << "Histogram `Yield_XiPm` is not found!" << std::endl;
    return;
  }

  TH1F* hYieldsDenom = (TH1F *)hHEPYield->Clone("YieldHEPDataClone");
  //TH1F* hYieldsRatio = (TH1F *)hYield[0]->Clone("YieldClone");
  TH1F* hYieldsRatio = (TH1F *)hYieldMBsummed->Clone("YieldClone");
  hYieldsRatio->Divide(hYieldsDenom);

  Double_t yieldYLow[2] = {0.1*1e-5, 0.2*1e-8};
  Double_t yieldYUp[2] = {0.04, 3*1e4};

  Int_t partType = 0;

  TCanvas* canvasComparison = new TCanvas("canvasComparison_Xi", "canvasComparison_Xi", 0, 70, 620, 850);
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
  legYield->SetHeader("Comparison with Published MB Run2 Data");
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
  legendTitle->AddEntry("", particleSymnbols[2] + ", |y| < 0.5", "");
  legendTitle->AddEntry("", "Statistical uncert. only", "");

  Int_t compStyle = 3;
  StyleHisto(hEffCorrYield, yieldYLow[partType], yieldYUp[partType], color[0], MarkerMult[0], "", hEffCorrYield->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[0], 0.0, 0.05, 0.0, 0.035, 0.005);
  StyleHisto(hHEPYield, yieldYLow[partType], yieldYUp[partType], color[compStyle], MarkerMult[compStyle], "", hEffCorrYield->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[compStyle], 0.0, 0.05, 0.0, 0.035, 0.005);

  // Dummy histograms to extend x-axis
  TH1F *hDummy = new TH1F("hDummy", "hDummy", 10000, 0, 8.5);
  // Up
  for (Int_t i = 1; i <= hDummy->GetNbinsX(); i++)
    hDummy->SetBinContent(i, 1e-12);
  canvasComparison->cd();
  padYieldUp->cd();
  StyleHisto(hDummy, yieldYLow[partType], yieldYUp[partType], 1, 1, "#it{p}_{T} (GeV/#it{c})", sdNdPtdY, "", 0, 0, 0, 1.5, 1.0, 0, 0.0, 0.05, 0.0, 0.035, 0.005);
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
  StyleHisto(hDummyLow, 0.8 , 1.2, 1, 1, "#it{p}_{T} (GeV/#it{c})", "Ratio to Published", "", 0, 0, 0, 1.0, 0.7, 0, 0.08, 0.08, 0.08, 0.07, 0.01);
  SetTickLength(hDummyLow, 0.025, 0.03);
  hDummyLow->GetXaxis()->SetRangeUser(0, hYieldsRatio->GetXaxis()->GetBinUpEdge(hYieldsRatio->GetNbinsX()) + 0.5);
  hDummyLow->GetYaxis()->CenterTitle();
  TAxis *axisDummyLow = hDummyLow->GetYaxis();
  axisDummyLow->ChangeLabel(1, -1, -1, -1, -1, -1, " ");
  axisDummyLow->ChangeLabel(-1, -1, -1, -1, -1, -1, " ");
  hDummyLow->Draw("same");

  padYieldUp->cd();
  hEffCorrYield->Draw("same ex0");
  //hEffCorrYield->SetFillStyle(0);
  //hEffCorrYield->Draw("same e2");
  legYield->AddEntry(hEffCorrYield, "pp, #sqrt{#it{s}} = 13.6 TeV", "pef");
  hHEPYield->Draw("same ex0");
  //hHEPYield->SetFillStyle(0);
  //hHEPYield->Draw("same e2");
  legYield->AddEntry(hHEPYield, "Eur.Phys.J.C.80 (2020) 167, pp, #sqrt{#it{s}} = 13 TeV", "pef");
  padYieldUp->SetLogy();

  padYieldLow->cd();
  StyleHisto(hYieldsRatio, 0.5 , 1.1, color[0], MarkerMult[0], "#it{p}_{T} (GeV/#it{c})", "Ratio to Published", "", 0, 0, 0, 1.0, 0.7, SizeMult[0], 0.08, 0.08, 0.08, 0.08, 0.005);
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
  std::cout << "\n************* Comparison Finished Successfully! *************\n";
  std::cout << "\x1B[0m"; // Reset text color

}