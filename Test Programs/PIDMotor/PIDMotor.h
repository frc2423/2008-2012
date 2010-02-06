
#ifndef PIDMOTOR_H
#define PIDMOTOR_H


#include <WPILib.h>

#include <WebDMA/WebDMA.h>
#include "ProxiedPIDController.h"

/**
	\class PIDMotor
	
	This class sets the speed of a motor via a PID feedback system
	
	The jaguar speed controllers should be set to 'coast' mode, for
	optimal control. 
*/
class PIDMotor : public PIDSource, public PIDOutput, public SpeedController 
{
public:
	
	/**
		PIDMotor Constructor
	*/
	PIDMotor(
		UINT32 motor_slot,
		UINT32 motor_channel,
		Encoder &encoder, 
		float p, float i, float d,
		const char * groupName,	WebDMA &webdma
	);
	
	void Enable();
	void Disable();
	
	// speed controller interface, should be public
	void Set(float value);
	float Get();
	
	// specified in meters per second
	void SetTopSpeed( double value );
	
private:

	// Implementation of PIDSource/PIDOutput interface
	double PIDGet();
	void PIDWrite(float value);

	
	// no default constructor
	PIDMotor();
	
	// no copying or assigning
	DISALLOW_COPY_AND_ASSIGN(PIDMotor);
	
	
	double 					m_last_distance;
	DoubleProxy 			m_max_velocity;
	
	Jaguar 					m_jaguar;
	Encoder 				&m_encoder;	
	ProxiedPIDController 	m_pid;
	
	SEM_ID 					m_mutex;
};


#endif
