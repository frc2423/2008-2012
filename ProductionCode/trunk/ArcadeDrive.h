/**
	\file 		ArcadeDrive.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef ARCADEDRIVE_H
#define ARCADEDRIVE_H

#include "Framework/KwarqsDriveBase.h"

class ArcadeDrive : public KwarqsDriveBase {
public:
	
	// constructor/destructors
	ArcadeDrive();
	~ArcadeDrive();
	
	void Disable(){}
	void Enable(){}

	bool IsPhysicalDrive() { return true; }
	
	/**
		\brief Performs a movement of the robot
		
		The KwarqsDriveController calls this method on every enabled drive
		type when Move() is called on it. 
		
		Both parameters are passed by reference, so they may be modified
		by filter drive types when this is called.
		
		@param speed	Speed of the robot (-1 to 1)
		@param rotation	Rotational speed of the robot (-1 to 1)
	*/
	void Move(float &speed, float &rotation);

	/// Return the name of the class
	const char * Name() { return "ArcadeDrive"; }


private:
	
	float Limit(float num);
	
	SpeedController * m_leftMotor;
	SpeedController * m_rightMotor;

};


#endif
