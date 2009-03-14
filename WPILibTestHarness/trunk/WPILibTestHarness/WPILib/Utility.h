
#ifndef EMULATION_UTILITY_H
#define EMULATION_UTILITY_H

#include <Simulator/Simulator.h>

static inline
double GetTime()
{
	return Simulator::GetTime();
}

static inline
double GetClock()
{
	return Simulator::GetTime();
}

#endif
