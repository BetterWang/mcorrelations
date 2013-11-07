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
#include <correlations/test/Tester.hh>
#include <iomanip>
#include <iterator>
#include <algorithm>

/**
 * Show usage information
 *
 * @param prog Run name
 */
void
usage(const char* prog)
{
  using correlations::test::helpline;
  std::cout << "Usage: " << prog << " [OPTIONS]\n\n"
            << "Options:" << std::endl;

  helpline(std::cout, 'h', "",         "This help",                         "");
  helpline(std::cout, 'v', "",         "Be verbose",                        "false");
  helpline(std::cout, 'l', "",         "Do not execute nested loops",       "true");
  helpline(std::cout, 'L', "",         "Do execute nested loops",           "false");
  helpline(std::cout, 'i', "FILENAME", "Input file name",                   "data.dat");
  helpline(std::cout, 'o', "FILENAME", "Output file name",                  "MODE.dat");
  helpline(std::cout, 'c', "FILENAME", "Existing file to compare to",       "");
  helpline(std::cout, 'n', "MAXH",     "Maximum correlator",                "6");
  helpline(std::cout, 't', "MODE",     "Which algorithm to use",            "closed");
}

struct to_upper
{
  char operator()(char c) { return std::toupper(c); }
};
struct to_lower
{
  char operator()(char c) { return std::tolower(c); }
};


/**
 * Entry point for program.
 *
 * Analyse generated data file, and exit
 *
 * A previously generated data file (default is @c
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
  bool           loops     = true;
  bool           verbose   = false;
  unsigned short maxH      = 6;
  std::string    input("data.dat");
  std::string    output("");
  std::string    smode("closed");
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'h': usage(argv[0]); return 0;
      case 'l': loops     = false; break;
      case 'L': loops     = true;  break;
      case 'v': verbose   = true;  break;
      case 'n': maxH      = atoi(argv[++i]); break;
      case 'i': input     = argv[++i]; break;
      case 'o': output    = argv[++i]; break;
      case 't': smode     = argv[++i]; break;
      default:
        std::cerr << argv[0] << ": Unknown option " << argv[i] << std::endl;
        return 1;
      }
    }
  }
  using correlations::test::Tester;

  std::transform(smode.begin(),smode.end(), smode.begin(), to_upper());
  std::ifstream in(input.c_str());

  Tester t(in, Tester::str2mode(smode), maxH, loops, verbose);
  while (t.event()) {};
  t.end(std::cout);

  in.close();

  if (output.empty()) {
    std::transform(smode.begin(), smode.end(), smode.begin(), to_lower());
    output = smode;
    output += ".dat";
  }
  std::ofstream out(output.c_str());
  t.save(out);
  out.close();

  return 0;
}
//
// EOF
//
