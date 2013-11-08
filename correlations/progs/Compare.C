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

Bool_t GetHistos(const TString& mode, TH1*& real, TH1*& imag, TProfile*& time)
{
  TString in(mode);
  in.ToLower();
  if (!in.EndsWith(".root")) in.Append(".root");
  TFile* file = TFile::Open(in, "READ");
  if (!file) {
      Error("GetHistos", "Failed to open %s", in.Data());
      return false;
  }
  real = static_cast<TH1*>(file->Get("reals"));
  imag = static_cast<TH1*>(file->Get("imags"));
  time = static_cast<TProfile*>(file->Get("timing"));
  if (!real || !imag || !time) {
    Error("GetHistos", "Failed to find all histograms");
    return false;
  }
  real->SetDirectory(0);
  imag->SetDirectory(0);
  time->SetDirectory(0);

  file->Close();

  return true;
}
void Compare(const TString& mode1, const TString& mode2)
{
  TH1*      r1 = 0;
  TH1*      i1 = 0;
  TH1*      r2 = 0;
  TH1*      i2 = 0;
  TProfile* t1 = 0;
  TProfile* t2 = 0;

  if (!GetHistos(mode1, r1, i1, t1)) return;
  if (!GetHistos(mode2, r2, i2, t2)) return;

  TH1* rd = static_cast<TH1*>(r1->Clone("diffReal"));
  TH1* id = static_cast<TH1*>(i1->Clone("diffImag"));
  TH1* td = static_cast<TH1*>(t1->Clone("diffTime"));

  rd->Add(r2, -1);
  id->Add(i2, -1);
  td->Add(t2, -1);

  rd->Divide(r2);
  id->Divide(i2);
  td->Divide(t2);

  TCanvas* can = new TCanvas("C", "C");
    can->SetTopMargin(0.15);
    can->SetBottomMargin(0.15);
    can->SetRightMargin(0.03);
    can->Divide(1,3, 0, 0);

    TVirtualPad* p = can->cd(3);
    p->SetRightMargin(0.01);
    TH1* copy = td->DrawCopy("hist e");
    copy->GetXaxis()->SetLabelSize(0.13);
    copy->GetYaxis()->SetLabelSize(0.08);


    p = can->cd(1);
    p->SetRightMargin(0.01);
    copy = rd->DrawCopy("hist");
    copy->GetXaxis()->SetLabelSize(0.13);
    copy->GetYaxis()->SetLabelSize(0.08);

    p = can->cd(2);
    p->SetRightMargin(0.01);
    copy = id->DrawCopy("hist");
    copy->GetXaxis()->SetLabelSize(0.13);
    copy->GetYaxis()->SetLabelSize(0.08);

    can->cd(0);
    TLatex* ltx = new TLatex(0.5,0.995,Form("(%s-%s)/%s",
                                            mode1.Data(), mode2.Data(), mode2.Data()));
    ltx->SetNDC(true);
    ltx->SetTextAlign(23);
    ltx->SetTextSize(0.04);
    ltx->Draw();

    can->Modified();
    can->Update();
    can->cd();
}

#ifdef AS_PROG
#include <TROOT.h>
#include <TApplication.h>
#include <iostream>
void usage(const char* progname)
{
  std::cout << "Usage: " << progname << " [OPTIONS]\n\n"
      << "Options:\n"
      << "  -h          This help\n"
      << "  -1 NAME     First set\n"
      << "  -2 NAME     Second set\n"
      << std::flush;
}
int
main(int argc, char** argv)
{
  TString a("closed");
  TString b("recursive");
  bool batch = false;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'h': usage(argv[0]); return 0;
      case '1': a = argv[++i]; break;
      case '2': b = argv[++i]; break;
      case 'b': batch     = false; break;
      case 'B': batch     = true; break;
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
