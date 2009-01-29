/**
	\file 		KwarqsWheelServo.h
	\author 	Dustin Spicuzza: last changed by $Author: dspicuzz $
	\date 		Last changed on $Date: 2009-01-25 03:23:53 -0500 (Sun, 25 Jan 2009) $
	\version 	$Rev: 46 $
*/

#ifndef KWARQS_WHEEL_SERVO_BASE_H
#define KWARQS_WHEEL_SERVO_BASE_H

#include "TunablePIDController.h"

/**
	\class KwarqsWheelServo
	\brief Controls the direction of the wheel, relative to the robot
	
	Ideally, this will be controlled by a PID loop just like a real servo
	would be controlled. Will need at least two variants of this class,
	since the front and rear motors are going to be different types so
	they will need different tuning.
	
	Only a KwarqsDriveBase derived class should create these. 
*/
class KwarqsWheelServo : public PIDSource, public PIDOutput {
public:

	KwarqsWheelServo(
		UINT32 pwm_port, 
		UINT32 encoder_port1, UINT32 encoder_port2,
		double outputScale,
		int encoderResolution
	);
	
	virtual ~KwarqsWheelServo();

	/// Set the angle that the wheel should be pointing
	void SetAngle(float angle);
	
	/// Get the angle that the wheel is supposed to be pointing
	float GetAngle();
	
	/// Get the angle that the wheel is actually pointing
	double GetCurrentAngle();

	
	// only accessed by the PIDController class
	void PIDWrite(float output);
	double PIDGet();
	
	// used to tune the class
	void TuneParameters(float p, float i, float d);
	
private:

	TunablePIDController * m_pidController;
	
	Victor 		m_motor;
	Encoder 	m_encoder;
	
	double 		m_outputScale;
	double		m_encoderResolution;
	
};

#endif
