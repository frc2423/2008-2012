/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Solenoid.h"

#include "Resource.h"
#include "Synchronized.h"
#include "Utility.h"

#include <Simulator/Simulator.h>


/**
 * Common function to implement constructor behavior.
 */
void Solenoid::InitSolenoid()
{
	m_value = false;
}

/**
 * Constructor.
 * 
 * @param channel The channel on the module to control.
 */
Solenoid::Solenoid(UINT32 channel)
{
	Simulator::GetInstance()->AddSolenoid(this, GetDefaultSolenoidModule(), channel);
	InitSolenoid();
}

/**
 * Constructor.
 * 
 * @param slot The slot that the 9472 module is plugged into.
 * @param channel The channel on the module to control.
 */
Solenoid::Solenoid(UINT32 slot, UINT32 channel)
{
	Simulator::GetInstance()->AddSolenoid(this, slot, channel);
	InitSolenoid();
}

/**
 * Destructor.
 */
Solenoid::~Solenoid()
{
	if (Simulator::GetInstance())
		Simulator::GetInstance()->DeleteSolenoid(this);
}

/**
 * Convert slot number to index.
 * 
 * @param slot The slot in the chassis where the module is plugged in.
 * @return An index to represent the module internally.
 */
UINT32 Solenoid::SlotToIndex(UINT32 slot)
{
	return 0;
}

/**
 * Set the value of a solenoid.
 * 
 * @param on Turn the solenoid output off or on.
 */
void Solenoid::Set(bool on)
{
	m_value = on;
}

/**
 * Read the current value of the solenoid.
 * 
 * @return The current value of the solenoid.
 */
bool Solenoid::Get()
{
	return m_value;
}
