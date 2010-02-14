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
		Roller();
	}
	void OnDisable(void){}
	
	
private:
	RobotResources& m_resources;
	
	void Drive()
	{
		m_resources.myRobot.ArcadeDrive(m_resources.stick2);
	}
	
	void Roller()
	{
		if(m_resources.stick2.GetRawButton(5)) m_resources.roller.Set(1.0);
		else if(m_resources.stick2.GetRawButton(7)) m_resources.roller.Set(-1.0);
	}
	
};

#endif

