#ifndef POSITIONINFORMATION_H
#define POSITIONINFORMATION_H

#include "RobotResources.h"
#include <WPILib.h>

class PositionInformation
{
public:
	PositionInformation (RobotResources& resources, const double wheel_base);
	
	void Start ();
	void getData(double &x, double &y, double &angle);

	
private:
	static void TimerFn(void * param); 
	void CalculationTimerFn();
	void ComputeNewPosition(double l, double r);
	void TransformCoordinates(double dx, double dy, double dtheta);
	
	PositionInformation();
	
	
	double m_theta;								// Heading of the robot
	DoubleProxy m_leftMeters, m_rightMeters;	// Stores distance in meters
	DoubleProxy m_x, m_y;
	DoubleProxy m_display_angle;
	double m_wheelBase;							// Wheel base of the robot
	
	IntProxy m_l, m_r;
	
	Notifier m_notifier;
	RobotResources& m_resources;
	Encoder* m_leftEncoder;
	Encoder* m_rightEncoder;
	
	SEM_ID m_mutex;
};


#endif


