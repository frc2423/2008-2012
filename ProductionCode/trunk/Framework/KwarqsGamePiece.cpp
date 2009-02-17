

#include <WPILib.h>

#include "KwarqsGamePiece.h"
#include "KwarqsConstants.h"

KwarqsGamePiece::KwarqsGamePiece() :
	m_low_arm_motor(SLOT_2, 6),
	m_upper_arm_motor(SLOT_2, 5)
{
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
	
	m_upper_arm_motor.Set(m_stick->GetZ()*-1);
}

