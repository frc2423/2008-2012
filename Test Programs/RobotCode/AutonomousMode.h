#ifndef AUTONOMOUSMODE_H
#define AUTONOMOUSMODE_H

#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"

class AutonomousMode : public RobotMode
{
public:
	AutonomousMode(RobotResources& resources):
		m_resources(resources)
	{}
	
	void OnEnable(void){}
	void Main(void)
	{
		m_resources.myRobot.Drive(0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		m_resources.myRobot.Drive(0.0, 0.0); 	// stop robot
	}
	void OnDisable(void){}





private:
	RobotResources& m_resources;

};

#endif
