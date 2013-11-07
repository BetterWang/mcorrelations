#ifndef WEIGHTS_H
#define WEIGHTS_H
/**
 * @file   correlations/test/Weights.hh
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 * 
 * @brief  Particle weights
 * 
 * Copyright (c) 2013 Christian Holm Christensen 
 */
#include <correlations/Types.hh>
#include <correlations/test/Random.hh>

namespace correlations {
  namespace test {
    /**
     * A structure that hold test weights
     * 
     * @headerfile "" <correlations/test/Weights.hh> 
     */
    struct Weights
    {
      /** 
       * Constructor 
       * 
       * @param n How many bins to make 
       */
      Weights(Size n=72)
	: fValue(n)
      {
	for (Size i = 0; i < n; i++) 
	  fValue[i] = 1 + Random::AsReal(-0.144, 0.144);
      }
      /** 
       * Find the weight corresponding to @a phi
       * 
       * @param phi Azimuthal angle 
       * 
       * @return Weight corresponding to phi
       */
      Real Eval(Real phi)
      {
	Size bin = phi / 2 / M_PI * fValue.size();
	return fValue[bin];
	// return 1;
      }
      /** Cache */
      RealVector fValue;
    };
  }
}
#endif
// Local Variables:
//  mode: C++
// End:
