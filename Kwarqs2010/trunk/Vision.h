#ifndef VISION_H
#define VISION_H

#include <WPILib.h>
#include "RobotResources.h"
#include "DashboardDataSender.h"
#include "SamplePIDOutput.h"
#include "ProxiedPIDController.h"

class Vision : public PIDOutput, public PIDSource
{
public:
	
	Vision(RobotResources& resources);

	void DisableMotorControl();
	
	bool IsRobotPointingAtTarget() const;
	
	void PreferLeft();
	void PreferRight();
	void PreferEither();
	
	virtual ~Vision();
	
private:
	
	static int TimerFn(void * param);
	void ProcessVision();
	
	void PIDWrite(float output);
	double PIDGet();
	
	SEM_ID 				m_mutex;
	
	// protects these variables
	DoubleProxy 		m_left_range;
	DoubleProxy			m_right_range;
	
	DoubleProxy			m_horizontalAngle;
	DoubleProxy			m_gyro_angle;
	IntProxy			m_numTargets;
	
	BoolProxy			m_setpointIsTarget;
	BoolProxy			m_isRobotAligned;
	
	RobotResources& 		m_resources;
	DashboardDataSender 	m_dds;
	ProxiedPIDController 	m_turnController;
	AxisCamera&				m_camera;
	
	Task 				m_task;
	
	Vision();
	DISALLOW_COPY_AND_ASSIGN(Vision);
};

#endif

