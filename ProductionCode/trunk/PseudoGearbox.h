/**
	\file 		PseudoGearbox.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef SPEEDLIMITER_H
#define SPEEDLIMITER_H

/**
	\class PseudoGearbox
	\brief This is a trivially simple filter drive class that limits the 
	speed of the robot. There is a question of whether this belongs here 
	or not. I feel.. no. 
*/
class PseudoGearbox : public KwarqsDriveBase 
{
public:

	PseudoGearBox() :
		m_stick(FIRST_JOYSTICK_PORT)
	{}

	void Disable(){}
	void Enable(){}

	/// this is a filter, so return false
	virtual bool IsPhysicalDrive() { return false; }

	/// limits the speed of the robot
	virtual void Move(double &speed, double &angle, double &rotation, bool &stop)
	{
		// don't touch this (turbo mode)
		if (m_stick.GetTopButton())
			return;
	
		// low speed action
		speed *= .5;
		rotation *= .5;
	}

	/// Return the name of the class
	virtual const char * Name() { return "PseudoGearbox"; }
	
private:

	KwarqsJoystick m_stick;
	
	double DeadFilter(double val, double max)
	{
		if (fabs(val) > max)
			return max;
		
		return val * max;
	}
	
};

#endif
