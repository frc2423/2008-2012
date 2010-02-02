#ifndef COORDINATESYSTEM_H
#define COORDINATESYSTEM_H

#include <WebDMA/WebDMA.h>
#include <WPILib.h>

class CoordinateSystem
{
public:
	CoordinateSystem (
			Encoder* leftencoder,
			Encoder* rightencoder,
			WebDMA* webdma
			);
	
	void Start ();
	void SetWheelInformation (const double wheel_radius, const double wheel_base);
	void getData(double &x, double &y, double &angle);

	
private:
	static void TimerFn(void * param); 
	void CalculationTimerFn();
	void ComputeNewPosition(double l, double r);
	void TransformCoordinates(double dx, double dy, double dtheta);
	
	CoordinateSystem();
	
	
	double m_theta;								// Heading of the robot
	DoubleProxy m_leftMeters, m_rightMeters;	// Stores distance in meters
	DoubleProxy m_x, m_y;
	DoubleProxy m_display_angle;
	double m_wheelBase;							// Wheel base of the robot
	
	IntProxy m_l, m_r;
	
	Notifier m_notifier;
	Encoder* m_leftEncoder;
	Encoder* m_rightEncoder;
	
	SEM_ID m_mutex;
};


#endif


