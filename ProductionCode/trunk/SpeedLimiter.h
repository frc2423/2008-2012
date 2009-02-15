/**
	\file 		SpeedLimiter.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef SPEEDLIMITER_H
#define SPEEDLIMITER_H

/**
	\class SpeedLimiter
	\brief This is a trivially simple filter drive class that limits the 
	speed of the robot. Mostly exists just to show how this works
*/
class SpeedLimiter : public KwarqsDriveBase 
{
	void Disable(){}
	void Enable(){}

	/// this is a filter, so return false
	virtual bool IsPhysicalDrive() { return false; }

	/// limits the speed of the robot
	virtual void Move(double &speed, double &angle, double &rotation, bool &stop)
	{
		if (speed < -.4)
			speed = -.4;
		else if (speed > .4)
			speed = .4;
	}

	/// Return the name of the class
	virtual const char * Name() { return "SpeedLimiter"; }
};

#endif
