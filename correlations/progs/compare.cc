/**
 * @file   correlations/progs/compare.cc
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 *
 * @brief  An example
 *
 * Copyright (c) 2013 Christian Holm Christensen
 */
#include <correlations/Types.hh>
#include <correlations/test/Printer.hh>
#include <correlations/test/Comparer.hh>
#include <fstream>

/**
 * Show usage information
 *
 * @param prog Run name
 */
void
usage(const char* prog)
{
  using correlations::test::helpline;
  std::cout << "Usage: " << prog << " [OPTIONS]\n\n" << "Options:" << std::endl;

  helpline(std::cout, 'h', "", "This help", "");
  helpline(std::cout, 'a', "FILENAME", "First (A) file", "");
  helpline(std::cout, 'b', "FILENAME", "Second (B) file", "closed.dat");
}

/**
 * Entry point for program.
 *
 * Compare to output files of analyze
 *
 * @param argc Number of arguments
 * @param argv Vector of arguments
 *
 * @return 0 on success
 */
int
main(int argc,
     char** argv)
{
  std::string a("");
  std::string b("closed.dat");
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'h':
        usage(argv[0]);
        return 0;
      case 'a': a = argv[++i]; break;
      case 'b': b = argv[++i]; break;
      default:
        std::cerr << argv[0] << ": Unknown option " << argv[i] << std::endl;
        return 1;
      }
    }
  }
  std::ifstream af(a.c_str());
  std::ifstream bf(b.c_str());
  if (!af || !bf) {
      std::cerr << "Failed to open one or more of " << a << " and " << b << std::endl;
      return 1;
  }
  bool ret = correlations::test::Comparer::compare(std::cout, af, bf);
  return ret ? 0 : 1;
}
/*
 * EOF
 */
