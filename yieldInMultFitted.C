#include "help.h"
#include "fitting.h"

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

void yieldInMultFitted(const Int_t nParticle = 2, // 0-2 : xi, 3-5 : omega
                       const Int_t inel = 0, // inel > N (0/1)
                       const Int_t typefit = 3, // mT scaling, Boltzmann, Fermi-Dir, Levi, Bose-Einstein
                       const TString workingDir = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM",
                       const TString postFix = "")
{
  // Start of Code
  std::cout << "\x1B[1;33m"; // Set text color to yellow
  std::cout << "\n************ Starting Fitting the Spectra in Multiplicity Classes ************\n";
  std::cout << "\x1B[0m"; // Reset text color

  gStyle->SetOptStat(0);

  TString outputfilePath = workingDir + "/yieldInMultFitted/" + "yieldInMultFitted_" + particleNames[nParticle] + "_inel" + inel + postFix + ".root";
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
  
  // Scale yields for a better separation on the plot
  TString sScaleFactor[numMult + 1];
  sScaleFactor[0] = Form(" (x2^{%i})", int(log2(ScaleFactorMB)));
  for (Int_t iFile = 1; iFile < numMult + 1; iFile++) {
    if (int(log2(ScaleFactor[iFile-1])) == 0) {
      sScaleFactor[iFile] = "";
    } else if (int(log2(ScaleFactor[iFile-1])) == 1) {
      sScaleFactor[iFile] = " (x2)";
    } else {
      sScaleFactor[iFile] = Form(" (x2^{%i})", int(log2(ScaleFactor[iFile-1])));
    } 
  }

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
  StyleHisto(hDummyLow, 0.1, 1.9, 1, 1, "#it{p}_{T} (GeV/#it{c})", "Data/Fit", "", 0, 0, 0, 1.0, 0.7, 0, 0.08, 0.08, 0.08, 0.07, 0.01);
  SetTickLength(hDummyLow, 0.025, 0.03);
  hDummyLow->GetXaxis()->SetRangeUser(0, hYieldsRatio[0]->GetXaxis()->GetBinUpEdge(hYieldsRatio[0]->GetNbinsX()) + 0.5);
  hDummyLow->GetYaxis()->CenterTitle();
  TAxis *axisDummyLow = hDummyLow->GetYaxis();
  axisDummyLow->ChangeLabel(-1, -1, -1, -1, -1, -1, " "); // last 
  axisDummyLow->ChangeLabel(1, -1, -1, -1, -1, -1, " "); // first
  hDummyLow->Draw("same");

  // Define colors
  for(int iClass = 0; iClass < numMult + 1; iClass++) { 
    if(iClass==0) {
      color[iClass] = kBlack;
    } else {
      color[iClass] = numMult + FI - iClass;
    }
  }

  // Setup Fitting
  TString namepwgfunc[numMult + 1];
  // Setup variables/arrays
  TFitResultPtr fFitResultPtr[numMult + 1];
  TF1 *fitFunction[numMult + 1];
  TF1 *fitFunctionScaled[numMult + 1];
  TH1F* hFitsRatio[numMult + 1];
  TH1F* hYieldForStats[numMult + 1] = {0};

  Double_t chi2NDF[numMult + 1] = {0};

  // Fit limits
  Double_t fitLow[numFitFuncs] = {0.6, 0.6, 0.6, 0.6, 0.6, 0.6};
  Double_t fitUp[numFitFuncs] = {2.5, 2.5, 2.2, 4.0, 2.5, 2.5};

  gROOT->SetBatch(kFALSE);
  // Plotting
  for (Int_t iFile = 0; iFile < numMult + 1; iFile++) {
    // Up
    StyleHisto(hYield[iFile], yieldYLow[partType], yieldYUp[partType], color[iFile], MarkerMult[iFile], "", hYield[iFile]->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[iFile], 0.0, 0.05, 0.0, 0.035, 0.005);
    hYield[iFile]->GetYaxis()->SetMaxDigits(4);
    hYield[iFile]->GetYaxis()->SetDecimals(kTRUE);
    legYield->AddEntry(hYield[iFile], SmoltBis[iFile], "pef");

    // Fitting
    namepwgfunc[iFile] = Form("fitpwgfunc_m%i_fit%i", iFile, typefit);
    std::cout << "\n********* Fitting pt spectra with: " << nameFit[typefit] << " *********\n";
    if (iFile > 0) {
      cout << "Mult. class: " << multiplicityPerc[iFile-1] <<  " - " << multiplicityPerc[iFile] << std::endl;
    } else {
      cout << "Minimum Bias" << std::endl;
    }
    std::cout << "\n****** Name of the fitting function: " << namepwgfunc[iFile] << " ******\n";
  
    // Save Yield histogram to make to-fit ratio after fitting
    hFitsRatio[iFile] = (TH1F *)hYield[iFile]->Clone(Form("hFitsRatio_m%i_typefit%i", iFile, typefit));
    // Save Yield histogram to pass it then to YieldMean function
    hYieldForStats[iFile] = (TH1F *)hYield[iFile]->Clone(Form("hYieldForStats_m%i_fit%i", iFile, typefit));
    // Setup fit function
    Double_t specNorm = hYield[iFile]->GetBinContent(hYield[iFile]->GetMaximumBin());
    switch (typefit) {
      case 0:
        fitFunction[iFile] = GetMTExpdNdptTimesPt(pdgMass[partType], 0.1, specNorm, namepwgfunc[iFile]); // mass, T, norm, name
        break;
      case 1:
        fitFunction[iFile] = GetBoltzmanndNdptTimesPt(pdgMass[partType], 0.1, specNorm, namepwgfunc[iFile]);
        break;
      case 2:
        fitFunction[iFile] = GetFermiDiracdNdptTimesPt(pdgMass[partType], 0.1, specNorm, namepwgfunc[iFile]);
        break;
      case 3:
        fitFunction[iFile] = GetLevidNdptTimesPt(pdgMass[partType], 0.1, 4, specNorm, namepwgfunc[iFile]); // m, T, n, norm
        fitFunction[iFile]->SetParLimits(0, 0, specNorm * 10); // norm
        fitFunction[iFile]->SetParLimits(1, 2, 30); // n
        fitFunction[iFile]->SetParLimits(2, 0.01, 10); // T (GeV)
        fitFunction[iFile]->SetParameter(2, 0.5); // T
        break;
      case 4:
        fitFunction[iFile] = GetBoseEinsteindNdptTimesPt(pdgMass[partType], 0.1, specNorm, namepwgfunc[iFile]); // mass, T, norm, name
        break;
      // case 5:
      //   fitFunction[iFile] = GetBGBWdNdptTimesPt(pdgMass[partType], 0.5, 0.1, 0.03, 0.04 * factor, namepwgfunc[iFile]); // doesn't work
      //   break;
      default:
        std::cerr << "Fit type is not implemented" << std::endl;
        break;
    }

    // Fit spectrum in range
    fitFunction[iFile]->SetRange(fitLow[typefit], fitUp[typefit]); // Fit Range
    fFitResultPtr[iFile] = hYield[iFile]->Fit(fitFunction[iFile], "SR0IQ");
    switch (typefit) {
      case 1:
      case 2:
      case 4:
        std::cout << "norm: " << fitFunction[iFile]->GetParameter(0) << std::endl;
        std::cout << "T: " << fitFunction[iFile]->GetParameter(1) << std::endl;
        break;
      case 3:
        std::cout << "norm: " << fitFunction[iFile]->GetParameter(0) << std::endl;
        std::cout << "n: " << fitFunction[iFile]->GetParameter(1) << std::endl;
        std::cout << "T: " << fitFunction[iFile]->GetParameter(2) << std::endl;
        std::cout << "mass: " << fitFunction[iFile]->GetParameter(3) << std::endl;
        break;
      default:
        std::cerr << "Fit type is not implemented" << std::endl;
        break;
    }

    chi2NDF[iFile] = fitFunction[iFile]->GetChisquare() / fitFunction[iFile]->GetNDF();
    std::cout << "chi/ndf: " << chi2NDF[iFile] << std::endl;

    // Scaling
    fitFunctionScaled[iFile] = (TF1 *)fitFunction[iFile]->Clone(namepwgfunc[iFile] + "_Scaled");
    fitFunctionScaled[iFile]->SetLineColor(color[iFile]);
    fitFunctionScaled[iFile]->SetLineWidth(2);
    fitFunctionScaled[iFile]->SetLineStyle(7);
    fitFunctionScaled[iFile]->SetRange(0., fitUp[typefit]); // Display range
    if (iFile == 0) {
      hYield[0]->Scale(ScaleFactorMB);
      fitFunctionScaled[0]->SetParameter(0, fitFunction[0]->GetParameter(0) * ScaleFactorMB);
    } else {
      hYield[iFile]->Scale(ScaleFactor[iFile-1]);
      fitFunctionScaled[iFile]->SetParameter(0, fitFunction[iFile]->GetParameter(0) * ScaleFactor[iFile-1]);
    }

    hYield[iFile]->SetFillStyle(0);
    padYieldUp->cd();
    hYield[iFile]->Draw("same ex0");
    hYield[iFile]->Draw("same e2");
    fitFunctionScaled[iFile]->Draw("same");
    padYieldUp->SetLogy();

    // Low
    // data yield/fit yield (ratio of integrated yield bin by bin)
    for (Int_t b = 1; b <= hFitsRatio[iFile]->GetNbinsX(); b++)
    {
      hFitsRatio[iFile]->SetBinContent(b, hFitsRatio[iFile]->GetBinContent(b) * hFitsRatio[iFile]->GetBinWidth(b) / fitFunction[iFile]->Integral(hFitsRatio[iFile]->GetXaxis()->GetBinLowEdge(b), hFitsRatio[iFile]->GetXaxis()->GetBinUpEdge(b)));
      hFitsRatio[iFile]->SetBinError(b, hFitsRatio[iFile]->GetBinError(b) * hFitsRatio[iFile]->GetBinWidth(b) / fitFunction[iFile]->Integral(hFitsRatio[iFile]->GetXaxis()->GetBinLowEdge(b), hFitsRatio[iFile]->GetXaxis()->GetBinUpEdge(b)));
    }
    StyleHisto(hFitsRatio[iFile], 0.1 , 1.9, color[iFile], MarkerMult[iFile], "#it{p}_{T} (GeV/#it{c})", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, SizeMult[iFile], 0.08, 0.08, 0.08, 0.08, 0.005);
    hFitsRatio[iFile]->SetFillStyle(0);
    padYieldLow->cd();
    hFitsRatio[iFile]->Draw("same e0x0");
  }

  padYieldUp->cd();
  canvasYield->Draw();
  legendTitle->Draw();

  TLegend *legendFit = new TLegend(0.19, 0.297, 0.383, 0.398);
  legendFit->SetFillStyle(0);
  legendFit->SetTextSize(0.04);
  legendFit->AddEntry(fitFunctionScaled[0], nameFit[typefit] + " fit", "l");
  legendFit->Draw();

  legYield->Draw();
  gPad->Update();
  canvasYield->Update();

  padYieldLow->cd();
  DrawHorLine(hFitsRatio[0]->GetXaxis()->GetBinUpEdge(hFitsRatio[0]->GetNbinsX()) + 0.5, 1.0);
  outputfile->cd();
  canvasYield->Write();

  // Analysis Of Integrated Yields
  TH1 *hhout[numMult + 1];
  // TString Titlehhout[9] = {"kYield",
  //                          "kYieldStat",
  //                          "kYieldSysHi",
  //                          "kYieldSysLo",
  //                          "kMean",
  //                          "kMeanStat",
  //                          "kMeanSysHi",
  //                          "kMeanSysLo",
  //                          "kExtra"};

  // Double_t YieldExtr[numMult + 1] = {0};
  // Double_t YieldErrStat[numMult + 1] = {0};
  // Double_t YieldErrSistHi[numMult + 1] = {0};
  // Double_t YieldErrSistLow[numMult + 1] = {0};

  // Double_t Mean[numMult + 1] = {0};
  // Double_t MeanErrStat[numMult + 1] = {0};
  // Double_t MeanErrSistHi[numMult + 1] = {0};
  // Double_t MeanErrSistLow[numMult + 1] = {0};

  // Double_t Temp[numMult + 1] = {0};
  // Double_t TempError[numMult + 1] = {0};

  gROOT->SetBatch(kTRUE);
  for (Int_t iFile = 0; iFile < numMult + 1; iFile++) {
    cout << "\n*** Calling YieldMean macro ***\n";
    hhout[iFile] = YieldMean(hYieldForStats[iFile], hYieldForStats[iFile], fitFunction[iFile], 0, 20, 0.01, 0.1, "0qI", "logs/yieldMean-log.root", fitLow[typefit], fitUp[typefit]);
    cout << "\n*** End of call ***\n";

    // hhout[iFile]->SetLineColor(ColorFit[typefit]);
    // hhout[iFile]->SetName("hhout_" + nameFit[typefit] + Form("_m%i", iFile));
    // hhout[iFile]->GetYaxis()->SetRangeUser(0, 2);
    // for (Int_t b = 1; b <= hhout[iFile]->GetNbinsX(); b++)
    // {
    //   hhout[iFile]->GetXaxis()->SetBinLabel(b, Titlehhout[b - 1]);
    // }

    // cout << "iFile " << iFile << " typefit " << typefit << endl;
    // Yield[iFile] = hhout[iFile]->GetBinContent(1);           // yield (spectra + extrapolated one)
    // YieldExtr[iFile] = hhout[iFile]->GetBinContent(9);       // extrapolated yield
    // YieldErrStat[iFile] = hhout[iFile]->GetBinContent(2);    // stat error
    // YieldErrSistHi[iFile] = hhout[iFile]->GetBinContent(3);  // syst error
    // YieldErrSistLow[iFile] = hhout[iFile]->GetBinContent(4); // syst error
    // Mean[iFile] = hhout[iFile]->GetBinContent(5);            // mean pt
    // MeanErrStat[iFile] = hhout[iFile]->GetBinContent(6);     // stat error
    // MeanErrSistHi[iFile] = hhout[iFile]->GetBinContent(7);   // syst error
    // MeanErrSistLow[iFile] = hhout[iFile]->GetBinContent(8);  // syst error
    // cout << "************************************" << endl;
    // cout << "Multiplicity class: " << SmoltBis[iFile] << endl;
    // cout << "Yield: " << Yield[iFile] << " +- " << YieldErrStat[iFile] << " (stat.) +" << YieldErrSistHi[iFile] << " - " << YieldErrSistLow[iFile] << " (syst.) " << endl;
    // cout << "Mean: " << Mean[iFile] << " +- " << MeanErrStat[iFile] << endl;

    // if (typefit == 3)
    // {
    //   Temp[iFile] = fitFunction[iFile]->GetParameter(2);
    //   TempError[iFile] = fitFunction[iFile]->GetParError(2);
    // }
    // else
    // {
    //   Temp[iFile] = fitFunction[iFile]->GetParameter(1);
    //   TempError[iFile] = fitFunction[iFile]->GetParError(1);
    // }
  }
  gROOT->SetBatch(kFALSE);

  // End of Code
  std::cout << "\x1B[1;32m"; // Set text color to green
  std::cout << "\n************* Spectra in Multiplicity Classes is Fitted! *************\n";
  std::cout << "\x1B[0m"; // Reset text color
}
