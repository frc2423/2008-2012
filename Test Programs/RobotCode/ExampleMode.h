#ifndef EXAMPLEMODE_H
#define EXAMPLEMODE_H

#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"

class ExampleMode: public RobotMode
{
public:
	ExampleMode(RobotResources& resources):
		m_resources(resources),
		time()
	{
		time.Start();
	}
		
	void Main()
	{
		Roller();
		Drive();
		Pneumatics();
	}
	
	void OnEnable()
	{
	
	}
	
	void OnDisable()
	{
		
	}
	
private:
	RobotResources& m_resources;
	Timer time;
	
	void Roller()
	{
		m_resources.roller.Set(m_resources.stick.GetThrottle());
	}
	void Drive()
	{
		m_resources.myRobot.ArcadeDrive(m_resources.stick);
	}
	void Pneumatics()
	{
		static bool ready = true;
		if(m_resources.stick.GetTrigger() && ready)
		{
			m_resources.engage.Set(true);
			ready = false;
			time.Reset();
	
		}	
		
		if(time.Get() > 500.0)
		{
			m_resources.engage.Set(false);
			m_resources.release.Set(true);
		}
		else if(time.Get() > 1000.0)
		{
			m_resources.release.Set(false);
			ready = true;
		}
	
	}

};

#endif

