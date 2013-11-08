/*
 * Compare.C
 *
 *  Created on: Nov 8, 2013
 *      Author: cholm
 */

#ifndef __CINT__
# include <TFile.h>
# include <TH1.h>
# include <TProfile.h>
# include <TCanvas.h>
# include <TError.h>
# include <TLatex.h>
#else
class TFile;
class TH1;
class TProfile;
class TCanvas;
#endif

Bool_t
GetHistos(const TString& mode,
          TH1*& h,
          TH1*& real,
          TH1*& imag,
          TProfile*& time)
{
  TString in(mode);
  in.ToLower();
  if (!in.EndsWith(".root")) in.Append(".root");
  TFile* file = TFile::Open(in, "READ");
  if (!file) {
    Error("GetHistos", "Failed to open %s", in.Data());
    return false;
  }
  h    = static_cast<TH1*>(file->Get("h"));
  real = static_cast<TH1*>(file->Get("reals"));
  imag = static_cast<TH1*>(file->Get("imags"));
  time = static_cast<TProfile*>(file->Get("timing"));
  if (!h || !real || !imag || !time) {
    Error("GetHistos", "Failed to find all histograms");
    return false;
  }
  real->SetDirectory(0);
  imag->SetDirectory(0);
  time->SetDirectory(0);

  file->Close();

  return true;
}

void DrawInPad(TVirtualPad* p,
               Int_t sub,
               TH1* h,
               Bool_t logy=false)
{
  TVirtualPad* pp = p->cd(sub);
  pp->SetRightMargin(0.02);
  if (logy) pp->SetLogy();
  TH1* copy = h->DrawCopy("hist");
  copy->GetXaxis()->SetLabelSize(0.13);
  copy->GetYaxis()->SetLabelSize(0.08);
  copy->SetDirectory(0);
}
void DrawTwoInPad(TVirtualPad* p,
                  Int_t sub,
                  TH1* h1,
                  TH1* h2)
{
  TVirtualPad* pp = p->cd(sub);
  pp->SetRightMargin(0.02);
  TH1* hs[] = { h1, h2, 0 };
  if (h1->GetMaximum() < h2->GetMaximum()) {
    hs[0] = h2;
    hs[1] = h1;
  }
  TH1** ph = hs;
  while (*ph) {
    TString opt("hist");
    if (ph != hs) opt.Append(" same");

    TH1* copy = h1->DrawCopy(opt);
    copy->GetXaxis()->SetLabelSize(0.13);
    copy->GetYaxis()->SetLabelSize(0.08);
    copy->SetDirectory(0);
  }
}
void
Compare(const TString& mode1,
        const TString& mode2)
{
  TH1* h1 = 0;
  TH1* h2 = 0;
  TH1* r1 = 0;
  TH1* i1 = 0;
  TH1* r2 = 0;
  TH1* i2 = 0;
  TProfile* t1 = 0;
  TProfile* t2 = 0;

  if (!GetHistos(mode1, h1, r1, i1, t1)) return;
  if (!GetHistos(mode2, h2, r2, i2, t2)) return;

  TH1* hd = static_cast<TH1*>(h1->Clone("diffH"));
  TH1* rd = static_cast<TH1*>(r1->Clone("diffReal"));
  TH1* id = static_cast<TH1*>(i1->Clone("diffImag"));
  TH1* td = static_cast<TH1*>(t1->Clone("diffTime"));

  hd->Add(h2, -1);
  rd->Add(r2, -1);
  id->Add(i2, -1);
  td->Add(t2, -1);

  rd->Divide(r2);
  id->Divide(i2);
  td->Divide(t2);

  {
    TCanvas* can = new TCanvas("diff", "Differences");
    can->SetTopMargin(0.15);
    can->SetBottomMargin(0.15);
    can->SetRightMargin(0.03);
    can->Divide(1, 4, 0, 0);

    DrawInPad(can, 4, td);
    DrawInPad(can, 3, id);
    DrawInPad(can, 2, rd);
    DrawInPad(can, 1, hd);

    can->cd(0);
    TLatex* ltx = new TLatex(
        0.5, 0.995, Form("(%s-%s)/%s", mode1.Data(), mode2.Data(), mode2.Data()));
    ltx->SetNDC(true);
    ltx->SetTextAlign(23);
    ltx->SetTextSize(0.04);
    ltx->Draw();

    can->Modified();
    can->Update();
    can->cd();
  }
  {
    TCanvas* can = new TCanvas("values", "Values");
    can->SetTopMargin(0.15);
    can->SetBottomMargin(0.15);
    can->SetRightMargin(0.03);
    can->Divide(1, 4, 0, 0);

    DrawTwoInPad(can, 4, t1, t2);
    DrawTwoInPad(can, 3, i1, i2);
    DrawTwoInPad(can, 2, r1, r2);
    DrawTwoInPad(can, 1, h1, h2);

    can->cd(0);
    TLatex* ltx = new TLatex(0.5, 0.995, Form("%s & %s", mode1.Data(), mode2.Data()));
    ltx->SetNDC(true);
    ltx->SetTextAlign(23);
    ltx->SetTextSize(0.04);
    ltx->Draw();

    can->Modified();
    can->Update();
    can->cd();
  }
}

#ifdef AS_PROG
#include <TROOT.h>
#include <TApplication.h>
#include <iostream>
void
usage(const char* progname)
{
  std::cout << "Usage: " << progname << " [OPTIONS]\n\n" << "Options:\n"
            << "  -h          This help\n" << "  -1 NAME     First set\n"
            << "  -2 NAME     Second set\n" << std::flush;
}
int
main(int argc,
     char** argv)
{
  TString a("closed");
  TString b("recursive");
  bool batch = false;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'h':
        usage(argv[0]);
        return 0;
      case '1':
        a = argv[++i];
        break;
      case '2':
        b = argv[++i];
        break;
      case 'b':
        batch = false;
        break;
      case 'B':
        batch = true;
        break;
      default:
        std::cerr << argv[0] << ": Unknown option " << argv[i] << std::endl;
        return 1;
      }
    }
  }
  TApplication app("app", 0, 0);
  if (batch) gROOT->SetBatch(true);

  Compare(a, b);

  if (!batch) app.Run();
  return 0;
}
#endif
//
// EOF
//
