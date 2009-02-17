
#ifndef KWARQS_JOYSTICK
#define KWARQS_JOYSTICK

#define DEAD_ZONE_FILTER 0.2

/**
	\class Joystick that implements a smart dead zone
*/
class KwarqsJoystick {
public:

	KwarqsJoystick(UINT32 port)
	{
		m_joystick = Joystick::GetStickForPort(port);
	}

	float GetX(JoystickHand hand = kRightHand)
	{
		return DeadFilter(m_joystick->GetX(hand));
	}
	
	float GetY(JoystickHand hand = kRightHand)
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
	
	virtual bool GetTrigger(JoystickHand hand = kRightHand)
	{
		return m_joystick->GetTrigger(hand);
	}
	
	virtual bool GetTop(JoystickHand hand = kRightHand)
	{
		return m_joystick->GetTop(hand);
	}
	
	/// for use when this doesn't expose enough interfaces
	Joystick * RawJoystick() 
	{
		return m_stick;
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
