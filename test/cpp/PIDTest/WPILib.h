
#ifndef WPILIB_H
#define WPILIB_H

#include <vector>
using namespace std;

#include <cassert>

#define wpi_assert assert
#define Priv_SetWriteFileAllowed(x) 

typedef unsigned int UINT32;

inline float GetTime()
{
	static float time = 0;
	time += 0.05F;
	return time;
}

class SpeedController {
public:

	void Set(float val)
	{
		m_val = val;
		printf("Motor set: %f\n", val);
	}

	float Get()
	{
		m_val += 1.0;
		return m_val;
	}

	float m_val;
};

#endif
