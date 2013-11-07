#ifndef CORRELATIONS_CORRELATOR_HH
#define CORRELATIONS_CORRELATOR_HH
/**
 * @file   correlations/Correlator.hh
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 *
 * @brief  Base class for correlators
 *
 * Copyright (c) 2013 Christian Holm Christensen
 */
#include <correlations/Result.hh>
#include <correlations/Types.hh>
#include <iostream>

namespace correlations {
  //____________________________________________________________________
  /**
   * Base class for calculating the correlations
   *
   * @headerfile ""  <correlations/Correlator.hh>
   */
  struct Correlator
  {
    enum {
      /** Value to mark ignored harmonics */
      kInvalidN = 0x7FFF
    };
    /**
     * Calculate up to the 8-particle correlator with the specified harmonics.
     *
     * @param h1 Harmonic 1
     * @param h2 Harmonic 2
     * @param h3 Harmonic 3
     * @param h4 Harmonic 4
     * @param h5 Harmonic 5
     * @param h6 Harmonic 6
     * @param h7 Harmonic 7
     * @param h8 Harmonic 8
     *
     * @return The correlation
     */
    Result calculate(const Harmonic h1,
		     const Harmonic h2=kInvalidN,
		     const Harmonic h3=kInvalidN,
		     const Harmonic h4=kInvalidN,
		     const Harmonic h5=kInvalidN,
		     const Harmonic h6=kInvalidN,
		     const Harmonic h7=kInvalidN,
		     const Harmonic h8=kInvalidN) const
    {
      const Harmonic hs[] = { h1, h2, h3, h4, h5, h6, h7, h8 };
      HarmonicVector hv;
      for (Size i = 0; i < 8; i++) {
	if (hs[i] == kInvalidN) break;
	hv.push_back(hs[i]);
      }
      return calculate(hv);
    }
    /**
     * Calculate the multi-particle correlation.
     *
     * @param h Vector of harmonics.  The correlator size is the size
     * of this vector.
     *
     * @return The correlation
     */
    Result calculate(const HarmonicVector& h) const
    {
      return calculate(h.size(), h);
    }
    /**
     * Calculate up the multi-particle  correlator.
     *
     * @param size How many particles to correlate
     * @param h    Harmonics of each term
     *
     * @return The correlation
     */
    Result calculate(const Size size, const HarmonicVector& h) const
    {
#if 0
      if (size > h.size()) {
	std::cerr << "Not enough harmonics (" << h.size() << ") to do "
		  << size << "-particle correlations" << std::endl;
	return Result();
      }
#endif
      if (size > _maxFixed) {
	// If we're above our fixed cut, or above 8, use the generic
	// algorithms
	return multi(size, h);
      }

      // Now, use specific implementations
      switch (size) {
      case 1: return C1(h);   break;
      case 2: return C2(h);   break;
      case 3: return C3(h); break;
      case 4: return C4(h);  break;
      case 5: return C5(h);  break;
      case 6: return C6(h);  break;
      case 7: return C7(h);  break;
      case 8: return C8(h);  break;
      }
      std::cerr << "Number of correlators too big:" << size << std::endl;
      return Result();
    }
    /**
     * @return Name of the correlator
     */
    virtual const char* name() const { return "Correlator"; }
    /**
     * Destructor
     */
    virtual ~Correlator() {}
  protected:
    Correlator(Size maxFixed=8) : _maxFixed(maxFixed > 8 ? 8 : maxFixed) {}
    /**
     * Calculate the @a n particle correlation using harmonics @a h.
     *
     * @param n How many particles to correlate
     * @param h Harmonic of each term
     *
     * @return The correlator and the summed weights
     */
    virtual Result multi(const Size n, const HarmonicVector& h) const = 0;
    /**
     * Calculate 1-particle correlator.
     * @f[
     * C\{1\} = \langle\exp[i(\sum_j^1 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonic @f$ h_j@f$
     *
     * @return @f$ C\{1\}@f$
     */
    virtual Result C1(const HarmonicVector& h) const = 0;
    /**
     * Calculate 2-particle correlator.
     * @f[
     * C\{2\} = \langle\exp[i(\sum_j^2 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$ C\{2\}@f$
     */
    virtual Result C2(const HarmonicVector& h) const = 0;
    /**
     * Calculate 3-particle correlator.
     * @f[
     * C\{3\} = \langle\exp[i(\sum_j^3 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$QC\{3\}@f$
     */
    virtual Result C3(const HarmonicVector& h) const = 0;
    /**
     * Calculate 4-particle correlator.
     * @f[
     * C\{4\} = \langle\exp[i(\sum_j^4 h_j\phi_j)]\rangle
     * @f]
     *
     * @param h Harmonics @f$ h_j@f$
     *
     * @return @f$QC\{4\}@f$
     */
    virtual Result C4(const HarmonicVector& h) const = 0;
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
    virtual Result C5(const HarmonicVector& h) const =0;
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
    virtual Result C6(const HarmonicVector& h) const =0;
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
    virtual Result C7(const HarmonicVector& h) const =0;
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
    virtual Result C8(const HarmonicVector& h) const =0;

    Size _maxFixed;
  };
}
#endif
// Local Variables:
//  mode: C++
// End:
