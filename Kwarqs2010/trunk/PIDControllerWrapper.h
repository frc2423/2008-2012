
#ifndef PIDCONTROLLERWRAPPER_H
#define PIDCONTROLLREWRAPPER_H

#include "ProxiedPIDController.h"
#include "kwarqs_math.h"


#define VISION_P 0.1
#define VISION_I 0.0
#define VISION_D 0.5

#define VISION_PERIOD 0.005


class PIDControllerWrapper : public ProxiedPIDController, public PIDOutput, public PIDSource
{
public:
	
	PIDControllerWrapper(RobotResources &resources):
		ProxiedPIDController( 
					VISION_P, VISION_I, VISION_D, 
					this, // source
					this, // output
					"Turn Controller", resources.webdma,
					VISION_PERIOD), // period
					
		m_resources(resources),
		m_current_speed(0.0)
	{
		
		/**
		 * Set up the PID controller with some parameters that should be pretty
		 * close for most kitbot robots.
		 */
		printf("Initializing PIDController\n");
		
		SetContinuous();
		SetInputRange(0.0, 360.0);
		SetOutputRange(-0.8, 0.8);
		SetTolerance(1.0 / 90.0 * 100);
		Disable();
	}
	
	virtual ~PIDControllerWrapper(){}
	
	// set current speed
	
	
	void PIDWrite(float output)
	{
		// first parameter is zero because we want to spin in place
		m_resources.myRobot.ArcadeDrive(m_current_speed, output);
	}

	double PIDGet()
	{
		// this returns a normalized angle from the gyro
		return angle_normalize( m_resources.gyro.PIDGet() );	
	}
		
	
private:
	
	SEM_ID 					m_mutex;
	
	RobotResources 			&m_resources;
	double 					m_current_speed;
};


#endif
