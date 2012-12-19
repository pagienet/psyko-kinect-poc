#include "utils/PrecisionTimer.h"
#include <Windows.h>

namespace psyko
{
	double CalculateMillisecondsPerCount()
	{
		__int64 countsPerSecond;
		::QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
		return 1000.0/(double)countsPerSecond;
	}


	double GetGlobalTimer()
	{
		static double msPerCount = CalculateMillisecondsPerCount();
		__int64 count = 0;
		::QueryPerformanceCounter((LARGE_INTEGER*)&count);
		return count * msPerCount;
	}

	PrecisionTimer::PrecisionTimer()
		: startCount(0), millisecondsPerCount(0)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&startCount);
		millisecondsPerCount = CalculateMillisecondsPerCount();		
	}


	PrecisionTimer::~PrecisionTimer()
	{
	}

	void PrecisionTimer::Reset()
	{
		startCount = GetGlobalTimer();
	}

	double PrecisionTimer::GetTimer() const
	{
		double delta = GetGlobalTimer() - startCount;
		
		// could be < 0 after call to Reset, so clamp
		return delta < 0.0? 0.0 : delta;
	}

}