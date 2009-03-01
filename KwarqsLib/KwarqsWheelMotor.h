/**
	\file 		KwarqsWheelMotor.h
	\author 	Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#ifndef KWARQSLIB_KWARQS_WHEELMOTOR_H
#define KWARQSLIB_KWARQS_WHEELMOTOR_H

#include "filters.h"
#include "DriverStationLCD.h"

/**
	\class KwarqsWheelMotor
	\brief A class that is used to control the speed of a motor
	
	This also encapsulates the encoder attached to the wheel, so that the 
	current actual speed of the wheel can be used
	
	A thought: it may be useful for the traction code to live in this class.
	So it would be constantly monitoring the current speed of the wheel and
	limiting the speed so that it doesn't slip or something to that effect. 
	
	This could simplify things considerably because each wheel would be
	monitoring itself.. the user would just set the speed that they wanted
	to go and this would make it go as fast as it can (without losing 
	traction of course). If this were the case, then it may be useful for 
	the object to have a method that returns the current error (ie, how 
	fast it was told to go vs. how fast its actually going). 
	
	Only a KwarqsDriveBase derived class should create these. 
	
*/
class KwarqsWheelMotor {
public:

	/// constructor: passes in the port for the speed controller and encoder
	KwarqsWheelMotor(
			UINT32 slot, UINT32 pwm_port, 
			UINT32 encoder_port1, UINT32 encoder_port2, 
			bool invert_motor, bool invert_encoder
	);

	/// Set the speed of the motor (-1 to 1)
	void SetSpeed(float speed, double velocity);
	
	/// Get the speed that the motor was assigned to go via SetSpeed
	float GetSetSpeed();
	
	/// Get the actual speed that the wheel is turning (via the encoder)
	double GetActualSpeed();

private:
	
	Jaguar m_motor;
	Encoder m_encoder;
	
	float m_invert;
	
	float m_lastSpeed;
	double m_lastUpdate;
	
	LowPassFilter m_filter;
	DriverStationLCD::Line m_line;
	
};

#endif
