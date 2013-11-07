#ifndef CORRELATIONS_RECURSIVE_FROMQVECTOR_HH
#define CORRELATIONS_RECURSIVE_FROMQVECTOR_HH
/**
 * @file   correlations/recursive/FromQVector.hh
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 *
 * @brief  Cumulant correlator using recursion
 *
 * Copyright (c) 2013 Christian Holm Christensen
 */
#include <correlations/QVector.hh>
#include <correlations/Types.hh>
#include <correlations/FromQVector.hh>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <iomanip>

namespace correlations {
  /**
   * Namespace for recursive calculations
   */
  namespace recursive {
    //____________________________________________________________________
    /**
     * Structure to calculate Cumulants of arbitrary order and power from
     * a given Q vector.
     *
     * This code used either generic recurssion to calculate @f$
     * QC\{n\}@f$ or - for a limited number of @f$ n@f$ specific
     * recursive functions.
     *
     * @headerfile ""  <correlations/recursive/FromQVector.hh>
     */
    struct FromQVector: public correlations::FromQVector
    {
      /**
       * Constructor
       *
       * @param q Q vector to use
       */
      FromQVector(QVector& q) : correlations::FromQVector(q) { _maxFixed = 0; }
      /**
       *  Destructor
       */
      virtual ~FromQVector() {}
      /**
       * @return Name of the correlator
       */
      virtual const char* name() const { return "Recursive cumulant"; }
    protected:
      /**
       * @{
       * @name Arbitrary order calculations using recursion
       */
      /** A vector of harmonic orders */
      typedef std::vector<Size> SizeVector;
      /**
       * Calculate the multi-particle correlation
       *
       * The calculation is done using the following algorithm
       *
       * @verbatim
       *    C = q(h_{n-1}, cnt_{n-1});
       *    if n == 1 then return c;
       *
       *    C *= UN(n-1, h, cnt);
       *
       *    if (cnt[n-1] > 1) return C;
       *
       *    for i from 0 upto downto n-2 do
       *       let h[i]   = h[i] + h[n-1];
       *       let cnt[i] = cnt[i]-1;
       *
       *       C -= (cnt[i]-1) * UN(n-1, h, cnt);
       *
       *       let cnt[i] = cnt[i]-1;
       *
       *       let h[i]   = h[i] - h[n-1];
       *    end for i
       * @endverbatim
       *
       * @param n Order of correlation (number of particles to correlate)
       * @param h Harmonics
       *
       * @return @f$ QC{n}@f$
       */
      Complex UN(const Size n, const HarmonicVector& h) const
      {
        SizeVector cnt(n);
        HarmonicVector hh(h);
        std::fill(cnt.begin(), cnt.end(), 1);
        return UN2(n, hh, cnt);
      }
      /**
       * Calculate the multi-particle correlation
       *
       *
       * @param n   Order of correlation (number of particles to correlate)
       * @param h   Harmonics
       * @param cnt Vector of term counts
       *
       * @return @f$ QC{n}@f$
       */
      Complex UN2(const Size n, HarmonicVector& h, SizeVector& cnt) const
      {
        Size    j = n-1;
        Complex c = _q(h[j], cnt[j]);
        if (n == 1) return c;

        c *= UN2(j, h, cnt);

        if (cnt[j] > 1) return c;

        for (Size i = 0; i < (n-1); i++) {
          h[i]   += h[j];
          cnt[i] =  cnt[i] + 1;
          c      -= Real(cnt[i] - 1) * UN2(j, h, cnt);

          cnt[i]--;
          h[i] -= h[j];
        }
        return c;
      }
      /* @} */
    };

  }
}

#endif
// Local Variables:
//  mode: C++
// End:

