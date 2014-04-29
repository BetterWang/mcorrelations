TProfile* GetTiming(const char* filename)
{
  TFile*    file = TFile::Open(filename, "READ");
  TProfile* p    = 0; 
  try {
    if (!file) {
      Warning("GetTiming", "Failed to open \"%s\"", filename);
      throw 1;
    }
    TObject* o = file->Get("timing");
    if (!o) { 
      Warning("GetTiming", "Failed to get profile \"timing\" from \"%s\"",
	      filename);
      file->ls();
      throw 2;
    }
    if (!o->IsA()->InheritsFrom(TProfile::Class())) {
      Warning("GetTiming", "Object \"timing\" from \"%s\" is a %s",
	      filename, o->ClassName());
      throw 3;
    }
    p = static_cast<TProfile*>(o);
    p->SetDirectory(0);
  }
  catch (int e) { 
    Error("GetTiming", "%d", e);
    p = 0;
    file->Close();
  }
  return p;
}

void SetAttr(TProfile* p, Int_t color, Int_t fill, Int_t marker)
{
  p->SetLineColor(color);
  p->SetFillColor(color);
  p->SetMarkerColor(color);
  p->SetFillStyle(0); // fill);
  p->SetMarkerStyle(marker);
  p->SetLineWidth(2);
  Int_t l = marker % 20+1;
  p->SetLineStyle(l == 3 ? 4 : l);
  p->GetXaxis()->SetLabelSize(0.07);
  switch (marker) { 
  case 24: 
  case 20: p->SetMarkerSize(1.4); break;
  case 25: 
  case 21: p->SetMarkerSize(1.5); break;
  case 22: p->SetMarkerSize(1.6); break;
  }
}

void
DrawArticlePlot(Bool_t noSub=true)
{
  TProfile* closed     = GetTiming("closed.root");
  TProfile* recurrence = GetTiming("recurrence.root");
  TProfile* recursive  = GetTiming("recursive.root");

  if (!closed || !recurrence || !recursive) return;
  
  TCanvas* c = new TCanvas("c", "C");
  c->SetRightMargin(0.02);
  c->SetTopMargin(0.02);
  c->SetLogy();
  gStyle->SetOptTitle(0);
  closed->SetTitle("Expanded");
  closed->SetName("closed");
  recurrence->SetTitle("Recurrence");
  recurrence->SetName("recurrence");
  recursive->SetTitle("Recursive");
  recursive->SetName("recursive");

  SetAttr(closed, kRed+2, 3004, 20);
  SetAttr(recurrence, kGreen+2, 3005, 21);
  SetAttr(recursive, kBlue+2, 3007, 22);

  TAxis* xAxis = closed->GetXaxis();
  for (Int_t i=1; i <= xAxis->GetNbins(); i++) {
    TString lab;
    if (noSub)  {
      lab = Form("#LT#LT%d#GT#GT", i+1);
    }
    else  {
      TString sub;
      if (i == 1) sub = "n_{1},n_{2}";
      else        sub = Form("n_{1},#3dots,n_{%d}", i+1);
      lab = Form("#LT%d#GT_{%s}", i+1, sub.Data());
    }
    xAxis->SetBinLabel(i, lab);
    recursive->GetXaxis()->SetBinLabel(i, lab);
    recurrence->GetXaxis()->SetBinLabel(i, lab);
  }

  THStack* stack = new THStack("stack", "Stack");
  stack->Add(recurrence, "pl");
  stack->Add(recursive, "pl");
  stack->Add(closed, "pl");

  stack->Draw("nostack hist");
  stack->GetXaxis()->SetLabelSize(.08);
  stack->GetYaxis()->SetLabelSize(.07);
  stack->GetYaxis()->SetTitleSize(.07);
  stack->GetYaxis()->SetTitleOffset(.65);
  stack->GetYaxis()->SetTitle("#LTt#GT [ns]");

  TLegend* l = c->BuildLegend(0.12, .6, .5, .97);
  l->SetBorderSize(0);
  l->SetFillColor(0);
  l->SetFillStyle(0);
  TList* ll = l->GetListOfPrimitives();
  for (Int_t i = 0; i < 3; i++) {
    TLegendEntry* e = static_cast<TLegendEntry*>(ll->At(i));
    e->SetOption("p");
  }
  
  c->Modified();
  c->Update();
  c->cd();

  c->SaveAs("algorithmsTiming.png");
  c->SaveAs("algorithmsTiming.eps");
  c->SaveAs("algorithmsTiming.pdf");
  c->SaveAs("test.C");
}

  
