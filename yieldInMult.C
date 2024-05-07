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

void yieldInMult(const Int_t nParticle = 2, // 0-2 : xi, 3-5 : omega
                 const Int_t inel = 0, // inel > N (0/1)
                 const TString workingDir = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM",
                 const TString postFix = "")
{
  //gROOT->SetBatch(kTRUE);
  // Start of Code
  std::cout << "\x1B[1;33m"; // Set text color to yellow
  std::cout << "\n************ Starting Making Spectra in Multiplicity Classes Plot ************\n";
  std::cout << "\x1B[0m"; // Reset text color

  gStyle->SetOptStat(0);

  TString outputfilePath = workingDir + "/yieldsInMult/" + "yieldInMult_" + particleNames[nParticle] + "_inel" + inel + postFix + ".root";
  TFile *outputfile = new TFile(outputfilePath, "RECREATE");

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
  TH1F* hYieldsRatio[numMult + 1];
  TH1F* hYieldsDenom;
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
    hYieldsRatio[iFile] = (TH1F *)hYield[iFile]->Clone("YieldClone_" + particleNames[nParticle]);
    if (iFile == 0) {
      hYieldsDenom = (TH1F *)hYield[iFile]->Clone("YieldCloneForDenom_" + particleNames[nParticle]);
    }

    hYieldsRatio[iFile]->Divide(hYieldsDenom);
    if (iFile != 0) {
      ErrRatioCorr(hYield[iFile], hYieldsDenom, hYieldsRatio[iFile], 0);
    }
  }

  // Scale yields for a better separation on the plot
  TString sScaleFactor[numMult + 1];
  hYield[0]->Scale(ScaleFactorMB);
  sScaleFactor[0] = Form(" (x2^{%i})", int(log2(ScaleFactorMB)));
  for (Int_t iFile = 1; iFile < numMult + 1; iFile++) {
    hYield[iFile]->Scale(ScaleFactor[iFile-1]);
    if (int(log2(ScaleFactor[iFile-1])) == 0) {
      sScaleFactor[iFile] = "";
    } else if (int(log2(ScaleFactor[iFile-1])) == 1) {
      sScaleFactor[iFile] = " (x2)";
    } else {
      sScaleFactor[iFile] = Form(" (x2^{%i})", int(log2(ScaleFactor[iFile-1])));
    }
  }

  // Yield canvas
  TCanvas* canvasYield = new TCanvas("yieldSummary_" + particleNames[nParticle], "yieldSummary_" + particleNames[nParticle], 0, 70, 620, 850);
  StyleCanvas(canvasYield, 0.15, 0.05, 0.05, 0.15);
  // Yield Pads
  TPad *padYieldUp = new TPad("padYieldUp_" + particleNames[nParticle], "padYieldUp_" + particleNames[nParticle], 0, 0.36, 1, 1);
  TPad *padYieldLow = new TPad("padYieldLow_" + particleNames[nParticle], "padYieldLow_" + particleNames[nParticle], 0, 0, 1, 0.36);
  StylePad(padYieldUp, 0.15, 0.05, 0.05, 0.);
  StylePad(padYieldLow, 0.15, 0.05, 0.02, 0.2);
  canvasYield->cd();
  padYieldUp->Draw();
  padYieldLow->Draw();

  // Yield Legend
  TLegend *legYield = new TLegend(0.186, 0.0625, 0.893, 0.3125);
  legYield->SetHeader("FT0M Multiplicity Percentile");
  legYield->SetNColumns(3);
  TLegendEntry *legYieldHeader = (TLegendEntry *)legYield->GetListOfPrimitives()->First();
  legYieldHeader->SetTextSize(0.04);
  //legYield->SetTextAlign(13); // adjust text wrt markers
  StyleLegend(legYield, 0.0, 0.0);

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
  
  TString SmoltBis[numMult + 1];
  for (Int_t m = 0; m < numMult + 1; m++) {
    if (m == 0) { 
      SmoltBis[m] += Form("%.1f#minus%.1f%s", 0.0, 100.0, "%") + sScaleFactor[m];
    } else {
      SmoltBis[m] += Form("%.1f#minus%.1f%s", multiplicityPerc[m-1], multiplicityPerc[m], "%") + sScaleFactor[m];
    }
  }

  Double_t yieldYLow[2] = {0.2*1e-9, 0.2*1e-8};
  Double_t yieldYUp[2] = {3*1e4, 3*1e4};

  Int_t partType;
  if (nParticle <= 2) {
    partType = 0; // Xi
  } else {
    partType = 1; // Omega
  }

  // Dummy histograms to extend x-axis
  TH1F *hDummy = new TH1F("hDummy", "hDummy", 10000, 0, 8.5);
  // Up
  for (Int_t i = 1; i <= hDummy->GetNbinsX(); i++)
    hDummy->SetBinContent(i, 1e-12);
  canvasYield->cd();
  padYieldUp->cd();
  StyleHisto(hDummy, yieldYLow[partType], yieldYUp[partType], 1, 1, "#it{p}_{T} (GeV/#it{c})", sdNdPtdY, "", 0, 0, 0, 1.5, 1.0, 0, 0.0, 0.05, 0.0, 0.035, 0.005);
  SetTickLength(hDummy, 0.025, 0.03);
  TAxis *axisYieldDummy = hDummy->GetYaxis();
  axisYieldDummy->ChangeLabel(1, -1, -1, -1, -1, -1, " ");
  hDummy->GetXaxis()->SetRangeUser(0, hYield[0]->GetXaxis()->GetBinUpEdge(hYield[0]->GetNbinsX()) + 0.5);
  hDummy->Draw("same");
  // Low
  TH1F *hDummyLow = new TH1F("hDummyLow", "hDummyLow", 10000, 0, 8.5);
  for (Int_t i = 1; i <= hDummyLow->GetNbinsX(); i++)
    hDummyLow->SetBinContent(i, 1);
  padYieldLow->cd();
  StyleHisto(hDummyLow, 0.09, 11, 1, 1, "#it{p}_{T} (GeV/#it{c})", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, 0, 0.08, 0.08, 0.08, 0.07, 0.01);
  SetTickLength(hDummyLow, 0.025, 0.03);
  hDummyLow->GetXaxis()->SetRangeUser(0, hYieldsRatio[0]->GetXaxis()->GetBinUpEdge(hYieldsRatio[0]->GetNbinsX()) + 0.5);
  hDummyLow->GetYaxis()->CenterTitle();
  hDummyLow->Draw("same");

  // Define colors
  for(int iClass = 0; iClass < numMult + 1; iClass++) { 
    if(iClass==0) {
      color[iClass] = kBlack;
    } else {
      color[iClass] = numMult + FI - iClass;
    }
  }

  // Plotting
  for (Int_t iFile = 0; iFile < numMult + 1; iFile++) {
    // Up
    padYieldUp->cd();
    StyleHisto(hYield[iFile], yieldYLow[partType], yieldYUp[partType], color[iFile], MarkerMult[iFile], "", hYield[iFile]->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[iFile], 0.0, 0.05, 0.0, 0.035, 0.005);
    hYield[iFile]->GetYaxis()->SetMaxDigits(4);
    hYield[iFile]->GetYaxis()->SetDecimals(kTRUE);
    legYield->AddEntry(hYield[iFile], SmoltBis[iFile], "pef");
    hYield[iFile]->Draw("same ex0");
    hYield[iFile]->SetFillStyle(0);
    hYield[iFile]->Draw("same e2"); // e2 - syst
    padYieldUp->SetLogy();

    // Low
    padYieldLow->cd();
    StyleHisto(hYieldsRatio[iFile], 0.1 , 11, color[iFile], MarkerMult[iFile], "#it{p}_{T} (GeV/#it{c})", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, SizeMult[iFile], 0.08, 0.08, 0.08, 0.08, 0.005);
    if (iFile != 0) {
      hYieldsRatio[iFile]->Draw("same ex0");
      hYieldsRatio[iFile]->SetFillStyle(0);
      hYieldsRatio[iFile]->Draw("same e2");
      padYieldLow->SetLogy();
    }
  }

  padYieldUp->cd();
  canvasYield->Draw();
  legendTitle->Draw();
  legYield->Draw();
  gPad->Update();
  canvasYield->Update();

  padYieldLow->cd();
  DrawHorLine(hYieldsRatio[0]->GetXaxis()->GetBinUpEdge(hYieldsRatio[0]->GetNbinsX()) + 0.5, 1.0);
  outputfile->cd();
  canvasYield->Write();


  // End of Code
  std::cout << "\x1B[1;32m"; // Set text color to green
  std::cout << "\n************* Spectra in Multiplicity Classes Plot is Created Successfully! *************\n";
  std::cout << "\x1B[0m"; // Reset text color
}