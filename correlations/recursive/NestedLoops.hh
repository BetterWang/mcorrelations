#ifndef CORRELATIONS_RECURSIVE_NESTEDLOOPS_HH
#define CORRELATIONS_RECURSIVE_NESTEDLOOPS_HH
/**
 * @file   correlations/recursive/NestedLoops.hh
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 *
 * @brief  Nested loop correlator that using recursion
 *
 * Copyright (c) 2013 Christian Holm Christensen
 */
#include <correlations/NestedLoops.hh>

namespace correlations {
  namespace recursive {
    //____________________________________________________________________
    /**
     * Structure to calculate the cumulants using nested loops
     *
     * This calculates the loop using recursion
     *
     * @headerfile ""  <correlations/recursive/NestedLoops.hh>
     */
    struct NestedLoops : public correlations::NestedLoops
    {
      /**
       * Constructor
       *
       * @param phis       Reference to phi array that will be filled
       * @param weights    Reference to weight array that will be filled
       * @param useWeights Whether to use weights or not
       */
      NestedLoops(RealVector& phis,
		  RealVector& weights,
		  bool        useWeights=true)
	: correlations::NestedLoops(phis, weights, useWeights)
      {}
      /**
       * @return Name of the correlator
       */
      virtual const char* name() const { return "Recursive loops"; }
    protected:
      /**
       * Do one loop over the input data
       *
       * @param cur   Current loop level (0 based, up to @a depth-1)
       * @param depth Maximum loop depth
       * @param idx   Current particle indices
       * @param h     Harmonics
       * @param p     Current list of angles scaled by harmonics
       * @param w     Current list of weights
       * @param c     The complex number to sum in
       * @param sumw  The number to sum weights in
       */
      void loop(const Size      cur,
		const Size      depth,
		Size*           idx,
		const HarmonicVector& h,
		Real*           p,
		Real*           w,
		Complex&        c,
		Real&           sumw) const
      {

	for (idx[cur] = 0; idx[cur] < _phis.size(); idx[cur]++) {
	  // Check for auto-correlation
	  if (!store(cur, idx, p, w, h)) continue;
	  if (cur == depth-1)
	    term(depth, w, p, c, sumw);
	  else
	    loop(cur+1u, depth, idx, h, p, w, c, sumw);
	}
      }
      /**
       * Calculate the @a n particle correlation using harmonics @a h
       *
       * @param n How many particles to correlate
       * @param h Harmonic of each term
       *
       * @return The correlator and the summed weights
       */
      Result cN(const Size n, const HarmonicVector& h) const
      {
	Complex  c;
	Real     sumw = 0;
	Real*    p    = new Real[n];
	Real*    w    = new Real[n];
	Size*    idx  = new Size[n];
	loop(0, n, idx, h, p, w, c, sumw);
	Result r(c, sumw);
	delete [] p;
	delete [] w;
	delete [] idx;
	return r;
      }
    };
  }
}
#endif
// Local Variables:
//  mode: C++
// End:
