/**
	\file 		KwarqsGameControl.cpp
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#include <WPILib.h>

#include "KwarqsLib/KwarqsConstants.h"
#include "KwarqsGameControl.h"

KwarqsGameControl::KwarqsGameControl() :
	m_low_arm_motor(DIGITAL_SLOT_2, 6),
	m_upper_arm_motor(DIGITAL_SLOT_2, 5)
{
	// we intentionally do not use a KwarqsJoystick here
	m_stick = Joystick::GetStickForPort(SECOND_JOYSTICK_PORT);
	TurnoffMotors();
}

void KwarqsGameControl::TurnoffMotors()
{
	m_upper_arm_motor.Set(0.0);
	m_low_arm_motor.Set(0.0);
}

void KwarqsGameControl::PerformMovement()
{
	if (m_stick->GetTrigger())
		m_low_arm_motor.Set(1);
	else if (m_stick->GetRawButton(2))
		m_low_arm_motor.Set(-1);
	else
		m_low_arm_motor.Set(0);
	
	float val = ((m_stick->GetZ()*-1.0F) + 1.0F)/ 2.0F;
	
	// scale this value so it can only go the correct direction
	m_upper_arm_motor.Set(val*-1.0F);
	
	//DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line6, 1, "%f", val);
}

