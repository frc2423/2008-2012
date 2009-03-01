

#ifndef FRAMEWORK_KWARQS_BCD_ENCODER_H
#define FRAMEWORK_KWARQS_BCD_ENCODER_H

#include <WPILib.h>

#include "KwarqsConstants.h"

inline int GetBCDInput()
{
	DriverStation * ds = DriverStation::GetInstance();
	
	int ret = (int)ds->GetDigitalIn(BCD_IN_3) << 3;
	ret |= (int)ds->GetDigitalIn(BCD_IN_2) << 2;
	ret |= (int)ds->GetDigitalIn(BCD_IN_1) << 1;
	ret |= (int)ds->GetDigitalIn(BCD_IN_0);
	
	return ret;
}

#endif
