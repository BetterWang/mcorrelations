#ifndef CORRELATIONS_TYPES_HH
#define CORRELATIONS_TYPES_HH
/**
 * @file   correlations/Types.hh
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Oct 24 23:45:40 2013
 *
 * @brief  Declarations of types
 *
 * Copyright (c) 2013 Christian Holm Christensen
 */
#include <vector>
#include <complex>

/**
 * @mainpage Calculate n-particle Correlations
 *
 * @section intro Introduction
 *
 * This code can calculate the _n_-particle correlator
 *
 * @f[
 *  QC\{N\}=
 *  \left\langle\exp\left[i\left(\sum_j^N h_j\phi_j\right)\right]\right\rangle
 * @f]
 *
 * for any @f$ N@f$ and harmonic orders @f$ h_i@f$ - limited by
 * computing time and available memory.
 *
 * @section algo Algorithms
 *
 * The correlation is calculated using the concrete sub-classes of
 * correlations::Correlator:
 *
 * - correlations::NestedLoops uses nested loops (or rather, recursive
 *   function calls.  This relies on storing all the observations of
 *   @f$ \phi@f$ and (possibly) the corresponding weights @f$ w@f$.
 *   This way of calculating the correlation scales like @f$ O(M^N)@f$
 *   where @f$ M@f$ is the event multiplicity and @f$ N@f$ is the
 *   correlation order to calculate.  It is indeed rather slow.
 *
 * - correlations::Cumulant used a @f$ Q@f$-vector to calculate the
 *   correlations.  Calculating the @f$ Q-vector@f$ is an order @f$
 *   O(N M)@f$ operation, while calculating the correlation is only
 *   @f$ O(N)@f$.  Still - it is much faster generally, then the
 *   nested loop method.
 *
 * Both algorithms are interfaced in the same way through the member
 * function correlations::Correlator::Calculate.
 *
 * The member function  correlations::Correlator::Calculate returns a
 * correlations::Result object, which contains the sum and
 * the sum of the weights.  The sum and the sum of weights can be
 * accumulated in an object of this time, and the final event average
 * result can be extracted using correlations::Correlator::Eval.
 *
 * @subsection recursion Recursive vs. closed-form
 *
 * The class correlations::Cumulant is again an abstract base class
 * (cannot make objects of that type), and two different
 * implementations are provided.
 *
 * - correlations::recursive::Cumulant which used recursion to
 *   simplify the expression, and has the added feature that it can
 *   calculate any order correlator - provided enough computing time
 *   and memory.
 * - correlations::closed::Cumulant which uses closed-form expression
 *   for the cumulants.  This means that we are limited to the order
 *   for which we have generated these closed forms.  Note, one can
 *   use the program print.cc to generate this, should it be needed.
 *
 * Similarly to correlations::recursive::Cumulant, there is a
 * sub-class of correlations::NestedLoops -
 * correlations::recursive::NestedLoops which does the nested loops
 * using recursion.
 *
 * @note The recursive algorithms could probably be implemented using
 * template recursion to let the compiler (and not run-time) deal with
 * branching, etc., which could speed up this tremendiously.
 *
 * @section qvec Q-Vector
 *
 * The class correlations::QVector is the container of the per-event
 * @f$Q@f$-vector
 *
 * @f[
 *  Q(h,p) = \sum_i w_i^p e^{ih\phi_i}
 * @f]

 * where @f$ \phi_i@f$ is the angle of the observation, and @f$ w_i@f$
 * it's weight. Observations are added to the @f$ Q@f$-vector using
 * the member function correlations::QVector::Fill.
 *
 * @section impl Implementation notes.
 *
 * - All code lives in the namespace correlations. This to minimize
 *   the polution of users code space.
 * - the file correlations/Types.hh defines a number of basic types,
 *   like correlations::Real and correlations::Complex and the rest of
 *   the code uses these type definitions rigeroursly.  This allows
 *   one to easily change the underlying types should the need be -
 *   e.g., changing correlations::Real to be an alias for long double
 *   rather than double would allow for greater precision.
 * - The methods are implemented in plain C++.
 * - Testing code is available in correlations::test
 *
 * @section exa Examples
 *
 * - <a href="print_8cc-example.html">print.cc</a> - Print the
 *   recursive expressions for the @f$Q@f$-cumulant
 * - <a href="analyze_8cc-example.html">analyze.ccC</a> - A program to
 *   generate and analyse the data using various techniques.
 * - <a href="_analyze_8_c-example.html">Analyze.C</a> - A ROOT based script
 *   example.
 *
 * @example analyze.cc A simple program that checks the algorithms
 * against each other.  Note that this used the class test::Test,
 * test::Distribution, test::Random, and test::Generator.
 *
 * @example Analyze.C A ROOT-based script to illustrate the use of
 * the code in ROOT
 *
 * @example print.cc A simple program that dumps the expressions for
 * the correlations using @f$ Q@f$-vector input and recursion.
 *
 */
/**
 * Namespace for correlations code
 */
namespace correlations {
  /** Type of Real */
  typedef double Real;
  /** Type of real array */
  typedef std::vector<Real> RealVector;
  /** Type defintion of the kind of complex we use */
  typedef std::complex<Real> Complex;
  /** Type definition of our internal storage */
  typedef std::vector<Complex> ComplexVector;
  /** Type of harmonics */
  typedef short Harmonic;
  /** Type definition of vector of harmonics */
  typedef std::vector<Harmonic> HarmonicVector;
  /** Type of Sizes */
  typedef unsigned short Size;
  /** Type of Powers */
  typedef unsigned short Power;

}
#endif
// Local Variables:
//   mode: C++
// End:
