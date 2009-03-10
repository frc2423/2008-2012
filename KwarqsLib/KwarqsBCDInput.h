/**
	\file 		KwarqsBCDInput.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef KWARQSLIB_KWARQS_BCD_ENCODER_H
#define KWARQSLIB_KWARQS_BCD_ENCODER_H

#include <WPILib.h>

#include "KwarqsConstants.h"


/**
	\brief Reads the input from a BCD thumbwheel on the driver station

	@todo make this more general, probably should add to some input 
	class or manager
	
*/
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
