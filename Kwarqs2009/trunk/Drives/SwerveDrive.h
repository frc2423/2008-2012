/**
	\file 		SwerveDrive.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef SWERVEDRIVE_H
#define SWERVEDRIVE_H

#include "../KwarqsLib/KwarqsDriveBase.h"
#include "../RobotChassis.h"

/**
	\class SwerveDrive
	\brief A class that controls a chassis with 4 independently steerable wheels
*/
class SwerveDrive : public KwarqsDriveBase {
public:
	
	// constructor/destructors
	SwerveDrive(RobotChassis * chassis);
	
	void Disable(){}
	void Enable(){}

	bool IsPhysicalDrive() { return true; }
	
	/**
		\brief Performs a movement of the robot
		
		The KwarqsDriveController calls this method on every enabled drive
		type when Move() is called on it. 
		
		Both parameters are passed by reference, so they may be modified
		by filter drive types when this is called.
		
		@param speed	Speed for the robot to move (-1 to 1)
	 	@param angle	Angle in degrees for the bot to move, where straight 
	 					ahead is 0 degrees, and angle increments positively in
						a clockwise direction
	  	@param rotation	Angular rotation (-1 to 1)
	  	@param stop		If this is true, then stop the robot
	*/
	void Move(double &speed, double &angle, double &rotation, bool &stop);

	/// Return the name of the class
	const char * Name() { return "SwerveDrive"; }


private:

	SwerveDrive();
	
	void Stop();
	void ShortestPath(double &speed, double &angle, double current_angle );
	
	RobotChassis * m_chassis;
	
	double m_time;
};


#endif
