/**
	\file 		PositionInformation.h
	\author 	Amory Galili: last changed by $Author$
	\date 		Last changed on $Date$
	\version 	$Rev$
*/


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
	DoubleProxy m_gyro_angle;
	double m_wheelBase;							// Wheel base of the robot
	
	IntProxy m_l, m_r;
	
	Notifier m_notifier;
	RobotResources& m_resources;
	
	SEM_ID m_mutex;
};


#endif


