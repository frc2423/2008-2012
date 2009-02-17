

#include <WPILib.h>

#include "KwarqsGamePiece.h"
#include "KwarqsConstants.h"
#include "DriverStationLCD.h"

KwarqsGamePiece::KwarqsGamePiece() :
	m_low_arm_motor(SLOT_2, 6),
	m_upper_arm_motor(SLOT_2, 5)
{
	// we intentionally do not use a KwarqsJoystick here
	m_stick = Joystick::GetStickForPort(2);
}

void KwarqsGamePiece::PerformMovement()
{
	if (m_stick->GetRawButton(3))
		m_low_arm_motor.Set(1);
	else if (m_stick->GetRawButton(2))
		m_low_arm_motor.Set(-1);
	else
		m_low_arm_motor.Set(0);
	
	double val = ((m_stick->GetZ()*-1) + 1.0)/ 2;
	
	// scale this value so it can only go the correct direction
	m_upper_arm_motor.Set(((m_stick->GetZ()*-1) + 1.0)/ 2);
	
	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line6, 1, "%f", val);
}

