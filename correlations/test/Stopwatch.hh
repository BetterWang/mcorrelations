#ifndef CORRELATIONS_TEST_STOPWATCH_H
#define CORRELATIONS_TEST_STOPWATCH_H
/**
 * @file   correlations/test/Stopwatch.hh
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Sun Oct 27 21:58:37 2013
 * 
 * @brief  Stop-watches
 */
#include <ctime>
#if defined(__unix__) || defined(__unix) || defined(unix)
# include <unistd.h>
#endif

namespace correlations {
  namespace test { 
    /** 
     * Base class for stop-watches 
     * 
     * @code 
     * correlations::test::Stopwatch* s = correlations::test::Stopwatch::Create();
     * @endcode 
     * returns the best possible stop-watch 
     *
     * @headerfile "" <correlations/test/Stopwatch.hh> 
     */
    struct Stopwatch 
    {
      /** 
       * Destructor 
       */
      virtual ~Stopwatch() {}
      /** 
       * Create a stop watch 
       */
      static Stopwatch* Create();
      /** 
       * Test if the stop watch is running
       * 
       * @return true if running
       */
      bool IsRunning() const { return fState == kRunning; }
      /** 
       * Test if the stop watch is stopped
       * 
       * @return true if non-running
       */
      bool IsStopped() const { return fState == kStopped; }
      /** 
       * Reset the stop-watch 
       */
      void Reset()
      {
	fState = kStopped;
	DoReset();
      }
      /** 
       * Start the stop-watch
       * 
       * @param reset If true, reset stop-watch before starting 
       */
      void Start(bool reset)
      {
	if (reset) Reset();
	if (IsRunning()) return;
	fState = kRunning;
	DoStart();
      }
      /** 
       * Stop the stop-watch. 
       *
       * @return accumulated time in seconds 
       */
      double Stop() 
      {
	if (IsStopped()) return Seconds();
	DoStop();
	fState      =  kStopped;
	return Seconds();
      }
      /** 
       * Get the accumulated elapsed time in seconds
       * 
       * @return Accumulated elapsed time in seconds
       */
      double Seconds() const { return DoSeconds(); }
    protected:
      /** The states */
      enum State_t {
	kRunning, 
	kStopped
      } fState;
      /** 
       * Constructor 
       */
      Stopwatch() : fState(kStopped) {}
      /** 
       * @{ 
       * @name Implementation interface 
       */
      virtual void DoStart() = 0;
      virtual void DoStop() = 0;
      virtual void DoReset() = 0;
      virtual double DoSeconds() const = 0;
      /* @} */
    };

    /**
     * Name space for ISO code 
     * 
     */
    namespace ISO {
      /** 
       * A stop watch using the standard clock() function.
       * 
       * @note @c clock() is part of the ISO C standard, while the @c
       * clock_gettime() function is POSIX, so we opt for @c clock()
       * for portability reasons.
       *
       * This has a precision of 10's of miliseconds 
       *
       * @headerfile "" <correlations/test/Stopwatch.hh> 
       */    
      struct Stopwatch : public correlations::test::Stopwatch
      {
	/** 
	 * Constructor 
	 */
	Stopwatch() : correlations::test::Stopwatch(), fStart(0), fSum(0) {}
      protected:
	/** 
	 * Reset the stop-watch 
	 */
	void DoReset() { fStart = 0; fSum = 0;}
	/** 
	 * Start the stop-watch
	 */
	void DoStart() { fStart = clock(); }
	/** 
	 * Stop the stop-watch. 
	 */
	void DoStop() 
	{ 
	  fSum += (clock() - fStart);
	}
	double DoSeconds() const 
	{
	  return double(fSum) / (CLOCKS_PER_SEC);
	}
	/** When the timer was last started */
	clock_t fStart;
	clock_t fSum;
      };
    }

#ifdef _POSIX_TIMERS    
    /**
     * Name space for POSIX code 
     * 
     */
    namespace POSIX 
    {
      /** 
       * A stop watch using the POSIX clock_gettime() function.  This
       * has a precision of nano-seconds
       *
       * @headerfile "" <correlations/test/Stopwatch.hh> 
       */    
      struct Stopwatch : public correlations::test::Stopwatch
      {
	/** 
	 * Constructor 
	 */
	Stopwatch() : correlations::test::Stopwatch(), fStart(), fSum() {}
      protected:
	/** 
	 * Reset the stop-watch 
	 */
	void DoReset() 
	{ 
	  fSum.tv_sec  = fStart.tv_sec  = 0; 
	  fSum.tv_nsec = fStart.tv_nsec = 0; 
	}
	/** 
	 * Start the stop-watch
	 */
	void DoStart() 
	{ 
	  clock_gettime(_POSIX_CPUTIME, &fStart);
	}
	/** 
	 * Stop the stop-watch. 
	 */
	void DoStop() 
	{ 
	  struct timespec ts;
	  clock_gettime(_POSIX_CPUTIME, &ts);
	  fSum.tv_sec   += (ts.tv_sec  - fStart.tv_sec);
	  fSum.tv_nsec  += (ts.tv_nsec - fStart.tv_nsec);
	}
	/** 
	 * Get the accumulated number of seconds
	 * 
	 * @return accumulated number of seconds
	 */
	double DoSeconds() const 
	{
	  return fSum.tv_sec + fSum.tv_nsec / 1e9;
	}
	/** When the timer was last started */
	struct timespec fStart;
	struct timespec fSum;
      };
    }   
#endif   
    Stopwatch* Stopwatch::Create() 
    {
#ifdef _POSIX_TIMERS
      return new POSIX::Stopwatch();
#else
      std::cerr << "Warning, using poor ISO stopwatch" << std::endl;
      return new ISO::Stopwatch();
#endif
    }
  }
}
#endif
// Local Variables: 
//  mode: C++
// End:
