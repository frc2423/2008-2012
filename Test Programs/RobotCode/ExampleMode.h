#ifndef EXAMPLEMODE_H
#define EXAMPLEMODE_H

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
		m_resources.roller.Set(m_resources.stick.GetThrottle());
		m_resources.myRobot.ArcadeDrive(m_resources.stick);
	}
	
	void OnEnable()
	{
	
	}
	
	void OnDisable()
	{
		
	}
	
private:
	RobotResources& m_resources;

};

#endif

