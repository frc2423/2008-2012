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
	void Disable(){}
	void Enable(){}

	/// this is a filter, so return false
	virtual bool IsPhysicalDrive() { return false; }

	/// limits the speed of the robot
	virtual void Move(double &speed, double &angle, double &rotation, bool &stop)
	{
		if (DriverStation::GetDigitalIn(DS_IN_GEARSHIFT))
			speed *= 0.5;
	
		speed = pow(speed, 3);
		rotation = pow(speed, 3);
	}

	/// Return the name of the class
	virtual const char * Name() { return "PseudoGearbox"; }
};

#endif
