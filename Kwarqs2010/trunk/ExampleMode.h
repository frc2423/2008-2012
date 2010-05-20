/**
	\file 		ExampleMode.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef EXAMPLEMODE_H
#define EXAMPLEMODE_H


#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"
#include "Error.h"
#include "NosePointer.h"

class ExampleMode: public RobotMode
{
	DoubleProxy m_adjust;
	
public:
	ExampleMode(RobotResources& resources, NosePointer &nosePointer):
		m_resources(resources),
		m_nosePointer(nosePointer)
	{
		
		m_adjust = resources.webdma.CreateDoubleProxy("Main", "Turn Adjustment",
				DoubleProxyFlags()
						.default_value(0.7)
						.minval(0.0)
						.maxval(1.0)
						.step(0.1)
		);
	}
		
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
		double y = m_resources.stick.GetY();
		double x = m_resources.stick.GetX() * m_adjust;
		
		// by default, enable smoother turning
		if ( !m_resources.stick.GetRawButton(2))
		{
			if (x >= 0.0)
			{
				x = (x * x);
			}
			else
			{
				x = -(x * x);
			}
		}
		
		// limit going backwards so we can pull the ball with us more easily
		if (y > 0)
			y = y * 0.6;
		
		m_resources.myRobot.ArcadeDrive( y, x, false );
		
		/*
		
		// point the robot in a particular direction if asked to... 
		if (m_resources.stick.GetRawButton(2))
		{
			double turn_rate = 0.0;
			
			if (m_resources.stick.GetMagnitude() > 0.1)
				turn_rate = m_nosePointer.GetTurnRate( m_resources.stick.GetDirectionDegrees() - 180.0); 
			
			m_resources.myRobot.ArcadeDrive( 0.0, turn_rate );
			
		}
		else
		{
			//m_resources.myRobot.ArcadeDrive(m_resources.stick);
			
			m_resources.myRobot.ArcadeDrive( 
					m_resources.stick.GetY(), 
					m_resources.stick.GetX(), 
					m_resources.stick.GetRawButton(2) );
		}
		*/
		
	}

};

#endif

