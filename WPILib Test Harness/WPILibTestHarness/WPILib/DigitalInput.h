/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef DIGITAL_INPUT_H_
#define DIGITAL_INPUT_H_

class DigitalModule;

#include "SensorBase.h"
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
	explicit DigitalInput(UINT32 channel) : m_value(false)
	{
		Simulator::GetInstance()->AddDigitalInput(this, SensorBase::GetDefaultDigitalModule(), channel);
	}
	DigitalInput(UINT32 slot, UINT32 channel) : m_value(false)
	{
		Simulator::GetInstance()->AddDigitalInput(this, slot, channel);
	}
	~DigitalInput()
	{
		if (Simulator::GetInstance())
			Simulator::GetInstance()->DeleteDigitalInput(this);
	}
	
	UINT32 Get() { return m_value ? 1 : 0; }

	void RequestInterrupts(tInterruptHandler handler, void *param=NULL){}
	void SetUpSourceEdge(bool risingEdge, bool fallingEdge){}
	void EnableInterrupts(){}
	void DisableInterrupts(){}
	
	bool m_value;
};

UINT32 GetDigitalInput(UINT32 slot, UINT32 channel);
UINT32 GetDigitalInput(UINT32 channel);

#endif

