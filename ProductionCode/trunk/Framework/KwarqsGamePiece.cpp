

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
	m_low_arm_motor.Set(m_stick->GetY()*-1);
	m_upper_arm_motor.Set(m_stick->GetZ()*-1);
}

