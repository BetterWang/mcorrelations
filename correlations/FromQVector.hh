#ifndef CORRELATIONS_FROMQVECTOR_HH
#define CORRELATIONS_FROMQVECTOR_HH
/**
 * @file   correlations/FromQVector.hh
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 *
 * @brief  Base class for cumulant code
 *
 * Copyright (c) 2013 Christian Holm Christensen
 */
#include <correlations/QVector.hh>
#include <correlations/Types.hh>
#include <correlations/Correlator.hh>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <iomanip>

namespace correlations {
  //____________________________________________________________________
  /**
   * Base class to calculate Cumulants of a given Q vector.
   *
   * @code
   * correlations::QVector q(maxN, maxP);
   * correlations:recursive::Cumulant c(q);
   *
   * for (size event = 0; event < nEvents; event++) {
   *   // Get the event data
   *
   *   // Fill into Q-vector
   *   q.Reset();
   *   for (size_t obs = 0; obs < nPhi; obs++) q.Fill(phi[obs],weight[obs])
   *
   *   // Do the QC calculations
   *   correlations::Harmonic n[] = { 2, 1 }
   *   correlations::Result two = c.Calculate(n);
   *   std::cout << two.Eval() << std::endl;
   * }
   * @endcode
   * @headerfile ""  <correlations/FromQVector.hh>
   */
  struct FromQVector : public Correlator
  {
    virtual ~FromQVector() {}
  protected:
    /**
     * Constructor
     *
     * @param q Q vector to use
     */
    FromQVector(QVector& q) : Correlator(0), _q(q) {}
    /**
     * @{
     * @name Drivers
     */
    /**
     * Calculate the @a n particle correlation using harmonics @a h
     *
     * @param n How many particles to correlate
     * @param h Harmonic of each term
     *
     * @return The correlator and the summed weights
     */
    virtual Result multi(const Size n, const HarmonicVector& h) const
    {
      static HarmonicVector null(_q.maxN());
      static bool filled = false;
      if (!filled) {
        std::fill(null.begin(), null.end(), 0);
        filled = true;
      }
      return Result(UN(n, h), UN(n, null).real());
    }
    /**
     * Calculate 1-particle correlation.
     * @f[
     * QC\{1\} = \langle\exp[i(h_1\phi_1)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$ QC\{1\}@f$
     */
    virtual Result C1(const HarmonicVector& h) const
    {
      return Result(U1(h[0]), U1(0).real());
    }
    /**
     * Generic two-particle correlation.
     * @f[
     * QC\{2\} = \langle\exp[i(\sum_j^2 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$ QC\{2\}@f$
     */
    virtual Result C2(const HarmonicVector& h) const
    {
      return Result(U2(h[0], h[1]), U2(0, 0).real());
    }
    /**
     * Generic 3-particle correlation.
     * @f[
     * QC\{3\} = \langle\exp[i(\sum_j^3 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$ QC\{3\}@f$
     */
    virtual Result C3(const HarmonicVector& h) const
    {
      return Result(U3(h[0], h[1], h[2]), U3(0,0,0).real());
    }
    /**
     * Generic 4-particle correlation.
     * @f[
     * QC\{4\} = \langle\exp[i(\sum_j^4 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$ QC\{4\}@f$
     */
    virtual Result C4(const HarmonicVector& h) const
    {
      return Result(U4(h[0],h[1],h[2],h[3]), U4(0,0,0,0).real());
    }
    /**
     * Calculate 5-particle correlator.
     * @f[
     * C\{5\} = \langle\exp[i(\sum_j^5 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$QC\{5\}@f$
     */
    virtual Result C5(const HarmonicVector& h) const
    {
      return Result(U5(h[0], h[1], h[2], h[3], h[4]),
                    U5(0, 0, 0, 0, 0).real());
    }
    /**
     * Calculate 6-particle correlator.
     * @f[
     * C\{6\} = \langle\exp[i(\sum_j^6 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$QC\{6\}@f$
     */
    virtual Result C6(const HarmonicVector& h) const
    {
      return Result(U6(h[0],h[1],h[2],h[3],h[4],h[5]),
                    U6(0,   0,   0,   0,   0,   0).real());
    }
    /**
     * Calculate 7-particle correlator.
     * @f[
     * C\{7\} = \langle\exp[i(\sum_j^7 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$QC\{7\}@f$
     */
    virtual Result C7(const HarmonicVector& h) const
    {
      std::cout << "Calculating C7" << std::endl;
      return Result(U7(h[0],h[1],h[2],h[3],h[4],h[5],h[6]),
                    U7(0,   0,   0,   0,   0,   0,   0).real());
    }
    /**
     * Calculate 8-particle correlator.
     * @f[
     * C\{8\} = \langle\exp[i(\sum_j^8 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$QC\{8\}@f$
     */
    virtual Result C8(const HarmonicVector& h) const
    {
      std::cout << "Calculating C8" << std::endl;
      return Result(U8(h[0],h[1],h[2],h[3],h[4],h[5],h[6],h[7]),
                    U8(0,   0,   0,   0,   0,   0,   0,   0).real());
    }
    /* @} */
    /**
     * @{
     * @name Abstract interface for fixed calculations
     */
    /**
     * Calculate the @f$ n@f$$-paricle correlator using harmonics
     * @f$ h = (h_1,\ldots,h_2)@f$
     *
     * @param n The number of particles to correlate
     * @param h Vector of harmonics.  @f$ n@f$ is set to the si
     *
     * @return @f$ \langle\exp[i(\sum_j^n h_j\phi_j)]\rangle@f$
     */
    virtual Complex UN(const Size n, const HarmonicVector& h) const = 0;
    /**
     * Generic 1-particle correlation.
     * @f[
     * QC\{1\} = \langle\exp[i(\sum_j^1 h_j\phi_j)]\rangle
     * @f]
     *
     * @param n1 Harmonics @f$ h_1@f$
     *
     * @return @f$ QC\{1\}@f$
     */
    virtual Complex U1(const Harmonic n1) const
    {
      return _q(n1, 1);
    }
    /**
     * Generic 2-particle correlation.
     * @f[
     * QC\{2\} = \langle\exp[i(\sum_j^2 h_j\phi_j)]\rangle
     * @f]
     *
     * @param n1 1st Harmonic
     * @param n2 2nd Harmonic
     *
     * @return the correlator
     */
    virtual Complex U2(const Harmonic n1, const Harmonic n2) const
    {
      HarmonicVector h(2);
      h[0] = n1; h[1] = n2;
      return UN(h.size(), h);
    }
    /**
     * Generic 3-particle correlation.
     * @f[
     * QC\{3\} = \langle\exp[i(\sum_j^3 h_j\phi_j)]\rangle
     * @f]
     *
     * @param n1 1st Harmonic
     * @param n2 2nd Harmonic
     * @param n3 3rd Harmonic
     *
     * @return the correlator
     */
    virtual Complex U3(const Harmonic n1,
                       const Harmonic n2,
                       const Harmonic n3) const
    {
      HarmonicVector h(3);
      h[0] = n1; h[1] = n2; h[2] = n3;
      return UN(h.size(), h);
    }
    /**
     * Generic 4-particle correlation.
     * @f[
     * QC\{4\} = \langle\exp[i(\sum_j^4 h_j\phi_j)]\rangle
     * @f]
     *
     *
     * @param n1 1st Harmonic
     * @param n2 2nd Harmonic
     * @param n3 3rd Harmonic
     * @param n4 4th Harmonic
     *
     * @return the correlator
     */
    virtual Complex U4(const Harmonic n1,
                       const Harmonic n2,
                       const Harmonic n3,
                       const Harmonic n4) const
    {
      HarmonicVector h(4);
      h[0] = n1; h[1] = n2; h[2] = n3; h[3] = n4;
      return UN(h.size(), h);
    }
    /**
     * Generic 5-particle correlation.
     * @f[
     * QC\{5\} = \langle\exp[i(\sum_j^5 h_j\phi_j)]\rangle
     * @f]
     *
     * @param n1 1st Harmonic
     * @param n2 2nd Harmonic
     * @param n3 3rd Harmonic
     * @param n4 4th Harmonic
     * @param n5 5th Harmonic
     *
     * @return The correlator
     */
    virtual Complex U5(const Harmonic n1,
                       const Harmonic n2,
                       const Harmonic n3,
                       const Harmonic n4,
                       const Harmonic n5) const
    {
      HarmonicVector h(5);
      h[0] = n1; h[1] = n2; h[2] = n3; h[3] = n4;
      h[4] = n5;
      return UN(h.size(), h);
    }
    /**
     * Calculate 6-particle correlator
     * @f[
     * C\{6\} = \langle\exp[i(\sum_j^6 h_j\phi_j)]\rangle
     * @f]
     *
     * @param n1 1st Harmonic
     * @param n2 2nd Harmonic
     * @param n3 3rd Harmonic
     * @param n4 4th Harmonic
     * @param n5 5th Harmonic
     * @param n6 6th Harmonic
     *
     * @return @f$QC\{6\}@f$
     */
    virtual Complex U6(const Harmonic n1,
                       const Harmonic n2,
                       const Harmonic n3,
                       const Harmonic n4,
                       const Harmonic n5,
                       const Harmonic n6) const
    {
      HarmonicVector h(6);
      h[0] = n1; h[1] = n2; h[2] = n3; h[3] = n4;
      h[4] = n5; h[5] = n6; // h[6] = n7; h[7] = n8;
      return UN(h.size(), h);
    }
    /**
     * Calculate 7-particle correlator
     * @f[
     * C\{7\} = \langle\exp[i(\sum_j^7 h_j\phi_j)]\rangle
     * @f]
     *
     * @param n1 1st Harmonic
     * @param n2 2nd Harmonic
     * @param n3 3rd Harmonic
     * @param n4 4th Harmonic
     * @param n5 5th Harmonic
     * @param n6 6th Harmonic
     * @param n7 7th Harmonic
     *
     * @return @f$QC\{7\}@f$
     */
    virtual Complex U7(const Harmonic n1,
                       const Harmonic n2,
                       const Harmonic n3,
                       const Harmonic n4,
                       const Harmonic n5,
                       const Harmonic n6,
                       const Harmonic n7) const
    {
      HarmonicVector h(7);
      h[0] = n1; h[1] = n2; h[2] = n3; h[3] = n4;
      h[4] = n5; h[5] = n6; h[6] = n7; // h[7] = n8;
      return UN(h.size(), h);
    }
    /**
     * Calculate 8-particle correlator
     * @f[
     * C\{8\} = \langle\exp[i(\sum_j^8 h_j\phi_j)]\rangle
     * @f]
     *
     * @param n1 1st Harmonic
     * @param n2 2nd Harmonic
     * @param n3 3rd Harmonic
     * @param n4 4th Harmonic
     * @param n5 5th Harmonic
     * @param n6 6th Harmonic
     * @param n7 7th Harmonic
     * @param n8 8th Harmonic
     *
     * @return @f$QC\{8\}@f$
     */
    virtual Complex U8(const Harmonic n1,
                       const Harmonic n2,
                       const Harmonic n3,
                       const Harmonic n4,
                       const Harmonic n5,
                       const Harmonic n6,
                       const Harmonic n7,
                       const Harmonic n8) const
    {
      HarmonicVector h(8);
      h[0] = n1; h[1] = n2; h[2] = n3; h[3] = n4;
      h[4] = n5; h[5] = n6; h[6] = n7; h[7] = n8;
      return UN(h.size(), h);
    }
    /* @} */
    QVector& _q;
  };
}
#endif
// Local Variables:
//  mode: C++
// End: