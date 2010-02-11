#ifndef EXAMPLEMODE_H
#define EXAMPLEMODE_H

#include <WPILib.h>
#include "RobotMode.h"

class ExampleMode: public RobotMode
{
public:
	ExampleMode(RobotResources& resources):
		m_resources(resources)		
	{}
		
	void Main()
	{
		
	}
	
	
private:
	RobotResources& m_resources;

};

#endif

