#ifndef AUTONOMOUSVISIONMODE_H
#define AUTONOMOUSVISIONMODE_H

#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"
#include "DashboardDataSender.h"
#include "SamplePIDOutput.h"
#include "Kicker.h"

class AutonomousVisionMode : public RobotMode
{
public:
	AutonomousVisionMode(RobotResources& resources, Kicker& kicker, int balls);
	
	void Main();
	
	void FindTarget();
	
	void OnEnable();
		
	void OnDisable();
		
private:
	RobotResources& m_resources;
	Kicker& m_kicker;
	int m_balls;
	SamplePIDOutput pidOutput;
	DashboardDataSender dds;
	PIDController turnController;
	AxisCamera &camera;
	
	AutonomousVisionMode();
	DISALLOW_COPY_AND_ASSIGN(AutonomousVisionMode);
	
};

#endif