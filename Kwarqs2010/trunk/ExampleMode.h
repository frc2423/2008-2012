#ifndef EXAMPLEMODE_H
#define EXAMPLEMODE_H

/**
	\file 		ExampleMode.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"
#include "Error.h"
#include "NosePointer.h"

class ExampleMode: public RobotMode
{
public:
	ExampleMode(RobotResources& resources, NosePointer &nosePointer):
		m_resources(resources),
		m_nosePointer(nosePointer)
	{}
		
	void Main()
	{
		Drive();
	}
	
	void OnEnable()
	{
		printf("Entering example mode\n");
	}
	
	void OnDisable()
	{
		printf("Leaving example mode\n");
	}
	
private:
	RobotResources& m_resources;
	NosePointer &m_nosePointer;

	void Drive()
	{
		if (m_resources.stick.GetRawButton(2))
		{
			double turn_rate = 0.0;
			
			if (m_resources.stick.GetMagnitude() > 0.1)
				turn_rate = m_nosePointer.GetTurnRate( m_resources.stick.GetDirectionDegrees() - 180.0); 
			
			m_resources.myRobot.ArcadeDrive( 0.0, turn_rate );
			
		}
		else
			m_resources.myRobot.ArcadeDrive(m_resources.stick);
		/*
		{
		
		
		//DO NOT RE-ENABLE THIS WITHOUT FIXING IT PROPERLY. PERIOD. 
		
		if(fabs(m_resources.stick.GetX()) <= .1 && fabs(m_resources.stick.GetY()) <= .05)
		{
			m_resources.myRobot.Drive(0.0, 0.0);
		}
		else if (fabs(m_resources.stick.GetX()) <= .1)
		{
			double speed = m_resources.stick.GetMagnitude() * m_resources.stick.GetY() / d_err(fabs(m_resources.stick.GetY()));
			m_resources.myRobot.Drive( speed, 0.0 );
		}
		else
		{
			double speed = m_resources.stick.GetMagnitude()* m_resources.stick.GetY() / d_err(fabs(m_resources.stick.GetY()));
			double throttle = d_err(((m_resources.stick.GetThrottle()+ 1.0) / 2.0));
			double turn_rate = pow( throttle, fabs(m_resources.stick.GetY())) * m_resources.stick.GetX(); 
			m_resources.myRobot.Drive( speed, turn_rate );
		}
		}
		*/
		
	}

};

#endif

