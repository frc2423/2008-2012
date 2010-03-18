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

class Vision 
{
public:
	
	Vision(RobotResources& resources);
	
	// these three tell the vision system which target to focus on
	
	void PreferLeft();
	void PreferRight();
	void PreferEither();
	
	// returns the angle the vision system thinks you should be pointing 
	// right now. Use in conjunction with IsRobotPointingAtTarget()
	// to figure out when to shoot the ball
	double GetVisionAngle();
	
	// returns true when the vision system believes the robot is pointing
	// at the target
	bool IsRobotPointingAtTarget() const;
	
	virtual ~Vision();
	
private:
	
	static int TimerFn(void * param);
	
	void ProcessVision();
	void AutoSweep();
	
	bool AngleWithinEdges( double setpoint ) const;
	
	RobotResources&			m_resources;
	
	SEM_ID 					m_mutex;
	
	DoubleProxy				m_vision_minscore;
	DoubleProxy				m_vision_score;
	
	// protects these variables
	DoubleProxy 			m_left_edge;
	DoubleProxy				m_right_edge;
	
	// values that are held until enabled
	DoubleProxy				m_setpoint;
	
	DoubleProxy				m_horizontalAngle;
	DoubleProxy				m_gyro_angle;
	IntProxy				m_numTargets;
	
	BoolProxy				m_setpointIsTarget;
	BoolProxy				m_isRobotAligned;
	
	DashboardDataSender 	m_dds;
	AxisCamera&				m_camera;
	
	Task 					m_task;
	
	Vision();
	DISALLOW_COPY_AND_ASSIGN(Vision);
};

#endif

