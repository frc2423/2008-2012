#ifndef VISIONMODE_H
#define VISIONMODE_H

#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"
#include "DashboardDataSender.h"
#include "SamplePIDOutput.h"

class VisionMode : public RobotMode
{
public:
	
	VisionMode(RobotResources& resources);

	void Main();
	
	void OnEnable();
	
	void OnDisable();
	
private:
	
	RobotResources& m_resources;
	SamplePIDOutput pidOutput;
	DashboardDataSender dds;
	PIDController turnController;
	AxisCamera &camera;
	
	VisionMode();
	DISALLOW_COPY_AND_ASSIGN(VisionMode);
};

#endif

