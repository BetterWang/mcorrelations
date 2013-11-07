/**
 * @file   correlations/test/prog.cc
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 *
 * @brief  An example
 *
 * Copyright (c) 2013 Christian Holm Christensen
 */
#include <correlations/Types.hh>
#include <correlations/Result.hh>
#include <correlations/recursive/FromQVector.hh>
#include <correlations/recursive/NestedLoops.hh>
#include <correlations/recurrence/FromQVector.hh>
#include <correlations/closed/FromQVector.hh>
#include <correlations/test/Random.hh>
#include <correlations/test/ReadData.hh>
#include <correlations/test/WriteData.hh>
#include <iomanip>
#include <iterator>

namespace correlations {
  namespace test {
    /**
     * A Test
     *
     */
    struct Test
    {

      /**
       * Constructor
       */
      Test(std::istream& input, char mode=1, Size maxN=8,
           bool doNested=false, bool verbose=false)
        : fH(maxN),
          fPhis(),
          fWeights(),
          fR(input),
          fQ(0,0, true),
          fC(0),
          fN(0),
          fRC(0),
          fRN(0),
          fS(0),
          fTC(0),
          fTN(0),
          fE(0),
          fV(verbose)
      {
        if (fV) std::cout << "Harmonics:" << std::flush;
        Size sum = 0;
        Random::Seed(54321);
        for (Size i = 0; i < maxN; i++) {
          fH[i] = Random::AsHarmonic(-6, 6);
          if (fV) std::cout << " " << fH[i];
          sum += std::abs(fH[i]);
        }
        if (fV) std::cout << " -> " << sum << std::endl;
        fQ.resize(fH);
        fRC.resize(maxN-1);
        fRN.resize(doNested ? maxN-1 : 0);
        fTC.resize(maxN-1);
        fTN.resize(doNested ? maxN-1 : 0);
        fS = Stopwatch::Create();
        switch (mode) {
        case 0: fC = new correlations::recursive::FromQVector(fQ); break;
        case 1: fC = new correlations::recurrence::FromQVector(fQ); break;
        case 2: fC = new correlations::closed::FromQVector(fQ);
        }
        if (doNested)
                switch (mode) {
                case 0:
                case 1: fN = new correlations::recursive::NestedLoops(fPhis,fWeights,true); break;
                case 2: fN = new correlations::NestedLoops(fPhis, fWeights, true); break;
        }
        if (fV) {
          std::cout << "Cumulant correlator: " << fC->name() << std::endl;
          if (fN)
            std::cout << "Nested loop correlator: " << fN->name() << std::endl;
        }
      }
      /**
       * Make a single event
       *
       */
      bool Event()
      {
        fQ.reset();
        if (!fR.Event(fQ, fPhis, fWeights)) return false;

        fE++;
        if (fV)
          std::cout << "Event # " << std::setw(4) << fE << ": "
                    << std::setw(4) << fPhis.size() << " particles "
                    << std::flush;
        for (Size i = 0; i < fRC.size(); i++) {
          Size n = i+2;
          if (fV) std::cout << (i == 0 ? "" : "..") << n << std::flush;
          fS->Start(true);
          fRC[i] += fC->calculate(n, fH);
          fTC[i] += fS->Stop();
          // fS->Print();
          // std::cout << "Calculated QC" << std::endl;

          if (fN) {
            if (fV) std::cout << '+' << std::flush;
            fS->Start(true);
            fRN[i] += fN->calculate(n, fH);
            fTN[i] += fS->Stop();
          }
          // std::cout << "Calculated nested loops" << std::endl;
        }
        if (fV) std::cout << " done" << std::endl;

        return true;
      }
      /**
       * Do calculations at the end
       *
       */
      void End(std::ostream& out)
      {
        PrintTitle(out, "From Q-vector", "From loops", "T_Q", "T_loops");
        for (Size i = 0; i < fRC.size(); i++) {
          Complex rc = fRC[i].eval();
          Complex rn = fN ? fRN[i].eval() : Complex(0,0);
          Real    tc = fTC[i] / fE;
          Real    tn = fN ? fTN[i] / fE   : -1;
          PrintResult(out, 2+i, rc, rn, tc, tn);
        }
      }
      void Save(std::ostream& out)
      {
        size_t savePrec = out.precision();
        out.precision(16);
        out << "# A total of " << fRC.size() << " cumulants\n"
            << "# Order   QC    NL     t_QC     t_NL" << std::endl;
        for (Size i = 0; i < fRC.size(); i++) {
          Complex rc = fRC[i].eval();
          Complex rn = fN ? fRN[i].eval() : Complex(0,0);
          Real    tc = fTC[i] / fE;
          Real    tn = fN ? fTN[i] / fE : -1;
          out << i+2 << "\t" << rc << "\t" << rn << "\t" << tc << "\t" << tn
              << std::endl;
        }
        out << "# EOF" << std::endl;
        out.precision(savePrec);
      }
      void Compare(std::ostream& out, std::istream& cf)
      {
        ComplexVector krc(fH.size());
        ComplexVector krn(fH.size());
        RealVector    ktc(fH.size());
        RealVector    ktn(fH.size());

        while (!cf.eof()) {
          std::string l;
          std::getline(cf, l, '\n');
          if (cf.bad()) {
            std::cerr << "Error while reading line from comparison file"
                      << std::endl;
            break;
          }
          if (l.empty()) continue;
          if (l[0] == '#') continue;

          std::stringstream sf(l);
          Size              i;
          Complex           rc;
          Complex           rn;
          Real              tc;
          Real              tn;
          sf >> i >> rc >> rn >> tc >> tn;
          if (sf.bad()) {
            std::cerr << "Error while extracting from line " << sf.str()
                      << std::endl;
            break;
          }

          if (i < 2) continue;
          Size n = i - 2;
          if (n >= fH.size()) continue;

          krc[n] = rc;
          krn[n] = rn;
          ktc[n] = tc;
          ktn[n] = tn;
        }

        PrintTitle(out, "Our Q-vector", "Other Q-vector", "(T_B-T_A)/T_A");
        for (Size i = 0; i < fRC.size(); i++) {
          const Complex& r  = fRC[i].eval();
          const Complex& k  = krc[i];
          Real           tc = fTC[i] / fE;
          Real           l  = ktc[i];
          Real           dt = (l-tc) / tc;
          PrintResult(out, 2+i, r, k, dt);
        }
      }
    protected:
      /**
       * Copy constructor - not defined
       */
      Test(const Test&);
      /**
       * Assignment operator - not defines
       */
      Test& operator=(const Test&);
      enum {
        kTitleW = 11,
        kComplexW = 27,
        kTimingW = 12

      };
      /**
       * Print column title
       *
       * @param out    Output stream
       * @param c1     Title of first column
       * @param c2     Title of second column
       * @param timing Show timing columns too
       */
      void PrintTitle(std::ostream& out,
                      const char* c1,
                      const char* c2,
                      const char* t1=0,
                      const char* t2=0) const
      {
        out << std::left
            << std::setw(kTitleW)   << "N-particle" << " |"
            << "A: " << std::setw(kComplexW-3) << c1 << " |"
            << "B: " << std::setw(kComplexW-3) << c2 << " |"
            << std::setw(kComplexW) << "Difference (B-A)" << " |";
        if (t1)
          out << std::setw(kTimingW-3) << t1 << "[s] |";
        if (t2)
          out << std::setw(kTimingW-3) << t2 << "[s] |";
        out << std::endl
            << std::setfill('-')
            << std::setw(kTitleW)   << "-" << "-+"
            << std::setw(kComplexW) << "-" << "-+"
            << std::setw(kComplexW) << "-" << "-+"
            << std::setw(kComplexW) << "-" << "-+";
        if (t1)
          out << std::setw(kTimingW) << "-" << "-+";
        if (t2)
          out << std::setw(kTimingW) << "-" << "-+";
        out << std::setfill(' ') << std::right << std::endl;
      }
      /**
       * Print end result
       *
       * @param out Output stream
       * @param n   Correlator number
       * @param c1  From cumulants
       * @param c2  From nested loops
       * @param t1  From cumulants
       * @param t2  From nested loops
       */
      void PrintResult(std::ostream&  out,
                       Size           n,
                       const Complex& c1,
                       const Complex& c2,
                       Real           t1=1e6,
                       Real           t2=1e6)
      {
        std::cout << std::setw(kTitleW)   << n << " |"
                  << std::setw(kComplexW) << c1 << " |"
                  << std::setw(kComplexW) << c2 << " |"
                  << std::setw(kComplexW) << (c2-c1) << " |";
        if (t1 < 1e6)
          out << std::setw(kTimingW) << t1 << " |";
        if (t2 < 1e6)
          out << std::setw(kTimingW) << t2 << " |";

        if (std::norm(c2) != 0 && std::norm(c1) != 00 &&
            (fabs((c2.real()-c1.real())/c1.real()) > 1e-6 ||
             fabs((c2.imag()-c1.imag())/c2.imag()) > 1e-6)) {
          out << " > (1e-6,1e-6)";
        }
        out << std::endl;
      }
      /** Harmonic vector */
      HarmonicVector fH;
      /** Phi cache */
      RealVector fPhis;
      /** Weight cache */
      RealVector fWeights;
      /** My generator */
      ReadData fR;
      /** Our Q vector */
      QVector fQ;
      /** Correlator that uses cumulants */
      FromQVector* fC;
      /** Correlator that uses nested loops */
      NestedLoops* fN;
      /** Cumulant results */
      ResultVector fRC;
      /** Nested loop results */
      ResultVector fRN;
      /** Stop watch */
      Stopwatch*   fS;
      /** Cumulant timing */
      RealVector fTC;
      /** Nested loop timing */
      RealVector fTN;
      /** Counter of events */
      Size fE;
      /** Be verbose */
      bool fV;
    };
  }
}

void
helpline(char o, const char* arg, const char* txt, const char* def)
{
  std::cout << "  -" << o << " " << std::left << std::setw(8) << arg
            << "  " << std::setw(30) << txt << " [" << def << "]"
            << std::right << std::endl;
}
/**
 * Show usage information
 *
 * @param prog Run name
 */
void
usage(const char* prog)
{
  std::cout << "Usage: " << prog << " [OPTIONS]\n\n"
            << "Options:" << std::endl;

  helpline('h', "",         "This help", "");
  helpline('v', "",         "Be verbose", "false");
  helpline('r', "",         "Use recursive algoritms",    "true");
  helpline('c', "",         "Use closed form algorithms", "false");
  helpline('l', "",         "Do not execute nested loops","true");
  helpline('L', "",         "Do execute nested loops",    "false");
  helpline('i', "FILENAME", "Input file name",            "data.dat");
  helpline('o', "FILENAME", "Output file name",           "recursive/closed.dat");
  helpline('O', "FILENAME", "Existing file to compare to","");
  helpline('n', "MAXH",     "Maximum correlator",         "6");
  helpline('w', "",         "Write data file, and exit",  "false");
  helpline('e', "NEVENTS",  "Number of events to write",  "100");
  helpline('m', "NPART",    "Least number of particles/events","800");
  helpline('M', "NPART",    "Largest number of particles/events","1000");
}

/**
 * Entry point for program.
 *
 * The program can be run in two modes:
 *
 * - Generation:  Generate data file, and exit
 * - Analysis: Analyse generated data file, and exit
 *
 * Generation is triggered by the flag @c -w.  How many events to make
 * are set by the option @c -e, and the multiplicity range is
 * specified with options @c -m and @c -M.  The result is written to
 * the file given by the option @c -i, or @c data.dat if not specified
 *
 * In analysis mode, a previously generated data file (default is @c
 * data.datm but can be set with the option @c -i) is read in and the
 * 2-particle to the @f$ n@f$-particle correlations are calculated.
 * The maximum @f$ n@f$ to calculate is given by the option @c -n
 *
 * The analysis can be done in two ways, selected by the options @c -c
 * and @c -r.
 *
 * - using closed form expressions for the @f$ QC\{n\}@f$ correlator,
 *   and fully specified nested loops. In this case, @f$ n@f$ can at
 *   max be 8.
 * - using recursive functions.  In this case there's no limit to what
 *   @f$ n@f$ can be - except for limitions imposed by computing speed
 *   and run-time memory.
 *
 * The result of the analysis, together with timing information - is
 * written to the file specified with the option @c -o. If @c -o is
 * not specified, then the output defaults to @c closed.dat for closed
 * form, full loop calculations, and @c recursive.dat for recursive
 * cumulant and nested loop calculations
 *
 * @param argc Number of arguments
 * @param argv Vector of arguments
 *
 * @return 0 on success
 */
int
main(int argc, char** argv)
{
  bool           write     = false;
  bool           loops     = true;
  bool           verbose   = false;
  unsigned short maxH      = 6;
  unsigned short nEvents   = 100;
  unsigned short minN      = 800;
  unsigned short maxN      = 1000;
  std::string    input("data.dat");
  std::string    output("");
  std::string    compare("");
  std::string    smode("");
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'h': usage(argv[0]); return 0;
      case 'w': write     = true;  break;
      case 'l': loops     = false; break;
      case 'L': loops     = true;  break;
      case 'v': verbose   = true;  break;
      case 'n': maxH      = atoi(argv[++i]); break;
      case 'e': nEvents   = atoi(argv[++i]); break;
      case 'm': minN      = atoi(argv[++i]); break;
      case 'M': maxN      = atoi(argv[++i]); break;
      case 'i': input     = argv[++i]; break;
      case 'o': output    = argv[++i]; break;
      case 'O': compare   = argv[++i]; break;
      case 't': smode     = argv[++i]; break;
      default:
        std::cerr << argv[0] << ": Unknown option " << argv[i] << std::endl;
        return 1;
      }
    }
  }
  if (write) {
    std::ofstream out(input.c_str());
    correlations::test::WriteData writer(minN, maxN);
    for (unsigned short ev = 0; ev < nEvents; ev++)
      writer.Event(out, ev);
    out.close();
    return 0;
  }

  char mode = 2;
  if      (smode == "recursive")  mode=0;
  else if (smode == "recurrence") mode=1;

  std::ifstream in(input.c_str());
  correlations::test::Test t(in, mode, maxH, loops, verbose);
  while (t.Event()) {};
  t.End(std::cout);
  in.close();

  if (output.empty()) { output = smode; output += ".dat"; }
  std::ofstream out(output.c_str());
  t.Save(out);
  out.close();

  if (compare.empty()) return 0;

  std::ifstream comp(compare.c_str());
  t.Compare(std::cout, comp);
  comp.close();

  return 0;
}
//
// EOF
//
