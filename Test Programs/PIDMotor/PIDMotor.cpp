
#include "PIDMotor.h"

#define PIDMOTOR_PERIOD 0.05


// constructor
PIDMotor::PIDMotor(
		UINT32 motor_slot,
		UINT32 motor_channel,
		Encoder &encoder, 
		float p, float i, float d,
		const char * groupName,	WebDMA &webdma
) :
	m_jaguar( motor_slot, motor_channel ),
	m_encoder( encoder ),
	m_pid( p, i, d, this, this, groupName, webdma, PIDMOTOR_PERIOD )
{
	// do any initialization here
	
	m_max_velocity = webdma.CreateDoubleProxy(groupName, "Max Velocity",
		DoubleProxyFlags()
			.default_value(0.5)
			.minval(0)
			.maxval(2.0)
			.step(0.1)
	);
	
	m_mutex = semMCreate(0);
}

// enable 
void PIDMotor::Enable()
{
	// reset state variables
	m_last_distance = m_encoder.GetDistance();
	m_pid.Enable();
}

void PIDMotor::Disable()
{
	m_pid.Disable();
	m_jaguar.Set(0);
}

void PIDMotor::SetTopSpeed(double value)
{
	m_max_velocity = value;
}

// PID interface: get wheel velocity
double PIDMotor::PIDGet()
{
	// calculate velocity
	double this_distance = m_encoder.GetDistance();
	double velocity = (this_distance - m_last_distance) / PIDMOTOR_PERIOD;
	m_last_distance = this_distance;
	
	return velocity;
}

// PID interface: set motor speed
void PIDMotor::PIDWrite(float value)
{
	// pass output to Jaguar
	m_jaguar.PIDWrite( value );
}


// speed controller interface: return what we're set to
float PIDMotor::Get()
{
	return m_pid.GetSetpoint();
}

// speed controller interface: set ideal speed
void PIDMotor::Set(float value)
{
	
	// bounds checking
	if (value < -1.0F )
		value = -1.0F;
	else if (value > 1.0F)
		value = 1.0F;

	// the input is going to be between -1 and 1, so we need
	// to scale that to match the velocity that we're specifying

	value = value * m_max_velocity;
		
	// finally, tell the PID controller what we're going for
	m_pid.SetSetpoint( value );
}


