#ifndef VISION_H
#define VISION_H

#include <WPILib.h>
#include "RobotResources.h"
#include "DashboardDataSender.h"
#include "SamplePIDOutput.h"

class Vision
{
public:
	
	Vision(RobotResources& resources);

	void run();
	
	void enable();
	
	void disable();
	
private:
	
	RobotResources& m_resources;
	SamplePIDOutput pidOutput;
	DashboardDataSender dds;
	PIDController turnController;
	AxisCamera &camera;
	
	Vision();
	DISALLOW_COPY_AND_ASSIGN(Vision);
};

#endif

