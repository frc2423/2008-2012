/**
	\file 		KwarqsWheelServoBase.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef KWARQS_WHEEL_SERVO_BASE_H
#define KWARQS_WHEEL_SERVO_BASE_H

/**
	\class KwarqsWheelServoBase
	\brief Controls the direction of the wheel, relative to the robot
	
	Ideally, this will be controlled by a PID loop just like a real servo
	would be controlled. Will need at least two variants of this class,
	since the front and rear motors are going to be different types so
	they will need different tuning.
	
	Only a KwarqsDriveBase derived class should create these. 
*/
class KwarqsWheelServoBase {
public:

	/// constructor: sets the parameters for this wheel
	KwarqsWheelServoBase(UINT32 pwm_port, UINT32 encoder_port);

	/// Set the angle that the wheel should be pointing
	virtual void SetAngle(float angle);
	
	/// Get the angle that the wheel is supposed to be pointing
	virtual float GetAngle();
	
	/// Get the angle that the wheel is actually pointing
	virtual float GetCurrentAngle();

};

#endif
