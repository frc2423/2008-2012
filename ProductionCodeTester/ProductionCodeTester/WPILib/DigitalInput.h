/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef DIGITAL_INPUT_H_
#define DIGITAL_INPUT_H_

class DigitalModule;

#include "DigitalSource.h"
#include <Simulator/Simulator.h>

#include <ChipObject/NiRio.h>

/**
 * Class to read a digital input.
 * This class will read digital inputs and return the current value on the channel. Other devices
 * such as encoders, gear tooth sensors, etc. that are implemented elsewhere will automatically
 * allocate digital inputs and outputs as required. This class is only for devices like switches
 * etc. that aren't implemented anywhere else.
 */
class DigitalInput : public DigitalSource {
public:
	DigitalInput(UINT32 slot, UINT32 channel){
		Simulator::AddDigitalInput(this, slot, channel);
	}
	~DigitalInput(){
		Simulator::DeleteDigitalInput(this);
	}
	
	UINT32 Get() { return m_lastValue; }

	void RequestInterrupts(tInterruptHandler handler, void *param=NULL){}
	void SetUpSourceEdge(bool risingEdge, bool fallingEdge){}
	void EnableInterrupts(){}
		void DisableInterrupts(){}
	
	bool m_lastValue;
};

UINT32 GetDigitalInput(UINT32 slot, UINT32 channel);
UINT32 GetDigitalInput(UINT32 channel);

#endif

