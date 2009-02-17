/**
	\file 		PsuedoGearbox.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef PSEUDOGEARBOX_H
#define PSEUDOGEARBOX_H

/**
	\class PsuedoGearbox
	\brief This is a trivially simple filter drive class that limits the 
	speed of the robot. There is a question of whether this belongs here 
	or not. I feel.. no. 
*/
class PsuedoGearbox : public KwarqsDriveBase 
{
public:

	PsuedoGearbox() :
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
	virtual const char * Name() { return "PsuedoGearbox"; }
	
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
