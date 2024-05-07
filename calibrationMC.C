#include "help.h"
#include "effHelp.h"

void calibrationMC(const TString fileMC = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/25apr_lhc24b1/AnalysisResults.root",
                   const TString fileData = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/22apr_lhc22o_pass6_medium/AnalysisResults.root",
                   const TString outputDir = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM",
                   const TString postFix = "_LHC24b1"){ // postfix to the output file
  // Start of Code
  std::cout << "\x1B[1;33m"; // Set text color to yellow
  std::cout << "\n************ MC Calibration in Classes ************\n";
  std::cout << "\x1B[0m"; // Reset text color

  // Set Style
  //gStyle->SetOptStat(0);
  gStyle->SetStatX(0.9);
  gStyle->SetStatY(0.9);
  //gROOT->SetBatch(kTRUE);
  gStyle->SetOptStat("me");

  // File with calibration histograms (cascqaanalysis output of MC)
  TFile* fileMCin = TFile::Open(fileMC);
  if (!fileMCin || fileMCin->IsZombie()) {
      std::cerr << "Error opening `fileMC` data file!" << std::endl;
      return;
  }

  // File with calibration histograms (cascqaanalysis output of DATA)
  TFile* fileDatain = TFile::Open(fileData);
  if (!fileDatain || fileDatain->IsZombie()) {
      std::cerr << "Error opening `fileData` data file!" << std::endl;
      return;
  }

  // Output File
  TString outputfilePath = outputDir + "/efficiencies/" + "effCalibration" + postFix + ".root";
  TFile *outputfile = new TFile(outputfilePath, "RECREATE");

  TDirectory* dirCalibHistos = outputfile->mkdir("calibrationHistos");
  dirCalibHistos->cd();

  Int_t nNtracksLimit = 220;
  Int_t nNchLimit = 220;

  // hNchFT0MPVContr MC histogram
  TH3F* hNchFT0MPVContr = (TH3F*)fileMCin->Get("lf-cascqaanalysis/hNchFT0MPVContr");
  TH3F* hNchFT0MPVContrClone = (TH3F*)hNchFT0MPVContr->Clone("hNchFT0MPVContrClone");
  hNchFT0MPVContrClone->GetZaxis()->SetRange(2, 3); // INEL>0

  TH2F* hProjPVContrNchFT0M = static_cast<TH2F*>(hNchFT0MPVContrClone->Project3D("xy"));
  TH1D* hProjPVContrMC = (hProjPVContrNchFT0M->ProjectionX());
  Double_t meanPVcontrMC = hProjPVContrMC->GetMean();
  std::cout << "\n********* mean number of PV contributors in MC: " << meanPVcontrMC << " *********\n";
  hProjPVContrNchFT0M->SetTitle("");
  hProjPVContrNchFT0M->GetXaxis()->SetRangeUser(0., nNtracksLimit); // Set Ntracks axis range
  hProjPVContrNchFT0M->GetYaxis()->SetRangeUser(0., nNchLimit); // Set Nch in FT0M region axis range
  hProjPVContrNchFT0M->Write();

  // hFT0MpvContr DATA histogram
  TH3F* hFT0MPVContr = (TH3F*)fileDatain->Get("lf-cascqaanalysis/hFT0MpvContr");
  TH3F* hFT0MPVContrClone = (TH3F*)hFT0MPVContr->Clone("hFT0MPVContrClone");
  hFT0MPVContrClone->GetZaxis()->SetRange(2, 3); // INEL>0
  TH2F* hProjPVContrFT0Mperc = static_cast<TH2F*>(hFT0MPVContrClone->Project3D("xy"));
  TH1D* hProjPVContrData = (hProjPVContrFT0Mperc->ProjectionX());
  Double_t meanPVcontrDATA = hProjPVContrData->GetMean();
  std::cout << "********* mean number of PV contributors in DATA: " << meanPVcontrDATA << " *********\n";
  hProjPVContrFT0Mperc->GetXaxis()->SetRangeUser(0., nNtracksLimit); // Set Ntracks axis range
  hProjPVContrFT0Mperc->GetYaxis()->SetRangeUser(0., 105.5); // Set FT0M axis range
  hProjPVContrFT0Mperc->GetYaxis()->SetTitle("FT0M Multiplicity percentile");
  hProjPVContrFT0Mperc->SetTitle("");
  hProjPVContrFT0Mperc->Write();

  // Just for QC, check how mean of PV contributors depends on Ngen in FT0M //
  TH2F* hProjPVContrNchFT0MClone = (TH2F*)hProjPVContrNchFT0M->Clone("hProjPVContrNchFT0MClone");
  TProfile *hProfileNFT0M = hProjPVContrNchFT0MClone->ProfileY("ProfileNgen", 1, hProjPVContrNchFT0MClone->GetXaxis()->GetNbins());
  hProfileNFT0M->SetName("TProfile_plot_for_NgenFT0M");
  hProfileNFT0M->GetYaxis()->SetTitle("<#it{N}_{tracks}>");
  hProfileNFT0M->Write();
  // Just for QC, check number of counts for each Ngen in FT0M bin //
  TH2F* hProjPVContrNchFT0MClone2 = (TH2F*)hProjPVContrNchFT0M->Clone("hProjPVContrNchFT0MClone2");
  TH1D* hProjNFT0M = (hProjPVContrNchFT0MClone2->ProjectionY());
  hProjNFT0M->GetYaxis()->SetTitle("Counts");
  hProjNFT0M->SetTitle("");
  hProjNFT0M->Write();

  std::cout << "\n****** Try to estimate the best mult. % division for MC to have a flat distribution (just for QC) *********\n";
  // Just for QC, try to evaluate best mult. % division for MC to have a flat distribution //
  std::cout << "****** Total number of events in GP MC: " << hProjNFT0M->Integral() << " *********\n";
  Double_t evIn1Perc = hProjNFT0M->Integral()/100.0;
  Double_t classWidth = 0;
  long int numEventsFLAT = 0;
  Int_t classCount = 0;
  do {
    classCount = 0;
    classWidth += 1;
    std::cout << "****** Iteration number " << classWidth << " *********\n";
    std::cout << "* Should be in " << classWidth << " % wide mult. class (GP MC): " << classWidth * evIn1Perc << " *\n";
    for (Int_t bin = 1; bin <= hProjNFT0M->GetNbinsX(); ++bin) {
      numEventsFLAT += hProjNFT0M->GetBinContent(bin);
      if(numEventsFLAT >= classWidth*evIn1Perc - evIn1Perc/3.){
        std::cout << "NumEvents: " << numEventsFLAT << " Ngen: " << hProjNFT0M->GetXaxis()->GetBinUpEdge(bin) << " Class: " << 100. - (classCount+1)*classWidth << " -- " << 100. - classCount*classWidth << std::endl;
        classCount++;
        numEventsFLAT = 0;
      }
    }

  } while (classCount != 100/int(classWidth));
  std::cout << "\n****** Class width to make calibration flat using GP MC is: " << classWidth << " %" <<  " *********\n";

  // Compare f(Ntracks) distributions in data and GP MC //
  TCanvas* canvasPVContrComp = new TCanvas("PVContrComp", "PVContrComp", 0, 70, 620, 850);
  StyleCanvas(canvasPVContrComp, 0.15, 0.05, 0.05, 0.15);
  TPad *padPVContrCompUp = new TPad("padPVContrCompUp", "padPVContrCompUp", 0, 0.36, 1, 1);
  TPad *padPVContrCompLow = new TPad("padPVContrCompLow", "padPVContrCompLow", 0, 0, 1, 0.36);
  StylePad(padPVContrCompUp, 0.15, 0.05, 0.05, 0.);
  StylePad(padPVContrCompLow, 0.15, 0.05, 0.02, 0.2);
  canvasPVContrComp->cd();
  padPVContrCompUp->Draw();
  padPVContrCompLow->Draw();

  // Dummy histograms
  TH1F *hDummyPVContrComp = new TH1F("hDummyPVContrComp", "hDummyPVContrComp", 10000, 0, nNtracksLimit);
  Int_t nNtracksLimitOffset = 100;
  // Up
  for (Int_t i = 1; i <= hDummyPVContrComp->GetNbinsX(); i++)
    hDummyPVContrComp->SetBinContent(i, 1e-12);
  canvasPVContrComp->cd();
  padPVContrCompUp->cd();
  StyleHistoMultiPlot(hDummyPVContrComp, 1e-6, 2, 1, 1, "#it{N}_{tracks}", "f(#it{N}_{tracks})", "", 0, 0, 0, 1.5, 1.0, 0, 0.0, 0.05, 0.0, 0.035, 0.005);
  SetTickLength(hDummyPVContrComp, 0.025, 0.03);
  TAxis *axisPVContrCompDummy = hDummyPVContrComp->GetYaxis();
  axisPVContrCompDummy->ChangeLabel(1, -1, -1, -1, -1, -1, " ");
  hDummyPVContrComp->GetXaxis()->SetRangeUser(0, nNtracksLimit - nNtracksLimitOffset);
  padPVContrCompUp->SetLogy();
  hDummyPVContrComp->Draw("same");
  // Low
  TH1F *hDummyPVContrLow = new TH1F("hDummyPVContrLow", "hDummyPVContrLow", 10000, 0, nNtracksLimit);
  for (Int_t i = 1; i <= hDummyPVContrLow->GetNbinsX(); i++)
    hDummyPVContrLow->SetBinContent(i, 1);
  padPVContrCompLow->cd();
  StyleHistoMultiPlot(hDummyPVContrLow, 0.09, 11, 1, 1, "#it{N}_{tracks}", "Data/MC", "", 0, 0, 0, 1.0, 0.7, 0, 0.08, 0.08, 0.08, 0.07, 0.01);
  SetTickLength(hDummyPVContrLow, 0.025, 0.03);
  TAxis *axisDummyPVContrLow = hDummyPVContrLow->GetYaxis();
  axisDummyPVContrLow->ChangeLabel(-1, -1, -1, -1, -1, -1, " "); // last 
  //axisDummyPVContrLow->ChangeLabel(1, -1, -1, -1, -1, -1, " "); // first
  hDummyPVContrLow->GetXaxis()->SetRangeUser(0, nNtracksLimit - nNtracksLimitOffset);
  hDummyPVContrLow->GetYaxis()->CenterTitle();
  hDummyPVContrLow->Draw("same");

  // Plotting
  padPVContrCompUp->cd();
  Int_t rebinFactor = 4;
  TH2F* hProjPVContrNchFT0MClone3 = (TH2F*)hProjPVContrNchFT0M->Clone("NtracksProjMC");
  TH1D* hProjPVContrMC2 = hProjPVContrNchFT0MClone3->ProjectionX();
  hProjPVContrMC2->Rebin(rebinFactor);
  Int_t markTypeMC = 3;
  hProjPVContrMC2->SetTitle("");
  hProjPVContrMC2->SetMarkerColor(color[markTypeMC]);
  hProjPVContrMC2->SetLineColor(color[markTypeMC]);
  hProjPVContrMC2->SetMarkerStyle(MarkerMult[markTypeMC]);
  hProjPVContrMC2->SetMarkerSize(SizeMult[markTypeMC]);
  hProjPVContrMC2->Scale(1.0/hProjPVContrMC2->Integral());
  hProjPVContrMC2->GetXaxis()->SetRangeUser(0, nNtracksLimit); // Ntracks axis range
  hProjPVContrMC2->GetYaxis()->SetRangeUser(1e-8, hProjPVContrMC2->GetMaximum() * 10); // f(Ntracks)
  hProjPVContrMC2->GetYaxis()->SetTitle("f(N_{tracks})");
  hProjPVContrMC2->Draw("same");

  TH2F* hProjPVContrFT0MpercClone = (TH2F*)hProjPVContrFT0Mperc->Clone("NtracksProjData");
  TH1D* hProjPVContrData2 = hProjPVContrFT0MpercClone->ProjectionX();
  hProjPVContrData2->Rebin(rebinFactor);
  Int_t markTypeData = 0;
  hProjPVContrData2->SetMarkerColor(color[markTypeData]);
  hProjPVContrData2->SetLineColor(color[markTypeData]);
  hProjPVContrData2->SetMarkerStyle(MarkerMult[markTypeData]);
  hProjPVContrData2->SetMarkerSize(SizeMult[4]);
  hProjPVContrData2->Scale(1.0/hProjPVContrData2->Integral());
  hProjPVContrData2->GetXaxis()->SetRangeUser(0, nNtracksLimit); // Set Ntracks axis range
  hProjPVContrData2->Draw("same");

  TLegend *legPVContrComp = new TLegend(0.42, 0.63, 0.94, 0.88);
  legPVContrComp->SetHeader("Comparison of N_{tracks} distributions");
  legPVContrComp->SetFillStyle(0);
  legPVContrComp->AddEntry(hProjPVContrData2, "Data", "p");
  legPVContrComp->AddEntry(hProjPVContrMC2, "MC", "p");
  StyleLegend(legPVContrComp, 0.0, 0.0);
  legPVContrComp->SetTextSize(0.04);
  legPVContrComp->Draw("same");

  padPVContrCompLow->cd();
  TH1D* hPVContrRatio = (TH1D*)hProjPVContrData2->Clone("PVContrData");
  hPVContrRatio->Divide(hProjPVContrMC2);
  StyleHistoMultiPlot(hPVContrRatio, 0.1 , 11, color[markTypeData], MarkerMult[markTypeData], "#it{N}_{tracks}", "Data/MC", "", 0, 0, 0, 1.0, 0.7, 0.7, 0.08, 0.08, 0.08, 0.07, 0.01);
  hPVContrRatio->SetStats(0);
  hPVContrRatio->Draw("same");

  canvasPVContrComp->Write();

  // Get <Ntracks> in FT0M intervals in Data //
  std::cout << "\n****** Compute <Ntracks> in FT0M intervals in Data *********\n";
  Double_t errMultInterval[numMult] = {0};
  Double_t errMean[numMult] = {0};
  Double_t meanYData[numMult] = {0};
  Double_t multiplicityPercMean[numMult] = {0};

  TDirectory* dirFT0Mintervals = outputfile->mkdir("meanInFT0MpercDATA");
  dirFT0Mintervals->cd();

  TH2F* hProjPVContrFT0MpercLoop[numMult];
  for(Int_t i = 0; i < numMult; i++){
    hProjPVContrFT0MpercLoop[i] = (TH2F*)hProjPVContrFT0Mperc->Clone(Form("hProjPVContrFT0MpercClone%d", i));
    hProjPVContrFT0MpercLoop[i]->GetYaxis()->SetRangeUser(multiplicityPerc[i], multiplicityPerc[i+1]); // Set FT0M interval
    TH1D* hProjPVContrLoopData = (hProjPVContrFT0MpercLoop[i]->ProjectionX());
    hProjPVContrLoopData->Sumw2();

    Int_t binYMin = hProjPVContrFT0Mperc->GetYaxis()->FindBin(multiplicityPerc[i] + 1e-3);
    Int_t binYMax = hProjPVContrFT0Mperc->GetYaxis()->FindBin(multiplicityPerc[i+1] - 1e-3);

    Double_t sumX = 0.0;
    Double_t numCountsInRange = 0;

    for (Int_t binY = binYMin; binY <= binYMax; ++binY) {
        for (Int_t binX = 1; binX <= hProjPVContrFT0Mperc->GetNbinsX(); ++binX) {
            Double_t xValue = hProjPVContrFT0Mperc->GetXaxis()->GetBinCenter(binX); // number of PV Contr
            Double_t binContent = hProjPVContrFT0Mperc->GetBinContent(binX, binY); // number of counts
            if(binContent != 0){
              sumX += binContent * xValue;
              numCountsInRange += binContent;
            }
        }
    }

    Double_t averageX = (numCountsInRange > 0) ? sumX / numCountsInRange : 0.0;

    //std::cout << "binYmin: " << binYMin << " mult. %: " << multiplicityPerc[i] << " binYMax: " << binYMax << " mult. %: " << multiplicityPerc[i+1] << " i = " << i << " average X: " << averageX << " sumx: " << sumX << " numCountsInRange: " << numCountsInRange << std::endl;

    meanYData[i] = averageX;
    errMean[i] = 0;
    multiplicityPercMean[i] = (multiplicityPerc[i] + multiplicityPerc[i+1])/2.;
    errMultInterval[i] = (multiplicityPerc[i+1] - multiplicityPerc[i])/2.;
    hProjPVContrLoopData->Write();
  }
  gROOT->SetBatch(kTRUE);
  TGraphErrors *gMeanYData = new TGraphErrors(numMult, multiplicityPercMean, meanYData, errMultInterval, errMean);
  gMeanYData->SetTitle("");
  gMeanYData->GetXaxis()->SetTitle("FT0M Multiplicity percentile");
  gMeanYData->GetXaxis()->SetRangeUser(-0.1, 100.1);
  gMeanYData->GetYaxis()->SetTitle("#it{N}_{tracks}");
  gMeanYData->SetMarkerStyle(MarkerMult[markTypeData]);
  gMeanYData->SetMarkerColor(color[markTypeData]);

  TCanvas *canvasMean = new TCanvas("MeanYFT0M","MeanYFT0M", 800, 600);
  StyleCanvas(canvasMean, 0.14, 0.05, 0.11, 0.15);
  canvasMean->cd();

  gMeanYData->Draw("AP");
  canvasMean->Update();
  canvasMean->Write();

  gROOT->SetBatch(kFALSE);

  TDirectory* dirCalibration = outputfile->mkdir("MCcalibration");
  dirCalibration->cd();

  // Perform calibration //
  std::cout << "\n****** Perform Calibration for Nch in FT0M region intervals (in MC) *********\n";
  Double_t meanYMC[numMult] = {0};
  Double_t NChFT0MCalib[numMult + 1] = {0};
  NChFT0MCalib[numMult] = 0;

  TH2F* hProjPVContrNchFT0MLoop[numMult][nNchLimit];
  Double_t lowNch = 0;
  Double_t eps = 0.6;
  for(Int_t i = 0; i < numMult; i++){
    (i == 0) ? eps = 1.6 : eps = 0.6;
    for(Int_t j = lowNch + 1; j < nNchLimit; j++){
      (i == (numMult - 1)) ? (j = nNchLimit, eps = 1e2) : j;
      hProjPVContrNchFT0MLoop[i][j] = (TH2F*)hProjPVContrNchFT0M->Clone(Form("hProjPVContrNchFT0MClone%d_%d", i, j));
      hProjPVContrNchFT0MLoop[i][j]->GetYaxis()->SetRangeUser(lowNch, j); // Set Nch in FT0M region interval
      TH1D* hProjPVContrLoopMC = (hProjPVContrNchFT0MLoop[i][j]->ProjectionX());
      hProjPVContrLoopMC->Sumw2();

      Int_t binYMin = hProjPVContrNchFT0M->GetYaxis()->FindBin(lowNch + 1e-3);
      Int_t binYMax = hProjPVContrNchFT0M->GetYaxis()->FindBin(j - 1e-3);

      Double_t sumX = 0.0;
      Double_t numCountsInRange = 0;

      for (Int_t binY = binYMin; binY <= binYMax; ++binY) {
          for (Int_t binX = 1; binX <= hProjPVContrNchFT0M->GetNbinsX(); ++binX) {
              Double_t xValue = hProjPVContrNchFT0M->GetXaxis()->GetBinCenter(binX); // number of PV Contr
              Double_t binContent = hProjPVContrNchFT0M->GetBinContent(binX, binY); // number of counts
              if(binContent != 0){
                sumX += binContent * xValue;
                numCountsInRange += binContent;
              }
          }
      }

      Double_t averageX = (numCountsInRange > 0) ? sumX / numCountsInRange : 0.0;

      hProjPVContrLoopMC->Write();

      //std::cout << "binYmin: " << binYMin << " mult. %: " << multiplicityPerc[numMult - i - 1] << " binYMax: " << binYMax << " mult. %: " << multiplicityPerc[numMult - i] << " i = " << i << " average X: " << averageX << " sumx: " << sumX << " numCountsInRange: " << numCountsInRange << std::endl;

      if(TMath::Abs(averageX - meanYData[numMult - i - 1]) < eps) {
        std::cout << "averageX: " << averageX << " meanY in data: " << meanYData[numMult - i - 1] << " nEvents/ClassWidth: " << numCountsInRange/(multiplicityPerc[numMult - i] - multiplicityPerc[numMult - i - 1]) << " mult. %: " << multiplicityPerc[numMult - i - 1] << " -- " << multiplicityPerc[numMult - i] << std::endl;
        //std::cout << "NchFT0M: " << j << std::endl;
        lowNch = j;
        meanYMC[numMult - i - 1] = averageX;
        NChFT0MCalib[numMult - i - 1] = j;
        break;
      }
    }
  }

  std::cout << "\n****** Calibrated NchFT0M Borders *********\n";
  for (Int_t i = 0; i < numMult + 1; i++) {
    std::cout << "mult. %: " << multiplicityPerc[i] << ", NchFT0M: " << NChFT0MCalib[i] << std::endl;
  }

  TGraphErrors *gMeanYMC = new TGraphErrors(numMult, multiplicityPercMean, meanYMC, errMultInterval, errMean);
  gMeanYMC->SetMarkerStyle(20);
  gMeanYMC->SetTitle("MC");
  gMeanYMC->SetMarkerStyle(MarkerMult[markTypeMC]);
  gMeanYMC->SetMarkerColor(color[markTypeMC]);

  TCanvas *canvasMeanFinal = new TCanvas("canvasMeanFinal","canvasMeanFinal",800,600);
  StyleCanvas(canvasMeanFinal, 0.14, 0.05, 0.11, 0.15);
  canvasMeanFinal->cd();

  auto mg = new TMultiGraph();
  mg->Add(gMeanYData);
  mg->Add(gMeanYMC);
  mg->GetXaxis()->SetTitle("FT0M Multiplicity percentile");
  mg->GetYaxis()->SetTitle("<#it{N}_{tracks}>");
  mg->Draw("AP");

  TLegend *legendMeanFinal = new TLegend(0.47, 0.61, 0.98, 0.864);
  legendMeanFinal->SetHeader("<#it{N}_{tracks}> comparison");
  legendMeanFinal->SetFillStyle(0);
  StyleLegend(legendMeanFinal, 0.0, 0.0);
  legendMeanFinal->SetTextSize(0.04);

  legendMeanFinal->AddEntry(gMeanYData, "Data", "p");
  legendMeanFinal->AddEntry(gMeanYMC, "MC", "p");
  legendMeanFinal->Draw("same");

  canvasMeanFinal->Update();
  canvasMeanFinal->Write();

  // Write tree with calibration values to the root file //
  TTree* calibValuesTree = new TTree("calibValuesTree", "CalibrationValues");
  calibValuesTree->Branch("NChFT0MCalib", NChFT0MCalib, Form("NChFT0MCalib[%d]/D", numMult + 1));
  calibValuesTree->Fill();
  calibValuesTree->Write(); // Write the tree to the file

  // N events in mult % classes for calibrated MC //
  TDirectory* dirAfterCalib = outputfile->mkdir("results");
  dirAfterCalib->cd();
  // Reverse array (from 0 to nNchLimit)
  Double_t NChFT0MCalibReversed[numMult + 1] = {0};
  for (Int_t i = 0; i < numMult + 1; i++) {
    NChFT0MCalibReversed[numMult - i] = NChFT0MCalib[i];
  }
  TH2D* hNchFT0MGenEvType = (TH2D*)fileMCin->Get("lf-cascqaanalysis/hNchFT0MGenEvType");
  hNchFT0MGenEvType->Write();
  hNchFT0MGenEvType->GetYaxis()->SetRange(2, 3); // INEL>0
  TH1D* hProjNchFT0MGen = (hNchFT0MGenEvType->ProjectionX());
  hProjNchFT0MGen->Sumw2();
  hProjNchFT0MGen = (TH1D*)hProjNchFT0MGen->Rebin(numMult, "hProjNchFT0MGenRebinned", NChFT0MCalibReversed);
  //hProjNchFT0MGen->Write();
  TH1D* hCalibFT0Mperc = new TH1D("hCalibFT0Mperc", "", numMult, multiplicityPerc);
  for (Int_t bin = 1; bin <= numMult; ++bin) {
    classWidth = multiplicityPerc[numMult - bin + 1]-multiplicityPerc[numMult - bin];
    //std::cout << "width: " << classWidth << std::endl;
    hCalibFT0Mperc->SetBinContent(bin, hProjNchFT0MGen->GetBinContent(bin)/classWidth);
  }
  StyleHisto(hCalibFT0Mperc, 0, 1.2 * hCalibFT0Mperc->GetBinContent(hCalibFT0Mperc->GetMaximumBin()), 1, 1, "FT0M Multiplicity percentile", "Counts/Class Width", "", 0, 0, 0, 1.0, 1.25, 1, 0.04, 0.04);

  TLegend *LegendCalibratedFT0M = new TLegend(0.071, 0.624, 0.501, 0.824);
  LegendCalibratedFT0M->SetFillStyle(0);
  LegendCalibratedFT0M->SetTextAlign(33);
  LegendCalibratedFT0M->SetTextSize(0.04);
  LegendCalibratedFT0M->SetTextFont(42);
  LegendCalibratedFT0M->SetLineColorAlpha(0.,0.);
  LegendCalibratedFT0M->SetFillColorAlpha(0.,0.);
  LegendCalibratedFT0M->SetBorderSize(0.);
  LegendCalibratedFT0M->AddEntry("", "#bf{ALICE Work In Progress}", "");
  LegendCalibratedFT0M->AddEntry("", "pp, #sqrt{#it{s}} = 13.6 TeV", "");
  LegendCalibratedFT0M->AddEntry("", "LHC24b1, INEL > 0", "");

  hCalibFT0Mperc->Draw();
  LegendCalibratedFT0M->Draw("same");
  hCalibFT0Mperc->Write();

  // Ntracks in classes Data//
  nNtracksLimitOffset = 140;
  Int_t rebinFactor2 = 4;
  TString SmoltBis[numMult + 1];
  for (Int_t m = 0; m < numMult + 1; m++) {
    if (m == 0) { 
      SmoltBis[m] += Form("%.1f#minus%.1f%s", 0.0, 100.0, "%");
    } else {
      SmoltBis[m] += Form("%.1f#minus%.1f%s", multiplicityPerc[m-1], multiplicityPerc[m], "%");
    }
  }
  // Legend
  TLegend *legPVContrInClassesData = new TLegend(0.186, 0.0625, 0.893, 0.3125);
  legPVContrInClassesData->SetHeader("FT0M Multiplicity Percentile");
  legPVContrInClassesData->SetNColumns(3);
  TLegendEntry *legPVContrInClassesDataHeader = (TLegendEntry *)legPVContrInClassesData->GetListOfPrimitives()->First();
  legPVContrInClassesDataHeader->SetTextSize(0.04);
  StyleLegend(legPVContrInClassesData, 0.0, 0.0);

  TLegend *legendTitleData = new TLegend(0.609, 0.693913, 0.908, 0.893);
  StyleLegend(legendTitleData, 0.0, 0.0);
  legendTitleData->SetTextAlign(33);
  legendTitleData->SetTextSize(0.05);
  legendTitleData->SetTextFont(42);
  legendTitleData->SetLineColorAlpha(0.,0.);
  legendTitleData->SetFillColorAlpha(0.,0.);
  legendTitleData->AddEntry("", "#bf{ALICE Work In Progress}", "");
  legendTitleData->AddEntry("", "pp, #sqrt{#it{s}} = 13.6 TeV", "");
  legendTitleData->AddEntry("", "LHC22o_pass6_medium", "");

  TCanvas* canvasPVContrInClassesData = new TCanvas("canvasPVContrInClassesData", "canvasPVContrInClassesData", 0, 70, 620, 850);
  StyleCanvas(canvasPVContrInClassesData, 0.15, 0.05, 0.05, 0.15);
  TPad *padPVContrInClassesDataUp = new TPad("padPVContrInClassesDataUp", "padPVContrInClassesDataUp", 0, 0.36, 1, 1);
  TPad *padPVContrInClassesDataLow = new TPad("padPVContrInClassesDataLow", "padPVContrInClassesDataLow", 0, 0, 1, 0.36);
  StylePad(padPVContrInClassesDataUp, 0.15, 0.05, 0.05, 0.);
  StylePad(padPVContrInClassesDataLow, 0.15, 0.05, 0.02, 0.2);
  canvasPVContrInClassesData->cd();
  padPVContrInClassesDataUp->Draw();
  padPVContrInClassesDataLow->Draw();

  // Dummy histograms
  TH1F *hDummyPVContrInClassesData = new TH1F("hDummyPVContrInClassesData", "hDummyPVContrInClassesData", 10000, 0, nNtracksLimit);
  // Up
  TH2F* hProjPVContrFT0MpercMB = (TH2F*)hProjPVContrFT0Mperc->Clone("hProjPVContrFT0MpercInClassesMB");
  TH1D* hProjPVContrDataMB = (hProjPVContrFT0MpercMB->ProjectionX());
  for (Int_t i = 1; i <= hDummyPVContrInClassesData->GetNbinsX(); i++)
    hDummyPVContrInClassesData->SetBinContent(i, 1e-12);
  canvasPVContrInClassesData->cd();
  padPVContrInClassesDataUp->cd();
  StyleHistoMultiPlot(hDummyPVContrInClassesData, 1e-7, 3, 1, 1, "#it{N}_{tracks}", "Normalized Counts", "", 0, 0, 0, 1.5, 1.0, 0, 0.0, 0.05, 0.0, 0.035, 0.005);
  SetTickLength(hDummyPVContrInClassesData, 0.025, 0.03);
  TAxis *axisPVContrInClassesDataDummy = hDummyPVContrInClassesData->GetYaxis();
  axisPVContrInClassesDataDummy->ChangeLabel(1, -1, -1, -1, -1, -1, " ");
  //axisPVContrInClassesDataDummy->ChangeLabel(-1, -1, -1, -1, -1, -1, " ");
  hDummyPVContrInClassesData->GetXaxis()->SetRangeUser(0, nNtracksLimit - nNtracksLimitOffset);
  padPVContrInClassesDataUp->SetLogy();
  hDummyPVContrInClassesData->Draw("same");
  // Low
  TH1F *hDummyPVContrInClassesDataLow = new TH1F("hDummyPVContrInClassesDataLow", "hDummyPVContrInClassesDataLow", 10000, 0, nNtracksLimit);
  for (Int_t i = 1; i <= hDummyPVContrInClassesDataLow->GetNbinsX(); i++)
    hDummyPVContrInClassesDataLow->SetBinContent(i, 1);
  padPVContrInClassesDataLow->cd();
  StyleHistoMultiPlot(hDummyPVContrInClassesDataLow, 1e-3, 3, 1, 1, "#it{N}_{tracks}", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, 0, 0.08, 0.08, 0.08, 0.07, 0.01);
  SetTickLength(hDummyPVContrInClassesDataLow, 0.025, 0.03);
  TAxis *axisDummyPVContrInClassesDataLow = hDummyPVContrInClassesDataLow->GetYaxis();
  //padPVContrInClassesDataLow->SetGridy(); // Only grid on x-axis
  //axisDummyPVContrInClassesDataLow->ChangeLabel(-1, -1, -1, -1, -1, -1, " "); // last 
  axisDummyPVContrInClassesDataLow->ChangeLabel(1, -1, -1, -1, -1, -1, " "); // first
  hDummyPVContrInClassesDataLow->GetXaxis()->SetRangeUser(0, nNtracksLimit - nNtracksLimitOffset);
  hDummyPVContrInClassesDataLow->GetYaxis()->CenterTitle();
  hDummyPVContrInClassesDataLow->Draw("same");
  padPVContrInClassesDataLow->SetLogy();

  // Plotting
  // Define colors
  for(int iClass = 0; iClass < numMult + 1; iClass++) { 
    if(iClass==0) {
      color[iClass] = kBlack;
    } else {
      color[iClass] = numMult + FI - iClass;
    }
  }
  padPVContrInClassesDataUp->cd();
  StyleHistoMultiPlot(hProjPVContrDataMB, 1e-6, hProjPVContrDataMB->GetMaximum()*1e2, color[0], MarkerMult[0], "", hProjPVContrDataMB->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[0], 0.0, 0.05, 0.0, 0.035, 0.005);
  hProjPVContrDataMB->Sumw2();
  hProjPVContrDataMB->Rebin(rebinFactor2);
  Double_t scalingData = hProjPVContrDataMB->Integral();
  hProjPVContrDataMB->Scale(1.0/scalingData);
  hProjPVContrDataMB->Draw("same");
  legPVContrInClassesData->AddEntry(hProjPVContrDataMB, SmoltBis[0], "pef");
  TH2F* hProjPVContrFT0MpercInClasses[numMult];
  TH1F* hProjPVContrFT0MpercInClassesRatio[numMult];
  TH1D* hProjPVContrInClassData[numMult];
  for (Int_t i = 0; i < numMult; i++) {
    padPVContrInClassesDataUp->cd();
    hProjPVContrFT0MpercInClasses[i] = (TH2F*)hProjPVContrFT0Mperc->Clone(Form("hProjPVContrFT0MpercInClasses_%i", i));
    hProjPVContrFT0MpercInClasses[i]->GetYaxis()->SetRangeUser(multiplicityPerc[i], multiplicityPerc[i + 1]);
    hProjPVContrInClassData[i] = (hProjPVContrFT0MpercInClasses[i]->ProjectionX());
    StyleHistoMultiPlot(hProjPVContrInClassData[i], 1e-6, hProjPVContrInClassData[i]->GetMaximum()*1e2, color[i + 1], MarkerMult[i + 1], "", hProjPVContrInClassData[i]->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[i + 1], 0.0, 0.05, 0.0, 0.035, 0.005);
    hProjPVContrInClassData[i]->Sumw2();
    hProjPVContrInClassData[i]->Rebin(rebinFactor2);
    hProjPVContrInClassData[i]->Scale(1.0/scalingData);
    hProjPVContrInClassData[i]->Draw("same");
    legPVContrInClassesData->AddEntry(hProjPVContrInClassData[i], SmoltBis[i+1], "pef");

    padPVContrInClassesDataLow->cd();
    hProjPVContrFT0MpercInClassesRatio[i] = (TH1F*)hProjPVContrInClassData[i]->Clone(Form("hProjPVContrInClassData_%i", i));
    hProjPVContrFT0MpercInClassesRatio[i]->Divide(hProjPVContrDataMB);
    StyleHistoMultiPlot(hProjPVContrFT0MpercInClassesRatio[i], 1e-3, 3, color[i+1], MarkerMult[i+1], "#it{N}_{tracks}", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, SizeMult[i+1], 0.08, 0.08, 0.08, 0.08, 0.005);
    hProjPVContrFT0MpercInClassesRatio[i]->Draw("same");
  }
  padPVContrInClassesDataUp->cd();
  legendTitleData->Draw("same");
  legPVContrInClassesData->Draw("same");

  // Colors are not displayed correctly
  canvasPVContrInClassesData->Write();

  // Ntracks in classes MC//
  // Legend
  TLegend *legPVContrInClassesMC = new TLegend(0.186, 0.0625, 0.893, 0.3125);
  legPVContrInClassesMC->SetHeader("FT0M Multiplicity Percentile");
  legPVContrInClassesMC->SetNColumns(3);
  TLegendEntry *legPVContrInClassesMCHeader = (TLegendEntry *)legPVContrInClassesMC->GetListOfPrimitives()->First();
  legPVContrInClassesMCHeader->SetTextSize(0.04);
  StyleLegend(legPVContrInClassesMC, 0.0, 0.0);

  TLegend *legendTitleMC = new TLegend(0.609, 0.693913, 0.908, 0.893);
  StyleLegend(legendTitleMC, 0.0, 0.0);
  legendTitleMC->SetTextAlign(33);
  legendTitleMC->SetTextSize(0.05);
  legendTitleMC->SetTextFont(42);
  legendTitleMC->SetLineColorAlpha(0.,0.);
  legendTitleMC->SetFillColorAlpha(0.,0.);
  legendTitleMC->AddEntry("", "#bf{ALICE Work In Progress}", "");
  legendTitleMC->AddEntry("", "pp, #sqrt{#it{s}} = 13.6 TeV", "");
  legendTitleMC->AddEntry("", "LHC24b1", "");

  TCanvas* canvasPVContrInClassesMC = new TCanvas("canvasPVContrInClassesMC", "canvasPVContrInClassesMC", 0, 70, 620, 850);
  StyleCanvas(canvasPVContrInClassesMC, 0.15, 0.05, 0.05, 0.15);
  TPad *padPVContrInClassesMCUp = new TPad("padPVContrInClassesMCUp", "padPVContrInClassesMCUp", 0, 0.36, 1, 1);
  TPad *padPVContrInClassesMCLow = new TPad("padPVContrInClassesMCLow", "padPVContrInClassesMCLow", 0, 0, 1, 0.36);
  StylePad(padPVContrInClassesMCUp, 0.15, 0.05, 0.05, 0.);
  StylePad(padPVContrInClassesMCLow, 0.15, 0.05, 0.02, 0.2);
  canvasPVContrInClassesMC->cd();
  padPVContrInClassesMCUp->Draw();
  padPVContrInClassesMCLow->Draw();

  // Dummy histograms
  TH1F *hDummyPVContrInClassesMC = new TH1F("hDummyPVContrInClassesMC", "hDummyPVContrInClassesMC", 10000, 0, nNtracksLimit);
  // Up
  TH2F* hProjPVContrNchFT0MMB = (TH2F*)hProjPVContrNchFT0M->Clone("hProjPVContrNchFT0MInClassesMB");
  TH1D* hProjPVContrMCMB = (hProjPVContrNchFT0MMB->ProjectionX());
  for (Int_t i = 1; i <= hDummyPVContrInClassesMC->GetNbinsX(); i++)
    hDummyPVContrInClassesMC->SetBinContent(i, 1e-12);
  canvasPVContrInClassesMC->cd();
  padPVContrInClassesMCUp->cd();
  StyleHistoMultiPlot(hDummyPVContrInClassesMC, 1e-11, 3, 1, 1, "#it{N}_{tracks}", "Normalized Counts", "", 0, 0, 0, 1.5, 1.0, 0, 0.0, 0.05, 0.0, 0.035, 0.005);
  SetTickLength(hDummyPVContrInClassesMC, 0.025, 0.03);
  TAxis *axisPVContrInClassesMCDummy = hDummyPVContrInClassesMC->GetYaxis();
  axisPVContrInClassesMCDummy->ChangeLabel(1, -1, -1, -1, -1, -1, " ");
  //axisPVContrInClassesMCDummy->ChangeLabel(-1, -1, -1, -1, -1, -1, " ");
  hDummyPVContrInClassesMC->GetXaxis()->SetRangeUser(0, nNtracksLimit - nNtracksLimitOffset);
  padPVContrInClassesMCUp->SetLogy();
  hDummyPVContrInClassesMC->Draw("same");
  // Low
  TH1F *hDummyPVContrInClassesMCLow = new TH1F("hDummyPVContrInClassesMCLow", "hDummyPVContrInClassesMCLow", 10000, 0, nNtracksLimit);
  for (Int_t i = 1; i <= hDummyPVContrInClassesMCLow->GetNbinsX(); i++)
    hDummyPVContrInClassesMCLow->SetBinContent(i, 1);
  padPVContrInClassesMCLow->cd();
  StyleHistoMultiPlot(hDummyPVContrInClassesMCLow, 1e-4, 3, 1, 1, "#it{N}_{tracks}", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, 0, 0.08, 0.08, 0.08, 0.07, 0.01);
  SetTickLength(hDummyPVContrInClassesMCLow, 0.025, 0.03);
  TAxis *axisDummyPVContrInClassesMCLow = hDummyPVContrInClassesMCLow->GetYaxis();
  //padPVContrInClassesMCLow->SetGridy(); // Only grid on x-axis
  //axisDummyPVContrInClassesMCLow->ChangeLabel(-1, -1, -1, -1, -1, -1, " "); // last 
  axisDummyPVContrInClassesMCLow->ChangeLabel(1, -1, -1, -1, -1, -1, " "); // first
  hDummyPVContrInClassesMCLow->GetXaxis()->SetRangeUser(0, nNtracksLimit - nNtracksLimitOffset);
  hDummyPVContrInClassesMCLow->GetYaxis()->CenterTitle();
  hDummyPVContrInClassesMCLow->Draw("same");
  padPVContrInClassesMCLow->SetLogy();

  // Plotting
  padPVContrInClassesMCUp->cd();
  StyleHistoMultiPlot(hProjPVContrMCMB, 1e-6, hProjPVContrMCMB->GetMaximum()*1e2, color[0], MarkerMult[0], "", hProjPVContrMCMB->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[0], 0.0, 0.05, 0.0, 0.035, 0.005);
  hProjPVContrMCMB->Sumw2();
  hProjPVContrMCMB->Rebin(rebinFactor2);
  Double_t scalingMC = hProjPVContrMCMB->Integral();
  hProjPVContrMCMB->Scale(1.0/scalingMC);
  hProjPVContrMCMB->Draw("same");
  legPVContrInClassesMC->AddEntry(hProjPVContrMCMB, SmoltBis[0], "pef");
  TH2F* hProjPVContrNchFT0MInClasses[numMult];
  TH1F* hProjPVContrNchFT0MInClassesRatio[numMult];
  TH1D* hProjPVContrInClassMC[numMult];
  for (Int_t i = 0; i < numMult; i++) {
    padPVContrInClassesMCUp->cd();
    hProjPVContrNchFT0MInClasses[i] = (TH2F*)hProjPVContrNchFT0M->Clone(Form("hProjPVContrNchFT0MInClasses_%i", i));
    hProjPVContrNchFT0MInClasses[i]->GetYaxis()->SetRangeUser(NChFT0MCalibReversed[numMult - i - 1] + 1e-3, NChFT0MCalibReversed[numMult - i] - 1e-3);
    hProjPVContrInClassMC[i] = (hProjPVContrNchFT0MInClasses[i]->ProjectionX());
    StyleHistoMultiPlot(hProjPVContrInClassMC[i], 1e-6, hProjPVContrInClassMC[i]->GetMaximum()*1e2, color[i + 1], MarkerMult[i + 1], "", hProjPVContrInClassMC[i]->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[i + 1], 0.0, 0.05, 0.0, 0.035, 0.005);
    hProjPVContrInClassMC[i]->Sumw2();
    hProjPVContrInClassMC[i]->Rebin(rebinFactor2);
    hProjPVContrInClassMC[i]->Scale(1.0/scalingMC);
    hProjPVContrInClassMC[i]->Draw("same");
    legPVContrInClassesMC->AddEntry(hProjPVContrInClassMC[i], SmoltBis[i+1], "pef");

    padPVContrInClassesMCLow->cd();
    hProjPVContrNchFT0MInClassesRatio[i] = (TH1F*)hProjPVContrInClassMC[i]->Clone(Form("hProjPVContrInClassMC_%i", i));
    hProjPVContrNchFT0MInClassesRatio[i]->Divide(hProjPVContrMCMB);
    StyleHistoMultiPlot(hProjPVContrNchFT0MInClassesRatio[i], 0.1 , 1.9, color[i+1], MarkerMult[i+1], "#it{N}_{tracks}", "Ratio to 0-100%", "", 0, 0, 0, 1.0, 0.7, SizeMult[i+1], 0.08, 0.08, 0.08, 0.08, 0.005);
    hProjPVContrNchFT0MInClassesRatio[i]->Draw("same");
  }
  padPVContrInClassesMCUp->cd();
  legendTitleMC->Draw("same");
  legPVContrInClassesMC->Draw("same");

  canvasPVContrInClassesMC->Write();

  // Event Reconstruction QC //
{
  TDirectory* dirEventRecQC = outputfile->mkdir("eventRecQC");
  dirEventRecQC->cd();
  Int_t nNchFt0MLimit = 130;
  TH3D* hAssocCollisions3D = (TH3D*)fileMCin->Get("lf-cascqaanalysis/hNchFT0MNAssocMCCollisionsSameType");
  hAssocCollisions3D->GetZaxis()->SetRange(2, 3); // INEL>0 sel
  //hRecCollisions3D->Write();
  TH2D* hAssocCollisions2D = static_cast<TH2D*>(hAssocCollisions3D->Project3D("yx"));
  //hAssocCollisions2D->Draw();

  TH2D* hAssocCollisions2D_0coll = (TH2D*)hAssocCollisions2D->Clone("0assoc");
  hAssocCollisions2D_0coll->GetYaxis()->SetRangeUser(-0.5, 0.5);
  TH1D* hAssocCollisions1D_0coll = (hAssocCollisions2D_0coll->ProjectionX());
  hAssocCollisions1D_0coll->Sumw2();

  TH2D* hAssocCollisions2D_1coll = (TH2D*)hAssocCollisions2D->Clone("1assoc");
  hAssocCollisions2D_1coll->GetYaxis()->SetRangeUser(0.5, 1.5);
  TH1D* hAssocCollisions1D_1coll = (hAssocCollisions2D_1coll->ProjectionX());
  hAssocCollisions1D_1coll->Sumw2();

  TH2D* hAssocCollisions2D_2coll = (TH2D*)hAssocCollisions2D->Clone("2assoc");
  hAssocCollisions2D_2coll->GetYaxis()->SetRangeUser(1.5, 2.5);
  TH1D* hAssocCollisions1D_2coll = (hAssocCollisions2D_2coll->ProjectionX());
  hAssocCollisions1D_2coll->Sumw2();

  Double_t lineWidth = 3;

  hAssocCollisions1D_0coll->SetLineColor(kBlack);
  hAssocCollisions1D_0coll->GetYaxis()->SetRangeUser(0, 1.7);
  hAssocCollisions1D_0coll->GetXaxis()->SetRangeUser(0, nNchFt0MLimit);
  hAssocCollisions1D_0coll->GetYaxis()->SetTitle("Fraction of events");
  hAssocCollisions1D_0coll->SetTitle("");
  hAssocCollisions1D_0coll->SetLineWidth(lineWidth);
  StyleHistoLight(hAssocCollisions1D_0coll);

  hAssocCollisions1D_1coll->SetLineColor(8);
  hAssocCollisions1D_1coll->SetLineWidth(lineWidth);
  StyleHistoLight(hAssocCollisions1D_1coll);

  hAssocCollisions1D_2coll->SetLineColor(4);
  hAssocCollisions1D_2coll->SetLineWidth(lineWidth);
  StyleHistoLight(hAssocCollisions1D_2coll);

  TCanvas *canvasMultRec = new TCanvas("canvasMultRec","canvasMultRec",800, 600);
  canvasMultRec->cd();
  StyleCanvas(canvasMultRec, 0.14, 0.08, 0.11, 0.15);

  for (int i = 1; i <= hAssocCollisions1D_0coll->GetNbinsX(); ++i) {
    Double_t sumInBin = hAssocCollisions1D_0coll->GetBinContent(i) + hAssocCollisions1D_1coll->GetBinContent(i) + hAssocCollisions1D_2coll->GetBinContent(i);
    if(sumInBin != 0){
      //std::cout << sumInBin << std::endl;
      Double_t value0 = hAssocCollisions1D_0coll->GetBinContent(i)/sumInBin;
      Double_t value1 = hAssocCollisions1D_1coll->GetBinContent(i)/sumInBin;
      Double_t value2 = hAssocCollisions1D_2coll->GetBinContent(i)/sumInBin;
      //std::cout << value0 << " " <<  value1 << " " << value2 << std::endl;
      hAssocCollisions1D_0coll->SetBinContent(i, value0);
      hAssocCollisions1D_1coll->SetBinContent(i, value1);
      hAssocCollisions1D_2coll->SetBinContent(i, value2);

      hAssocCollisions1D_0coll->SetBinError(i, 0);
      hAssocCollisions1D_1coll->SetBinError(i, 0);
      hAssocCollisions1D_2coll->SetBinError(i, 0);
    }
    //std::cout << hAssocCollisions1D_0coll->GetBinContent(i)/sumInBin + hAssocCollisions1D_1coll->GetBinContent(i)/sumInBin + hAssocCollisions1D_2coll->GetBinContent(i)/sumInBin << std::endl;
  }

  hAssocCollisions1D_0coll->Draw();
  hAssocCollisions1D_1coll->Draw("SAME");
  hAssocCollisions1D_2coll->Draw("SAME");

  // Event density distribution
  TH2F* hNchFT0MPVContrClone2 = (TH2F*)hProjPVContrNchFT0M->Clone("hProjPVContrNchFT0M");
  hNchFT0MPVContrClone2->GetXaxis()->SetRangeUser(0, nNtracksLimit); // Set Ntracks axis range
  hNchFT0MPVContrClone2->GetYaxis()->SetRangeUser(0, nNchFt0MLimit); // Set Ngen axis range
  TH1D* hProjNchFT0MPVContrClone2 = (hNchFT0MPVContrClone2->ProjectionY());
  hProjNchFT0MPVContrClone2->SetTitle("");
  hProjNchFT0MPVContrClone2->Scale(4.0/hProjNchFT0MPVContrClone2->Integral());
  
  int numBins = hProjNchFT0MPVContrClone2->GetNbinsX();
  double countsToAdd = 1.0;
  for (int bin = 1; bin <= numBins; ++bin) {
      double currentBinContent = hProjNchFT0MPVContrClone2->GetBinContent(bin);
      hProjNchFT0MPVContrClone2->SetBinContent(bin, currentBinContent + countsToAdd);
  }

  hProjNchFT0MPVContrClone2->SetLineColor(kRed);
  hProjNchFT0MPVContrClone2->SetLineWidth(2);
  StyleHistoLight(hProjNchFT0MPVContrClone2);
  hProjNchFT0MPVContrClone2->Draw("SAME");

  TLegend *LegendTitleMultRec = new TLegend(0.28, 0.25, 0.71, 0.45);
  LegendTitleMultRec->SetFillStyle(0);
  LegendTitleMultRec->SetTextAlign(33);
  LegendTitleMultRec->SetTextSize(0.04);
  LegendTitleMultRec->SetTextFont(42);
  LegendTitleMultRec->SetLineColorAlpha(0.,0.);
  LegendTitleMultRec->SetFillColorAlpha(0.,0.);
  LegendTitleMultRec->SetBorderSize(0.);
  LegendTitleMultRec->AddEntry("", "#bf{ALICE}", "");
  LegendTitleMultRec->AddEntry("", "pp, #sqrt{#it{s}} = 13.6 TeV", "");
  LegendTitleMultRec->AddEntry("", "LHC24b1, INEL > 0", "");
  LegendTitleMultRec->Draw("same");

  TLegend *LegendTextMultRec = new TLegend(0.055, 0.634, 0.489, 0.850);
  LegendTextMultRec->SetFillStyle(0);
  LegendTextMultRec->SetTextAlign(33);
  LegendTextMultRec->SetTextSize(0.04);
  LegendTextMultRec->SetTextFont(42);
  LegendTextMultRec->SetLineColorAlpha(0.,0.);
  LegendTextMultRec->SetFillColorAlpha(0.,0.);
  LegendTextMultRec->SetBorderSize(0.);
  LegendTextMultRec->AddEntry("", "Event Loss and Splitting", "");
  LegendTextMultRec->Draw("same");

  TLegend*  lLegend     = new TLegend(0.194, 0.695, 0.691, 0.796);
  lLegend->SetNColumns(3);
  lLegend->SetTextFont(42);
  lLegend->SetLineColorAlpha(0.,0.);
  lLegend->SetFillColorAlpha(0.,0.);
  lLegend->SetBorderSize(0.);
  lLegend->SetTextSize(0.04);
  lLegend->AddEntry  (hAssocCollisions1D_0coll,"N_{assoc.} = 0","lpf");
  lLegend->AddEntry  (hAssocCollisions1D_1coll,"N_{assoc.} = 1","lpf");
  lLegend->AddEntry  (hAssocCollisions1D_2coll,"N_{assoc.} = 2","lpf");
  lLegend->Draw("same");

  TLegend*  lLegend2     = new TLegend(0.524, 0.619, 0.959, 0.690);
  lLegend2->SetNColumns(3);
  lLegend2->SetTextFont(42);
  lLegend2->SetLineColorAlpha(0.,0.);
  lLegend2->SetFillColorAlpha(0.,0.);
  lLegend2->SetBorderSize(0.);
  lLegend2->SetTextSize(0.04);
  lLegend2->AddEntry  (hProjNchFT0MPVContrClone2,"Event density distribution","lpf");
  lLegend2->Draw("same");
  
  //draw an axis on the right side
  TGaxis *axis = new TGaxis(nNchFt0MLimit,1.0,nNchFt0MLimit,1.6,0,0.15,3,"+L");
  axis->SetLineColor(kRed);
  axis->SetLabelColor(kRed);
  axis->Draw("same");

  DrawLine(NChFT0MCalib[numMult - 1], 1, "70 %");
  DrawLine(NChFT0MCalib[5], 1, "30 %");
  DrawLine(NChFT0MCalib[3], 1, "10 %");
  DrawLine(NChFT0MCalib[2], 1, "5 %");
  DrawLine(NChFT0MCalib[1], 1, "1 %");

  canvasMultRec->Update();
  canvasMultRec->Write();
}

  // End of Code
  std::cout << "\x1B[1;32m"; // Set text color to green
  std::cout << "\n************* MC Calibration Finished! *************\n";
  std::cout << "\x1B[0m"; // Reset text color
}
