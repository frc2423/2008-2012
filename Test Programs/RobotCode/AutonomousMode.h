#ifndef AUTONOMOUSMODE_H
#define AUTONMOUSMODE_H

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
	void Main(void){}
	void OnDisable(void){}





private:
	RobotResources& m_resources;

};

#endif
