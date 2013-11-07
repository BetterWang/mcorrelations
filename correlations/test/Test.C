/*
 * This script should be AcLic compiled - i.e.,
 *
 *   Root> .L test/Test.C++
 *
 * due to the nested loops
 */
#ifndef __CINT__
# include <correlations/Types.hh>
# include <correlations/Result.hh>
# include <correlations/QVector.hh>
# include <correlations/recursive/FromQVector.hh>
# include <correlations/recurrence/FromQVector.hh>
# include <correlations/closed/FromQVector.hh>
# include <correlations/test/ReadData.hh>
# include <TH1.h>
# include <TProfile.h>
# include <TCanvas.h>
# include <TStopwatch.h>
# include <TRandom.h>
# include <TMath.h>
#else
// To force loading of libraries
class TF1;
class TH1F;
class TProfile;
class TFile;
class TCanvas;
#endif

/**
 * A test
 *
 * @param maxH     Max harmonic
 * @param nEvents  Number of events
 * @param minN     Least multiplicity
 * @param maxN     Largest multiplicity
 */
void
Test(Char_t      mode=true,
     UShort_t    maxH=6,
     const char* input="data.dat",
     Bool_t      doLoops=false)
{
#ifdef __CINT__
  gROOT->LoadMacro("correlations/Types.hh++");
  gROOT->LoadMacro("correlations/Result.hh++");
  gROOT->LoadMacro("correlations/QVector.hh++");
  gROOT->LoadMacro("correlations/recursive/Cumulant.hh++");
  gROOT->LoadMacro("correlations/closed/Cumulant.hh++");
  gROOT->LoadMacro("correlations/test/ReadData.hh++");

#endif


  // --- Some histograms ---------------------------------------------
  TH1* phiDist = new TH1D("phiDist", "(#phi-#Phi_{R}) distribution",
			  72, 0, TMath::TwoPi());
  phiDist->SetFillColor(kGreen+1);
  phiDist->SetFillStyle(3001);
  phiDist->SetXTitle("#phi-#Phi_{R}");

  TProfile* timing = new TProfile("timing", "Timing", maxH-2+1, 2, maxH+1);
  timing->SetFillColor(kRed+1);
  timing->SetFillStyle(3001);
  for (UShort_t i = 0; i < maxH-1; i++)
    timing->GetXaxis()->SetBinLabel(i+1, Form("QC{%d}", i+2));

  TStopwatch timer;

  // --- Setup of harmonics, etc -------------------------------------
  std::ifstream in(input);
  correlations::test::ReadData r(in);
  correlations::QVector        q(0,0,false);
  correlations::FromQVector*   c;
  correlations::HarmonicVector h(maxH);
  for (UShort_t i = 0; i < maxH; i++)
    // Generate random harmonicx
    h[i] = -6 + gRandom->Integer(12);

  // Resize the Q-vector to fit the harmonics
  q.resize(h);
  switch (mode) {
  case 0: c = new correlations::recursive::FromQVector(q); break;
  case 1: c = new correlations::recurrence::FromQVector(q); break;
  case 2: c = new correlations::closed::FromQVector(q);
  }
  Printf("Correlator: %s", c->name());

  // --- The results -------------------------------------------------
  const UShort_t             nQ = maxH - 1;
  correlations::ResultVector qs(nQ);
  correlations::RealVector   phis;
  correlations::RealVector   weights;

  // --- Event loop --------------------------------------------------
  UShort_t event = 0;
  while (r.Event(q, phis, weights)) {
    printf("Event # %4d  %4lu particles ", event++, phis.size());
    for (UShort_t i = 0; i < nQ; i++) {
      UShort_t n = i + 2;
      printf("%s%d", i == 0 ? "" : "..", n);
      timer.Reset();
      timer.Start();
      qs[i] += c->calculate(n, h);
      timer.Stop();
      timing->Fill(n+.5, timer.CpuTime());
    }
    for (UShort_t i = 0; i < phis.size(); i++) {
      Double_t phi = phis[i] - r.PhiR();
      if (phi < 0) phi += TMath::TwoPi();
      phiDist->Fill(phi);
    }
    printf(" done\n");
  }
  in.close();

  for (UShort_t i = 0; i < nQ; i++) {
    UShort_t iq = i+2;
    correlations::Complex rc = qs[i].eval();
    Printf("QC{%2d}: %12g + %12gi  <t>: %10gs",
	   iq, rc.real(), rc.imag(), timing->GetBinContent(i+1));
  }
  TCanvas* can = new TCanvas("C", "C");
  can->Divide(2,1);
  can->cd(1);
  phiDist->DrawCopy();
  TVirtualPad* p = can->cd(2);
  p->SetLogy();
  timing->DrawCopy("hist e");
  can->Modified();
  can->Update();
  can->cd();
  // can->Print(recursive ? "recursive.png" : "closed.png");
}

#ifdef AS_PROG
#include <string>
int
main(int argc, char** argv)
{
  bool           recursive = true;
  bool           loops     = true;
  unsigned short maxH      = 6;
  std::string    input("data.dat");

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'r': recursive = true;  break;
      case 'c': recursive = false; break;
      case 'l': loops     = false; break;
      case 'L': loops     = true; break;
      case 'n': maxH      = atoi(argv[++i]); break;
      case 'i': input     = argv[++i]; break;
      default:
	std::cerr << argv[0] << ": Unknown option " << argv[i] << std::endl;
	return 1;
      }
    }
  }
  Test(recursive, maxH, input.c_str(), loops);

  return 0;
}
#endif
//
// EOF
//
