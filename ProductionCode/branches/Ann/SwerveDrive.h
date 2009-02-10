/**
	\file 		SwerveDrive.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef SWERVEDRIVE_H
#define SWERVEDRIVE_H

#include "Framework/KwarqsDriveBase.h"
#include "Framework/KwarqsWheelMotor.h"
#include "Framework/KwarqsWheelServo.h"

class SwerveDrive : public KwarqsDriveBase {
public:
	
	// constructor/destructors
	SwerveDrive();
	~SwerveDrive();
	
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
		@param angle 	Angle the robot should be driving
		@param rotation	Rotational speed of the robot (-1 to 1)
	*/
	void Move(double &speed, double &angle, double &rotation);

	/// Return the name of the class
	const char * Name() { return "SwerveDrive"; }


private:
	
	// servos
	KwarqsWheelServo		m_servo_lf;
	KwarqsWheelServo		m_servo_rf;
	KwarqsWheelServo		m_servo_lr;
	KwarqsWheelServo		m_servo_rr;

	// motors
	KwarqsWheelMotor		m_motor_lf;
	KwarqsWheelMotor		m_motor_rf;
	KwarqsWheelMotor		m_motor_lr;
	KwarqsWheelMotor		m_motor_rr;
	
};


#endif
