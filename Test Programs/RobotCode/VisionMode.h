#ifndef VISIONMODE_H
#define VISIONMODE_H

#include <WPILib.h>
#include "RobotMode.h"
#include "RobotResources.h"
#include "DashboardDataSender.h"

#define MINIMUM_SCORE 0.01

class SamplePIDOutput : public PIDOutput {
public:
	SamplePIDOutput(RobotDrive& base) :
		m_base (base)
	{}

	void PIDWrite(float output) {
		m_base.ArcadeDrive(0.0, output);
	}
private:
	RobotDrive& m_base;
};

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




