#ifndef CORRELATIONS_TEST_PRINTER_HH
#define CORRELATIONS_TEST_PRINTER_HH
/**
 * @file   correlations/test/Tester.hh
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 *
 * @brief  An example
 *
 * Copyright (c) 2013 Christian Holm Christensen
 */
#include <correlations/Types.hh>
#include <iostream>
#include <iomanip>
namespace correlations
{
  namespace test
  {
    struct Printer
    {
      /**
       * Field widths
       */
      enum
      {
        TITLEW = 6, COMPLEXW = 27, TIMINGW = 16
      };
      /**
       * Print column title
       *
       * @param out    Output stream
       * @param c1     Title of first column
       * @param c2     Title of second column
       * @param timing Show timing columns too
       */
      static void
      title(std::ostream& out, const char* c1, const char* c2,
          const char* t1 = 0, const char* t2 = 0)
      {
        out << std::left << std::setw(TITLEW) << "N-part" << " |" << "A: "
            << std::setw(COMPLEXW - 3) << c1 << " |" << "B: "
            << std::setw(COMPLEXW - 3) << c2 << " |" << std::setw(COMPLEXW)
            << "Difference (B-A)" << " |";
        if (t1)
          out << std::setw(TIMINGW - 3) << t1 << "[s] |";
        if (t2)
          out << std::setw(TIMINGW - 3) << t2 << "[s] |";
        out << std::endl << std::setfill('-') << std::setw(TITLEW) << "-"
            << "-+" << std::setw(COMPLEXW) << "-" << "-+" << std::setw(COMPLEXW)
            << "-" << "-+" << std::setw(COMPLEXW) << "-" << "-+";
        if (t1)
          out << std::setw(TIMINGW) << "-" << "-+";
        if (t2)
          out << std::setw(TIMINGW) << "-" << "-+";
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
      static bool
      result(std::ostream& out, Size n, const Complex& c1,
          const Complex& c2, Real t1 = 1e6, Real t2 = 1e6)
      {
        std::cout << std::setw(TITLEW) << n << " |" << std::setw(COMPLEXW) << c1
            << " |" << std::setw(COMPLEXW) << c2 << " |" << std::setw(COMPLEXW)
            << (c2 - c1) << " |";
        if (t1 < 1e6)
          out << std::setw(TIMINGW) << t1 << " |";
        if (t2 < 1e6)
          out << std::setw(TIMINGW) << t2 << " |";

        bool ret = true;
        if (std::norm(c2) != 0 && std::norm(c1) != 00
            && (fabs((c2.real() - c1.real()) / c1.real()) > 1e-6
                || fabs((c2.imag() - c1.imag()) / c1.imag()) > 1e-6))
          {
            out << " > (1e-6,1e-6)";
            ret = false;
          }
        out << std::endl;
        return ret;
      }
    };
    /**
     * Print a help-line
     *
     * @param out Output stream
     * @param o   Option character
     * @param arg Dummy argument
     * @param txt Description
     * @param def Default value
     */
    void helpline(std::ostream& out, char o, const char* arg, const char* txt, const char* def)
    {
      out << "  -" << o << " " << std::left << std::setw(8) << arg
                << "  " << std::setw(30) << txt << " [" << def << "]"
                << std::right << std::endl;
    }
  }
}

#endif /* PRINTER_HH_ */
