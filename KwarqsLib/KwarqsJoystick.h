/**
	\file 		KwarqsJoystick.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef KWARQS_JOYSTICK_H
#define KWARQS_JOYSTICK_H

#include <WPILib.h>
#include "math.h"


#define DEAD_ZONE_VALUE 0.15F

/**
	\class KwarqsJoystick 
	\brief A joystick class that implements a 'smarter' dead zone, and
	allows multiple classes to easily hold instances of the same Joystick.
	
	See the Joystick class for a reference to the functions contained
	in this class, its pretty much just a mirror of it.
*/
class KwarqsJoystick {
public:

	KwarqsJoystick(UINT32 port)
	{
		m_joystick = Joystick::GetStickForPort(port);
	}

	float GetX(Joystick::JoystickHand hand = Joystick::kRightHand)
	{
		return DeadFilter(m_joystick->GetX(hand));
	}
	
	float GetY(Joystick::JoystickHand hand = Joystick::kRightHand)
	{
		return DeadFilter(m_joystick->GetY(hand));
	}
	
	float GetZ()
	{
		return DeadFilter(m_joystick->GetZ());
	}
	
	float GetTwist()
	{
		return DeadFilter(m_joystick->GetTwist());
	}
	
	float GetThrottle()
	{
		return DeadFilter(m_joystick->GetTwist());
	}
	
	bool GetTrigger(Joystick::JoystickHand hand = Joystick::kRightHand)
	{
		return m_joystick->GetTrigger(hand);
	}
	
	bool GetTop(Joystick::JoystickHand hand = Joystick::kRightHand)
	{
		return m_joystick->GetTop(hand);
	}
	
	bool GetRawButton(UINT32 button)
	{
		return m_joystick->GetRawButton(button);
	}
	
private:

	float DeadFilter(float val)
	{
		if (fabs(val) < DEAD_ZONE_VALUE)
			return 0.0;
		
		if (val < 0)
			return (val + DEAD_ZONE_VALUE)/(1 - DEAD_ZONE_VALUE);
		
		return (val - DEAD_ZONE_VALUE)/(1 - DEAD_ZONE_VALUE);
	}

	Joystick * m_joystick;

};


#endif
