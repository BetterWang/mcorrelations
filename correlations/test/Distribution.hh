#ifndef CORRELATIONS_TEST_DISTRIBUTION_H
#define CORRELATIONS_TEST_DISTRIBUTION_H
/**
 * @file   correlations/test/Distribution.hh
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 * 
 * @brief  A phi distribution
 * 
 * Copyright (c) 2013 Christian Holm Christensen 
 */
#include <correlations/test/Random.hh>
#include <correlations/Types.hh>
#include <cmath>
#include <vector>

namespace correlations {
  namespace test {
    //====================================================================
    /** 
     * The phi distribution 
     *
     * @f[
     *  f(\phi) = 1 + 2\sum_{i=1}^{6} \cos(i(\phi-\Phi_R))
     * @f]
     *
     * The indefinite integral is given by 
     * @f[
     * \int d\phi f(\phi) = 2\pi + 2 \sum_{i=1}^{6} \frac{\sin(i(\phi-\Phi_R))}{i}
     * @f]
     * 
     * or for the definte integral over @f$[a,b]@f$
     * @f[
     * \int_a^b f(\phi) = (b-a) + 2 \sum_{i=1}^6 
     *   \frac{\sin(i(b-\Phi_R))-\sin(i(a-\Phi_R))}{i}
     * @f]
     * 
     * @headerfile "" <correlations/test/Distribution.hh> 
     */
    struct Distribution
    {
      static Real func(Real* xx, 
		       Real* pp)
      {
	Real phi = xx[0];
	Real psi = pp[0];
	Real ret = 1;
	for (Size i = 1; i < 7; i++) 
	  ret += 2 * pp[i] * cos(i * (phi-psi));
	return ret;
      }

      /** 
       * Constructor 
       */
      Distribution() 
	: fNpx(100),
	  fIntegral(fNpx+1), 
	  fAlpha(fNpx), 
	  fBeta(fNpx), 
	  fGamma(fNpx)
      {
	fV[0] = 0;    // psi
	fV[1] = 0.01; // v_1
	fV[2] = 0.05; // v_2
	fV[3] = 0.03; // v_3
	fV[4] = 0.02; // v_4
	fV[5] = 0.01; // v_5
	fV[6] = 0.01; // v_6;
      }
      /** 
       * Set-up for sampling 
       */
      void Setup(Real psi)
      {
	fV[0] = psi;
	Real dx    = 2*M_PI / fNpx;
	fIntegral[0] = 0;
	// Calculate cumulated integral in bins 
	for (Size i = 0; i < fNpx; i++) {
	  Real intg = Integral(i*dx, (i+1)*dx);
	  fIntegral[i+1] = fIntegral[i] + intg;
	}
	// Normalize intergral 
	Real full = fIntegral[fNpx];
	for (Size i = 0; i <= fNpx; i++) 
	  fIntegral[i] /= full;
	// Approximate intergal r in each bin with a parabolla 
	// 
	//  x = alpha + beta * r + gamma * r * r;
	for (Size i = 0; i <= fNpx; i++) {
	  Real r2 = fIntegral[i+1] - fIntegral[i];     // End point
	  Real r1 = Integral(i*dx, (i+.5)*dx) / full;  // Mid point
	  Real r3 = 2 * r2 - 3 * r1;
	  fGamma[i] = 2 * (fabs(r3) > 1e-8) ? r3 / (dx*dx) : 0;
	  fBeta[i]  = r2 / dx - fGamma[i] * dx / 2;
	  fAlpha[i] = i * dx;
	}    
      }
      /** 
       * Evaluate the function
       * 
       * @param phi Where to evaluate 
       * 
       * @return The function evaluated at @a phi
       */
      Real Eval(Real phi) const
      {
	Real* pp = const_cast<Real*>(&(fV[0]));
	return func(&phi, pp);
      }
      /** 
       * Evaluate the integral of the function from @a a to @a b 
       * 
       * @param a Lower limit 
       * @param b Upper limit 
       * 
       * @return Integral from @a a to @a b 
       */
      Real Integral(Real a, 
		    Real b) const
      {
	Real ret = (b-a);
	Real psi = fV[0];
	for (Size i = 1; i < 7; i++) 
	  ret += 2 * fV[i] * (sin(i*(b-psi))-sin(i*(a-psi)));
	return ret;
      }
      /** 
       * Draw a random value from the function considered a probability
       * distribution.
       * 
       * @return A random number distributed according to the function
       */
      Real Random() const
      {
	Real        rnd   = Random::AsReal(0,1);
	const Real* first = &(fIntegral[0]);
	const Real* last  = first + fIntegral.size();
	const Real* ptr   = std::lower_bound(first, last, rnd);
	Size        bin   = 0;
	if (ptr != (last) && (*ptr == rnd)) bin = (ptr-first);
	else                                bin = (ptr-first-1);
	Real  rr    = rnd - fIntegral[bin];
    
	Real yy = 0;
	Real g  = fGamma[bin];
	Real b  = fBeta[bin];
	Real a  = fAlpha[bin];
	if (g != 0) 
	  yy = (-b + sqrt(b*b+2*g*rr))/g;
	else 
	  yy = rr / b;
	Real x = a + yy;
	return x;
      }
      /** Reaction plane and Flow strength */
      Real fV[7];
      /** Number of points to evaluate the function at */
      Size fNpx;
      /** Integral of function in steps */
      RealVector fIntegral;
      /** Parameterization of integral */
      RealVector fAlpha;
      /** Parameterization of integral */
      RealVector fBeta;
      /** Parameterization of integral */
      RealVector fGamma;
    };
  }
}
#endif
// Local Variables:
//  mode: C++
// End:
