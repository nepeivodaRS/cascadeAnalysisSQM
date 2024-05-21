void PrintBinning(TH1* hist)
{
  const Int_t nBinsX = hist->GetXaxis()->GetNbins();

  cout << "const Int_t nBins = " << nBinsX << ";" << endl
       << "Double_t bins[nBins+1] = " << endl
       << "{";
  for(Int_t bin = 1; bin <= nBinsX; bin++) {
    
    printf("%.2f, ", hist->GetXaxis()->GetBinLowEdge(bin));
    if (bin%10 == 0) 
      cout << endl;
  }
  
  printf("%.2f", hist->GetXaxis()->GetBinUpEdge(nBinsX));
  cout << "}" << endl;  
}

void SetErrorDivide(TH1D* histFinal, const TH1D* hist1, const TH1D* hist2)
{
  const Int_t nBins = hist1->GetXaxis()->GetNbins();
  for(Int_t bin = 1; bin <= nBins; bin++) {
    Int_t k = hist1->GetBinContent(bin);
    Int_t n = hist2->GetBinContent(bin);
    Double_t errorInBin = sqrt(((Double_t)k+1)*((Double_t)k+2)/(n+2)/(n+3) - pow((Double_t)(k+1),2)/pow(n+2,2));
    histFinal->SetBinError(bin, errorInBin);
  }
}

template <typename THisto>
void StyleHisto(THisto *histo, Double_t Low, Double_t Up, Int_t color, Int_t style, TString titleX, TString titleY, TString title, 
                Bool_t XRange, Double_t XLow, Double_t XUp, Double_t xOffset, Double_t yOffset, Double_t mSize, 
                Double_t xTitleSize, Double_t yTitleSize)
{
  histo->GetYaxis()->SetRangeUser(Low, Up);
  if (XRange)
    histo->GetXaxis()->SetRangeUser(XLow, XUp);
  histo->SetLineColor(color);
  histo->SetMarkerColor(color);
  histo->SetMarkerStyle(style);
  histo->SetMarkerSize(mSize);
  histo->GetXaxis()->SetTitle(titleX);
  //histo->GetXaxis()->SetLabelSize(0.05);
  histo->GetXaxis()->SetTitleSize(xTitleSize);
  histo->GetXaxis()->SetTitleOffset(xOffset);
  histo->GetYaxis()->SetTitle(titleY);
  histo->GetYaxis()->SetTitleSize(yTitleSize);
  //histo->GetYaxis()->SetLabelSize(0.05);
  histo->GetYaxis()->SetTitleOffset(yOffset);
  histo->SetTitle(title);
}

template <typename TMG>
void StyleMultGraph(TMG *mg, Double_t Low, Double_t Up, TString titleX, TString titleY, TString title, 
                Bool_t XRange, Double_t XLow, Double_t XUp, Double_t xOffset, Double_t yOffset, 
                Double_t xTitleSize, Double_t yTitleSize)
{
  mg->GetYaxis()->SetRangeUser(Low, Up);
  if (XRange)
    mg->GetXaxis()->SetRangeUser(XLow, XUp);
  mg->GetXaxis()->SetTitle(titleX);
  //histo->GetXaxis()->SetLabelSize(0.05);
  mg->GetXaxis()->SetTitleSize(xTitleSize);
  mg->GetXaxis()->SetTitleOffset(xOffset);
  mg->GetYaxis()->SetTitle(titleY);
  mg->GetYaxis()->SetTitleSize(yTitleSize);
  //histo->GetYaxis()->SetLabelSize(0.05);
  mg->GetYaxis()->SetTitleOffset(yOffset);
  mg->SetTitle(title);
}

template <typename THisto>
void StyleHistoMultiPlot(THisto *histo, Double_t Low, Double_t Up, Int_t color, Int_t style, TString titleX, TString titleY, TString title, 
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

template <typename THisto>
void Style2DHisto(THisto *histo, TString titleX, TString titleY, TString title,
  Double_t xTitleSize, Double_t yTitleSize, 
  Double_t xLabelSize, Double_t yLabelSize, Double_t zLabelSize)
{
  histo->GetXaxis()->SetTitle(titleX);
  histo->GetYaxis()->SetTitle(titleY);
  histo->SetTitle(title);
  histo->GetXaxis()->SetTitleSize(xTitleSize);
  histo->GetYaxis()->SetTitleSize(yTitleSize);
  histo->GetXaxis()->SetLabelSize(xLabelSize);
  histo->GetYaxis()->SetLabelSize(yLabelSize);
  histo->GetZaxis()->SetLabelSize(zLabelSize);
}

void StyleCanvas(TCanvas *canvas, Float_t LMargin, Float_t RMargin, Float_t TMargin, Float_t BMargin)
{
  canvas->SetFillColor(0);
  canvas->SetTickx(1);
  canvas->SetTicky(1);
  canvas->SetLeftMargin(LMargin);
  canvas->SetRightMargin(RMargin);
  canvas->SetTopMargin(TMargin);
  canvas->SetBottomMargin(BMargin);
  //gStyle->SetOptStat(0);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetLegendFillColor(0);
  gStyle->SetLegendFont(42);
  // gStyle->SetPalette(55, 0);
}

template <typename THisto>
void StyleHistoLight(THisto *histo)
{
  histo->GetXaxis()->SetLabelSize(0.05);
  histo->GetXaxis()->SetTitleSize(0.05);
  histo->GetYaxis()->SetTitleSize(0.05);
  histo->GetYaxis()->SetLabelSize(0.05);
  histo->GetXaxis()->SetLabelOffset(0.02);
  histo->GetXaxis()->SetTitleOffset(1.1);
}

void DrawLine(Double_t x, Double_t y, const Char_t* labelText){
  TLine *line = new TLine(x, 0, x, y);
  line->SetLineStyle(2); // Set the line style to dashed (2)
  line->SetLineColor(kBlack); // Set the line color (kRed is a ROOT predefined color)
  line->Draw("same"); // Draw the line on the same canvas

  TLatex *label = new TLatex(x-1, y * 0.5, labelText);
  label->SetTextSize(0.03); // Set the text size
  label->SetTextAngle(90);
  label->SetTextColor(kBlack); // Set the text color to match the line
  label->Draw();
}

void DrawHorLineSyst(Double_t xMin, Double_t xMax, Double_t y, const Char_t* labelText){
  TLine *line = new TLine(xMin, y, xMax, y);
  line->SetLineStyle(2); // Set the line style to dashed (2)
  line->SetLineColor(kGray); // Set the line color (kRed is a ROOT predefined color)
  line->Draw("same"); // Draw the line on the same canvas

  TLatex *label = new TLatex(xMin+(xMax-xMin)/20, y + 0.01/3., labelText);
  label->SetTextSize(0.05); // Set the text size
  label->SetTextAngle(0);
  label->SetTextColor(kGray); // Set the text color to match the line
  label->Draw();
}