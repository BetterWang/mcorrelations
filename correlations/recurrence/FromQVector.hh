#ifndef CORRELATIONS_RECURRANCE_FROMQVECTOR_HH
#define CORRELATIONS_RECURRANCE_FROMQVECTOR_HH
/**
 * @file   correlations/recurrance/FromQVector.hh
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
  namespace recurrence {
    //____________________________________________________________________
    /**
     * Structure to calculate Cumulants of arbitrary order and power from
     * a given Q vector.
     *
     * This code used either generic recurssion to calculate @f$
     * QC\{n\}@f$ or - for a limited number of @f$ n@f$ specific
     * recursive functions.
     *
     * @headerfile ""  <correlations/recurrance/FromQVector.hh>
     */
    struct FromQVector : public correlations::FromQVector
    {
      /**
       * Constructor
       *
       * @param q Q vector to use
       */
      FromQVector(QVector& q) : correlations::FromQVector(q) { _maxFixed=3; }
      /**
       * @return Name of the correlator
       */
      virtual const char* name() const { return "Recurrance cumulant"; }
    protected:
      /**
       * @{
       * @name Fixed expression
       */
      /**
       * Calculate
       * @f[
       * QC\{1\} = \langle\exp[i(h_1\phi_1)]\rangle
       * @f]
       *
       * @param n1 Harmonics @f$ h_1@f$
       *
       * @return @f$ QC\{1\}@f$
       */
      Complex U1(const Harmonic n1) const
      {
        return _q(n1, 1);
      }
      /**
       * Do the 2-particle calculation
       *
       * @param n1 1st Harmonic
       * @param n2 2nd Harmonic
       *
       * @return the correlator
       */
      Complex U2(const Harmonic n1, const Harmonic n2) const
      {
        return _q(n1,1) * _q(n2,1) - _q(n1+n2,2);
      }
      /**
       * Do the 3-particle calculation
       *
       * @param n1 1st Harmonic
       * @param n2 2nd Harmonic
       * @param n3 3rd Harmonic
       *
       * @return the correlator
       */
      Complex U3(const Harmonic n1,
                  const Harmonic n2,
                  const Harmonic n3) const
      {
        const Real c2 = 2;
        return (U2(n1,n2) * _q(n3,1)
                - _q(n2,1) * _q(n1+n3,2)
                - _q(n1,1) * _q(n2+n3,2)
                + c2 * _q(n1+n2+n3,3));
      }
      /**
       * Do the 4-particle calculation
       * @f[
       * QC_{1234}\{4\} = QC_{123}\{3\} QC_{4}\{1\}
       *   - QC_{23}\{2\} QC_{1+4}\{2\}^2
       *   - QC_{13}\{2\} QC_{2+4}\{2\}^2
       *   - QC_{12}\{2\} QC_{3+4}\{2\}^2
       *   + 2 QC_{3}\{1\} QC_{1+2+4}\{1\}^3
       *   + 2 QC_{2}\{1\} QC_{1+3+4}\{1\}^3
       *   + 2 QC_{1}\{1\} QC_{2+3+4}\{1\}^3
       *   - 6 QC_{1+2+3+4}\{1\}^4
       * @f]
       *
       * @param n1 1st Harmonic
       * @param n2 2nd Harmonic
       * @param n3 3rd Harmonic
       * @param n4 4th Harmonic
       *
       * @return the correlator
       */
      Complex U4(const Harmonic n1,
                  const Harmonic n2,
                  const Harmonic n3,
                  const Harmonic n4) const
      {
        const Real c2 = 2;
        const Real c6 = 6;
        return (U3(n1,n2,n3)*_q(n4,1)
                - U2(n2,n3) * _q(n1+n4,2)
                - U2(n1,n3) * _q(n2+n4,2)
                - U2(n1,n2) * _q(n3+n4,2)
                + c2 * U1(n3) * _q(n1+n2+n4,3)
                + c2 * U1(n2) * _q(n1+n3+n4,3)
                + c2 * U1(n1) * _q(n2+n3+n4,3)
                - c6 * _q(n1+n2+n3+n4,4));
      }
      /**
       * @f[
       * QC_{12345}\{5\} = QC_{1234}\{4\} QC_5\{1\}
       *   - QC_{234}\{3\} QC_{1+5}\{1\}^2
       *   - QC_{134}\{3\} QC_{2+5}\{1\}^2
       *   - QC_{124}\{3\} QC_{3+5}\{1\}^2
       *   - QC_{123}\{3\} QC_{3+5}\{1\}^2
       *   + 2 QC_{34}\{2\} QC_{1+2+5}\{1\}^3
       *   + 2 QC_{24}\{2\} QC_{1+3+5}\{1\}^3
       *   + 2 QC_{14}\{2\} QC_{2+3+5}\{1\}^3
       *   + 2 QC_{23}\{2\} QC_{1+4+5}\{1\}^3
       *   + 2 QC_{13}\{2\} QC_{2+4+5}\{1\}^3
       *   + 2 QC_{12}\{2\} QC_{3+4+5}\{1\}^3
       *   - 6 QC_4\{1\} QC_{1+2+3+5}\{1\}^4
       *   - 6 QC_3\{1\} QC_{1+2+4+5}\{1\}^4
       *   - 6 QC_2\{1\} QC_{1+3+4+5}\{1\}^4
       *   - 6 QC_1\{1\} QC_{2+3+4+5}\{1\}^4
       *   + 24 QC_{1+2+3+4+5}\{1\}^5
       * @f]
       *
       *
       * @param n1 1st Harmonic
       * @param n2 2nd Harmonic
       * @param n3 3rd Harmonic
       * @param n4 4th Harmonic
       * @param n5 5th Harmonic
       *
       * @return The correlator
       */
      Complex U5(const Harmonic n1,
                  const Harmonic n2,
                  const Harmonic n3,
                  const Harmonic n4,
                  const Harmonic n5) const
      {
        const Real c2  = 2;
        const Real c6  = 6;
        const Real c24 = 6;
        return (U4(n1,n2,n3,n4) * _q(n5, 1)
                - U3(n2,n3,n4) * _q(n1+n5,2)
                - U3(n1,n3,n4) * _q(n2+n5,2)
                - U3(n1,n2,n4) * _q(n3+n5,2)
                - U3(n1,n2,n3) * _q(n4+n5,2)
                + c2 * U2(n3,n4) * _q(n1+n2+n5,3)
                + c2 * U2(n2,n4) * _q(n1+n3+n5,3)
                + c2 * U2(n1,n4) * _q(n2+n3+n5,3)
                + c2 * U2(n2,n3) * _q(n1+n4+n5,3)
                + c2 * U2(n1,n3) * _q(n2+n4+n5,3)
                + c2 * U2(n1,2)  * _q(n3+n4+n5,3)
                - c6 * U1(n4)    * _q(n1+n2+n3+n5,4)
                - c6 * U1(n3)    * _q(n1+n2+n4+n5,4)
                - c6 * U1(n2)    * _q(n1+n3+n4+n5,4)
                - c6 * U1(n1)    * _q(n2+n3+n4+n5,4)
                + c24 * _q(n1+n2+n3+n4+n5,5));
      }
      /* @} */
      /**
       * @{
       * @name Arbitrary order calculations using recursion
       */
      /** A vector of harmonic orders */
      typedef std::vector<Size> SizeVector;
      /**
       * Return the next combination of @a k element from the range
       * [@a first, @a last).
       *
       * @param first  Start of the range
       * @param k      Number of elements to get
       * @param last   End of range
       *
       * @return true if there are more (unique) combinations
       */
      static bool nextCombination(const SizeVector::iterator first,
                                  SizeVector::iterator       k,
                                  const SizeVector::iterator last)
      {
        /* Credits: Mark Nelson http://marknelson.us */
        if ((first == last) /* No range? */
            || (first == k) /* Nothing to select? */
            || (last == k)  /* Nothing to select? */) return false;
        SizeVector::iterator i1 = first;
        SizeVector::iterator i2 = last;
        ++i1; // Go in one
        if (last == i1)
          // If this point to the end, there's nothing to selec from
          return false;

        // Point to the second to last
        i1 = last;
        --i1;
        // Point at the end of our selection
        i1 = k;
        --i2;

        // Then loop backward to the start
        while (first != i1) {
          if (*--i1 < *i2) {
            // If the value at marker is less than the last entry,
            // then point to our division
            SizeVector::iterator j = k;

            // and if it's not smaller than current value, step into
            // unused territory
            while (!(*i1 < *j)) ++j;

            // Swap the greater value with our current object
            std::iter_swap(i1,j);

            // Increment one
            ++i1;
            ++j;

            // and go back to the divider
            i2 = k;

            // Rotate elements in the range i1->j, and put them at the end
            std::rotate(i1,j,last);

            // Then, while we haven't found our greater value,
            // increment into unused territory
            while (last != j) {
              ++j;
              ++i2;
            }

            // And rotate
            std::rotate(k,i2,last);
            return true;
          }
        }
        // And rotate
        std::rotate(first,k,last);
        return false;
      }
      /**
       * Calculate the multi-particle correlation
       *
       * The calculation is done using the following algorithm
       *
       * @verbatim
       *    C = (0,0)
       *    for k from n-1 downto 0 do
       *       for each combination c of k harmonics except h_n do
       *         let m = sum of harmonics not in c
       *         let p = number of harmonics not in c
       *         let s = -1^(n-k)
       *         C += s * (n-1-k)! * QC{k}(combination) * Q(m,p)
       *       end for each c
       *    end for k
       * @endverbatim
       *
       * @param n Order of correlation (number of particles to correlate)
       * @param h Harmonics
       *
       * @return @f$ QC{n}@f$
       */
      Complex UN(const Size n, const HarmonicVector& h) const
      {
        SizeVector v(n-1); // Allocate cache here
        return UN2(n, h, v);
      }
      /**
       * Calculate the multi-particle correlation
       *
       * The calculation is done using the following algorithm
       *
       * @verbatim
       *    C = (0,0)
       *    for k from n-1 downto 0 do
       *       for each combination c of k harmonics except h_n do
       *         let m = sum of harmonics not in c
       *         let p = number of harmonics not in c
       *         let s = -1^(n-k)
       *         C += s * (n-1-k)! * QC{k}(combination) * Q(m,p)
       *       end for each c
       *    end for k
       * @endverbatim
       *
       * @param n Order of correlation (number of particles to correlate)
       * @param h Harmonics
       * @param v Cache - note, this is larger than it need be, so we
       * take care of iterations by being explicit about the bounds
       *
       * @return @f$ QC{n}@f$
       */
      Complex UN2(const Size n,
                  const HarmonicVector& h,
                  SizeVector v) const
      {
        if (n == 0) return Complex(1,0);
        if (n == 1) return U1(h[0]);

        // Make vector of harmonics - expensive allocation
        HarmonicVector hh(n);
        hh[n-1] =  h[n-1];

        // Make vector of indices
        // SizeVector v(n-1);
        // for (size_t i = 0; i < v.size(); i++) v[i] = i;

        Complex r; // = x;
        Real    f = 1;
        Real    s = 1;
        Power   p = 1;
        for (Size m = n; m > 0; m--) {
          // Reset indices for combinations
          for (size_t i = 0; i < size_t(n-1); i++) v[i] = i;
          Size k  =  m-1;
          do {
            for (size_t i = 0; i < size_t(n-1); i++) hh[i] = h[v[i]];
            // Harmonic second = 0;
            // for (size_t i = k; i < n-1; i++) second += h[v[i]];

            Complex t = UN2(k, hh, v);

            // The calculation
            Harmonic a = std::accumulate(hh.begin()+k, hh.begin()+n,0);
            Complex  x = s * f * t * _q(a, p);
            r += x;
          } while (nextCombination(v.begin(), v.begin()+k, v.begin()+n-1));
          f       *= (n-k);
          s       *= -1;
          p++;
        }
        return r;
      }
      /* @} */
    };
  }
}
#endif
// Local Variables:
//  mode: C++
// End: