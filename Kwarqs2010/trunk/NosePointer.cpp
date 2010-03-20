/**
	\file 		NosePointer.cpp
	\author 	Amory Galili/Dustin Spicuzza: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


// define one of these to pick which pointer type to use


#define VISION_P 0.1
#define VISION_I 0.0		// don't use an I parameter
#define VISION_D 0.5

#define VISION_PERIOD 0.005


#include "NosePointer.h"

/**
	\brief Default Constructor

	@param turnHigh		Max output value for the turn rate
	@param turnOn		Min value that the robot should be turning at full speed
*/
NosePointer::NosePointer(
	RobotResources &resources, 
	double turnHigh, 
	double turnOn
) :				
	m_resources(resources),	
	m_pid( 
		VISION_P, VISION_I, VISION_D, 
		this, // source
		this, // output
		"Nose Pointer", resources.webdma,
		VISION_PERIOD),
		
	m_pid_output(0.0),
	m_turnOn(turnOn),
	m_last_angle(0.0)
{
	// create a mutex to allow us to synchronize access to variables
	// since we're going to be running in multiple threads
	m_mutex = semMCreate(0);


	if (turnHigh < 0.0)
		turnHigh = 0.0;
	else if (turnHigh > 1.0)
		turnHigh = 1.0;
	
	m_turnHigh = m_resources.webdma.CreateDoubleProxy("Nose Pointer", "Turn Rate",
				DoubleProxyFlags()
					.default_value(turnHigh)
					.minval(0.0)
					.maxval(1.0)
					.step(0.1)
			);
	
	// set it up to switch between the two types of pointers
	m_use_compass_drive = resources.webdma.CreateBoolProxy("Nose Pointer", "Use Compass Drive", 
			BoolProxyFlags().default_value(false));


	/**
	 * Set up the PID controller with some parameters that should be pretty
	 * close for most kitbot robots.
	 */
	printf("Initializing PIDController\n");
	
	m_pid.SetContinuous();
	m_pid.SetInputRange(0.0, 360.0);
	m_pid.SetOutputRange(-1.0, 1.0);
	m_pid.SetTolerance(1.0 / 90.0 * 100);
	m_pid.Enable();
}



/**
	\brief Gets the turn rate calculated for the robot, given a particular angle 
	to point at
	
	@param angleToPoint		The angle the robot should be pointing at

	@return	Turn rate to be given to the robot
*/
double NosePointer::GetTurnRate(double angleToPoint)
{
	angleToPoint = angle_normalize(angleToPoint);
	
	m_last_angle = angleToPoint;
	m_pid.SetSetpoint(angleToPoint);

	if (m_use_compass_drive)
	{
		// compass drive nose pointing implementation
	
		float direction_Robot = m_resources.gyro.GetAngle();		
		double rate = m_turnHigh;
		
		if( (angle_distance( angleToPoint, direction_Robot ) / 180.0) < m_turnOn )
		{
			float maxRate = 180.0 * m_turnOn;
			rate = m_turnHigh * angle_distance( angleToPoint, direction_Robot ) / maxRate;
		}
		
		int short_Direction = shortDirection(angleToPoint, direction_Robot);
			
		return rate * (float)short_Direction;
	}
	else
	{
		// pid controller based pointing implementation 
		Synchronized lock(m_mutex);
		return m_pid_output * m_turnHigh;
	}
}

bool NosePointer::IsPointingCorrectly()
{
	if (m_use_compass_drive)
	{
		// TODO: Fix this
		return (angle_distance( m_last_angle , m_resources.gyro.GetAngle() ) / 180.0 ) < m_turnOn;
	}	
	else
	{
		return m_pid.OnTarget();
	}
}


void NosePointer::PIDWrite(float output)
{
	Synchronized lock(m_mutex);
	m_pid_output = output;
}

double NosePointer::PIDGet()
{
	// this returns a normalized angle from the gyro
	return angle_normalize( m_resources.gyro.PIDGet() );	
}

