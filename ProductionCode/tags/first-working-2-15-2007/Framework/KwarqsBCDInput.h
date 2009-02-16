

#ifndef FRAMEWORK_KWARQS_BCD_ENCODER_H
#define FRAMEWORK_KWARQS_BCD_ENCODER_H

#include <WPILib.h>

inline int GetBCDInput()
{
	DriverStation * ds = DriverStation::GetInstance();
	
	int ret = (int)ds->GetDigitalIn(3) << 3;
	ret |= (int)ds->GetDigitalIn(4) << 2;
	ret |= (int)ds->GetDigitalIn(6) << 1;
	ret |= (int)ds->GetDigitalIn(7);
	
	return ret;
}

#endif
