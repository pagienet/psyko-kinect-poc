#ifndef _PSYKO_PRECISIONTIMER_
#define _PSYKO_PRECISIONTIMER_

#include "utils/PrecisionTimer.h"

#ifdef WIN32

namespace psyko
{
	double GetGlobalTimer();

	class PrecisionTimer
	{
	public:
		PrecisionTimer();
		~PrecisionTimer();

		// Resets the timer's start count.
		void Reset();
		
		// Returns the time in milliseconds since the timer's creation or the last call to Reset
		double GetTimer() const;

	
	private:
		double startCount;
		double millisecondsPerCount;
	};
}

#endif // WIN32

#endif