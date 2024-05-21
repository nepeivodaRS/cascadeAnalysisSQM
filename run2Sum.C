#include "help.h"
#include "fitting.h"
#include "effHelp.h"

void run2Sum(const TString workingDir = "/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM",
             const TString postFix = "20may")
{
  // Start of Code
  std::cout << "\x1B[1;33m"; // Set text color to yellow
  std::cout << "\n************ Starting Run2 cross-check: MB = sum in classes ************\n";
  std::cout << "\x1B[0m"; // Reset text color

  gStyle->SetOptStat(0);

  TString outputfilePath = workingDir + "/comparisons/" + "run2Sum_" + postFix + ".root";
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

  TH1F* hYields[11];
  TH1F* hYieldsStat[11];
  TH1F* hYieldsSyst[11];
  TH1F* hYieldClonesStat[11];
  TH1F* hYieldClonesSyst[11];
  TH1F* hYieldSummedStat;
  TH1F* hYieldSummedSyst;

  for(Int_t i = 1; i <= 11; i++) {
    hYields[i-1] = (TH1F *)yieldHEPDir->Get(Form("Hist1D_y%i", i));
    hYieldsStat[i-1] = (TH1F *)yieldHEPDir->Get(Form("Hist1D_y%i_e1", i)); // stat.
    hYieldsSyst[i-1] = (TH1F *)yieldHEPDir->Get(Form("Hist1D_y%i_e2", i)); // syst.
    hYieldClonesStat[i-1] = (TH1F *)hYields[i-1]->Clone(Form("Hist1D_y%i_clone_stat", i));
    hYieldClonesSyst[i-1] = (TH1F *)hYields[i-1]->Clone(Form("Hist1D_y%i_clone_syst", i));
    for (Int_t j = 1; j <= hYieldClonesStat[i-1]->GetNbinsX(); j++) {
       hYieldClonesStat[i-1]->SetBinError(j, hYieldsStat[i-1]->GetBinContent(j)); // apply stat. uncert.
       hYieldClonesSyst[i-1]->SetBinError(j, hYieldsSyst[i-1]->GetBinContent(j)); // apply syst. uncert.
    }
  }

  hYieldSummedStat = (TH1F *)hYields[0]->Clone("summedMB_stat");
  hYieldSummedSyst = (TH1F *)hYields[0]->Clone("summedMB_syst");
  hYieldSummedStat->Reset();
  hYieldSummedSyst->Reset();

  for(Int_t i = 1; i <= 10; i++) { // except MB
    hYieldClonesStat[i-1]->Scale(fractionsINEL0run2[i] - fractionsINEL0run2[i-1]);
    hYieldClonesSyst[i-1]->Scale(fractionsINEL0run2[i] - fractionsINEL0run2[i-1]);
    std::cout << "weight: " << fractionsINEL0run2[i] - fractionsINEL0run2[i-1] << std::endl;
    hYieldSummedStat->Add(hYieldClonesStat[i-1]);
    hYieldSummedSyst->Add(hYieldClonesSyst[i-1]);
  }
  hYieldSummedStat->Scale(1./100.);
  hYieldSummedSyst->Scale(1./100.);

  {
    outputfile->cd();
    TCanvas* canvasRun2 = new TCanvas("run2Comp", "run2Comp", 0, 70, 620, 850);
    StyleCanvas(canvasRun2, 0.15, 0.05, 0.05, 0.15);
    TPad *padrun2CompUp = new TPad("padrun2CompUp", "padrun2CompUp", 0, 0.36, 1, 1);
    TPad *padrun2CompLow = new TPad("padrun2CompLow", "padrun2CompLow", 0, 0, 1, 0.36);
    StylePad(padrun2CompUp, 0.15, 0.05, 0.05, 0.);
    StylePad(padrun2CompLow, 0.15, 0.05, 0.02, 0.2);
    canvasRun2->cd();
    padrun2CompUp->Draw();
    padrun2CompLow->Draw();

    // Dummy histograms
    TH1F *hDummyrun2Comp = new TH1F("hDummyrun2Comp", "hDummyrun2Comp", 10000, 0., 6.5 + 0.5);
    // Up
    for (Int_t i = 1; i <= hDummyrun2Comp->GetNbinsX(); i++)
      hDummyrun2Comp->SetBinContent(i, 1e-12);
    canvasRun2->cd();
    padrun2CompUp->cd();
    hDummyrun2Comp->GetYaxis()->SetMaxDigits(3);
    hDummyrun2Comp->GetYaxis()->SetDecimals(kTRUE);
    Double_t maxCounts = (hYieldSummedStat->GetMaximum() > hYieldClonesStat[10]->GetMaximum()) ? hYieldSummedStat->GetMaximum() : hYieldClonesStat[10]->GetMaximum();
    StyleHistoMultiPlot(hDummyrun2Comp, 1e-5, maxCounts*1e1, 1, 1, "#it{p}_{T} (GeV/#it{c})", sdNdPtdY, "", 0, 0, 0, 1.5, 1.2, 0, 0.0, 0.05, 0.0, 0.035, 0.005);
    SetTickLength(hDummyrun2Comp, 0.025, 0.03);
    TAxis *axisrun2CompDummy = hDummyrun2Comp->GetYaxis();
    axisrun2CompDummy->ChangeLabel(1, -1, -1, -1, -1, -1, " ");
    hDummyrun2Comp->GetXaxis()->SetRangeUser(0., 6.5 + 0.5);
    padrun2CompUp->SetLogy();
    hDummyrun2Comp->Draw("same");
    // Low
    TH1F *hDummyrun2CompLow = new TH1F("hDummyrun2CompLow", "hDummyrun2CompLow", 10000, 0., 6.5 + 0.5);
    for (Int_t i = 1; i <= hDummyrun2CompLow->GetNbinsX(); i++)
      hDummyrun2CompLow->SetBinContent(i, 1);
    padrun2CompLow->cd();
    StyleHistoMultiPlot(hDummyrun2CompLow, 0.9, 1.1, 1, 1, "#it{p}_{T} (GeV/#it{c})", "MB(sum)/0-100%", "", 0, 0, 0, 1.0, 0.7, 0, 0.08, 0.08, 0.08, 0.07, 0.01);
    SetTickLength(hDummyrun2CompLow, 0.025, 0.03);
    TAxis *axisDummyrun2CompLow = hDummyrun2CompLow->GetYaxis();
    axisDummyrun2CompLow->ChangeLabel(-1, -1, -1, -1, -1, -1, " "); // last 
    axisDummyrun2CompLow->ChangeLabel(1, -1, -1, -1, -1, -1, " "); // first
    hDummyrun2CompLow->GetXaxis()->SetRangeUser(0., 6.5 + 0.5);
    hDummyrun2CompLow->GetYaxis()->CenterTitle();
    hDummyrun2CompLow->Draw("same");

    // Plotting
    padrun2CompUp->cd();
    StyleHistoMultiPlot(hYieldSummedStat, 1e-6, hYieldSummedStat->GetMaximum()*1e2, color[0], MarkerMult[0], "", hYieldSummedStat->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[0], 0.0, 0.05, 0.0, 0.035, 0.005);
    StyleHistoMultiPlot(hYieldClonesStat[10], 1e-6, hYieldClonesStat[10]->GetMaximum()*1e2, color[3], MarkerMult[3], "", hYieldClonesStat[10]->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[3], 0.0, 0.05, 0.0, 0.035, 0.005);
    StyleHistoMultiPlot(hYieldSummedSyst, 1e-6, hYieldSummedSyst->GetMaximum()*1e2, color[0], MarkerMult[0], "", hYieldSummedSyst->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[0], 0.0, 0.05, 0.0, 0.035, 0.005);
    StyleHistoMultiPlot(hYieldClonesSyst[10], 1e-6, hYieldClonesSyst[10]->GetMaximum()*1e2, color[3], MarkerMult[3], "", hYieldClonesSyst[10]->GetYaxis()->GetTitle(), "", 0, 0, 0, 1.5, 1.0, SizeMult[3], 0.0, 0.05, 0.0, 0.035, 0.005);
    hYieldSummedStat->Draw("same ex0");
    hYieldClonesStat[10]->Draw("same ex0");

    hYieldSummedStat->SetFillStyle(0);
    hYieldClonesStat[10]->SetFillStyle(0);
    hYieldSummedStat->Draw("same e2");
    hYieldClonesStat[10]->Draw("same e2");

    TLegend *legrun2Comp = new TLegend(0.47, 0.64, 0.99, 0.89);
    legrun2Comp->SetHeader("Run 2 MB yield cross-check");
    legrun2Comp->SetFillStyle(0);
    legrun2Comp->AddEntry(hYieldSummedStat, "MB(Sum)", "p");
    legrun2Comp->AddEntry(hYieldClonesStat[10], "0-100%", "p");
    StyleLegend(legrun2Comp, 0.0, 0.0);
    legrun2Comp->SetTextSize(0.04);
    legrun2Comp->Draw("same");

    padrun2CompLow->cd();
    TH1F *hRatioYieldsStat = (TH1F* )hYieldSummedStat->Clone("hRatioYieldsStat");
    hRatioYieldsStat->Divide(hYieldClonesStat[10]);
    StyleHistoMultiPlot(hRatioYieldsStat, 0.1 , 11, color[0], MarkerMult[0], "", "", "", 0, 0, 0, 1.0, 0.7, 0.7, 0.08, 0.08, 0.08, 0.07, 0.01);

    TH1F *hRatioYieldsSyst = (TH1F* )hYieldSummedSyst->Clone("hRatioYieldsSyst");
    hRatioYieldsSyst->Divide(hYieldClonesSyst[10]);
    StyleHistoMultiPlot(hRatioYieldsSyst, 0.1 , 11, color[0], MarkerMult[0], "", "", "", 0, 0, 0, 1.0, 0.7, 0.7, 0.08, 0.08, 0.08, 0.07, 0.01);

    hRatioYieldsStat->SetStats(0);
    hRatioYieldsStat->Draw("same ex0");
    hRatioYieldsSyst->SetFillStyle(0);
    hRatioYieldsSyst->Draw("same e2");
    canvasRun2->Write();
  }

  // End of Code
  std::cout << "\x1B[1;32m"; // Set text color to green
  std::cout << "\n************* Spectra in Multiplicity Classes is Fitted! *************\n";
  std::cout << "\x1B[0m"; // Reset text color
}
