#ifndef VISION_H
#define VISION_H

#include <WPILib.h>
#include "RobotResources.h"
#include "DashboardDataSender.h"
#include "SamplePIDOutput.h"

class Vision : public PIDOutput
{
public:
	
	Vision(RobotResources& resources);

	void EnableMotorControl();
	void DisableMotorControl();
	
	bool IsRobotAligned() const;
	
	virtual ~Vision(){}
	
private:
	
	static void TimerFn(void *);
	void ProcessVision();
	
	void PIDWrite(float output);
	
	DoubleProxy			m_horizontalAngle;
	IntProxy			m_numTargets;
	BoolProxy			m_isRobotAligned;
	
	RobotResources& 	m_resources;
	DashboardDataSender m_dds;
	PIDController 		m_turnController;
	AxisCamera&			m_camera;
	
	Notifier			m_notifier;
	
	Vision();
	DISALLOW_COPY_AND_ASSIGN(Vision);
};

#endif

