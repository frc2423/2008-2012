#ifndef PSUSBMODE_H
#define PSUSBMODE_H

#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"

class PSUSBMode : public RobotMode
{
public:
	PSUSBMode(RobotResources& resources):
		m_resources(resources)
	{}
		
	void OnEnable(void){}
	void Main(void)
	{
		Drive();
	}
	void OnDisable(void){}
	
	
private:
	RobotResources& m_resources;
	
	void Drive()
	{
		m_resources.myRobot.ArcadeDrive(m_resources.stick2);
	}
	
	
};

#endif

