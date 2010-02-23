/**
	\file 		Vision.cpp
	\author 	Dustin Spicuzza, Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/

#ifndef VISION_H
#define VISION_H

#include <WPILib.h>
#include "RobotResources.h"
#include "DashboardDataSender.h"
#include "PIDControllerWrapper.h"

class Vision 
{
public:
	
	Vision(RobotResources& resources, PIDControllerWrapper &turnController);

	void DisableMotorControl();
	
	bool IsRobotPointingAtTarget() const;
	
	void PreferLeft();
	void PreferRight();
	void PreferEither();
	
	virtual ~Vision();
	
private:
	
	static int TimerFn(void * param);
	
	void ProcessVision();
	void AutoSweep();
	
	bool AngleWithinEdges( double setpoint ) const;
	
	SEM_ID 					m_mutex;
	
	// protects these variables
	DoubleProxy 			m_left_edge;
	DoubleProxy				m_right_edge;
	
	// values that are held until enabled
	DoubleProxy				m_setpoint;
	BoolProxy				m_enabled;
	
	DoubleProxy				m_horizontalAngle;
	DoubleProxy				m_gyro_angle;
	IntProxy				m_numTargets;
	
	BoolProxy				m_setpointIsTarget;
	BoolProxy				m_isRobotAligned;
	
	RobotResources& 		m_resources;
	DashboardDataSender 	m_dds;
	PIDControllerWrapper& 	m_turnController;
	AxisCamera&				m_camera;
	
	Task 					m_task;
	
	Vision();
	DISALLOW_COPY_AND_ASSIGN(Vision);
};

#endif

