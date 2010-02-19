#ifndef AUTONOMOUSVISIONMODE_H
#define AUTONOMOUSVISIONMODE_H

#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"
#include "Kicker.h"
#include "Vision.h"

class AutonomousVisionMode : public RobotMode
{
public:
	AutonomousVisionMode(RobotResources& resources, Kicker& kicker, Vision &vision, int balls);
	
	void Main();
	
	void OnEnable();
		
	void OnDisable();
		
private:
	RobotResources& m_resources;
	Kicker& m_kicker;
	Vision& m_vision;
	
	int m_balls;
	
	AutonomousVisionMode();
	DISALLOW_COPY_AND_ASSIGN(AutonomousVisionMode);
	
};

#endif
