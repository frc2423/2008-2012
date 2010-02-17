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
		Drive();
	}
	
	void OnEnable()
	{
	
	}
	
	void OnDisable()
	{
		
	}
	
private:
	RobotResources& m_resources;

	void Drive()
	{
		m_resources.myRobot.ArcadeDrive(m_resources.stick);
	}

};

#endif

