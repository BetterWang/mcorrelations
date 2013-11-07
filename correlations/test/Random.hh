#ifndef CORRELATIONS_TEST_RANDOM_H
#define CORRELATIONS_TEST_RANDOM_H
/**
 * @file   correlations/test/Random.hh
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 * 
 * @brief  Random number generator
 * 
 * Copyright (c) 2013 Christian Holm Christensen 
 */
#include <correlations/Types.hh>
#include <cstdlib>
#include <ctime>

namespace correlations {
  namespace test {
    /**
     * Random number generator
     * 
     * @headerfile "" <correlations/test/Random.hh> 
     */
    struct Random
    {
      static void Seed(unsigned int seed) 
      { 
	srand(seed == 0 ? time(0) : seed); 
      }
      // using Real;
      // using Size;
      static Real AsReal(Real min, Real max) 
      {
	return min + (max-min) * Real(rand()) / RAND_MAX;
      }
      static Harmonic AsHarmonic(Harmonic min, Harmonic max) 
      {
	return min + Harmonic((max-min) * Real(rand()) / RAND_MAX);
      }
      static Size AsSize(Size min, Size max) 
      {
	return min + Size((max-min) * Real(rand()) / RAND_MAX);
      }
    };
  }
}
#endif
// Local Variables:
//  mode: C++
// End:
