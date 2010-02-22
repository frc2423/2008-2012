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

class ExampleMode: public RobotMode
{
public:
	ExampleMode(RobotResources& resources):
		m_resources(resources)
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

	void Drive()
	{
		
		m_resources.myRobot.ArcadeDrive(m_resources.stick);
		
		/*
		
		DO NOT RE-ENABLE THIS WITHOUT FIXING IT PROPERLY. PERIOD. 
		
		if(fabs(m_resources.stick.GetX()) <= .1 && fabs(m_resources.stick.GetY()) <= .05)
		{
			m_resources.myRobot.Drive(0.0, 0.0);
		}
		else if (fabs(m_resources.stick.GetX()) <= .1)
		{
			double speed = m_resources.stick.GetMagnitude() * m_resources.stick.GetY() / fabs(m_resources.stick.GetY());
			m_resources.myRobot.Drive( speed, 0.0 );
		}
		else
		{
			double speed = m_resources.stick.GetMagnitude()* m_resources.stick.GetY() / fabs(m_resources.stick.GetY());
			double turn_rate = pow( (m_resources.stick.GetThrottle()+ 1.0) / 2.0, fabs(m_resources.stick.GetY())) * m_resources.stick.GetX(); 
			m_resources.myRobot.Drive( speed, turn_rate );
		}
		*/
	}

};

#endif

