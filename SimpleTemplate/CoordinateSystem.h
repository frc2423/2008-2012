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
	void SetWheelInformation (double wheel_radius, double wheel_base);
	
private:
	static void TimerFn(void * param); 
	void CalculationTimerFn ();
	
	CoordinateSystem();
	
	double m_Angle;								// Heading of the robot
	DoubleProxy m_leftMeters, m_rightMeters;	// Stores distance in meters
	double m_wheelBase;							// Wheel base of the robot
	
	Notifier m_notifier;
	Encoder* m_leftEncoder;
	Encoder* m_rightEncoder;
	
	SEM_ID m_mutex;
};



#endif
